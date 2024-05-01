/* InspIRCd functions
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/encryption.h"
#include "modules/cs_mode.h"
#include "modules/httpd.h"
#include "modules/sasl.h"

typedef std::map<char, unsigned> ListLimits;

struct SASLUser final
{
	Anope::string uid;
	Anope::string acc;
	time_t created;
};

namespace
{
	// The SID of a server we are waiting to squit.
	Anope::string rsquit_id;

	// The hostname of a server we are waiting to squit.
	Anope::string rsquit_server;

	// Non-introduced users who have authenticated via SASL.
	std::list<SASLUser> saslusers;

	// A reference to the SHA-2 algorithm provider.
	static ServiceReference<Encryption::Provider> sha256("Encryption::Provider", "sha256");

	// The version of the InspIRCd protocol that we are using.
	size_t spanningtree_proto_ver = 1205;

	// Parses a module name in the format "m_foo.so=bar" to {foo, bar}.
	void ParseModule(const Anope::string &module, Anope::string &modname, Anope::string &moddata)
	{
		size_t sep = module.find('=');

		// Extract and clean up the module name.
		modname = module.substr(0, sep);
		if (modname.compare(0, 2, "m_", 2) == 0)
			modname.erase(0, 2);

		if (modname.length() > 3 && modname.compare(modname.length() - 3, 3, ".so", 3) == 0)
			modname.erase(modname.length() - 3);

		// Extract the module link data (if any).
		moddata = (sep == Anope::string::npos) ? "" : module.substr(sep + 1);

		if (Anope::ProtocolDebug)
			Log(LOG_DEBUG) << "Parsed module: name=" << modname << " data=" << moddata;
	}

	void ParseModuleData(const Anope::string &moddata, Anope::map<Anope::string> &modmap)
	{
		sepstream datastream(moddata, '&');
		for (Anope::string datapair; datastream.GetToken(datapair); )
		{
			size_t split = datapair.find('=');
			std::pair<Anope::map<Anope::string>::iterator, bool> res;
			if (split == Anope::string::npos)
				res = modmap.emplace(datapair, "");
			else
				res = modmap.emplace(datapair.substr(0, split), HTTPUtils::URLDecode(datapair.substr(split + 1)));

			if (Anope::ProtocolDebug && res.second)
				Log(LOG_DEBUG) << "Parsed module data: key=" << res.first->first << " value=" << res.first->second;
		}
	}
}

class InspIRCdProto final
	: public IRCDProto
{
private:
	static Anope::string GetAccountNicks(NickAlias* na)
	{
		if (!na)
			return {};

		Anope::string nicks;
		for (const auto &na : *na->nc->aliases)
			nicks += " " + na->nick;
		return nicks.substr(1);
	}

	static void SendChgIdentInternal(const Anope::string &uid, const Anope::string &vident)
	{
		if (!Servers::Capab.count("CHGIDENT"))
		{
			Log() << "Unable to change the vident of " << uid << " as the remote server does not have the chgident module loaded.";
			return;
		}
		Uplink::Send("ENCAP", uid.substr(0, 3), "CHGIDENT", uid, vident);
	}

	static void SendChgHostInternal(const Anope::string &uid, const Anope::string &vhost)
	{
		if (!Servers::Capab.count("CHGHOST"))
		{
			Log() << "Unable to change the vhost of " << uid << " as the remote server does not have the chghost module loaded.";
			return;
		}
		Uplink::Send("ENCAP", uid.substr(0, 3), "CHGHOST", uid, vhost);
	}

	static void SendAddLine(const Anope::string &xtype, const Anope::string &mask, time_t duration, const Anope::string &addedby, const Anope::string &reason)
	{
		Uplink::Send("ADDLINE", xtype, mask, addedby, Anope::CurTime, duration, reason);
	}

	static void SendDelLine(const Anope::string &xtype, const Anope::string &mask)
	{
		Uplink::Send("DELLINE", xtype, mask);
	}

	static void SendAccount(const Anope::string &uid, NickAlias *na)
	{
		Uplink::Send("METADATA", uid, "accountid", na ? Anope::ToString(na->nc->GetId()) : Anope::string());
		Uplink::Send("METADATA", uid, "accountname", na ? na->nc->display : Anope::string());
		if (spanningtree_proto_ver >= 1206)
			Uplink::Send("METADATA", uid, "accountnicks", GetAccountNicks(na));
	}

public:
	PrimitiveExtensibleItem<ListLimits> maxlist;

	InspIRCdProto(Module *creator) : IRCDProto(creator, "InspIRCd 3+"), maxlist(creator, "maxlist")
	{
		DefaultPseudoclientModes = "+oI";
		CanSVSNick = true;
		CanSVSJoin = true;
		CanSetVHost = true;
		CanSetVIdent = true;
		CanSQLine = true;
		CanSZLine = true;
		CanSVSLogout = true;
		CanCertFP = true;
		RequiresID = true;
		MaxModes = 0;
		MaxLine = 0;
	}

	size_t GetMaxListFor(Channel *c, ChannelMode *cm) override
	{
		ListLimits *limits = maxlist.Get(c);
		if (limits)
		{
			ListLimits::const_iterator limit = limits->find(cm->mchar);
			if (limit != limits->end())
				return limit->second;
		}

		// Fall back to the config limit if we can't find the mode.
		return IRCDProto::GetMaxListFor(c, cm);
	}


	void SendConnect() override
	{
		Uplink::Send("CAPAB", "START", 1206);
		Uplink::Send("CAPAB", "CAPABILITIES", "CASEMAPPING=" + Config->GetBlock("options")->Get<const Anope::string>("casemap", "ascii") + (sha256 ? " CHALLENGE=*" : ""));
		Uplink::Send("CAPAB", "END");
	}

	void SendSASLMechanisms(std::vector<Anope::string> &mechanisms) override
	{
		Anope::string mechlist;
		for (const auto &mechanism : mechanisms)
			mechlist += "," + mechanism;

		Uplink::Send("METADATA", "*", "saslmechlist", mechanisms.empty() ? "" : mechlist.substr(1));
	}

	void SendSVSKill(const MessageSource &source, User *user, const Anope::string &buf) override
	{
		IRCDProto::SendSVSKill(source, user, buf);
		user->KillInternal(source, buf);
	}

	void SendForceNickChange(User *u, const Anope::string &newnick, time_t when) override
	{
		Uplink::Send("SVSNICK", u->GetUID(), newnick, when, u->timestamp);
	}

	void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) override
	{
		Uplink::Send(bi, "NOTICE", "$" + dest->GetName(), msg);
	}

	void SendGlobalPrivmsg(BotInfo *bi, const Server *dest, const Anope::string &msg) override
	{
		Uplink::Send(bi, "PRIVMSG", "$" + dest->GetName(), msg);
	}

	void SendContextNotice(BotInfo *bi, User *target, Channel *context, const Anope::string &msg) override
	{
		if (spanningtree_proto_ver >= 1206)
		{
			IRCD->SendNoticeInternal(bi, target->GetUID(), msg, {
				{ "~context", context->name },
			});
			return;
		}
		IRCDProto::SendContextNotice(bi, target, context, msg);
	}

	void SendContextPrivmsg(BotInfo *bi, User *target, Channel *context, const Anope::string &msg) override
	{
		if (spanningtree_proto_ver >= 1206)
		{
			IRCD->SendPrivmsgInternal(bi, target->GetUID(), msg, {
				{ "~context", context->name },
			});
			return;
		}
		IRCDProto::SendContextPrivmsg(bi, target, context, msg);
	}

	void SendClearBans(const MessageSource &user, Channel *c, User* u) override
	{
		Uplink::Send(user, "SVSCMODE", u->GetUID(), c->name, 'b');
	}

	void SendPong(const Anope::string &servname, const Anope::string &who) override
	{
		Server *serv = servname.empty() ? NULL : Server::Find(servname);
		if (!serv)
			serv = Me;

		Uplink::Send(serv, "PONG", who);
	}

	void SendAkillDel(const XLine *x) override
	{
		{
			/* InspIRCd may support regex bans
			 * Mask is expected in format: 'n!u@h\sr' and spaces as '\s'
			 * We remove the '//' and replace '#' and any ' ' with '\s'
			 */
			if (x->IsRegex() && Servers::Capab.count("RLINE"))
			{
				Anope::string mask = x->mask;
				if (mask.length() >= 2 && mask[0] == '/' && mask[mask.length() - 1] == '/')
					mask = mask.substr(1, mask.length() - 2);
				size_t h = mask.find('#');
				if (h != Anope::string::npos)
				{
					mask = mask.replace(h, 1, "\\s");
					mask = mask.replace_all_cs(" ", "\\s");
				}
				SendDelLine("R", mask);
				return;
			}
			else if (x->IsRegex() || x->HasNickOrReal())
				return;

			/* ZLine if we can instead */
			if (x->GetUser() == "*")
			{
				cidr addr(x->GetHost());
				if (addr.valid())
				{
					IRCD->SendSZLineDel(x);
					return;
				}
			}

			SendDelLine("G", x->GetUser() + "@" + x->GetHost());
		}
	}

	void SendInvite(const MessageSource &source, const Channel *c, User *u) override
	{
		Uplink::Send(source, "INVITE", u->GetUID(), c->name, c->creation_time);
	}

	void SendTopic(const MessageSource &source, Channel *c) override
	{
		if (Servers::Capab.count("TOPICLOCK"))
		{
			Uplink::Send(c->WhoSends(), "SVSTOPIC", c->name, c->topic_ts, c->topic_setter, c->topic);
		}
		else
		{
			/* If the last time a topic was set is after the TS we want for this topic we must bump this topic's timestamp to now */
			time_t ts = c->topic_ts;
			if (c->topic_time > ts)
				ts = Anope::CurTime;
			/* But don't modify c->topic_ts, it should remain set to the real TS we want as ci->last_topic_time pulls from it */
			Uplink::Send(source, "FTOPIC", c->name, c->creation_time, ts, c->topic_setter, c->topic);
		}
	}

	void SendVHostDel(User *u) override
	{
		UserMode *um = ModeManager::FindUserModeByName("CLOAK");

		if (um && !u->HasMode(um->name))
			// Just set +x if we can
			u->SetMode(NULL, um);
		else
			// Try to restore cloaked host
			this->SendChgHostInternal(u->nick, u->chost);
	}

	void SendAkill(User *u, XLine *x) override
	{
		// Calculate the time left before this would expire
		time_t timeleft = x->expires ? x->expires - Anope::CurTime : x->expires;

		/* InspIRCd may support regex bans, if they do we can send this and forget about it
		 * Mask is expected in format: 'n!u@h\sr' and spaces as '\s'
		 * We remove the '//' and replace '#' and any ' ' with '\s'
		 */
		if (x->IsRegex() && Servers::Capab.count("RLINE"))
		{
			Anope::string mask = x->mask;
			if (mask.length() >= 2 && mask[0] == '/' && mask[mask.length() - 1] == '/')
				mask = mask.substr(1, mask.length() - 2);
			size_t h = mask.find('#');
			if (h != Anope::string::npos)
			{
				mask = mask.replace(h, 1, "\\s");
				mask = mask.replace_all_cs(" ", "\\s");
			}
			SendAddLine("R", mask, timeleft, x->by, x->GetReason());
			return;
		}
		else if (x->IsRegex() || x->HasNickOrReal())
		{
			if (!u)
			{
				/* No user (this akill was just added), and contains nick and/or realname. Find users that match and ban them */
				for (const auto &[_, user] : UserListByNick)
					if (x->manager->Check(user, x))
						this->SendAkill(user, x);
				return;
			}

			const XLine *old = x;

			if (old->manager->HasEntry("*@" + u->host))
				return;

			/* We can't akill x as it has a nick and/or realname included, so create a new akill for *@host */
			x = new XLine("*@" + u->host, old->by, old->expires, old->reason, old->id);
			old->manager->AddXLine(x);

			Log(Config->GetClient("OperServ"), "akill") << "AKILL: Added an akill for " << x->mask << " because " << u->GetMask() << "#" << u->realname << " matches " << old->mask;
		}

		/* ZLine if we can instead */
		if (x->GetUser() == "*")
		{
			cidr addr(x->GetHost());
			if (addr.valid())
			{
				IRCD->SendSZLine(u, x);
				return;
			}
		}

		SendAddLine("G", x->GetUser() + "@" + x->GetHost(), timeleft, x->by, x->GetReason());
	}

	void SendNumericInternal(int numeric, const Anope::string &dest, const std::vector<Anope::string> &params) override
	{
		auto newparams = params;
		newparams.insert(newparams.begin(), { Me->GetSID(), dest, Anope::ToString(numeric) });
		Uplink::SendInternal({}, Me, "NUM", newparams);
	}

	void SendModeInternal(const MessageSource &source, Channel *chan, const Anope::string &modes, const std::vector<Anope::string> &values) override
	{
		auto params = values;
		params.insert(params.begin(), { chan->name, Anope::ToString(chan->creation_time), modes });
		Uplink::SendInternal({}, source, "FMODE", params);
	}

	void SendClientIntroduction(User *u) override
	{
		if (spanningtree_proto_ver >= 1206)
		{
			Uplink::Send("UID", u->GetUID(), u->timestamp, u->nick, u->host, u->host, u->GetIdent(), u->GetIdent(),
				"0.0.0.0", u->timestamp, "+" + u->GetModes(), u->realname);
		}
		else
		{
			Uplink::Send("UID", u->GetUID(), u->timestamp, u->nick, u->host, u->host, u->GetIdent(), "0.0.0.0",
				u->timestamp, "+" + u->GetModes(), u->realname);
		}

		if (u->GetModes().find('o') != Anope::string::npos)
		{
			// Mark as introduced so we can send an oper type.
			BotInfo *bi = BotInfo::Find(u->nick, true);
			if (bi)
				bi->introduced = true;

			Uplink::Send(u, "OPERTYPE", "service");
		}
	}

	void SendServer(const Server *server) override
	{
		/* if rsquit is set then we are waiting on a squit */
		if (rsquit_id.empty() && rsquit_server.empty())
			Uplink::Send("SERVER", server->GetName(), server->GetSID(), server->GetDescription());
	}

	void SendSquit(Server *s, const Anope::string &message) override
	{
		if (s != Me)
		{
			rsquit_id = s->GetSID();
			rsquit_server = s->GetName();
			Uplink::Send("RSQUIT", s->GetName(), message);
		}
		else
			Uplink::Send("SQUIT", s->GetName(), message);
	}

	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override
	{
		Uplink::Send("FJOIN", c->name, c->creation_time, "+" + c->GetModes(true, true), "," + user->GetUID());
		/* Note that we can send this with the FJOIN but choose not to
		 * because the mode stacker will handle this and probably will
		 * merge these modes with +nrt and other mlocked modes
		 */
		if (status)
		{
			/* First save the channel status incase uc->Status == status */
			ChannelStatus cs = *status;
			/* If the user is internally on the channel with flags, kill them so that
			 * the stacker will allow this.
			 */
			ChanUserContainer *uc = c->FindUser(user);
			if (uc != NULL)
				uc->status.Clear();

			BotInfo *setter = BotInfo::Find(user->GetUID());
			for (auto mode : cs.Modes())
				c->SetMode(setter, ModeManager::FindChannelModeByChar(mode), user->GetUID(), false);

			if (uc != NULL)
				uc->status = cs;
		}
	}

	void SendSQLineDel(const XLine *x) override
	{
		if (IRCD->CanSQLineChannel && (x->mask[0] == '#'))
			SendDelLine("CBAN", x->mask);
		else
			SendDelLine("Q", x->mask);
	}

	void SendSQLine(User *u, const XLine *x) override
	{
		// Calculate the time left before this would expire
		time_t timeleft = x->expires ? x->expires - Anope::CurTime : x->expires;

		if (IRCD->CanSQLineChannel && (x->mask[0] == '#'))
			SendAddLine("CBAN", x->mask, timeleft, x->by, x->GetReason());
		else
			SendAddLine("Q", x->mask, timeleft, x->by, x->GetReason());
	}

	void SendVHost(User *u, const Anope::string &vident, const Anope::string &vhost) override
	{
		if (!vident.empty())
			this->SendChgIdentInternal(u->GetUID(), vident);

		if (!vhost.empty())
			this->SendChgHostInternal(u->GetUID(), vhost);
	}

	void SendSVSHold(const Anope::string &nick, time_t t) override
	{
		Uplink::Send(Config->GetClient("NickServ"), "SVSHOLD", nick, t, "Being held for a registered user");
	}

	void SendSVSHoldDel(const Anope::string &nick) override
	{
		Uplink::Send(Config->GetClient("NickServ"), "SVSHOLD", nick);
	}

	void SendSZLineDel(const XLine *x) override
	{
		SendDelLine("Z", x->GetHost());
	}

	void SendSZLine(User *u, const XLine *x) override
	{
		// Calculate the time left before this would expire
		time_t timeleft = x->expires ? x->expires - Anope::CurTime : x->expires;

		SendAddLine("Z", x->GetHost(), timeleft, x->by, x->GetReason());
	}

	void SendSVSJoin(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &other) override
	{
		Uplink::Send(source, "SVSJOIN", u->GetUID(), chan);
	}

	void SendSVSPart(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &param) override
	{
		if (!param.empty())
			Uplink::Send(source, "SVSPART", u->GetUID(), chan, param);
		else
			Uplink::Send(source, "SVSPART", u->GetUID(), chan);
	}

	void SendSWhois(const MessageSource &bi, const Anope::string &who, const Anope::string &mask) override
	{
		User *u = User::Find(who);
		Uplink::Send("METADATA", u->GetUID(), "swhois", mask);
	}

	void SendBOB() override
	{
		Uplink::Send("BURST", Anope::CurTime);
		Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);

		if (spanningtree_proto_ver >= 1206)
		{
			Uplink::Send("SINFO", "customversion", Anope::printf("%s -- (%s) -- %s", IRCD->GetProtocolName().c_str(), enc ? enc->name.c_str() : "none", Anope::VersionBuildString().c_str()));
			Uplink::Send("SINFO", "rawbranch", "Anope-" + Anope::VersionShort());
		}
		else
		{
			Uplink::Send("SINFO", "version", Anope::printf("Anope-%s %s :%s -- (%s) -- %s", Anope::Version().c_str(), Me->GetName().c_str(), IRCD->GetProtocolName().c_str(), enc ? enc->name.c_str() : "none", Anope::VersionBuildString().c_str()));
			Uplink::Send("SINFO", "fullversion", Anope::printf("Anope-%s %s :[%s] %s -- (%s) -- %s", Anope::Version().c_str(), Me->GetName().c_str(), Me->GetSID().c_str(), IRCD->GetProtocolName().c_str(), enc ? enc->name.c_str() : "none", Anope::VersionBuildString().c_str()));
		}
		Uplink::Send("SINFO", "rawversion", "Anope-" + Anope::VersionShort());
	}

	void SendEOB() override
	{
		Uplink::Send("ENDBURST");
	}

	void SendGlobops(const MessageSource &source, const Anope::string &buf) override
	{
		if (Servers::Capab.count("GLOBOPS"))
			Uplink::Send(source, "SNONOTICE", 'g', buf);
		else
			Uplink::Send(source, "SNONOTICE", "A", buf);
	}

	void SendLogin(User *u, NickAlias *na) override
	{
		/* InspIRCd uses an account to bypass chmode +R, not umode +r, so we can't send this here */
		if (!na->nc->HasExt("UNCONFIRMED"))
			SendAccount(u->GetUID(), na);
	}

	void SendLogout(User *u) override
	{
		SendAccount(u->GetUID(), nullptr);
	}

	void SendChannel(Channel *c) override
	{
		Uplink::Send("FJOIN", c->name, c->creation_time, "+" + c->GetModes(true, true), "");
	}

	void SendSASLMessage(const SASL::Message &message) override
	{
		if (message.ext.empty())
			Uplink::Send("ENCAP", message.target.substr(0, 3), "SASL", message.source, message.target, message.type, message.data);
		else
			Uplink::Send("ENCAP", message.target.substr(0, 3), "SASL", message.source, message.target, message.type, message.data, message.ext);
	}

	void SendSVSLogin(const Anope::string &uid, NickAlias *na) override
	{
		SendAccount(uid, na);

		// Expire old pending sessions or other sessions for this user.
		for (auto it = saslusers.begin(); it != saslusers.end(); )
		{
			const SASLUser &u = *it;
			if (u.created + 30 < Anope::CurTime || u.uid == uid)
				it = saslusers.erase(it);
			else
				++it;
		}

		if (na)
		{
			if (!na->GetVHostIdent().empty())
				SendChgHostInternal(uid, na->GetVHostIdent());

			if (!na->GetVHostHost().empty())
				SendChgHostInternal(uid, na->GetVHostHost());

			// Mark this SASL session as pending user introduction.
			SASLUser su;
			su.uid = uid;
			su.acc = na->nc->display;
			su.created = Anope::CurTime;
			saslusers.push_back(su);
		}
	}

	bool IsExtbanValid(const Anope::string &mask) override
	{
		return mask.length() >= 3 && mask[1] == ':';
	}

	bool IsIdentValid(const Anope::string &ident) override
	{
		if (ident.empty() || ident.length() > IRCD->MaxUser)
			return false;

		for (auto c : ident)
		{
			if (c >= 'A' && c <= '}')
				continue;

			if ((c >= '0' && c <= '9') || c == '-' || c == '.')
				continue;

			return false;
		}

		return true;
	}

	bool IsTagValid(const Anope::string &tname, const Anope::string &tvalue) override
	{
		// InspIRCd accepts arbitrary message tags.
		return true;
	}
};

