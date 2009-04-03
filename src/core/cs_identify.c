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




/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User * u)
{
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_IDENTIFY);
}

class CommandCSIdentify : public Command
{
 public:
	CommandCSIdentify(const std::string &cname) : Command(cname, 2, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *pass = params[1].c_str();
		ChannelInfo *ci;
		struct u_chaninfolist *uc;

		if (!(ci = cs_findchan(chan)))
		{
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
			return MOD_CONT;
		}

		if (!nick_identified(u))
		{
			notice_lang(s_ChanServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
			return MOD_CONT;
		}

		if (is_founder(u, ci))
		{
			notice_lang(s_ChanServ, u, NICK_ALREADY_IDENTIFIED);
			return MOD_CONT;
		}

		int res;

		if ((res = enc_check_password(pass, ci->founderpass)) == 1)
		{
			if (!is_identified(u, ci))
			{
				uc = new u_chaninfolist;
				uc->prev = NULL;
				uc->next = u->founder_chans;
				if (u->founder_chans)
					u->founder_chans->prev = uc;
				u->founder_chans = uc;
				uc->chan = ci;
				alog("%s: %s!%s@%s identified for %s", s_ChanServ, u->nick,
					 u->GetIdent().c_str(), u->host, ci->name);
			}

			notice_lang(s_ChanServ, u, CHAN_IDENTIFY_SUCCEEDED, chan);
		}
		else if (res < 0)
		{
			alog("%s: check_password failed for %s", s_ChanServ, ci->name);
			notice_lang(s_ChanServ, u, CHAN_IDENTIFY_FAILED);
		}
		else
		{
			alog("%s: Failed IDENTIFY for %s by %s!%s@%s",
				 s_ChanServ, ci->name, u->nick, u->GetIdent().c_str(), u->host);
			notice_lang(s_ChanServ, u, PASSWORD_INCORRECT);
			bad_password(u);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_IDENTIFY);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "IDENTIFY", CHAN_IDENTIFY_SYNTAX);
	}
};

class CSIdentify : public Module
{
 public:
	CSIdentify(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSIdentify("IDENTIFY"), MOD_UNIQUE);
		// XXX: we need aliases.
		this->AddCommand(CHANSERV, new CommandCSIdentify("ID"), MOD_UNIQUE);

		this->SetChanHelp(myChanServHelp);
	}
};


MODULE_INIT("cs_identify", CSIdentify)
