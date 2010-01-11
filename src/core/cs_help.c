/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
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

class CommandCSHelp : public Command
{
 public:
	CommandCSHelp() : Command("HELP", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetFlag(CFLAG_STRIP_CHANNEL);
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ci::string cmd = params[0];

		if (cmd == "LEVELS DESC")
		{
			int i;
			notice_help(Config.s_ChanServ, u, CHAN_HELP_LEVELS_DESC);
			if (!levelinfo_maxwidth)
			{
				for (i = 0; levelinfo[i].what >= 0; i++)
				{
					int len = strlen(levelinfo[i].name);
					if (len > levelinfo_maxwidth)
						levelinfo_maxwidth = len;
				}
			}
			for (i = 0; levelinfo[i].what >= 0; i++)
			{
				notice_help(Config.s_ChanServ, u, CHAN_HELP_LEVELS_DESC_FORMAT, levelinfo_maxwidth, levelinfo[i].name, getstring(u, levelinfo[i].desc));
			}
		}
		else
			mod_help_cmd(Config.s_ChanServ, u, CHANSERV, cmd.c_str());

		return MOD_CONT;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP);
		FOREACH_MOD(I_OnChanServHelp, OnChanServHelp(u));
		if (Config.CSExpire >= 86400)
			notice_help(Config.s_ChanServ, u, CHAN_HELP_EXPIRES, Config.CSExpire / 86400);
		if (u->nc && u->nc->IsServicesOper())
			notice_help(Config.s_ChanServ, u, CHAN_SERVADMIN_HELP);
	}
};


class CSHelp : public Module
{
 public:
	CSHelp(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSHelp());
	}
};

MODULE_INIT(CSHelp)
