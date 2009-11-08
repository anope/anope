/* NickServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

class CommandNSInfo : public Command
{
 private:
	// cannot be const, as it is modified
	void CheckOptStr(std::string &buf, NickCoreFlag opt, const std::string &str, NickCore *nc, bool reverse_logic = false)
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
	CommandNSInfo() : Command("INFO", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		/* Show hidden info to nick owners and sadmins when the "ALL" parameter is
		 * supplied. If a nick is online, the "Last seen address" changes to "Is
		 * online from".
		 * Syntax: INFO <nick> {ALL}
		 * -TheShadow (13 Mar 1999)
		 */
		const char *nick = params[0].c_str();
		ci::string param = params.size() > 1 ? params[1] : "";

		NickAlias *na;
		NickRequest *nr = NULL;
		/* Being an oper is enough from now on -GD */

		if (!(na = findnick(nick)))
		{
			if ((nr = findrequestnick(nick)))
			{
				notice_lang(s_NickServ, u, NICK_IS_PREREG);
				if (!param.empty() && param == "ALL" && u->nc && u->nc->IsServicesOper())
					notice_lang(s_NickServ, u, NICK_INFO_EMAIL, nr->email);
				else
				{
					if (u->nc && u->nc->IsServicesOper())
						notice_lang(s_NickServ, u, NICK_INFO_FOR_MORE, s_NickServ, nr->nick);
				}
			}
			else if (nickIsServices(nick, 1))
				notice_lang(s_NickServ, u, NICK_X_IS_SERVICES, nick);
			else
				notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
		}
		else if (na->HasFlag(NS_FORBIDDEN))
		{
			if (is_oper(u) && na->last_usermask)
				notice_lang(s_NickServ, u, NICK_X_FORBIDDEN_OPER, nick, na->last_usermask, na->last_realname ? na->last_realname : getstring(u, NO_REASON));
			else
				notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);
		}
		else
		{
			struct tm *tm;
			char buf[BUFSIZE];
			int nick_online = 0;
			int show_hidden = 0;
			time_t expt;

			/* Is the real owner of the nick we're looking up online? -TheShadow */
			if (finduser(na->nick))
				nick_online = 1;

			/* Only show hidden fields to owner and sadmins and only when the ALL
			 * parameter is used. -TheShadow */
			if (!param.empty() && param == "ALL" && u->nc && (na->nc == u->nc || u->nc->IsServicesOper()))
				show_hidden = 1;

			notice_lang(s_NickServ, u, NICK_INFO_REALNAME, na->nick, na->last_realname);

			if (na->nc->IsServicesOper())
			{
				if (show_hidden || (!(na->nc->HasFlag(NI_HIDE_STATUS))))
				{
					notice_lang(s_NickServ, u, NICK_INFO_SERVICES_OPERTYPE, na->nick, na->nc->ot->GetName().c_str());

				}
			}

			if (nick_online)
			{
				if (show_hidden || !(na->nc->HasFlag(NI_HIDE_MASK)))
					notice_lang(s_NickServ, u, NICK_INFO_ADDRESS_ONLINE, na->last_usermask);
				else
					notice_lang(s_NickServ, u, NICK_INFO_ADDRESS_ONLINE_NOHOST, na->nick);
			}
			else {
				if (show_hidden || !(na->nc->HasFlag(NI_HIDE_MASK)))
					notice_lang(s_NickServ, u, NICK_INFO_ADDRESS, na->last_usermask);
			}

			tm = localtime(&na->time_registered);
			strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
			notice_lang(s_NickServ, u, NICK_INFO_TIME_REGGED, buf);

			if (!nick_online)
			{
				tm = localtime(&na->last_seen);
				strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
				notice_lang(s_NickServ, u, NICK_INFO_LAST_SEEN, buf);
			}

			if (na->last_quit && (show_hidden || !(na->nc->HasFlag(NI_HIDE_QUIT))))
				notice_lang(s_NickServ, u, NICK_INFO_LAST_QUIT, na->last_quit);

			if (na->nc->url)
				notice_lang(s_NickServ, u, NICK_INFO_URL, na->nc->url);
			if (na->nc->email && (show_hidden || !(na->nc->HasFlag(NI_HIDE_EMAIL))))
				notice_lang(s_NickServ, u, NICK_INFO_EMAIL, na->nc->email);
			if (na->nc->icq)
				notice_lang(s_NickServ, u, NICK_INFO_ICQ, na->nc->icq);

			if (show_hidden)
			{
				if (s_HostServ && ircd->vhost) {
					if (getvHost(na->nick) != NULL) {
						if (ircd->vident && getvIdent(na->nick) != NULL) {
							notice_lang(s_NickServ, u, NICK_INFO_VHOST2,
										getvIdent(na->nick),
										getvHost(na->nick));
						} else {
							notice_lang(s_NickServ, u, NICK_INFO_VHOST,
										getvHost(na->nick));
						}
					}
				}
				if (na->nc->greet)
					notice_lang(s_NickServ, u, NICK_INFO_GREET, na->nc->greet);

				std::string optbuf;

				CheckOptStr(optbuf, NI_KILLPROTECT, getstring(u, NICK_INFO_OPT_KILL), na->nc);
				CheckOptStr(optbuf, NI_SECURE, getstring(u, NICK_INFO_OPT_SECURE), na->nc);
				CheckOptStr(optbuf, NI_PRIVATE, getstring(u, NICK_INFO_OPT_PRIVATE), na->nc);
				CheckOptStr(optbuf, NI_MSG, getstring(u, NICK_INFO_OPT_MSG), na->nc);
				CheckOptStr(optbuf, NI_AUTOOP, getstring(u, NICK_INFO_OPT_AUTOOP), na->nc, true);

				notice_lang(s_NickServ, u, NICK_INFO_OPTIONS, optbuf.empty() ? getstring(u, NICK_INFO_OPT_NONE) : optbuf.c_str());

				if (na->nc->HasFlag(NI_SUSPENDED))
				{
					if (na->last_quit)
						notice_lang(s_NickServ, u, NICK_INFO_SUSPENDED, na->last_quit);
					else
						notice_lang(s_NickServ, u, NICK_INFO_SUSPENDED_NO_REASON);
				}

				if (na->HasFlag(NS_NO_EXPIRE))
					notice_lang(s_NickServ, u, NICK_INFO_NO_EXPIRE);
				else
				{
					if (u->nc->IsServicesOper())
					{
						expt = na->last_seen + NSExpire;
						tm = localtime(&expt);
						strftime_lang(buf, sizeof(buf), finduser(na->nick), STRFTIME_DATE_TIME_FORMAT, tm);
						notice_lang(s_NickServ, u, NICK_INFO_EXPIRE, buf);
					}
				}
			}

			if (!show_hidden)
				notice_lang(s_NickServ, u, NICK_INFO_FOR_MORE, s_NickServ, na->nick);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		if (u->nc && u->nc->IsServicesOper())
			notice_help(s_NickServ, u, NICK_SERVADMIN_HELP_INFO);
		else
			notice_help(s_NickServ, u, NICK_HELP_INFO);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_NickServ, u, "INFO", NICK_INFO_SYNTAX);
	}
};

class NSInfo : public Module
{
 public:
	NSInfo(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSInfo());

		ModuleManager::Attach(I_OnNickServHelp, this);
	}
	void OnNickServHelp(User *u)
	{
		notice_lang(s_NickServ, u, NICK_HELP_CMD_INFO);
	}
};

MODULE_INIT(NSInfo)