class InspIRCdAutoOpMode final
	: public ChannelModeList
{
public:
	InspIRCdAutoOpMode(char mode) : ChannelModeList("AUTOOP", mode)
	{
	}

	bool IsValid(Anope::string &mask) const override
	{
		// We can not validate this because we don't know about the
		// privileges of the setter so just reject attempts to set it.
		return false;
	}
};

// NOTE: matchers for the following extbans have not been implemented:
//
// * class(n):    data not available
// * country(G):  data not available
// * gateway(w):  data not available in v3
// * oper(o):     todo
// * realmask(a): todo
namespace InspIRCdExtBan
{
	class Base
		: public ChannelModeVirtual<ChannelModeList>
	{
	private:
		unsigned char xbchar;
		Anope::string xbname;

	public:
		Base(const Anope::string &mname, const Anope::string &xname, char xchar)
			: ChannelModeVirtual<ChannelModeList>(mname, "BAN")
			, xbchar(xchar)
			, xbname(xname)
		{
		}

		ChannelMode *Wrap(Anope::string &param) override
		{
			param = Anope::string(xbchar) + ":" + param;
			return ChannelModeVirtual<ChannelModeList>::Wrap(param);
		}

		ChannelMode *Unwrap(ChannelMode *cm, Anope::string &param) override
		{
			// The mask must be in the format [!]<letter>:<value> or [!]<name>:<value>.
			if (cm->type != MODE_LIST)
				return cm;

			auto startpos = 0;
			if (param[0] == '!')
				startpos++;

			auto endpos = param.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", startpos);
			if (endpos == Anope::string::npos || param[endpos] != ':')
				return cm;

			auto name = param.substr(startpos, endpos - startpos);
			if (param.length() >= endpos || (name.length() == 1 ? name[0] != xbchar : name != xbname))
				return cm;

			param.erase(0, endpos);
			return this;
		}
	};

