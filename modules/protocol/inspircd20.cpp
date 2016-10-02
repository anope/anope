/*
 * Anope IRC Services
 *
 * Copyright (C) 2005-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"
#include "modules/sasl.h"
#include "modules/chanserv/mode.h"
#include "modules/chanserv/set.h"

struct SASLUser
{
	Anope::string uid;
	Anope::string acc;
	time_t created;
};

static std::list<SASLUser> saslusers;
static Anope::string rsquit_server, rsquit_id;
static unsigned int spanningtree_proto_ver = 0;

class InspIRCd20Proto : public IRCDProto
{
 private:
	void SendSVSKillInternal(const MessageSource &source, User *user, const Anope::string &buf) override
	{
		IRCDProto::SendSVSKillInternal(source, user, buf);
		user->KillInternal(source, buf);
	}

	void SendChgIdentInternal(const Anope::string &nick, const Anope::string &vIdent)
	{
		if (!Servers::Capab.count("CHGIDENT"))
			Log() << "CHGIDENT not loaded!";
		else
			Uplink::Send(Me, "CHGIDENT", nick, vIdent);
	}

	void SendChgHostInternal(const Anope::string &nick, const Anope::string &vhost)
	{
		if (!Servers::Capab.count("CHGHOST"))
			Log() << "CHGHOST not loaded!";
		else
			Uplink::Send(Me, "CHGHOST", nick, vhost);
	}

	void SendAddLine(const Anope::string &xtype, const Anope::string &mask, time_t duration, const Anope::string &addedby, const Anope::string &reason)
	{
		Uplink::Send(Me, "ADDLINE", xtype, mask, addedby, Anope::CurTime, duration, reason);
	}

	void SendDelLine(const Anope::string &xtype, const Anope::string &mask)
	{
		Uplink::Send(Me, "DELLINE", xtype, mask);
	}

 public:
	InspIRCd20Proto(Module *creator) : IRCDProto(creator, "InspIRCd 2.0")
	{
		DefaultPseudoclientModes = "+I";
		CanSVSNick = true;
		CanSVSJoin = true;
		CanSetVHost = true;
		CanSetVIdent = true;
		CanSQLine = true;
		CanSZLine = true;
		CanSVSHold = true;
		CanCertFP = true;
		RequiresID = true;
		MaxModes = 20;
	}

	void SendConnect() override
	{
		Uplink::Send("CAPAB START 1202");
		Uplink::Send("CAPAB CAPABILITIES :PROTOCOL=1202");
		Uplink::Send("CAPAB END");
		SendServer(Me);
	}

	void SendGlobalNotice(ServiceBot *bi, const Server *dest, const Anope::string &msg) override
	{
		Uplink::Send(bi, "NOTICE", "$" + dest->GetName(), msg);
	}

	void SendGlobalPrivmsg(ServiceBot *bi, const Server *dest, const Anope::string &msg) override
	{
		Uplink::Send(bi, "PRIVMSG", "$" + dest->GetName(), msg);
	}

	void SendAkillDel(XLine *x) override
	{
		/* InspIRCd may support regex bans */
		if (x->IsRegex() && Servers::Capab.count("RLINE"))
		{
			Anope::string mask = x->GetMask();
			size_t h = mask.find('#');
			if (h != Anope::string::npos)
				mask = mask.replace(h, 1, ' ');
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

	void SendTopic(const MessageSource &source, Channel *c) override
	{
		if (Servers::Capab.count("SVSTOPIC"))
		{
			Uplink::Send(c->ci->WhoSends(), "SVSTOPIC", c->name, c->topic_ts, c->topic_setter, c->topic);
		}
		else
		{
			/* If the last time a topic was set is after the TS we want for this topic we must bump this topic's timestamp to now */
			time_t ts = c->topic_ts;
			if (c->topic_time > ts)
				ts = Anope::CurTime;
			/* But don't modify c->topic_ts, it should remain set to the real TS we want as ci->last_topic_time pulls from it */
			Uplink::Send(source, "FTOPIC", c->name, ts, c->topic_setter, c->topic);
		}
	}

	void SendVhostDel(User *u) override
	{
		if (u->HasMode("CLOAK"))
			this->SendChgHostInternal(u->nick, u->chost);
		else
			this->SendChgHostInternal(u->nick, u->host);

		if (Servers::Capab.count("CHGIDENT") && u->GetIdent() != u->GetVIdent())
			this->SendChgIdentInternal(u->nick, u->GetIdent());
	}

	void SendAkill(User *u, XLine *x) override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->GetExpires() - Anope::CurTime;
		if (timeleft > 172800 || !x->GetExpires())
			timeleft = 172800;

		/* InspIRCd may support regex bans, if they do we can send this and forget about it */
		if (x->IsRegex() && Servers::Capab.count("RLINE"))
		{
			Anope::string mask = x->GetMask();
			size_t h = mask.find('#');
			if (h != Anope::string::npos)
				mask = mask.replace(h, 1, ' ');
			SendAddLine("R", mask, timeleft, x->GetBy(), x->GetReason());
			return;
		}
		else if (x->IsRegex() || x->HasNickOrReal())
		{
			if (!u)
			{
				/* No user (this akill was just added), and contains nick and/or realname. Find users that match and ban them */
				for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
					if (x->GetManager()->Check(it->second, x))
						this->SendAkill(it->second, x);
				return;
			}

			XLine *old = x;

			if (old->GetManager()->HasEntry("*@" + u->host))
				return;

			/* We can't akill x as it has a nick and/or realname included, so create a new akill for *@host */
			x = Serialize::New<XLine *>();
			x->SetMask("*@" + u->host);
			x->SetBy(old->GetBy());
			x->SetExpires(old->GetExpires());
			x->SetReason(old->GetReason());
			old->GetManager()->AddXLine(x);

			Log(Config->GetClient("OperServ"), "akill") << "AKILL: Added an akill for " << x->GetMask() << " because " << u->GetMask() << "#" << u->realname << " matches " << old->GetMask();
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

		SendAddLine("G", x->GetUser() + "@" + x->GetHost(), timeleft, x->GetBy(), x->GetReason());
	}

	void SendNumericInternal(int numeric, const Anope::string &dest, const Anope::string &buf) override
	{
		User *u = User::Find(dest);
		Uplink::Send("PUSH", dest, ":" + Me->GetName() + " " + numeric + " " + (u ? u->nick : dest) + " " + buf);
	}

	void SendModeInternal(const MessageSource &source, const Channel *dest, const Anope::string &buf) override
	{
		IRCMessage message(source, "FMODE", dest->name, dest->creation_time);
		message.TokenizeAndPush(buf);
		Uplink::SendMessage(message);
	}

	void SendClientIntroduction(User *u) override
	{
		Anope::string modes = "+" + u->GetModes();
		Uplink::Send(Me, "UID", u->GetUID(), u->timestamp, u->nick, u->host, u->host, u->GetIdent(), "0.0.0.0", u->timestamp, modes, u->realname);
		if (modes.find('o') != Anope::string::npos)
			Uplink::Send(u, "OPERTYPE", "services");
	}

	/* SERVER services-dev.chatspike.net password 0 :Description here */
	void SendServer(const Server *server) override
	{
		/* if rsquit is set then we are waiting on a squit */
		if (rsquit_id.empty() && rsquit_server.empty())
			Uplink::Send("SERVER", server->GetName(), Config->Uplinks[Anope::CurrentUplink].password, server->GetHops(), server->GetSID(), server->GetDescription());
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
		{
			Uplink::Send("SQUIT", s->GetName(), message);
		}
	}

	/* JOIN */
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override
	{
		Uplink::Send(Me, "FJOIN", c->name, c->creation_time, "+" + c->GetModes(true, true), "," + user->GetUID());

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

			ServiceBot *setter = ServiceBot::Find(user->nick);
			for (size_t i = 0; i < cs.Modes().length(); ++i)
				c->SetMode(setter, ModeManager::FindChannelModeByChar(cs.Modes()[i]), user->GetUID(), false);

			if (uc != NULL)
				uc->status = cs;
		}
	}

	/* UNSQLINE */
	void SendSQLineDel(XLine *x) override
	{
		SendDelLine("Q", x->GetMask());
	}

	/* SQLINE */
	void SendSQLine(User *, XLine *x) override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->GetExpires() - Anope::CurTime;
		if (timeleft > 172800 || !x->GetExpires())
			timeleft = 172800;
		SendAddLine("Q", x->GetMask(), timeleft, x->GetBy(), x->GetReason());
	}

	void SendVhost(User *u, const Anope::string &vIdent, const Anope::string &vhost) override
	{
		if (!vIdent.empty())
			this->SendChgIdentInternal(u->nick, vIdent);
		if (!vhost.empty())
			this->SendChgHostInternal(u->nick, vhost);
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick, time_t t) override
	{
		Uplink::Send(Config->GetClient("NickServ"), "SVSHOLD", nick, t, "Being held for registered user");
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick) override
	{
		Uplink::Send(Config->GetClient("NickServ"), "SVSHOLD", nick);
	}

	/* UNSZLINE */
	void SendSZLineDel(XLine *x) override
	{
		SendDelLine("Z", x->GetHost());
	}

	/* SZLINE */
	void SendSZLine(User *, XLine *x) override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->GetExpires() - Anope::CurTime;
		if (timeleft > 172800 || !x->GetExpires())
			timeleft = 172800;
		SendAddLine("Z", x->GetHost(), timeleft, x->GetBy(), x->GetReason());
	}

	void SendSVSJoin(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &) override
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

	void SendSWhois(const MessageSource &, const Anope::string &who, const Anope::string &mask) override
	{
		User *u = User::Find(who);

		Uplink::Send(Me, "METADATA", u->GetUID(), "swhois", mask);
	}

	void SendBOB() override
	{
		Uplink::Send(Me, "BURST", Anope::CurTime);
		Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);
		Uplink::Send(Me, "VERSION", "Anope-" + Anope::Version() + " " + Me->GetName() + " :" + IRCD->GetProtocolName() + " - (" + (enc ? enc->name : "none") + ") -- " + Anope::VersionBuildString());
	}

	void SendEOB() override
	{
		Uplink::Send(Me, "ENDBURST");
	}

	void SendGlobopsInternal(const MessageSource &source, const Anope::string &buf) override
	{
		if (Servers::Capab.count("GLOBOPS"))
			Uplink::Send(source, "SNONOTICE", "g", buf);
		else
			Uplink::Send(source, "SNONOTICE", "A", buf);
	}

	void SendLogin(User *u, NickServ::Nick *na) override
	{
		/* InspIRCd uses an account to bypass chmode +R, not umode +r, so we can't send this here */
		if (na->GetAccount()->HasFieldS("UNCONFIRMED"))
			return;

		Uplink::Send(Me, "METADATA", u->GetUID(), "accountname", na->GetAccount()->GetDisplay());
	}

	void SendLogout(User *u) override
	{
		Uplink::Send(Me, "METADATA", u->GetUID(), "accountname", "");
	}

	void SendChannel(Channel *c) override
	{
		Uplink::Send(Me, "FJOIN", c->name, c->creation_time, "+" + c->GetModes(true, true), "");
	}

	void SendSASLMechanisms(std::vector<Anope::string> &mechanisms) override
	{
		Anope::string mechlist;
		for (unsigned i = 0; i < mechanisms.size(); ++i)
			mechlist += "," + mechanisms[i];

		Uplink::Send(Me, "METADATA", "*", "saslmechlist", mechlist.empty() ? "" : mechlist.substr(1));
	}

	void SendSASLMessage(const SASL::Message &message) override
	{
		if (!message.ext.empty())
			Uplink::Send(Me, "ENCAP", message.target.substr(0, 3), "SASL",
				message.source, message.target,
				message.type, message.data, message.ext);
		else
			Uplink::Send(Me, "ENCAP", message.target.substr(0, 3), "SASL",
				message.source, message.target,
				message.type, message.data);
	}

	void SendSVSLogin(const Anope::string &uid, const Anope::string &acc, const Anope::string &vident, const Anope::string &vhost) override
	{
		Uplink::Send(Me, "METADATA", uid, "accountname", acc);

		SASLUser su;
		su.uid = uid;
		su.acc = acc;
		su.created = Anope::CurTime;

		for (std::list<SASLUser>::iterator it = saslusers.begin(); it != saslusers.end();)
		{
			SASLUser &u = *it;

			if (u.created + 30 < Anope::CurTime || u.uid == uid)
				it = saslusers.erase(it);
			else
				++it;
		}

		saslusers.push_back(su);
	}

	bool IsExtbanValid(const Anope::string &mask) override
	{
		return mask.length() >= 3 && mask[1] == ':';
	}

	bool IsIdentValid(const Anope::string &ident) override
	{
		if (ident.empty() || ident.length() > Config->GetBlock("networkinfo")->Get<unsigned>("userlen"))
			return false;

		for (unsigned i = 0; i < ident.length(); ++i)
		{
			const char &c = ident[i];

			if (c >= 'A' && c <= '}')
				continue;

			if ((c >= '0' && c <= '9') || c == '-' || c == '.')
				continue;

			return false;
		}

		return true;
	}
};

