/* OperServ core functions
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

class CommandOSChanList : public Command
{
 public:
	CommandOSChanList() : Command("CHANLIST", 0, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		const char *pattern = params.size() > 0 ? params[0].c_str() : NULL;
		ci::string opt = params.size() > 1 ? params[1] : "";

		int modes = 0;
		User *u2;

		if (!opt.empty() && opt == "SECRET")
			modes |= (anope_get_secret_mode() | anope_get_private_mode());

		if (pattern && (u2 = finduser(pattern)))
		{
			struct u_chanlist *uc;

			notice_lang(s_OperServ, u, OPER_CHANLIST_HEADER_USER, u2->nick);

			for (uc = u2->chans; uc; uc = uc->next)
			{
				if (modes && !(uc->chan->mode & modes))
					continue;
				notice_lang(s_OperServ, u, OPER_CHANLIST_RECORD, uc->chan->name, uc->chan->usercount, chan_get_modes(uc->chan, 1, 1), uc->chan->topic ? uc->chan->topic : "");
			}
		}
		else
		{
			int i;
			Channel *c;

			notice_lang(s_OperServ, u, OPER_CHANLIST_HEADER);

			for (i = 0; i < 1024; ++i)
			{
				for (c = chanlist[i]; c; c = c->next)
				{
					if (pattern && !Anope::Match(c->name, pattern, false))
						continue;
					if (modes && !(c->mode & modes))
						continue;
					notice_lang(s_OperServ, u, OPER_CHANLIST_RECORD, c->name, c->usercount, chan_get_modes(c, 1, 1), c->topic ? c->topic : "");
				}
			}
		}

		notice_lang(s_OperServ, u, OPER_CHANLIST_END);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_CHANLIST);
		return true;
	}
};

class OSChanList : public Module
{
 public:
	OSChanList(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSChanList());
	}
	void OperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_CHANLIST);
	}
};

MODULE_INIT(OSChanList)
