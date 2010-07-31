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
			const char *commastr = getstring(nc, COMMA_SPACE);
			if (!buf.empty())
				buf += commastr;

			buf += str;
		}
	}
 public:
	CommandNSInfo() : Command("INFO", 1, 1)
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
				notice_lang(Config.s_NickServ, u, NICK_IS_PREREG);
				if (has_auspex)
					notice_lang(Config.s_NickServ, u, NICK_INFO_EMAIL, nr->email.c_str());
			}
			else if (nickIsServices(nick, true))
				notice_lang(Config.s_NickServ, u, NICK_X_IS_SERVICES, nick.c_str());
			else
				notice_lang(Config.s_NickServ, u, NICK_X_NOT_REGISTERED, nick.c_str());
		}
		else if (na->HasFlag(NS_FORBIDDEN))
		{
			if (is_oper(u) && !na->last_usermask.empty())
				notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN_OPER, nick.c_str(), na->last_usermask.c_str(), !na->last_realname.empty() ? na->last_realname.c_str() : getstring(u, NO_REASON));
			else
				notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, nick.c_str());
		}
		else
		{
			struct tm *tm;
			char buf[BUFSIZE];
			bool nick_online = false, show_hidden = false;
			time_t expt;

			/* Is the real owner of the nick we're looking up online? -TheShadow */
			User *u2 = finduser(na->nick);
			if (u2 && u2->Account() == na->nc)
				nick_online = true;

			if (has_auspex || (u->Account() && na->nc == u->Account()))
				show_hidden = true;

			notice_lang(Config.s_NickServ, u, NICK_INFO_REALNAME, na->nick.c_str(), na->last_realname.c_str());

			if (na->nc->IsServicesOper() && (show_hidden || !na->nc->HasFlag(NI_HIDE_STATUS)))
				notice_lang(Config.s_NickServ, u, NICK_INFO_SERVICES_OPERTYPE, na->nick.c_str(), na->nc->ot->GetName().c_str());

			if (nick_online)
			{
				if (show_hidden || !na->nc->HasFlag(NI_HIDE_MASK))
					notice_lang(Config.s_NickServ, u, NICK_INFO_ADDRESS_ONLINE, na->last_usermask.c_str());
				else
					notice_lang(Config.s_NickServ, u, NICK_INFO_ADDRESS_ONLINE_NOHOST, na->nick.c_str());
			}
			else
			{
				if (show_hidden || !na->nc->HasFlag(NI_HIDE_MASK))
					notice_lang(Config.s_NickServ, u, NICK_INFO_ADDRESS, na->last_usermask.c_str());
			}

			tm = localtime(&na->time_registered);
			strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
			notice_lang(Config.s_NickServ, u, NICK_INFO_TIME_REGGED, buf);

			if (!nick_online)
			{
				tm = localtime(&na->last_seen);
				strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
				notice_lang(Config.s_NickServ, u, NICK_INFO_LAST_SEEN, buf);
			}

			if (!na->last_quit.empty() && (show_hidden || !na->nc->HasFlag(NI_HIDE_QUIT)))
				notice_lang(Config.s_NickServ, u, NICK_INFO_LAST_QUIT, na->last_quit.c_str());

			if (!na->nc->email.empty() && (show_hidden || !na->nc->HasFlag(NI_HIDE_EMAIL)))
				notice_lang(Config.s_NickServ, u, NICK_INFO_EMAIL, na->nc->email.c_str());

			if (show_hidden)
			{
				if (!Config.s_HostServ.empty() && ircd->vhost && na->hostinfo.HasVhost())
				{
					if (ircd->vident && !na->hostinfo.GetIdent().empty())
						notice_lang(Config.s_NickServ, u, NICK_INFO_VHOST2, na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str());
					else
						notice_lang(Config.s_NickServ, u, NICK_INFO_VHOST, na->hostinfo.GetHost().c_str());
				}
				if (!na->nc->greet.empty())
					notice_lang(Config.s_NickServ, u, NICK_INFO_GREET, na->nc->greet.c_str());

				Anope::string optbuf;

				CheckOptStr(optbuf, NI_KILLPROTECT, getstring(u, NICK_INFO_OPT_KILL), na->nc);
				CheckOptStr(optbuf, NI_SECURE, getstring(u, NICK_INFO_OPT_SECURE), na->nc);
				CheckOptStr(optbuf, NI_PRIVATE, getstring(u, NICK_INFO_OPT_PRIVATE), na->nc);
				CheckOptStr(optbuf, NI_MSG, getstring(u, NICK_INFO_OPT_MSG), na->nc);
				CheckOptStr(optbuf, NI_AUTOOP, getstring(u, NICK_INFO_OPT_AUTOOP), na->nc);

				notice_lang(Config.s_NickServ, u, NICK_INFO_OPTIONS, optbuf.empty() ? getstring(u, NICK_INFO_OPT_NONE) : optbuf.c_str());

				if (na->nc->HasFlag(NI_SUSPENDED))
				{
					if (!na->last_quit.empty())
						notice_lang(Config.s_NickServ, u, NICK_INFO_SUSPENDED, na->last_quit.c_str());
					else
						notice_lang(Config.s_NickServ, u, NICK_INFO_SUSPENDED_NO_REASON);
				}

				if (na->HasFlag(NS_NO_EXPIRE))
					notice_lang(Config.s_NickServ, u, NICK_INFO_NO_EXPIRE);
				else
				{
					expt = na->last_seen + Config.NSExpire;
					tm = localtime(&expt);
					strftime_lang(buf, sizeof(buf), finduser(na->nick), STRFTIME_DATE_TIME_FORMAT, tm);
					notice_lang(Config.s_NickServ, u, NICK_INFO_EXPIRE, buf);
				}
			}

			FOREACH_MOD(I_OnNickInfo, OnNickInfo(u, na, show_hidden));
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_INFO);

		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_NickServ, u, "INFO", NICK_INFO_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_INFO);
	}
};

class NSInfo : public Module
{
 public:
	NSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, new CommandNSInfo());
	}
};

MODULE_INIT(NSInfo)