class InspIRCdExtBan : public ChannelModeVirtual<ChannelModeList>
{
	char ext;

 public:
	InspIRCdExtBan(const Anope::string &mname, const Anope::string &basename, char extban) : ChannelModeVirtual<ChannelModeList>(mname, basename)
		, ext(extban)
	{
	}

	ChannelMode *Wrap(Anope::string &param) override
	{
		param = Anope::string(ext) + ":" + param;
		return ChannelModeVirtual<ChannelModeList>::Wrap(param);
	}

	ChannelMode *Unwrap(ChannelMode *cm, Anope::string &param) override
	{
		if (cm->type != MODE_LIST || param.length() < 3 || param[0] != ext || param[1] != ':')
			return cm;

		param = param.substr(2);
		return this;
	}
};

namespace InspIRCdExtban
{
	class EntryMatcher : public InspIRCdExtBan
	{
	 public:
		EntryMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : InspIRCdExtBan(mname, mbase, c)
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();
			Anope::string real_mask = mask.substr(3);

			return Entry(this->name, real_mask).Matches(u);
		}
	};

	class ChannelMatcher : public InspIRCdExtBan
	{
	 public:
		ChannelMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : InspIRCdExtBan(mname, mbase, c)
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();

			Anope::string channel = mask.substr(3);

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

	class AccountMatcher : public InspIRCdExtBan
	{
	 public:
	 	AccountMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : InspIRCdExtBan(mname, mbase, c)
	 	{
	 	}

		bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();
			Anope::string real_mask = mask.substr(2);

			return u->IsIdentified() && real_mask.equals_ci(u->Account()->GetDisplay());
		}
	};

	class RealnameMatcher : public InspIRCdExtBan
	{
	 public:
	 	RealnameMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : InspIRCdExtBan(mname, mbase, c)
	 	{
	 	}

	 	bool Matches(User *u, const Entry *e) override
	 	{
	 		const Anope::string &mask = e->GetMask();
	 		Anope::string real_mask = mask.substr(2);
	 		return Anope::Match(u->realname, real_mask);
	 	}
	};

	class ServerMatcher : public InspIRCdExtBan
	{
	 public:
	 	ServerMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : InspIRCdExtBan(mname, mbase, c)
	 	{
	 	}

	 	bool Matches(User *u, const Entry *e) override
	 	{
	 		const Anope::string &mask = e->GetMask();
	 		Anope::string real_mask = mask.substr(2);
	 		return Anope::Match(u->server->GetName(), real_mask);
	 	}
	};

	class FingerprintMatcher : public InspIRCdExtBan
	{
	 public:
		FingerprintMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : InspIRCdExtBan(mname, mbase, c)
	 	{
	 	}

	 	bool Matches(User *u, const Entry *e) override
	 	{
	 		const Anope::string &mask = e->GetMask();
	 		Anope::string real_mask = mask.substr(2);
	 		return !u->fingerprint.empty() && Anope::Match(u->fingerprint, real_mask);
	 	}
	};

	class UnidentifiedMatcher : public InspIRCdExtBan
	{
	 public:
		UnidentifiedMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : InspIRCdExtBan(mname, mbase, c)
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
	 		const Anope::string &mask = e->GetMask();
	 		Anope::string real_mask = mask.substr(2);
			return !u->Account() && Entry("BAN", real_mask).Matches(u);
		}
	};
}