	class EntryMatcher final
		: public Base
	{
	public:
		EntryMatcher(const Anope::string &mname, const Anope::string &xname, char xchar)
			: Base(mname, xname, xchar)
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();
			Anope::string real_mask = mask.substr(2);

			return Entry(this->name, real_mask).Matches(u);
		}
	};

	class ChannelMatcher final
		: public Base
	{
	public:
		ChannelMatcher(const Anope::string &mname, const Anope::string &xname, char xchar)
			: Base(mname, xname, xchar)
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();

			Anope::string channel = mask.substr(2);

			ChannelMode *cm = NULL;
			if (channel[0] != '#')
			{
				char modeChar = ModeManager::GetStatusChar(channel[0]);
				channel.erase(channel.begin());
				cm = ModeManager::FindChannelModeByChar(modeChar);
				if (cm != NULL && cm->type != MODE_STATUS)
					cm = NULL;
			}

			Channel *c = Channel::Find(channel);
			if (c != NULL)
			{
				ChanUserContainer *uc = c->FindUser(u);
				if (uc != NULL)
					if (cm == NULL || uc->status.HasMode(cm->mchar))
						return true;
			}

			return false;
		}
	};

	class AccountMatcher final
		: public Base
	{
	public:
		AccountMatcher(const Anope::string &mname, const Anope::string &xname, char xchar)
			: Base(mname, xname, xchar)
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();
			Anope::string real_mask = mask.substr(2);

			return u->IsIdentified() && real_mask.equals_ci(u->Account()->display);
		}
	};

	class RealnameMatcher final
		: public Base
	{
	public:
		RealnameMatcher(const Anope::string &mname, const Anope::string &xname, char xchar)
			: Base(mname, xname, xchar)
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();
			Anope::string real_mask = mask.substr(2);
			return Anope::Match(u->realname, real_mask);
		}
	};

	class ServerMatcher final
		: public Base
	{
	public:
		ServerMatcher(const Anope::string &mname, const Anope::string &xname, char xchar)
			: Base(mname, xname, xchar)
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();
			Anope::string real_mask = mask.substr(2);
			return Anope::Match(u->server->GetName(), real_mask);
		}
	};

	class FingerprintMatcher final
		: public Base
	{
	public:
		FingerprintMatcher(const Anope::string &mname, const Anope::string &xname, char xchar)
			: Base(mname, xname, xchar)
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();
			Anope::string real_mask = mask.substr(2);
			return !u->fingerprint.empty() && Anope::Match(u->fingerprint, real_mask);
		}
	};

	class UnidentifiedMatcher final
		: public Base
	{
	public:
		UnidentifiedMatcher(const Anope::string &mname, const Anope::string &xname, char xchar)
			: Base(mname, xname, xchar)
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();
			Anope::string real_mask = mask.substr(2);
			return !u->Account() && Entry("BAN", real_mask).Matches(u);
		}
	};

	class OperTypeMatcher
		: public Base
	{
	 public:
		OperTypeMatcher(const Anope::string &mname, const Anope::string &xname, char xchar)
			: Base(mname, xname, xchar)
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			Anope::string *opertype = u->GetExt<Anope::string>("opertype");
			if (!opertype)
				return false; // Not an operator.

			const Anope::string &mask = e->GetMask();
			Anope::string real_mask = mask.substr(2);
			return Anope::Match(opertype->replace_all_cs(' ', '_'), real_mask);
		}
	};
}

class ColonDelimitedParamMode
	: public ChannelModeParam
{
public:
	ColonDelimitedParamMode(const Anope::string &modename, char modeChar) : ChannelModeParam(modename, modeChar, true) { }

	bool IsValid(Anope::string &value) const override
	{
		return IsValid(value, false);
	}

	static bool IsValid(const Anope::string &value, bool historymode)
	{
		if (value.empty())
			return false; // empty param is never valid

		Anope::string::size_type pos = value.find(':');
		if ((pos == Anope::string::npos) || (pos == 0))
			return false; // no ':' or it's the first char, both are invalid

		Anope::string rest;
		if (Anope::Convert<int>(value, 0, &rest) <= 0)
			return false; // negative numbers and zero are invalid

		rest = rest.substr(1);
		if (historymode)
		{
			// For the history mode, the part after the ':' is a duration and it
			// can be in the user friendly "1d3h20m" format, make sure we accept that
			return Anope::DoTime(rest) <= 0;
		}
		else
		{
			return Anope::Convert(rest, 0) <= 0;
		}
	}
};

class SimpleNumberParamMode final
	: public ChannelModeParam
{
public:
	SimpleNumberParamMode(const Anope::string &modename, char modeChar) : ChannelModeParam(modename, modeChar, true) { }

	bool IsValid(Anope::string &value) const override
	{
		if (value.empty())
			return false; // empty param is never valid

		return Anope::Convert<int>(value, 0) <= 0;
	}
};

class ChannelModeFlood final
	: public ColonDelimitedParamMode
{
public:
	ChannelModeFlood(char modeChar) : ColonDelimitedParamMode("FLOOD", modeChar) { }

	bool IsValid(Anope::string &value) const override
	{
		// The parameter of this mode is a bit different, it may begin with a '*',
		// ignore it if that's the case
		Anope::string v = value[0] == '*' ? value.substr(1) : value;
		return ((!value.empty()) && (ColonDelimitedParamMode::IsValid(v)));
	}
};

class ChannelModeHistory final
	: public ColonDelimitedParamMode
{
public:
	ChannelModeHistory(char modeChar) : ColonDelimitedParamMode("HISTORY", modeChar) { }

	bool IsValid(Anope::string &value) const override
	{
		return (ColonDelimitedParamMode::IsValid(value, true));
	}
};

class ChannelModeRedirect final
	: public ChannelModeParam
{
public:
	ChannelModeRedirect(char modeChar) : ChannelModeParam("REDIRECT", modeChar, true) { }

	bool IsValid(Anope::string &value) const override
	{
		// The parameter of this mode is a channel, and channel names start with '#'
		return ((!value.empty()) && (value[0] == '#'));
	}
};

