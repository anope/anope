/* NickServ core functions
 *
 * (C) 2003-2010 Anope Team
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
	// cannot be const, as it is modified
	void CheckOptStr(Anope::string &buf, NickCoreFlag opt, const Anope::string &str, NickCore *nc, bool reverse_logic = false)
	{
		if (reverse_logic ? !nc->HasFlag(opt) : nc->HasFlag(opt))
		{
			if (!buf.empty())
				buf += ", ";

			buf += str;
		}
	}
 public:
	CommandNSInfo() : Command("INFO", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params[0];

		NickAlias *na = findnick(nick);
		bool has_auspex = u->IsIdentified() && u->Account()->HasPriv("nickserv/auspex");

		if (!na)
		{
			NickRequest *nr = findrequestnick(nick);
			if (nr)
			{
				u->SendMessage(NickServ, NICK_IS_PREREG);
				if (has_auspex)
					u->SendMessage(NickServ, NICK_INFO_EMAIL, nr->email.c_str());
			}
			else if (nickIsServices(nick, true))
				u->SendMessage(NickServ, NICK_X_IS_SERVICES, nick.c_str());
			else
				u->SendMessage(NickServ, NICK_X_NOT_REGISTERED, nick.c_str());
		}
		else if (na->HasFlag(NS_FORBIDDEN))
		{
			if (is_oper(u) && !na->last_usermask.empty())
				u->SendMessage(NickServ, NICK_X_FORBIDDEN_OPER, nick.c_str(), na->last_usermask.c_str(), !na->last_realname.empty() ? na->last_realname.c_str() : GetString(u, NO_REASON).c_str());
			else
				u->SendMessage(NickServ, NICK_X_FORBIDDEN, nick.c_str());
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

			u->SendMessage(NickServ, NICK_INFO_REALNAME, na->nick.c_str(), na->last_realname.c_str());

			if (na->nc->IsServicesOper() && (show_hidden || !na->nc->HasFlag(NI_HIDE_STATUS)))
				u->SendMessage(NickServ, NICK_INFO_SERVICES_OPERTYPE, na->nick.c_str(), na->nc->ot->GetName().c_str());

			if (nick_online)
			{
				if (show_hidden || !na->nc->HasFlag(NI_HIDE_MASK))
					u->SendMessage(NickServ, NICK_INFO_ADDRESS_ONLINE, na->last_usermask.c_str());
				else
					u->SendMessage(NickServ, NICK_INFO_ADDRESS_ONLINE_NOHOST, na->nick.c_str());
			}
			else
			{
				if (show_hidden || !na->nc->HasFlag(NI_HIDE_MASK))
					u->SendMessage(NickServ, NICK_INFO_ADDRESS, na->last_usermask.c_str());
			}

			u->SendMessage(NickServ, NICK_INFO_TIME_REGGED, do_strftime(na->time_registered).c_str());

			if (!nick_online)
			{
				u->SendMessage(NickServ, NICK_INFO_LAST_SEEN, do_strftime(na->last_seen).c_str());
			}

			if (!na->last_quit.empty() && (show_hidden || !na->nc->HasFlag(NI_HIDE_QUIT)))
				u->SendMessage(NickServ, NICK_INFO_LAST_QUIT, na->last_quit.c_str());

			if (!na->nc->email.empty() && (show_hidden || !na->nc->HasFlag(NI_HIDE_EMAIL)))
				u->SendMessage(NickServ, NICK_INFO_EMAIL, na->nc->email.c_str());

			if (show_hidden)
			{
				if (!Config->s_HostServ.empty() && ircd->vhost && na->hostinfo.HasVhost())
				{
					if (ircd->vident && !na->hostinfo.GetIdent().empty())
						u->SendMessage(NickServ, NICK_INFO_VHOST2, na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str());
					else
						u->SendMessage(NickServ, NICK_INFO_VHOST, na->hostinfo.GetHost().c_str());
				}
				if (!na->nc->greet.empty())
					u->SendMessage(NickServ, NICK_INFO_GREET, na->nc->greet.c_str());

				Anope::string optbuf;

				CheckOptStr(optbuf, NI_KILLPROTECT, GetString(u, NICK_INFO_OPT_KILL).c_str(), na->nc);
				CheckOptStr(optbuf, NI_SECURE, GetString(u, NICK_INFO_OPT_SECURE).c_str(), na->nc);
				CheckOptStr(optbuf, NI_PRIVATE, GetString(u, NICK_INFO_OPT_PRIVATE).c_str(), na->nc);
				CheckOptStr(optbuf, NI_MSG, GetString(u, NICK_INFO_OPT_MSG).c_str(), na->nc);
				CheckOptStr(optbuf, NI_AUTOOP, GetString(u, NICK_INFO_OPT_AUTOOP).c_str(), na->nc);

				u->SendMessage(NickServ, NICK_INFO_OPTIONS, optbuf.empty() ? GetString(u, NICK_INFO_OPT_NONE).c_str() : optbuf.c_str());

				if (na->nc->HasFlag(NI_SUSPENDED))
				{
					if (!na->last_quit.empty())
						u->SendMessage(NickServ, NICK_INFO_SUSPENDED, na->last_quit.c_str());
					else
						u->SendMessage(NickServ, NICK_INFO_SUSPENDED_NO_REASON);
				}

				if (na->HasFlag(NS_NO_EXPIRE))
					u->SendMessage(NickServ, NICK_INFO_NO_EXPIRE);
				else
					u->SendMessage(NickServ, NICK_INFO_EXPIRE, do_strftime(na->last_seen + Config->NSExpire).c_str());
			}

			FOREACH_MOD(I_OnNickInfo, OnNickInfo(u, na, show_hidden));
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(NickServ, NICK_HELP_INFO);

		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(NickServ, u, "INFO", NICK_INFO_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(NickServ, NICK_HELP_CMD_INFO);
	}
};

class NSInfo : public Module
{
	CommandNSInfo commandnsinfo;

 public:
	NSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsinfo);
	}
};

MODULE_INIT(NSInfo)