struct IRCDMessageCapab : Message::Capab
{
	IRCDMessageCapab(Module *creator) : Message::Capab(creator, "CAPAB") { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		if (params[0].equals_cs("START"))
		{
			if (params.size() >= 2)
				spanningtree_proto_ver = (Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0);

			if (spanningtree_proto_ver < 1202)
			{
				Uplink::Send("ERROR", "Protocol mismatch, no or invalid protocol version given in CAPAB START");
				Anope::QuitReason = "Protocol mismatch, no or invalid protocol version given in CAPAB START";
				Anope::Quitting = true;
				return;
			}

			/* reset CAPAB */
			Servers::Capab.insert("SERVERS");
			Servers::Capab.insert("CHGHOST");
			Servers::Capab.insert("CHGIDENT");
			Servers::Capab.insert("TOPICLOCK");
			IRCD->CanSVSHold = false;
		}
		else if (params[0].equals_cs("CHANMODES") && params.size() > 1)
		{
			spacesepstream ssep(params[1]);
			Anope::string capab;

			while (ssep.GetToken(capab))
			{
				if (capab.find('=') == Anope::string::npos)
					continue;

				Anope::string modename = capab.substr(0, capab.find('='));
				Anope::string modechar = capab.substr(capab.find('=') + 1);
				char symbol = 0;

				if (modechar.empty())
					continue;

				if (modechar.length() == 2)
				{
					symbol = modechar[0];
					modechar = modechar.substr(1);
				}

				ChannelMode *cm = ModeManager::FindChannelModeByChar(modechar[0]);
				if (cm == nullptr)
				{
					Log(this->GetOwner()) << "Warning: Uplink has unknown channel mode " << modename << "=" << modechar;
					continue;
				}

				char modesymbol = cm->type == MODE_STATUS ? (anope_dynamic_static_cast<ChannelModeStatus *>(cm))->symbol : 0;
				if (symbol != modesymbol)
				{
					Log(this->GetOwner()) << "Warning: Channel mode " << modename << " has a misconfigured status character";
					continue;
				}
			}
		}
		if (params[0].equals_cs("USERMODES") && params.size() > 1)
		{
			spacesepstream ssep(params[1]);
			Anope::string capab;

			while (ssep.GetToken(capab))
			{
				if (capab.find('=') == Anope::string::npos)
					continue;

				Anope::string modename = capab.substr(0, capab.find('='));
				Anope::string modechar = capab.substr(capab.find('=') + 1);

				if (modechar.empty())
					continue;

				UserMode *um = ModeManager::FindUserModeByChar(modechar[0]);
				if (um == nullptr)
				{
					Log(this->GetOwner()) << "Warning: Uplink has unknown user mode " << modename << "=" << modechar;
					continue;
				}
			}
		}
		else if (params[0].equals_cs("MODULES") && params.size() > 1)
		{
			spacesepstream ssep(params[1]);
			Anope::string module;

			while (ssep.GetToken(module))
			{
				if (module.equals_cs("m_svshold.so"))
					IRCD->CanSVSHold = true;
				else if (module.find("m_rline.so") == 0)
				{
					Servers::Capab.insert("RLINE");
					const Anope::string &regexengine = Config->GetBlock("options")->Get<Anope::string>("regexengine");
					if (!regexengine.empty() && module.length() > 11 && regexengine != module.substr(11))
						Log() << "Warning: InspIRCd is using regex engine " << module.substr(11) << ", but we have " << regexengine << ". This may cause inconsistencies.";
				}
				else if (module.equals_cs("m_topiclock.so"))
					Servers::Capab.insert("TOPICLOCK");
			}
		}
		else if (params[0].equals_cs("MODSUPPORT") && params.size() > 1)
		{
			spacesepstream ssep(params[1]);
			Anope::string module;

			while (ssep.GetToken(module))
			{
				if (module.equals_cs("m_services_account.so"))
					Servers::Capab.insert("SERVICES");
				else if (module.equals_cs("m_chghost.so"))
					Servers::Capab.insert("CHGHOST");
				else if (module.equals_cs("m_chgident.so"))
					Servers::Capab.insert("CHGIDENT");
			}
		}
		else if (params[0].equals_cs("CAPABILITIES") && params.size() > 1)
		{
			spacesepstream ssep(params[1]);
			Anope::string capab;
			while (ssep.GetToken(capab))
			{
				if (capab.find("MAXMODES=") != Anope::string::npos)
				{
					Anope::string maxmodes(capab.begin() + 9, capab.end());
					IRCD->MaxModes = maxmodes.is_pos_number_only() ? convertTo<unsigned>(maxmodes) : 3;
				}
				else if (capab == "GLOBOPS=1")
					Servers::Capab.insert("GLOBOPS");
			}
		}
		else if (params[0].equals_cs("END"))
		{
			if (!Servers::Capab.count("SERVICES"))
			{
				Uplink::Send("ERROR", "m_services_account.so is not loaded. This is required by Anope");
				Anope::QuitReason = "ERROR: Remote server does not have the m_services_account module loaded, and this is required.";
				Anope::Quitting = true;
				return;
			}
			if (!ModeManager::FindUserModeByName("PRIV"))
			{
				Uplink::Send("ERROR", "m_hidechans.so is not loaded. This is required by Anope");
				Anope::QuitReason = "ERROR: Remote server does not have the m_hidechans module loaded, and this is required.";
				Anope::Quitting = true;
				return;
			}
			if (!IRCD->CanSVSHold)
				Log() << "SVSHOLD missing, Usage disabled until module is loaded.";
			if (!Servers::Capab.count("CHGHOST"))
				Log() << "CHGHOST missing, Usage disabled until module is loaded.";
			if (!Servers::Capab.count("CHGIDENT"))
				Log() << "CHGIDENT missing, Usage disabled until module is loaded.";
		}

		Message::Capab::Run(source, params);
	}
};

