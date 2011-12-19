/* NickServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandNSInfo : public Command
{
 private:
	template<typename T, unsigned END> void CheckOptStr(User *u, Anope::string &buf, T opt, const char *str, Flags<T, END> *nc, bool reverse_logic = false)
	{
		if (reverse_logic ? !nc->HasFlag(opt) : nc->HasFlag(opt))
		{
			if (!buf.empty())
				buf += ", ";

			buf += translate(u, str);
		}
	}
 public:
	CommandNSInfo(Module *creator) : Command(creator, "nickserv/info", 0, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Displays information about a given nickname"));
		this->SetSyntax(_("[\037nickname\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &nick = params.size() ? params[0] : (u->Account() ? u->Account()->display : u->nick);
		NickAlias *na = findnick(nick);
		bool has_auspex = u->IsIdentified() && u->HasPriv("nickserv/auspex");

		if (!na)
		{
			if (nickIsServices(nick, true))
				source.Reply(_("Nick \002%s\002 is part of this Network's Services."), nick.c_str());
			else
				source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
		}
		else
		{
			bool nick_online = false, show_hidden = false;

			/* Is the real owner of the nick we're looking up online? -TheShadow */
			User *u2 = finduser(na->nick);
			if (u2 && u2->Account() == na->nc)
				nick_online = true;

			if (has_auspex || (u->Account() && na->nc == u->Account()))
				show_hidden = true;

			source.Reply(_("%s is %s"), na->nick.c_str(), na->last_realname.c_str());

			if (na->nc->HasFlag(NI_UNCONFIRMED))
				source.Reply(_("%s nickname is unconfirmed."), na->nick.c_str());

			if (na->nc->IsServicesOper() && (show_hidden || !na->nc->HasFlag(NI_HIDE_STATUS)))
				source.Reply(_("%s is a services operator of type %s."), na->nick.c_str(), na->nc->o->ot->GetName().c_str());

			InfoFormatter info(u);

			if (nick_online)
			{
				if (show_hidden && !na->last_realhost.empty())
					info[_("Online from")] = na->last_realhost;
				if (show_hidden || !na->nc->HasFlag(NI_HIDE_MASK))
					info[_("Online from")] = na->last_usermask;
				else
					source.Reply(_("%s is currently online."), na->nick.c_str());
			}
			else
			{
				if (show_hidden || !na->nc->HasFlag(NI_HIDE_MASK))
					info[_("Last seen address")] = na->last_usermask;
				if (show_hidden && !na->last_realhost.empty())
					info[_("Last seen address")] = na->last_realhost;
			}

			info[_("Time registered")] = do_strftime(na->time_registered);

			if (!nick_online)
				info[_("Last seen")] = do_strftime(na->last_seen);

			if (!na->last_quit.empty() && (show_hidden || !na->nc->HasFlag(NI_HIDE_QUIT)))
				info[_("Last quit message")] = na->last_quit;

			if (!na->nc->email.empty() && (show_hidden || !na->nc->HasFlag(NI_HIDE_EMAIL)))
				info[_("Email address")] = na->nc->email;

			if (show_hidden)
			{
				if (na->hostinfo.HasVhost())
				{
					if (ircd->vident && !na->hostinfo.GetIdent().empty())
						info[_("VHost")] = na->hostinfo.GetIdent() + "@" + na->hostinfo.GetHost();
					else
						info[_("VHost")] = na->hostinfo.GetHost();
				}

				if (!na->nc->greet.empty())
					info[_("Greet")] = na->nc->greet;

				Anope::string optbuf;

				CheckOptStr<NickCoreFlag, NI_END>(u, optbuf, NI_KILLPROTECT, _("Protection"), na->nc);
				CheckOptStr<NickCoreFlag, NI_END>(u, optbuf, NI_SECURE, _("Security"), na->nc);
				CheckOptStr<NickCoreFlag, NI_END>(u, optbuf, NI_PRIVATE, _("Private"), na->nc);
				CheckOptStr<NickCoreFlag, NI_END>(u, optbuf, NI_MSG, _("Message mode"), na->nc);
				CheckOptStr<NickCoreFlag, NI_END>(u, optbuf, NI_AUTOOP, _("Auto-op"), na->nc);
				CheckOptStr<NickCoreFlag, NI_END>(u, optbuf, NI_SUSPENDED, _("Suspended"), na->nc);
				CheckOptStr<NickNameFlag, NS_END>(u, optbuf, NS_NO_EXPIRE, _("No expire"), na);

				info[_("Options")] = optbuf.empty() ? _("None") : optbuf;

				if (na->nc->HasFlag(NI_UNCONFIRMED) == false)
				{
					if (na->HasFlag(NS_NO_EXPIRE) || !Config->NSExpire)
						;
					else
						info[_("Expires")] = do_strftime(na->last_seen + Config->NSExpire);
				}
				else
					info[_("Expires")] = do_strftime(na->time_registered + Config->NSUnconfirmedExpire);
			}

			FOREACH_MOD(I_OnNickInfo, OnNickInfo(source, na, info, show_hidden));

			std::vector<Anope::string> replies;
			info.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Displays information about the given nickname, such as\n"
				"the nick's owner, last seen address and time, and nick\n"
				"options. If no nick is given, and you are identified,\n"
				"your account name is used, else your current nickname is\n"
				"used."));

		return true;
	}
};

class NSInfo : public Module
{
	CommandNSInfo commandnsinfo;

 public:
	NSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnsinfo(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(NSInfo)
