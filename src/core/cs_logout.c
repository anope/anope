/* ChanServ core functions
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

class CommandCSLogout : public Command
{
 private:
	int make_unidentified(User *u, ChannelInfo *ci)
	{
		struct u_chaninfolist *uci;

		if (!u || !ci)
			return 0;

		for (uci = u->founder_chans; uci; uci = uci->next)
		{
			if (uci->chan == ci)
			{
				if (uci->next)
					uci->next->prev = uci->prev;
				if (uci->prev)
					uci->prev->next = uci->next;
				else
					u->founder_chans = uci->next;
				delete uci;

				return 1;
			}
		}

		return 0;
	}

 public:
	CommandCSLogout() : Command("LOGOUT", 1, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *nick = params.size() > 1 ? params[1].c_str() : NULL;
		ChannelInfo *ci;
		User *u2 = NULL;
		int is_admin = u->nc->HasCommand("chanserv/logout");

		if (!is_admin && !nick)
			this->OnSyntaxError(u);
		else if (!(ci = cs_findchan(chan)))
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
		else if (nick && !(u2 = finduser(nick)))
			notice_lang(s_ChanServ, u, NICK_X_NOT_IN_USE, nick);
		else if (u2 != u && !is_real_founder(u, ci) && !is_admin)
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
		else if (u2 == u && is_real_founder(u, ci))
			/* Since founders can not logout we should tell them -katsklaw */
			notice_lang(s_ChanServ, u, CHAN_LOGOUT_FOUNDER_FAILED, chan);
		else
		{
			if (u2)
			{
				if (make_unidentified(u2, ci))
				{
					notice_lang(s_ChanServ, u, CHAN_LOGOUT_SUCCEEDED, nick, chan);
					alog("%s: User %s!%s@%s has been logged out of channel %s.", s_ChanServ, u2->nick, u2->GetIdent().c_str(), u2->host, chan);
				}
				else
					notice_lang(s_ChanServ, u, CHAN_LOGOUT_NOT_LOGGEDIN, nick, chan);
			}
			else
			{
				int i;
				for (i = 0; i < 1024; i++)
					for (u2 = userlist[i]; u2; u2 = u2->next)
						make_unidentified(u2, ci);
				notice_lang(s_ChanServ, u, CHAN_LOGOUT_ALL_SUCCEEDED, chan);
				alog("%s: User %s!%s@%s logged all identified users out of channel %s.", s_ChanServ, u->nick, u->GetIdent().c_str(), u->host, chan);
			}

		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (u->nc->HasCommand("chanserv/logout"))
			notice_help(s_NickServ, u, CHAN_SERVADMIN_HELP_LOGOUT);
		else
			notice_help(s_NickServ, u, CHAN_HELP_LOGOUT);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		if (u->nc->HasCommand("chanserv/logout"))
			syntax_error(s_NickServ, u, "LOGOUT", CHAN_LOGOUT_SERVADMIN_SYNTAX);
		else
			syntax_error(s_NickServ, u, "LOGOUT", CHAN_LOGOUT_SYNTAX);
	}
};

class CSLogout : public Module
{
 public:
	CSLogout(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSLogout(), MOD_UNIQUE);
	}
	void ChanServHelp(User *u)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_LOGOUT);
	}
};

MODULE_INIT("cs_logout", CSLogout)