struct IRCDMessageEncap : IRCDMessage
{
	IRCDMessageEncap(Module *creator) : IRCDMessage(creator, "ENCAP", 4) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::Match(Me->GetSID(), params[0]) == false)
			return;

		if (params[1] == "CHGIDENT")
		{
			User *u = User::Find(params[2]);
			if (!u || u->server != Me)
				return;

			u->SetIdent(params[3]);
			Uplink::Send(u, "FIDENT", params[3]);
		}
		else if (params[1] == "CHGHOST")
		{
			User *u = User::Find(params[2]);
			if (!u || u->server != Me)
				return;

			u->SetDisplayedHost(params[3]);
			Uplink::Send(u, "FHOST", params[3]);
		}
		else if (params[1] == "CHGNAME")
		{
			User *u = User::Find(params[2]);
			if (!u || u->server != Me)
				return;

			u->SetRealname(params[3]);
			Uplink::Send(u, "FNAME", params[3]);
		}
	}
};

struct IRCDMessageEndburst : IRCDMessage
{
	IRCDMessageEndburst(Module *creator) : IRCDMessage(creator, "ENDBURST", 0) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		Server *s = source.GetServer();

		Log(LOG_DEBUG) << "Processed ENDBURST for " << s->GetName();

		s->Sync(true);
	}
};