struct IRCDMessageAway final
	: Message::Away
{
	IRCDMessageAway(Module *creator) : Message::Away(creator, "AWAY") { SetFlag(FLAG_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		std::vector<Anope::string> newparams(params);
		if (newparams.size() > 1)
			newparams.erase(newparams.begin());

		Message::Away::Run(source, newparams, tags);
	}
};

struct IRCDMessageCapab final
	: IRCDMessage
{
	struct ExtBanInfo final
	{
		// The letter assigned to the extban.
		char letter = 0;

		// The name of the extban.
		Anope::string name;

		// The type of extban.
		Anope::string type;
	};

	struct ModeInfo final
	{
		// The letter assigned to the mode.
		char letter = 0;

		// If a prefix mode then the rank of the prefix.
		unsigned level = 0;

		// The name of the mode.
		Anope::string name;

		// If a prefix mode then the symbol associated with the prefix.
		char symbol = 0;

		// The type of mode.
		Anope::string type;
	};

	// The HMAC challenge sent by the remote server.
	Anope::string challenge;

	Anope::string GetPassword()
	{
		if (challenge.empty() || !sha256)
			return Config->Uplinks[Anope::CurrentUplink].password;

		Anope::string b64challenge;
		Anope::B64Encode(sha256->HMAC(Config->Uplinks[Anope::CurrentUplink].password, challenge), b64challenge);
		challenge.clear();

		return "AUTH:" + b64challenge.rtrim('=');
	}

	static std::pair<Anope::string, Anope::string> ParseCapability(const Anope::string &token)
	{
		Anope::string key;
		Anope::string value;
		auto sep = token.find('=');
		if (sep == Anope::string::npos)
		{
			// FOO
			key = token;
		}
		else
		{
			// FOO=bar
			key = token.substr(0, sep);
			value = token.substr(sep + 1);
		}

		if (Anope::ProtocolDebug)
			Log(LOG_DEBUG) << "Parsed capability: key=" << key << " value=" << value;

		return { key, value };
	}

	static bool ParseExtBan(const Anope::string &token, ExtBanInfo &extban)
	{
		// acting:foo=f  matching:foo=f
		//       A   B           A   B
		auto a = token.find(':');
		if (a == Anope::string::npos)
			return false;

		auto b = token.find(':', a + 1);
		if (b == Anope::string::npos)
			return false;

		extban.type = token.substr(0, a);
		extban.name = token.substr(a + 1, b - a - 1);
		extban.letter = token[b + 1];

		if (Anope::ProtocolDebug)
			Log(LOG_DEBUG) << "Parsed extban: type=" << extban.type << " name=" << extban.name << " letter=" << extban.letter;
		return true;
	}

	static bool ParseMode(const Anope::string &token, ModeInfo &mode)
	{
		// list:ban=b  param-set:limit=l  param:key=k  prefix:30000:op=@o  simple:noextmsg=n
		//     A   C            A     C        A   C         A     B  C          A        C
		Anope::string::size_type a = token.find(':');
		if (a == Anope::string::npos)
			return false;

		// If the mode is a prefix mode then it also has a rank.
		mode.type = token.substr(0, a);
		if (mode.type == "prefix")
		{
			Anope::string::size_type b = token.find(':', a + 1);
			if (b == Anope::string::npos)
				return false;

			const Anope::string modelevel = token.substr(a + 1, b - a - 1);
			mode.level = Anope::Convert<unsigned>(modelevel, 0);
			a = b;
		}

		Anope::string::size_type c = token.find('=', a + 1);
		if (c == Anope::string::npos)
			return false;

		mode.name = token.substr(a + 1, c - a - 1);
		switch (token.length() - c)
		{
			case 2:
				mode.letter = token[c + 1];
				break;
			case 3:
				mode.symbol = token[c + 1];
				mode.letter = token[c + 2];
				break;
			default:
				return false;
		}

		if (Anope::ProtocolDebug)
		{
			Log(LOG_DEBUG) << "Parsed mode: type=" << mode.type << " name=" << mode.name << " level="
				<< mode.level << " symbol=" << mode.symbol << " letter=" << mode.letter;
		}
		return true;
	}

	IRCDMessageCapab(Module *creator)
		: IRCDMessage(creator, "CAPAB", 1)
	{
		SetFlag(FLAG_SOFT_LIMIT);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		if (params[0].equals_cs("START"))
		{
			spanningtree_proto_ver = 0;
			if (params.size() >= 2)
				spanningtree_proto_ver = Anope::Convert<size_t>(params[1], 0);

			if (spanningtree_proto_ver < 1205)
				throw ProtocolException("Protocol mismatch, no or invalid protocol version given in CAPAB START.");

			Servers::Capab.clear();
			IRCD->CanClearBans = false;
			IRCD->CanSQLineChannel = false;
			IRCD->CanSVSHold = false;
			IRCD->DefaultPseudoclientModes = "+oI";
		}
		else if (params[0].equals_cs("CHANMODES") && params.size() > 1)
		{
			spacesepstream ssep(params[1]);
			Anope::string capab;

			while (ssep.GetToken(capab))
			{
				ModeInfo mode;
				if (!ParseMode(capab, mode))
					continue;

				ChannelMode *cm = NULL;
				if (mode.name.equals_cs("admin"))
					cm = new ChannelModeStatus("PROTECT", mode.letter, mode.symbol, mode.level);
				else if (mode.name.equals_cs("allowinvite"))
				{
					cm = new ChannelMode("ALLINVITE", mode.letter);
					if (spanningtree_proto_ver < 1206)
						ModeManager::AddChannelMode(new InspIRCdExtBan::EntryMatcher("INVITEBAN", "", 'A'));
				}
				else if (mode.name.equals_cs("auditorium"))
					cm = new ChannelMode("AUDITORIUM", mode.letter);
				else if (mode.name.equals_cs("autoop"))
					cm = new InspIRCdAutoOpMode(mode.letter);
				else if (mode.name.equals_cs("ban"))
					cm = new ChannelModeList("BAN", mode.letter);
				else if (mode.name.equals_cs("banexception"))
					cm = new ChannelModeList("EXCEPT", mode.letter);
				else if (mode.name.equals_cs("blockcaps"))
				{
					cm = new ChannelMode("BLOCKCAPS", mode.letter);
					if (spanningtree_proto_ver < 1206)
						ModeManager::AddChannelMode(new InspIRCdExtBan::EntryMatcher("BLOCKCAPSBAN", "", 'B'));
				}
				else if (mode.name.equals_cs("blockcolor"))
				{
					cm = new ChannelMode("BLOCKCOLOR", mode.letter);
					if (spanningtree_proto_ver < 1206)
						ModeManager::AddChannelMode(new InspIRCdExtBan::EntryMatcher("BLOCKCOLORBAN", "", 'c'));
				}
				else if (mode.name.equals_cs("c_registered"))
					cm = new ChannelModeNoone("REGISTERED", mode.letter);
				else if (mode.name.equals_cs("censor"))
					cm = new ChannelMode("CENSOR", mode.letter);
				else if (mode.name.equals_cs("delayjoin"))
					cm = new ChannelMode("DELAYEDJOIN", mode.letter);
				else if (mode.name.equals_cs("delaymsg"))
					cm = new SimpleNumberParamMode("DELAYMSG", mode.letter);
				else if (mode.name.equals_cs("filter"))
					cm = new ChannelModeList("FILTER", mode.letter);
				else if (mode.name.equals_cs("flood"))
					cm = new ChannelModeFlood(mode.letter);
				else if (mode.name.equals_cs("founder"))
					cm = new ChannelModeStatus("OWNER", mode.letter, mode.symbol, mode.level);
				else if (mode.name.equals_cs("halfop"))
					cm = new ChannelModeStatus("HALFOP", mode.letter, mode.symbol, mode.level);
				else if (mode.name.equals_cs("history"))
					cm = new ChannelModeHistory(mode.letter);
				else if (mode.name.equals_cs("invex"))
					cm = new ChannelModeList("INVITEOVERRIDE", mode.letter);
				else if (mode.name.equals_cs("inviteonly"))
					cm = new ChannelMode("INVITE", mode.letter);
				else if (mode.name.equals_cs("joinflood"))
					cm = new ColonDelimitedParamMode("JOINFLOOD", mode.letter);
				else if (mode.name.equals_cs("key"))
					cm = new ChannelModeKey(mode.letter);
				else if (mode.name.equals_cs("kicknorejoin"))
					cm = new SimpleNumberParamMode("NOREJOIN", mode.letter);
				else if (mode.name.equals_cs("limit"))
					cm = new ChannelModeParam("LIMIT", mode.letter, true);
				else if (mode.name.equals_cs("moderated"))
					cm = new ChannelMode("MODERATED", mode.letter);
				else if (mode.name.equals_cs("nickflood"))
					cm = new ColonDelimitedParamMode("NICKFLOOD", mode.letter);
				else if (mode.name.equals_cs("noctcp"))
				{
					cm = new ChannelMode("NOCTCP", mode.letter);
					if (spanningtree_proto_ver < 1206)
						ModeManager::AddChannelMode(new InspIRCdExtBan::EntryMatcher("NOCTCPBAN", "", 'C'));
				}
				else if (mode.name.equals_cs("noextmsg"))
					cm = new ChannelMode("NOEXTERNAL", mode.letter);
				else if (mode.name.equals_cs("nokick"))
				{
					cm = new ChannelMode("NOKICK", mode.letter);
					if (spanningtree_proto_ver < 1206)
						ModeManager::AddChannelMode(new InspIRCdExtBan::EntryMatcher("NOKICKBAN", "", 'Q'));
				}
				else if (mode.name.equals_cs("noknock"))
					cm = new ChannelMode("NOKNOCK", mode.letter);
				else if (mode.name.equals_cs("nonick"))
				{
					cm = new ChannelMode("NONICK", mode.letter);
					if (spanningtree_proto_ver < 1206)
						ModeManager::AddChannelMode(new InspIRCdExtBan::EntryMatcher("NONICKBAN", "", 'N'));
				}
				else if (mode.name.equals_cs("nonotice"))
				{
					cm = new ChannelMode("NONOTICE", mode.letter);
					if (spanningtree_proto_ver < 1206)
						ModeManager::AddChannelMode(new InspIRCdExtBan::EntryMatcher("NONOTICEBAN", "", 'T'));
				}
				else if (mode.name.equals_cs("official-join"))
					cm = new ChannelModeStatus("OFFICIALJOIN", mode.letter, mode.symbol, mode.level);
				else if (mode.name.equals_cs("op"))
					cm = new ChannelModeStatus("OP", mode.letter, mode.symbol, mode.level);
				else if (mode.name.equals_cs("operonly"))
				{
					cm = new ChannelModeOperOnly("OPERONLY", mode.letter);
					if (spanningtree_proto_ver < 1206)
						ModeManager::AddChannelMode(new InspIRCdExtBan::OperTypeMatcher("OPERTYPEBAN", "", 'O'));
				}
				else if (mode.name.equals_cs("operprefix"))
					cm = new ChannelModeStatus("OPERPREFIX", mode.letter, mode.symbol, mode.level);
				else if (mode.name.equals_cs("permanent"))
					cm = new ChannelMode("PERM", mode.letter);
				else if (mode.name.equals_cs("private"))
					cm = new ChannelMode("PRIVATE", mode.letter);
				else if (mode.name.equals_cs("redirect"))
					cm = new ChannelModeRedirect(mode.letter);
				else if (mode.name.equals_cs("reginvite"))
					cm = new ChannelMode("REGISTEREDONLY", mode.letter);
				else if (mode.name.equals_cs("regmoderated"))
					cm = new ChannelMode("REGMODERATED", mode.letter);
				else if (mode.name.equals_cs("secret"))
					cm = new ChannelMode("SECRET", mode.letter);
				else if (mode.name.equals_cs("sslonly"))
				{
					cm = new ChannelMode("SSL", mode.letter);
					if (spanningtree_proto_ver < 1206)
						ModeManager::AddChannelMode(new InspIRCdExtBan::FingerprintMatcher("SSLBAN", "", 'z'));
				}
				else if (mode.name.equals_cs("stripcolor"))
				{
					cm = new ChannelMode("STRIPCOLOR", mode.letter);
					if (spanningtree_proto_ver < 1206)
						ModeManager::AddChannelMode(new InspIRCdExtBan::EntryMatcher("STRIPCOLORBAN", "", 'S'));
				}
				else if (mode.name.equals_cs("topiclock"))
					cm = new ChannelMode("TOPIC", mode.letter);
				else if (mode.name.equals_cs("voice"))
					cm = new ChannelModeStatus("VOICE", mode.letter, mode.symbol, mode.level);

				// Handle unknown modes.
				else if (mode.type.equals_cs("list"))
					cm = new ChannelModeList(mode.name.upper(), mode.letter);
				else if (mode.type.equals_cs("param-set"))
					cm = new ChannelModeParam(mode.name.upper(), mode.letter, true);
				else if (mode.type.equals_cs("param"))
					cm = new ChannelModeParam(mode.name.upper(), mode.letter, false);
				else if (mode.type.equals_cs("prefix"))
					cm = new ChannelModeStatus(mode.name.upper(), mode.letter, mode.symbol, mode.level);
				else if (mode.type.equals_cs("simple"))
					cm = new ChannelMode(mode.name.upper(), mode.letter);
				else
					Log(LOG_DEBUG) << "Unknown channel mode: " << capab;

				if (cm)
					ModeManager::AddChannelMode(cm);
			}
		}
		if (params[0].equals_cs("USERMODES") && params.size() > 1)
		{
			spacesepstream ssep(params[1]);
			Anope::string capab;

			while (ssep.GetToken(capab))
			{
				ModeInfo mode;
				if (!ParseMode(capab, mode))
					continue;

				UserMode *um = NULL;
				if (mode.name.equals_cs("bot"))
				{
					um = new UserMode("BOT", mode.letter);
					IRCD->DefaultPseudoclientModes += mode.letter;
				}
				else if (mode.name.equals_cs("callerid"))
					um = new UserMode("CALLERID", mode.letter);
				else if (mode.name.equals_cs("cloak"))
					um = new UserMode("CLOAK", mode.letter);
				else if (mode.name.equals_cs("deaf"))
					um = new UserMode("DEAF", mode.letter);
				else if (mode.name.equals_cs("deaf_commonchan"))
					um = new UserMode("COMMONCHANS", mode.letter);
				else if (mode.name.equals_cs("helpop"))
					um = new UserModeOperOnly("HELPOP", mode.letter);
				else if (mode.name.equals_cs("hidechans"))
					um = new UserMode("PRIV", mode.letter);
				else if (mode.name.equals_cs("hideoper"))
					um = new UserModeOperOnly("HIDEOPER", mode.letter);
				else if (mode.name.equals_cs("invisible"))
					um = new UserMode("INVIS", mode.letter);
				else if (mode.name.equals_cs("invis-oper"))
					um = new UserModeOperOnly("INVISIBLE_OPER", mode.letter);
				else if (mode.name.equals_cs("oper"))
					um = new UserModeOperOnly("OPER", mode.letter);
				else if (mode.name.equals_cs("regdeaf"))
					um = new UserMode("REGPRIV", mode.letter);
				else if (mode.name.equals_cs("servprotect"))
				{
					um = new UserModeNoone("PROTECTED", mode.letter);
					IRCD->DefaultPseudoclientModes += mode.letter;
				}
				else if (mode.name.equals_cs("showwhois"))
					um = new UserMode("WHOIS", mode.letter);
				else if (mode.name.equals_cs("u_censor"))
					um = new UserMode("CENSOR", mode.letter);
				else if (mode.name.equals_cs("u_registered"))
					um = new UserModeNoone("REGISTERED", mode.letter);
				else if (mode.name.equals_cs("u_stripcolor"))
					um = new UserMode("STRIPCOLOR", mode.letter);
				else if (mode.name.equals_cs("wallops"))
					um = new UserMode("WALLOPS", mode.letter);

				// Handle unknown modes.
				else if (mode.type.equals_cs("param-set"))
					um = new UserModeParam(mode.name.upper(), mode.letter);
				else if (mode.type.equals_cs("simple"))
					um = new UserMode(mode.name.upper(), mode.letter);
				else
					Log(LOG_DEBUG) << "Unknown user mode: " << capab;

				if (um)
					ModeManager::AddUserMode(um);
			}
		}
		else if (params[0].equals_cs("EXTBANS"))
		{
			spacesepstream ssep(params[1]);
			Anope::string capab;

			while (ssep.GetToken(capab))
			{
				ExtBanInfo extban;
				if (!ParseExtBan(capab, extban))
					continue;

				InspIRCdExtBan::Base *xb = nullptr;
				if (extban.name.equals_cs("account"))
					xb = new InspIRCdExtBan::AccountMatcher("ACCOUNTBAN", extban.name, extban.letter);
				else if (extban.name.equals_cs("blockcolor"))
					xb = new InspIRCdExtBan::EntryMatcher("BLOCKCOLORBAN", extban.name, extban.letter);
				else if (extban.name.equals_cs("blockinvite"))
					xb = new InspIRCdExtBan::EntryMatcher("INVITEBAN", extban.name, extban.letter);
				else if (extban.name.equals_cs("channel"))
					xb = new InspIRCdExtBan::ChannelMatcher("CHANNELBAN", extban.name, extban.letter);
				else if (extban.name.equals_cs("fingerprint"))
					xb = new InspIRCdExtBan::FingerprintMatcher("SSLBAN", extban.name, extban.letter);
				else if (extban.name.equals_cs("mute"))
					xb = new InspIRCdExtBan::EntryMatcher("QUIET", extban.name, extban.letter);
				else if (extban.name.equals_cs("noctcp"))
					xb = new InspIRCdExtBan::EntryMatcher("NOCTCPBAN", extban.name, extban.letter);
				else if (extban.name.equals_cs("nokick"))
					xb = new InspIRCdExtBan::EntryMatcher("NOKICKBAN", extban.name, extban.letter);
				else if (extban.name.equals_cs("nonick"))
					xb = new InspIRCdExtBan::EntryMatcher("NONICKBAN", extban.name, extban.letter);
				else if (extban.name.equals_cs("nonotice"))
					xb = new InspIRCdExtBan::EntryMatcher("NONOTICEBAN", extban.name, extban.letter);
				else if (extban.name.equals_cs("opertype"))
					xb = new InspIRCdExtBan::OperTypeMatcher("OPERTYPEBAN", extban.name, extban.letter);
				else if (extban.name.equals_cs("opmoderated"))
					xb = new InspIRCdExtBan::EntryMatcher("OPMODERATEDBAN", extban.name, extban.letter);
				else if (extban.name.equals_cs("realname"))
					xb = new InspIRCdExtBan::RealnameMatcher("REALNAMEBAN", extban.name, extban.letter);
				else if (extban.name.equals_cs("server"))
					xb = new InspIRCdExtBan::ServerMatcher("SERVERBAN", extban.name, extban.letter);
				else if (extban.name.equals_cs("stripcolor"))
					xb = new InspIRCdExtBan::EntryMatcher("STRIPCOLORBAN", extban.name, extban.letter);
				else if (extban.name.equals_cs("unauthed"))
					xb = new InspIRCdExtBan::UnidentifiedMatcher("UNREGISTEREDBAN", extban.name, extban.letter);

				// Handle unknown extbans.
				else if (extban.type.equals_cs("acting"))
					xb = new InspIRCdExtBan::EntryMatcher(extban.name.upper() + "BAN", extban.name, extban.letter);
				else
					Log(LOG_DEBUG) << "Unknown extban: " << capab;

				if (xb)
					ModeManager::AddChannelMode(xb);
			}
		}

		else if ((params[0].equals_cs("MODULES") || params[0].equals_cs("MODSUPPORT")) && params.size() > 1)
		{
			spacesepstream ssep(params[1]);
			Anope::string module;

			Anope::string inspircdregex;
			while (ssep.GetToken(module))
			{
				Anope::string modname, moddata;
				ParseModule(module, modname, moddata);

				if (spanningtree_proto_ver >= 1206)
				{
					// InspIRCd v4
					Anope::map<Anope::string> modmap;
					ParseModuleData(moddata, modmap);

					if (modname.equals_cs("account"))
						Servers::Capab.insert("ACCOUNT");

					else if (modname.equals_cs("cban"))
						IRCD->CanSQLineChannel = true;

					else if (modname.equals_cs("globops"))
						Servers::Capab.insert("GLOBOPS");

					else if (modname.equals_cs("rline"))
					{
						Servers::Capab.insert("RLINE");
						auto iter = modmap.find("regex");
						if (iter != modmap.end())
							inspircdregex = "regex/" + iter->second;
					}

					else if (modname.equals_cs("services"))
					{
						IRCD->CanClearBans = true;
						IRCD->CanSVSHold = true;
						Servers::Capab.insert("SERVICES");
						Servers::Capab.insert("TOPICLOCK");
					}
				}
				else
				{
					// InspIRCd v3
					if (modname.equals_cs("cban") && moddata.equals_cs("glob"))
						IRCD->CanSQLineChannel = true;

					else if (modname.equals_cs("channelban"))
						ModeManager::AddChannelMode(new InspIRCdExtBan::ChannelMatcher("CHANNELBAN", "", 'j'));

					else if (modname.equals_cs("gecosban"))
						ModeManager::AddChannelMode(new InspIRCdExtBan::RealnameMatcher("REALNAMEBAN", "", 'r'));

					else if (modname.equals_cs("muteban"))
						ModeManager::AddChannelMode(new InspIRCdExtBan::EntryMatcher("QUIET", "", 'm'));

					else if (modname.equals_cs("nopartmsg"))
						ModeManager::AddChannelMode(new InspIRCdExtBan::EntryMatcher("PARTMESSAGEBAN", "", 'p'));

					else if (modname.equals_cs("rline"))
					{
						Servers::Capab.insert("RLINE");
						inspircdregex = moddata;
					}

					else if (modname.equals_cs("serverban"))
						ModeManager::AddChannelMode(new InspIRCdExtBan::ServerMatcher("SERVERBAN", "", 's'));

					else if (modname.equals_cs("services_account"))
					{
						Servers::Capab.insert("ACCOUNT");
						Servers::Capab.insert("SERVICES");
						ModeManager::AddChannelMode(new InspIRCdExtBan::AccountMatcher("ACCOUNTBAN", "", 'R'));
						ModeManager::AddChannelMode(new InspIRCdExtBan::UnidentifiedMatcher("UNREGISTEREDBAN", "", 'U'));
					}

					else if (modname.equals_cs("svshold"))
						IRCD->CanSVSHold = true;

					else if (modname.equals_cs("topiclock"))
						Servers::Capab.insert("TOPICLOCK");
				}

				// InspIRCd v3 and v4
				if (modname.equals_cs("chghost"))
					Servers::Capab.insert("CHGHOST");

				else if (modname.equals_cs("chgident"))
					Servers::Capab.insert("CHGIDENT");
			}

			const auto &anoperegex = Config->GetBlock("options")->Get<const Anope::string>("regexengine");
			if (!anoperegex.empty() && !inspircdregex.empty() && anoperegex != inspircdregex)
				Log() << "Warning: InspIRCd is using regex engine " << inspircdregex << ", but we have " << anoperegex << ". This may cause inconsistencies.";
		}
		else if (params[0].equals_cs("CAPABILITIES") && params.size() > 1)
		{
			spacesepstream ssep(params[1]);
			Anope::string capab;
			while (ssep.GetToken(capab))
			{
				auto [tokname, tokvalue] = ParseCapability(capab);
				if (tokname == "CHALLENGE")
					challenge = tokvalue;
				else if (tokname == "MAXCHANNEL")
					IRCD->MaxChannel = Anope::Convert<size_t>(tokvalue, IRCD->MaxChannel);
				else if (tokname == "MAXHOST")
					IRCD->MaxHost = Anope::Convert<size_t>(tokvalue, IRCD->MaxHost);
				else if (tokname == "MAXNICK")
					IRCD->MaxNick = Anope::Convert<size_t>(tokvalue, IRCD->MaxNick);
				else if (tokname == "MAXUSER")
					IRCD->MaxUser = Anope::Convert<size_t>(tokvalue, IRCD->MaxUser);

				// Deprecated 1205 keys.
				else if (tokname == "CHANMAX")
					IRCD->MaxChannel = Anope::Convert<size_t>(tokvalue, IRCD->MaxChannel);
				else if (tokname == "GLOBOPS" && Anope::Convert<bool>(tokvalue, false))
					Servers::Capab.insert("GLOBOPS");
				else if (tokname == "IDENTMAX")
					IRCD->MaxUser = Anope::Convert<size_t>(tokvalue, IRCD->MaxUser);
				else if (tokname == "NICKMAX")
					IRCD->MaxNick = Anope::Convert<size_t>(tokvalue, IRCD->MaxNick);
			}
		}
		else if (params[0].equals_cs("END"))
		{
			if (spanningtree_proto_ver < 1206)
			{
				if (!Servers::Capab.count("ACCOUNT") || !Servers::Capab.count("SERVICES"))
					throw ProtocolException("The services_account module is not loaded. This is required by Anope.");
			}
			else
			{
				if (!Servers::Capab.count("ACCOUNT"))
					throw ProtocolException("The account module is not loaded. This is required by Anope.");

				if (!Servers::Capab.count("SERVICES"))
					throw ProtocolException("The services module is not loaded. This is required by Anope.");
			}

			if (!ModeManager::FindUserModeByName("PRIV"))
				throw ProtocolException("The hidechans module is not loaded. This is required by Anope.");

			if (!IRCD->CanSVSHold)
				Log() << "The remote server does not have the svshold module; fake users will be used for nick protection until the module is loaded.";

			if (!IRCD->CanSQLineChannel)
				Log() << "The remote server does not have the cban module; services will manually enforce forbidden channels until the module is loaded.";

			if (!Servers::Capab.count("CHGHOST"))
				Log() << "The remote server does not have the chghost module; vhosts are disabled until the module is loaded.";

			if (!Servers::Capab.count("CHGIDENT"))
				Log() << "The remote server does not have the chgident module; vidents are disabled until the module is loaded.";

			if (!Servers::Capab.count("GLOBOPS"))
				Log() << "The remote server does not have the globops module; oper notices will be sent as announcements until the module is loaded.";

			if (spanningtree_proto_ver < 1206)
				Uplink::Send("SERVER", Me->GetName(), GetPassword(), 0, Me->GetSID(), Me->GetDescription());
			else
				Uplink::Send("SERVER", Me->GetName(), GetPassword(), Me->GetSID(), Me->GetDescription());
		}
	}
};

struct IRCDMessageChgHost final
	: IRCDMessage
{
	IRCDMessageChgHost(Module *creator)
		: IRCDMessage(creator, "CHGHOST", 2)
	{
		SetFlag(FLAG_REQUIRE_USER);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		auto *u = User::Find(params[0]);
		if (!u || u->server != Me)
			return;

		u->SetDisplayedHost(params[1]);
		if (spanningtree_proto_ver >= 1206)
			Uplink::Send(u, "FHOST", u->GetDisplayedHost(), '*');
		else
			Uplink::Send(u, "FHOST", u->GetDisplayedHost());
	}
};

struct IRCDMessageChgIdent final
	: IRCDMessage
{
	IRCDMessageChgIdent(Module *creator)
		: IRCDMessage(creator, "CHGIDENT", 2)
	{
		SetFlag(FLAG_REQUIRE_USER);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		auto *u = User::Find(params[0]);
		if (!u || u->server != Me)
			return;

		u->SetIdent(params[1]);
		if (spanningtree_proto_ver >= 1206)
			Uplink::Send(u, "FIDENT", u->GetIdent(), '*');
		else
			Uplink::Send(u, "FIDENT", u->GetIdent());
	}
};

struct IRCDMessageChgName final
	: IRCDMessage
{
	IRCDMessageChgName(Module *creator)
		: IRCDMessage(creator, "CHGNAME", 2)
	{
		SetFlag(FLAG_REQUIRE_USER);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		auto *u = User::Find(params[0]);
		if (!u || u->server != Me)
			return;

		u->SetRealname(params[1]);
		Uplink::Send(u, "FNAME", u->realname);
	}
};

struct IRCDMessageEncap final
	: IRCDMessage
{
	IRCDMessageEncap(Module *creator) : IRCDMessage(creator, "ENCAP", 2)
	{
		SetFlag(FLAG_SOFT_LIMIT);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		if (!Anope::Match(Me->GetSID(), params[0]) && !Anope::Match(Me->GetName(), params[0]))
			return;

		std::vector<Anope::string> newparams(params.begin() + 2, params.end());
		Anope::ProcessInternal(source, params[1], newparams, tags);
	}
};

struct IRCDMessageFHost final
	: IRCDMessage
{
	IRCDMessageFHost(Module *creator)
		: IRCDMessage(creator, "FHOST", 1)
	{
		SetFlag(FLAG_REQUIRE_USER);
		SetFlag(FLAG_SOFT_LIMIT);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		User *u = source.GetUser();
		if (params[0] != "*")
		{
			if (u->HasMode("CLOAK"))
				u->RemoveModeInternal(source, ModeManager::FindUserModeByName("CLOAK"));
			u->SetDisplayedHost(params[0]);
		}

		if (params.size() > 1 && params[1] != "*")
			u->host = params[1];
	}
};

struct IRCDMessageFIdent final
	: IRCDMessage
{
	IRCDMessageFIdent(Module *creator)
		: IRCDMessage(creator, "FIDENT", 1)
	{
		SetFlag(FLAG_REQUIRE_USER);
		SetFlag(FLAG_SOFT_LIMIT);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		User *u = source.GetUser();
		if (params[0] != "*")
			u->SetDisplayedHost(params[0]);
	}
};

struct IRCDMessageKick final
	: IRCDMessage
{
	IRCDMessageKick(Module *creator) : IRCDMessage(creator, "KICK", 3) { SetFlag(FLAG_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		// Received: :715AAAAAA KICK #chan 715AAAAAD :reason
		// Received: :715AAAAAA KICK #chan 628AAAAAA 4 :reason
		Channel *c = Channel::Find(params[0]);
		if (!c)
			return;

		const Anope::string &reason = params.size() > 3 ? params[3] : params[2];
		c->KickInternal(source, params[1], reason);
	}
};

struct IRCDMessageSASL final
	: IRCDMessage
{
	IRCDMessageSASL(Module *creator)
		: IRCDMessage(creator, "SASL", 4)
	{
		SetFlag(FLAG_SOFT_LIMIT);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		if (!SASL::sasl)
			return;

		SASL::Message m;
		m.source = params[0];
		m.target = params[1];
		m.type = params[2];
		m.data = params[3];
		m.ext = params.size() > 4 ? params[4] : "";
		SASL::sasl->ProcessMessage(m);
	}
};

struct IRCDMessageSave final
	: IRCDMessage
{
	time_t last_collide = 0;

	IRCDMessageSave(Module *creator) : IRCDMessage(creator, "SAVE", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		User *targ = User::Find(params[0]);
		auto ts = IRCD->ExtractTimestamp(params[1]);
		if (!targ || !ts || targ->timestamp != ts)
			return;

		BotInfo *bi;
		if (targ->server == Me && (bi = dynamic_cast<BotInfo *>(targ)))
		{
			if (last_collide == Anope::CurTime)
			{
				Anope::QuitReason = "Nick collision fight on " + targ->nick;
				Anope::Quitting = true;
				return;
			}

			IRCD->SendKill(Me, targ->nick, "Nick collision");
			IRCD->SendNickChange(targ, targ->nick);
			last_collide = Anope::CurTime;
		}
		else
			targ->ChangeNick(targ->GetUID());
	}
};

class IRCDMessageMetadata final
	: IRCDMessage
{
	const bool &do_topiclock;
	const bool &do_mlock;
	PrimitiveExtensibleItem<ListLimits> &maxlist;

public:
	IRCDMessageMetadata(Module *creator, const bool &handle_topiclock, const bool &handle_mlock, PrimitiveExtensibleItem<ListLimits> &listlimits) : IRCDMessage(creator, "METADATA", 3), do_topiclock(handle_topiclock), do_mlock(handle_mlock), maxlist(listlimits) { SetFlag(FLAG_REQUIRE_SERVER); SetFlag(FLAG_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		// We deliberately ignore non-bursting servers to avoid pseudoserver fights
		// Channel METADATA has an additional parameter: the channel TS
		// Received: :715 METADATA #chan 1572026333 mlock :nt
		if ((params[0][0] == '#') && (params.size() > 3) && (!source.GetServer()->IsSynced()))
		{
			Channel *c = Channel::Find(params[0]);
			if (c)
			{
				if ((c->ci) && (do_mlock) && (params[2] == "mlock"))
				{
					ModeLocks *modelocks = c->ci->GetExt<ModeLocks>("modelocks");
					Anope::string modes;
					if (modelocks)
						modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "");

					// Mode lock string is not what we say it is?
					if (modes != params[3])
						Uplink::Send("METADATA", c->name, c->creation_time, "mlock", modes);
				}
				else if ((c->ci) && (do_topiclock) && (params[2] == "topiclock"))
				{
					bool mystate = c->ci->HasExt("TOPICLOCK");
					bool serverstate = (params[3] == "1");
					if (mystate != serverstate)
						Uplink::Send("METADATA", c->name, c->creation_time, "topiclock", !!mystate);
				}
				else if (params[2] == "maxlist")
				{
					ListLimits limits;
					spacesepstream limitstream(params[3]);
					Anope::string modechr, modelimit;
					while (limitstream.GetToken(modechr) && limitstream.GetToken(modelimit))
					{
						limits.emplace(modechr[0], Anope::Convert<unsigned>(modelimit, 0));
					}
					maxlist.Set(c, limits);
				}
			}
		}
		else if (isdigit(params[0][0]))
		{
			auto *u = User::Find(params[0]);
			if (!u)
				return;

			if (params[1].equals_cs("accountname"))
			{
				if (params[2].empty())
				{
					// The user has been logged out by the IRC server.
					u->Logout();
				}
				else
				{
					// If we're bursting then then the user was probably logged
					// in during a previous connection.
					NickCore *nc = NickCore::Find(params[2]);
					if (nc)
						u->Login(nc);
				}
			}

			/*
			 *   possible incoming ssl_cert messages:
			 *   Received: :409 METADATA 409AAAAAA ssl_cert :vTrSe c38070ce96e41cc144ed6590a68d45a6 <...> <...>
			 *   Received: :409 METADATA 409AAAAAC ssl_cert :vTrSE Could not get peer certificate: error:00000000:lib(0):func(0):reason(0)
			 */
			else if (params[1].equals_cs("ssl_cert"))
			{
				u->Extend<bool>("ssl");
				Anope::string data = params[2].c_str();
				size_t pos1 = data.find(' ') + 1;
				size_t pos2 = data.find(' ', pos1);
				if ((pos2 - pos1) >= 32) // inspircd supports md5 and sha1 fingerprint hashes -> size 32 or 40 bytes.
				{
					u->fingerprint = data.substr(pos1, pos2 - pos1);
				}
				FOREACH_MOD(OnFingerprint, (u));
			}
		}
		else if (params[0] == "*")
		{
			// Wed Oct  3 15:40:27 2012: S[14] O :20D METADATA * modules :-m_svstopic.so

			if (params[1].equals_cs("modules") && !params[2].empty())
			{
				// only interested when it comes from our uplink
				Server *server = source.GetServer();
				if (!server || server->GetUplink() != Me)
					return;

				bool plus = (params[2][0] == '+');
				if (!plus && params[2][0] != '-')
					return;

				bool required = false;
				Anope::string capab, modname, moddata;
				ParseModule(params[2].substr(1), modname, moddata);

				if (modname.equals_cs("account"))
					required = true;

				else if (modname.equals_cs("cban"))
				{
					if (plus && (spanningtree_proto_ver >= 1206 || moddata == "glob"))
						IRCD->CanSQLineChannel = true;
					else
						IRCD->CanSQLineChannel = false;
				}

				else if (modname.equals_cs("cban") && spanningtree_proto_ver >= 1206)
					IRCD->CanSQLineChannel = plus;

				else if (modname.equals_cs("chghost"))
					capab = "CHGHOST";

				else if (modname.equals_cs("chgident"))
					capab = "CHGIDENT";

				else if (modname.equals_cs("globops"))
					capab = "GLOBOPS";

				else if (modname.equals_cs("hidechans"))
					required = true;

				else if (modname.equals_cs("rline"))
					capab = "RLINE";

				else if (modname.equals_cs("services"))
					required = true;

				// Deprecated 1205 modules.
				else if (modname.equals_cs("services_account"))
					required = true;

				else if (modname.equals_cs("svshold"))
					IRCD->CanSVSHold = plus;

				else if (modname.equals_cs("topiclock"))
					capab = "TOPICLOCK";

				else
					return;

				if (required)
				{
					if (plus)
						Log() << "Warning: InspIRCd unloaded the " << modname << " module. Anope won't function correctly without it.";
				}
				else
				{
					if (plus && !capab.empty())
						Servers::Capab.insert(capab);
					else if (!capab.empty())
						Servers::Capab.erase(capab);

					Log() << "InspIRCd " << (plus ? "loaded" : "unloaded") << " the " << modname << " module; adjusted functionality.";
				}

			}
		}
	}
};

struct IRCDMessageEndburst final
	: IRCDMessage
{
	IRCDMessageEndburst(Module *creator) : IRCDMessage(creator, "ENDBURST", 0) { SetFlag(FLAG_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		Server *s = source.GetServer();

		Log(LOG_DEBUG) << "Processed ENDBURST for " << s->GetName();

		s->Sync(true);
	}
};

struct IRCDMessageFJoin final
	: IRCDMessage
{
	IRCDMessageFJoin(Module *creator) : IRCDMessage(creator, "FJOIN", 2) { SetFlag(FLAG_REQUIRE_SERVER); SetFlag(FLAG_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		Anope::string modes;
		if (params.size() >= 3)
		{
			for (unsigned i = 2; i < params.size() - 1; ++i)
				modes += " " + params[i];
			if (!modes.empty())
				modes.erase(modes.begin());
		}

		std::list<Message::Join::SJoinUser> users;

		spacesepstream sep(params[params.size() - 1]);
		Anope::string buf;
		while (sep.GetToken(buf))
		{
			Message::Join::SJoinUser sju;

			/* Loop through prefixes and find modes for them */
			for (char c; (c = buf[0]) != ',' && c;)
			{
				buf.erase(buf.begin());
				sju.first.AddMode(c);
			}
			/* Erase the , */
			if (!buf.empty())
				buf.erase(buf.begin());

			/* Erase the :membid */
			if (!buf.empty())
			{
				Anope::string::size_type membid = buf.find(':');
				if (membid != Anope::string::npos)
					buf.erase(membid, Anope::string::npos);
			}

			sju.second = User::Find(buf);
			if (!sju.second)
			{
				Log(LOG_DEBUG) << "FJOIN for nonexistent user " << buf << " on " << params[0];
				continue;
			}

			users.push_back(sju);
		}

		auto ts = IRCD->ExtractTimestamp(params[1]);
		Message::Join::SJoin(source, params[0], ts, modes, users);
	}
};

struct IRCDMessageFMode final
	: IRCDMessage
{
	IRCDMessageFMode(Module *creator) : IRCDMessage(creator, "FMODE", 3) { SetFlag(FLAG_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		/* :source FMODE #test 12345678 +nto foo */

		Anope::string modes = params[2];
		for (unsigned n = 3; n < params.size(); ++n)
			modes += " " + params[n];

		Channel *c = Channel::Find(params[0]);
		auto ts = IRCD->ExtractTimestamp(params[1]);
		if (c)
			c->SetModesInternal(source, modes, ts);
	}
};

struct IRCDMessageFTopic final
	: IRCDMessage
{
	IRCDMessageFTopic(Module *creator) : IRCDMessage(creator, "FTOPIC", 4) { SetFlag(FLAG_SOFT_LIMIT);  }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		// :source FTOPIC channel ts topicts :topic
		// :source FTOPIC channel ts topicts setby :topic (burst or RESYNC)

		auto ts = IRCD->ExtractTimestamp(params[2]);
		const Anope::string &setby = params.size() > 4 ? params[3] : source.GetName();
		const Anope::string &topic = params.size() > 4 ? params[4] : params[3];

		Channel *c = Channel::Find(params[0]);
		if (c)
			c->ChangeTopicInternal(NULL, setby, topic, ts);
	}
};

struct IRCDMessageIdle final
	: IRCDMessage
{
	IRCDMessageIdle(Module *creator) : IRCDMessage(creator, "IDLE", 1) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		BotInfo *bi = BotInfo::Find(params[0]);
		if (bi)
			Uplink::Send(bi, "IDLE", source.GetSource(), Anope::StartTime, Anope::CurTime - bi->lastmsg);
		else
		{
			User *u = User::Find(params[0]);
			if (u && u->server == Me)
				Uplink::Send(u, "IDLE", source.GetSource(), Anope::StartTime, 0);
		}
	}
};

struct IRCDMessageIJoin final
	: IRCDMessage
{
	IRCDMessageIJoin(Module *creator) : IRCDMessage(creator, "IJOIN", 2) { SetFlag(FLAG_REQUIRE_USER); SetFlag(FLAG_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		// :<uid> IJOIN <chan> <membid> [<ts> [<flags>]]
		Channel *c = Channel::Find(params[0]);
		if (!c)
		{
			// When receiving an IJOIN, first check if the target channel exists. If it does not exist,
			// ignore the join (that is, do not create the channel) and send a RESYNC back to the source.
			Uplink::Send("RESYNC", params[0]);
			return;
		}

		// If the channel does exist then join the user, and in case any prefix modes were sent (found in
		// the 3rd parameter), compare the TS of the channel to the TS in the IJOIN (2nd parameter). If
		// the timestamps match, set the modes on the user, otherwise ignore the modes.
		Message::Join::SJoinUser user;
		user.second = source.GetUser();

		time_t chants = Anope::CurTime;
		if (params.size() >= 4)
		{
			chants = IRCD->ExtractTimestamp(params[2]);
			for (auto mode : params[3])
				user.first.AddMode(mode);
		}

		std::list<Message::Join::SJoinUser> users;
		users.push_back(user);
		Message::Join::SJoin(source, params[0], chants, "", users);
	}
};

struct IRCDMessageLMode final
	: IRCDMessage
{
	IRCDMessageLMode(Module *creator)
		: IRCDMessage(creator, "LMODE", 3)
	{
		SetFlag(FLAG_SOFT_LIMIT);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		// :<sid> LMODE <chan> <chants> <modechr> [<mask> <setts> <setter>]+
		auto *chan = Channel::Find(params[0]);
		if (!chan)
			return; // Channel doesn't exist.

		// If the TS is greater than ours, we drop the mode and don't pass it anywhere.
		auto chants = IRCD->ExtractTimestamp(params[1]);
		if (chants > chan->creation_time)
			return;

		auto *cm = ModeManager::FindChannelModeByChar(params[2][0]);
		if (!cm || cm->type != MODE_LIST)
			return; // Mode doesn't exist or isn't a list mode.

		if (params.size() % 3)
			return; // Invalid parameter count.

		for (auto it = params.begin() + 3; it != params.end(); it += 3)
		{
			// TODO: Anope doesn't store set time and setter for list modes yet.
			chan->SetModeInternal(source, cm, *it);
		}
	}
};


struct IRCDMessageMode final
	: IRCDMessage
{
	IRCDMessageMode(Module *creator) : IRCDMessage(creator, "MODE", 2) { SetFlag(FLAG_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		if (IRCD->IsChannelValid(params[0]))
		{
			Channel *c = Channel::Find(params[0]);

			Anope::string modes = params[1];
			for (unsigned n = 2; n < params.size(); ++n)
				modes += " " + params[n];

			if (c)
				c->SetModesInternal(source, modes);
		}
		else
		{
			/* InspIRCd lets opers change another
			   users modes, we have to kludge this
			   as it slightly breaks RFC1459
			 */
			User *u = User::Find(params[0]);
			if (u)
				u->SetModesInternal(source, params[1]);
		}
	}
};

struct IRCDMessageNick final
	: IRCDMessage
{
	IRCDMessageNick(Module *creator) : IRCDMessage(creator, "NICK", 2) { SetFlag(FLAG_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		source.GetUser()->ChangeNick(params[0]);
	}
};

struct IRCDMessageOperType final
	: IRCDMessage
{
	PrimitiveExtensibleItem<Anope::string> opertype;

	IRCDMessageOperType(Module *creator)
		: IRCDMessage(creator, "OPERTYPE", 1)
		, opertype(creator, "opertype")
	{
		SetFlag(FLAG_REQUIRE_USER);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		/* opertype is equivalent to mode +o because servers
		   don't do this directly */
		User *u = source.GetUser();
		if (!u->HasMode("OPER"))
			u->SetModesInternal(source, "+o");

		opertype.Set(u, params[0]);
	}
};

struct IRCDMessagePing final
	: IRCDMessage
{
	IRCDMessagePing(Module *creator) : IRCDMessage(creator, "PING", 1) { SetFlag(FLAG_SOFT_LIMIT); SetFlag(FLAG_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		if (params[0] == Me->GetSID())
			IRCD->SendPong(params[0], source.GetServer()->GetSID());
	}
};

struct IRCDMessageRSQuit final
	: IRCDMessage
{
	IRCDMessageRSQuit(Module *creator) : IRCDMessage(creator, "RSQUIT", 1) { SetFlag(FLAG_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		Server *s = Server::Find(params[0]);
		const Anope::string &reason = params.size() > 1 ? params[1] : "";
		if (!s)
			return;

		Uplink::Send("SQUIT", s->GetSID(), reason);
		s->Delete(s->GetName() + " " + s->GetUplink()->GetName());
	}
};

struct IRCDMessageServer final
	: IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(FLAG_REQUIRE_SERVER); SetFlag(FLAG_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		size_t paramcount = spanningtree_proto_ver < 1206 ? 5 : 4;
		if (!source.GetServer() && params.size() == paramcount)
		{
			/*
			 * SERVER testnet.inspircd.org hunter7 0 123 :InspIRCd Test Network
			 * 0: name
			 * 1: pass
			 * 2: unused (v3 only)
			 * 3(2): numeric
			 * 4(3): desc
			 */
			new Server(Me, params[0], 0, params.back(), params[spanningtree_proto_ver < 1206 ? 3 : 2]);
		}
		else if (source.GetServer())
		{
			/*
			 * SERVER testnet.inspircd.org 123 burst=1234 hidden=0 :InspIRCd Test Network
			 * 0: name
			 * 1: numeric
			 * 2 to N-1: various key=value pairs.
			 * N: desc
			 */
			new Server(source.GetServer(), params[0], 1, params.back(), params[1]);
		}
	}
};

struct IRCDMessageSQuit final
	: Message::SQuit
{
	IRCDMessageSQuit(Module *creator) : Message::SQuit(creator) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		if (params[0] == rsquit_id || params[0] == rsquit_server)
		{
			/* squit for a recently squit server, introduce the juped server now */
			Server *s = Server::Find(rsquit_server);

			rsquit_id.clear();
			rsquit_server.clear();

			if (s && s->IsJuped())
				IRCD->SendServer(s);
		}
		else
			Message::SQuit::Run(source, params, tags);
	}
};

struct IRCDMessageUID final
	: IRCDMessage
{
	IRCDMessageUID(Module *creator) : IRCDMessage(creator, "UID", 8) { SetFlag(FLAG_REQUIRE_SERVER); SetFlag(FLAG_SOFT_LIMIT); }

	/*
	 * [Nov 03 22:09:58.176252 2009] debug: Received: :964 UID 964AAAAAC 1225746297 w00t2 localhost testnet.user w00t 127.0.0.1 1225746302 +iosw +ACGJKLNOQcdfgjklnoqtx :Robin Burchell <w00t@inspircd.org>
	 * 0: uid
	 * 1: ts
	 * 2: nick
	 * 3: host
	 * 4: dhost
	 * 5: ident
	 * 6: dident (v4 only)
	 * 7: ip
	 * 8: signon
	 * 9+: modes and params -- IMPORTANT, some modes (e.g. +s) may have parameters. So don't assume a fixed position of realname!
	 * last: realname
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		size_t offset = params[8][0] == '+' ? 0 : 1;
		auto ts = IRCD->ExtractTimestamp(params[1]);

		Anope::string modes = params[8+offset];
		for (unsigned i = 9+offset; i < params.size() - 1; ++i)
			modes += " " + params[i];

		NickAlias *na = NULL;
		if (SASL::sasl)
			for (std::list<SASLUser>::iterator it = saslusers.begin(); it != saslusers.end();)
			{
				SASLUser &u = *it;

				if (u.created + 30 < Anope::CurTime)
					it = saslusers.erase(it);
				else if (u.uid == params[0])
				{
					na = NickAlias::Find(u.acc);
					it = saslusers.erase(it);
				}
				else
					++it;
			}

		User *u = User::OnIntroduce(params[2], params[5+offset], params[3], params[4], params[6+offset], source.GetServer(), params[params.size() - 1], ts, modes, params[0], na ? *na->nc : NULL);
		if (u)
			u->signon = IRCD->ExtractTimestamp(params[7+offset]);
	}
};

class ProtoInspIRCd final
	: public Module
{
	InspIRCdProto ircd_proto;
	ExtensibleItem<bool> ssl;

	/* Core message handlers */
	Message::Error message_error;
	Message::Invite message_invite;
	Message::Kill message_kill;
	Message::MOTD message_motd;
	Message::Notice message_notice;
	Message::Part message_part;
	Message::Privmsg message_privmsg;
	Message::Quit message_quit;
	Message::Privmsg message_squery;
	Message::Stats message_stats;
	Message::Time message_time;

	/* Our message handlers */
	IRCDMessageAway message_away;
	IRCDMessageCapab message_capab;
	IRCDMessageChgHost message_chghost;
	IRCDMessageChgIdent message_chgident;
	IRCDMessageChgName message_chgname;
	IRCDMessageEncap message_encap;
	IRCDMessageEndburst message_endburst;
	IRCDMessageFHost message_fhost;
	IRCDMessageFIdent message_fident;
	IRCDMessageFJoin message_fjoin;
	IRCDMessageFMode message_fmode;
	IRCDMessageFTopic message_ftopic;
	IRCDMessageIdle message_idle;
	IRCDMessageIJoin message_ijoin;
	IRCDMessageKick message_kick;
	IRCDMessageLMode message_lmode;
	IRCDMessageMetadata message_metadata;
	IRCDMessageMode message_mode;
	IRCDMessageNick message_nick;
	IRCDMessageOperType message_opertype;
	IRCDMessagePing message_ping;
	IRCDMessageRSQuit message_rsquit;
	IRCDMessageSASL message_sasl;
	IRCDMessageSave message_save;
	IRCDMessageServer message_server;
	IRCDMessageSQuit message_squit;
	IRCDMessageUID message_uid;

	bool use_server_side_topiclock, use_server_side_mlock;

	static void SendChannelMetadata(Channel *c, const Anope::string &metadataname, const Anope::string &value)
	{
		Uplink::Send("METADATA", c->name, c->creation_time, metadataname, value);
	}

public:
	ProtoInspIRCd(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, PROTOCOL | VENDOR)
		, ircd_proto(this), ssl(this, "ssl")
		, message_error(this)
		, message_invite(this)
		, message_kill(this)
		, message_motd(this)
		, message_notice(this)
		, message_part(this)
		, message_privmsg(this)
		, message_quit(this)
		, message_squery(this, "SQUERY")
		, message_stats(this)
		, message_time(this)
		, message_away(this)
		, message_capab(this)
		, message_chghost(this)
		, message_chgident(this)
		, message_chgname(this)
		, message_encap(this)
		, message_endburst(this)
		, message_fhost(this)
		, message_fident(this)
		, message_fjoin(this)
		, message_fmode(this)
		, message_ftopic(this)
		, message_idle(this)
		, message_ijoin(this)
		, message_kick(this)
		, message_lmode(this)
		, message_metadata(this, use_server_side_topiclock, use_server_side_mlock, ircd_proto.maxlist)
		, message_mode(this)
		, message_nick(this)
		, message_opertype(this)
		, message_ping(this)
		, message_rsquit(this)
		, message_sasl(this)
		, message_save(this)
		, message_server(this)
		, message_squit(this)
		, message_uid(this)
	{
	}

	void OnReload(Configuration::Conf *conf) override
	{
		use_server_side_topiclock = conf->GetModule(this)->Get<bool>("use_server_side_topiclock");
		use_server_side_mlock = conf->GetModule(this)->Get<bool>("use_server_side_mlock");
	}

	void OnUserNickChange(User *u, const Anope::string &) override
	{
		u->RemoveModeInternal(Me, ModeManager::FindUserModeByName("REGISTERED"));
	}

	void OnChannelSync(Channel *c) override
	{
		if (c->ci)
			this->OnChanRegistered(c->ci);
	}

	void OnChanRegistered(ChannelInfo *ci) override
	{
		ModeLocks *modelocks = ci->GetExt<ModeLocks>("modelocks");
		if (use_server_side_mlock && ci->c && modelocks && !modelocks->GetMLockAsString(false).empty())
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "");
			SendChannelMetadata(ci->c, "mlock", modes);
		}

		if (use_server_side_topiclock && Servers::Capab.count("TOPICLOCK") && ci->c)
		{
			if (ci->HasExt("TOPICLOCK"))
				SendChannelMetadata(ci->c, "topiclock", "1");
		}
	}

	void OnDelChan(ChannelInfo *ci) override
	{
		if (use_server_side_mlock && ci->c)
			SendChannelMetadata(ci->c, "mlock", "");

		if (use_server_side_topiclock && Servers::Capab.count("TOPICLOCK") && ci->c)
			SendChannelMetadata(ci->c, "topiclock", "");
	}

	EventReturn OnMLock(ChannelInfo *ci, ModeLock *lock) override
	{
		ModeLocks *modelocks = ci->GetExt<ModeLocks>("modelocks");
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (use_server_side_mlock && cm && ci->c && modelocks && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM))
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "") + cm->mchar;
			SendChannelMetadata(ci->c, "mlock", modes);
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChannelInfo *ci, ModeLock *lock) override
	{
		ModeLocks *modelocks = ci->GetExt<ModeLocks>("modelocks");
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (use_server_side_mlock && cm && ci->c && modelocks && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM))
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "").replace_all_cs(cm->mchar, "");
			SendChannelMetadata(ci->c, "mlock", modes);
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnSetChannelOption(CommandSource &source, Command *cmd, ChannelInfo *ci, const Anope::string &setting) override
	{
		if (cmd->name == "chanserv/topic" && ci->c)
		{
			if (setting == "topiclock on")
				SendChannelMetadata(ci->c, "topiclock", "1");
			else if (setting == "topiclock off")
				SendChannelMetadata(ci->c, "topiclock", "0");
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ProtoInspIRCd)