struct IRCDMessageFHost : IRCDMessage
{
	IRCDMessageFHost(Module *creator) : IRCDMessage(creator, "FHOST", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();
		if (u->HasMode("CLOAK"))
			u->RemoveModeInternal(source, ModeManager::FindUserModeByName("CLOAK"));
		u->SetDisplayedHost(params[0]);
	}
};

struct IRCDMessageFIdent : IRCDMessage
{
	IRCDMessageFIdent(Module *creator) : IRCDMessage(creator, "FIDENT", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		source.GetUser()->SetIdent(params[0]);
	}
};

struct IRCDMessageFJoin : IRCDMessage
{
	IRCDMessageFJoin(Module *creator) : IRCDMessage(creator, "FJOIN", 2) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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

			sju.second = User::Find(buf);
			if (!sju.second)
			{
				Log(LOG_DEBUG) << "FJOIN for non-existent user " << buf << " on " << params[0];
				continue;
			}

			users.push_back(sju);
		}

		time_t ts = Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : Anope::CurTime;
		Message::Join::SJoin(source, params[0], ts, modes, users);
	}
};

struct IRCDMessageFMode : IRCDMessage
{
	IRCDMessageFMode(Module *creator) : IRCDMessage(creator, "FMODE", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		/* :source FMODE #test 12345678 +nto foo */

		Anope::string modes = params[2];
		for (unsigned n = 3; n < params.size(); ++n)
			modes += " " + params[n];

		Channel *c = Channel::Find(params[0]);
		time_t ts;

		try
		{
			ts = convertTo<time_t>(params[1]);
		}
		catch (const ConvertException &)
		{
			ts = 0;
		}

		if (c)
			c->SetModesInternal(source, modes, ts);
	}
};

struct IRCDMessageFTopic : IRCDMessage
{
	IRCDMessageFTopic(Module *creator) : IRCDMessage(creator, "FTOPIC", 4) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		/* :source FTOPIC channel topicts setby :topic */

		Channel *c = Channel::Find(params[0]);
		if (c)
			c->ChangeTopicInternal(NULL, params[2], params[3], Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : Anope::CurTime);
	}
};

struct IRCDMessageIdle : IRCDMessage
{
	IRCDMessageIdle(Module *creator) : IRCDMessage(creator, "IDLE", 1) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		ServiceBot *bi = ServiceBot::Find(params[0]);
		if (bi)
		{
			Uplink::Send(bi, "IDLE", source.GetSource(), Anope::StartTime, Anope::CurTime - bi->lastmsg);
		}
		else
		{
			User *u = User::Find(params[0]);
			if (u && u->server == Me)
				Uplink::Send(u, "IDLE", source.GetSource(), Anope::StartTime, 0);
		}
	}
};

/*
 *   source     = numeric of the sending server
 *   params[0]  = uuid
 *   params[1]  = metadata name
 *   params[2]  = data
 */
struct IRCDMessageMetadata : IRCDMessage
{
	const bool &do_topiclock, &do_mlock;

	IRCDMessageMetadata(Module *creator, const bool &handle_topiclock, const bool &handle_mlock)
		: IRCDMessage(creator, "METADATA", 3)
		, do_topiclock(handle_topiclock)
		, do_mlock(handle_mlock)
       	{
		SetFlag(IRCDMESSAGE_REQUIRE_SERVER);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		if (isdigit(params[0][0]))
		{
			if (params[1].equals_cs("accountname"))
			{
				User *u = User::Find(params[0]);
				NickServ::Account *nc = NickServ::FindAccount(params[2]);
				if (u && nc)
					u->Login(nc);
			}

			/*
			 *   possible incoming ssl_cert messages:
			 *   Received: :409 METADATA 409AAAAAA ssl_cert :vTrSe c38070ce96e41cc144ed6590a68d45a6 <...> <...>
			 *   Received: :409 METADATA 409AAAAAC ssl_cert :vTrSE Could not get peer certificate: error:00000000:lib(0):func(0):reason(0)
			 */
			else if (params[1].equals_cs("ssl_cert"))
			{
				User *u = User::Find(params[0]);
				if (!u)
					return;
				u->Extend<bool>("ssl", true);
				Anope::string data = params[2].c_str();
				size_t pos1 = data.find(' ') + 1;
				size_t pos2 = data.find(' ', pos1);
				if ((pos2 - pos1) >= 32) // inspircd supports md5 and sha1 fingerprint hashes -> size 32 or 40 bytes.
				{
					u->fingerprint = data.substr(pos1, pos2 - pos1);
				}
				EventManager::Get()->Dispatch(&Event::Fingerprint::OnFingerprint, u);
			}
		}
		// We deliberately ignore non-bursting servers to avoid pseudoserver fights
		else if ((params[0][0] == '#') && (!source.GetServer()->IsSynced()))
		{
			Channel *c = Channel::Find(params[0]);
			if (c && c->ci)
			{
				if ((do_mlock) && (params[1] == "mlock"))
				{
					ModeLocks *modelocks = c->ci->GetExt<ModeLocks>("modelocks");
					Anope::string modes;
					if (modelocks)
						modes = modelocks->GetMLockAsString(c->ci, false).replace_all_cs("+", "").replace_all_cs("-", "");

					// Mode lock string is not what we say it is?
					if (modes != params[2])
						Uplink::Send(Me, "METADATA", c->name, "mlock", modes);
				}
				else if ((do_topiclock) && (params[1] == "topiclock"))
				{
					bool mystate = c->ci->GetExt<bool>("TOPICLOCK");
					bool serverstate = (params[2] == "1");
					if (mystate != serverstate)
						Uplink::Send(Me, "METADATA", c->name, "topiclock", mystate ? "1" : "");
				}
			}
		}
		else if (params[0] == "*")
		{
			// Wed Oct  3 15:40:27 2012: S[14] O :20D METADATA * modules :-m_svstopic.so

			if (params[1].equals_cs("modules") && !params[2].empty())
			{
				// only interested when it comes from our uplink
				Server* server = source.GetServer();
				if (!server || server->GetUplink() != Me)
					return;

				bool plus = (params[2][0] == '+');
				if (!plus && params[2][0] != '-')
					return;

				bool required = false;
				Anope::string capab, module = params[2].substr(1);

				if (module.equals_cs("m_services_account.so"))
					required = true;
				else if (module.equals_cs("m_hidechans.so"))
					required = true;
				else if (module.equals_cs("m_chghost.so"))
					capab = "CHGHOST";
				else if (module.equals_cs("m_chgident.so"))
					capab = "CHGIDENT";
				else if (module.equals_cs("m_svshold.so"))
					capab = "SVSHOLD";
				else if (module.equals_cs("m_rline.so"))
					capab = "RLINE";
				else if (module.equals_cs("m_topiclock.so"))
					capab = "TOPICLOCK";
				else
					return;

				if (required)
				{
					if (!plus)
						Log() << "Warning: InspIRCd unloaded module " << module << ", Anope won't function correctly without it";
				}
				else
				{
					if (plus)
						Servers::Capab.insert(capab);
					else
						Servers::Capab.erase(capab);

					Log() << "InspIRCd " << (plus ? "loaded" : "unloaded") << " module " << module << ", adjusted functionality";
				}

			}
		}
	}
};

struct IRCDMessageMode : IRCDMessage
{
	IRCDMessageMode(Module *creator) : IRCDMessage(creator, "MODE", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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
			User *u = source.GetUser();
			// This can happen with server-origin modes.
			if (!u)
				u = User::Find(params[0]);
			// if it's still null, drop it like fire.
			// most likely situation was that server introduced a nick which we subsequently akilled
			if (u)
				u->SetModesInternal(source, "%s", params[1].c_str());
		}
	}
};

struct IRCDMessageNick : IRCDMessage
{
	IRCDMessageNick(Module *creator) : IRCDMessage(creator, "NICK", 2) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		source.GetUser()->ChangeNick(params[0]);
	}
};

struct IRCDMessageOperType : IRCDMessage
{
	IRCDMessageOperType(Module *creator) : IRCDMessage(creator, "OPERTYPE", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		/* opertype is equivalent to mode +o because servers
		   dont do this directly */
		User *u = source.GetUser();
		if (!u->HasMode("OPER"))
			u->SetModesInternal(source, "+o");
	}
};

struct IRCDMessageRSQuit : IRCDMessage
{
	IRCDMessageRSQuit(Module *creator) : IRCDMessage(creator, "RSQUIT", 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		Server *s = Server::Find(params[0]);
		const Anope::string &reason = params.size() > 1 ? params[1] : "";
		if (!s)
			return;

		Uplink::Send(Me, "SQUIT", s->GetSID(), reason);
		s->Delete(s->GetName() + " " + s->GetUplink()->GetName());
	}
};

struct IRCDMessageSave : IRCDMessage
{
	time_t last_collide;

	IRCDMessageSave(Module *creator) : IRCDMessage(creator, "SAVE", 2), last_collide(0) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		User *targ = User::Find(params[0]);
		time_t ts;

		try
		{
			ts = convertTo<time_t>(params[1]);
		}
		catch (const ConvertException &)
		{
			return;
		}

		if (!targ || targ->timestamp != ts)
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

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 5) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*
	 * [Nov 04 00:08:46.308435 2009] debug: Received: SERVER irc.inspircd.com pass 0 964 :Testnet Central!
	 * 0: name
	 * 1: pass
	 * 2: hops
	 * 3: numeric
	 * 4: desc
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		unsigned int hops = Anope::string(params[2]).is_pos_number_only() ? convertTo<unsigned>(params[2]) : 0;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, params[4], params[3]);
	}
};

struct IRCDMessageSQuit : Message::SQuit
{
	IRCDMessageSQuit(Module *creator) : Message::SQuit(creator) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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
			Message::SQuit::Run(source, params);
	}
};

struct IRCDMessageTime : IRCDMessage
{
	IRCDMessageTime(Module *creator) : IRCDMessage(creator, "TIME", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		Uplink::Send(Me, "TIME", source.GetSource(), params[1], Anope::CurTime);
	}
};

struct IRCDMessageUID : IRCDMessage
{
	ServiceReference<SASL::Service> sasl;
	
	IRCDMessageUID(Module *creator) : IRCDMessage(creator, "UID", 8) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	/*
	 * [Nov 03 22:09:58.176252 2009] debug: Received: :964 UID 964AAAAAC 1225746297 w00t2 localhost testnet.user w00t 127.0.0.1 1225746302 +iosw +ACGJKLNOQcdfgjklnoqtx :Robin Burchell <w00t@inspircd.org>
	 * 0: uid
	 * 1: ts
	 * 2: nick
	 * 3: host
	 * 4: dhost
	 * 5: ident
	 * 6: ip
	 * 7: signon
	 * 8+: modes and params -- IMPORTANT, some modes (e.g. +s) may have parameters. So don't assume a fixed position of realname!
	 * last: realname
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		time_t ts = convertTo<time_t>(params[1]);

		Anope::string modes = params[8];
		for (unsigned i = 9; i < params.size() - 1; ++i)
			modes += " " + params[i];

		NickServ::Nick *na = NULL;
		if (sasl)
			for (std::list<SASLUser>::iterator it = saslusers.begin(); it != saslusers.end();)
			{
				SASLUser &u = *it;

				if (u.created + 30 < Anope::CurTime)
					it = saslusers.erase(it);
				else if (u.uid == params[0])
				{
					na = NickServ::FindNick(u.acc);
					it = saslusers.erase(it);
				}
				else
					++it;
			}

		User *u = User::OnIntroduce(params[2], params[5], params[3], params[4], params[6], source.GetServer(), params[params.size() - 1], ts, modes, params[0], na ? na->GetAccount() : NULL);
		if (u)
			u->signon = convertTo<time_t>(params[7]);
	}
};

class ProtoInspIRCd20 : public Module
	, public EventHook<Event::UserNickChange>
	, public EventHook<Event::ChannelSync>
	, public EventHook<Event::ChanRegistered>
	, public EventHook<Event::DelChan>
	, public EventHook<Event::MLockEvents>
	, public EventHook<Event::SetChannelOption>
{
	InspIRCd20Proto ircd_proto;
	ExtensibleItem<bool> ssl;
	ServiceReference<ModeLocks> mlocks;

	/* Core message handlers */
	Message::Away message_away;
	Message::Error message_error;
	Message::Invite message_invite;
	Message::Join message_join;
	Message::Kick message_kick;
	Message::Kill message_kill;
	Message::MOTD message_motd;
	Message::Notice message_notice;
	Message::Part message_part;
	Message::Ping message_ping;
	Message::Privmsg message_privmsg;
	Message::Quit message_quit;
	Message::Stats message_stats;
	Message::Topic message_topic;

	/* Our message handlers */
	IRCDMessageCapab message_capab;
	IRCDMessageEncap message_encap;
	IRCDMessageEndburst message_endburst;
	IRCDMessageFHost message_fhost;
	IRCDMessageFIdent message_fident;
	IRCDMessageFJoin message_fjoin;
	IRCDMessageFMode message_fmode;
	IRCDMessageFTopic message_ftopic;
	IRCDMessageIdle message_idle;
	IRCDMessageMetadata message_metadata;
	IRCDMessageMode message_mode;
	IRCDMessageNick message_nick;
	IRCDMessageOperType message_opertype;
	IRCDMessageRSQuit message_rsquit;
	IRCDMessageSave message_save;
	IRCDMessageServer message_server;
	IRCDMessageSQuit message_squit;
	IRCDMessageTime message_time;
	IRCDMessageUID message_uid;

	bool use_server_side_topiclock, use_server_side_mlock;

	void SendChannelMetadata(Channel *c, const Anope::string &metadataname, const Anope::string &value)
	{
		Uplink::Send(Me, "METADATA", c->name, metadataname, value);
	}

 public:
	ProtoInspIRCd20(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR)
		, EventHook<Event::UserNickChange>(this)
		, EventHook<Event::ChannelSync>(this)
		, EventHook<Event::ChanRegistered>(this)
		, EventHook<Event::DelChan>(this)
		, EventHook<Event::MLockEvents>(this)
		, EventHook<Event::SetChannelOption>(this)

		, ircd_proto(this)
		, ssl(this, "ssl")
		, message_away(this)
		, message_error(this)
		, message_invite(this)
		, message_join(this)
		, message_kick(this)
		, message_kill(this)
		, message_motd(this)
		, message_notice(this)
		, message_part(this)
		, message_ping(this)
		, message_privmsg(this)
		, message_quit(this)
		, message_stats(this)
		, message_topic(this)

		, message_capab(this)
		, message_encap(this)
		, message_endburst(this)
		, message_fhost(this)
		, message_fident(this)
		, message_fjoin(this)
		, message_fmode(this)
		, message_ftopic(this)
		, message_idle(this)
		, message_metadata(this, use_server_side_topiclock, use_server_side_mlock)
		, message_mode(this)
		, message_nick(this)
		, message_opertype(this)
		, message_rsquit(this)
		, message_save(this)
		, message_server(this)
		, message_squit(this)
		, message_time(this)
		, message_uid(this)
	{
	}

	void OnReload(Configuration::Conf *conf) override
	{
		use_server_side_topiclock = conf->GetModule(this)->Get<bool>("use_server_side_topiclock");
		use_server_side_mlock = conf->GetModule(this)->Get<bool>("use_server_side_mlock");

		for (int i = 0; i < conf->CountBlock("extban"); ++i)
		{
			Configuration::Block *extban = conf->GetBlock("extban", i);
			Anope::string name = extban->Get<Anope::string>("name"),
					type = extban->Get<Anope::string>("type"),
					base = extban->Get<Anope::string>("base"),
					character = extban->Get<Anope::string>("character");

			ChannelMode *cm;

			if (character.empty())
				continue;

			if (type == "channel")
				cm = new InspIRCdExtban::ChannelMatcher(name, base, character[0]);
			else if (type == "entry")
				cm = new InspIRCdExtban::EntryMatcher(name, base, character[0]);
			else if (type == "realname")
				cm = new InspIRCdExtban::RealnameMatcher(name, base, character[0]);
			else if (type == "account")
				cm = new InspIRCdExtban::AccountMatcher(name, base, character[0]);
			else if (type == "fingerprint")
				cm = new InspIRCdExtban::FingerprintMatcher(name, base, character[0]);
			else if (type == "unidentified")
				cm = new InspIRCdExtban::UnidentifiedMatcher(name, base, character[0]);
			else if (type == "server")
				cm = new InspIRCdExtban::ServerMatcher(name, base, character[0]);
			else
				continue;

			if (!ModeManager::AddChannelMode(cm))
				delete cm;
		}
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

	void OnChanRegistered(ChanServ::Channel *ci) override
	{
		if (use_server_side_mlock && ci->c && mlocks && !mlocks->GetMLockAsString(ci, false).empty())
		{
			Anope::string modes = mlocks->GetMLockAsString(ci, false).replace_all_cs("+", "").replace_all_cs("-", "");
			SendChannelMetadata(ci->c, "mlock", modes);
		}

		if (use_server_side_topiclock && Servers::Capab.count("TOPICLOCK") && ci->c)
		{
			if (ci->HasFieldS("TOPICLOCK"))
				SendChannelMetadata(ci->c, "topiclock", "1");
		}
	}

	void OnDelChan(ChanServ::Channel *ci) override
	{
		if (use_server_side_mlock && ci->c)
			SendChannelMetadata(ci->c, "mlock", "");

		if (use_server_side_topiclock && Servers::Capab.count("TOPICLOCK") && ci->c)
			SendChannelMetadata(ci->c, "topiclock", "");
	}

	EventReturn OnMLock(ChanServ::Channel *ci, ModeLock *lock) override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->GetName());
		if (use_server_side_mlock && cm && ci->c && mlocks && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM))
		{
			Anope::string modes = mlocks->GetMLockAsString(ci, false).replace_all_cs("+", "").replace_all_cs("-", "") + cm->mchar;
			SendChannelMetadata(ci->c, "mlock", modes);
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChanServ::Channel *ci, ModeLock *lock) override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->GetName());
		if (use_server_side_mlock && cm && ci->c && mlocks && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM))
		{
			Anope::string modes = mlocks->GetMLockAsString(ci, false).replace_all_cs("+", "").replace_all_cs("-", "").replace_all_cs(cm->mchar, "");
			SendChannelMetadata(ci->c, "mlock", modes);
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnSetChannelOption(CommandSource &source, Command *cmd, ChanServ::Channel *ci, const Anope::string &setting) override
	{
		if (cmd->GetName() == "chanserv/topic" && ci->c)
		{
			if (setting == "topiclock on")
				SendChannelMetadata(ci->c, "topiclock", "1");
			else if (setting == "topiclock off")
				SendChannelMetadata(ci->c, "topiclock", "0");
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ProtoInspIRCd20)
