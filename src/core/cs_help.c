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

class CommandCSHelp : public Command
{
 public:
	CommandCSHelp() : Command("HELP", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetFlag(CFLAG_STRIP_CHANNEL);
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *cmd = params[0].c_str();

		if (!stricmp(cmd, "LEVELS DESC"))
		{
			int i;
			notice_help(s_ChanServ, u, CHAN_HELP_LEVELS_DESC);
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
				notice_help(s_ChanServ, u, CHAN_HELP_LEVELS_DESC_FORMAT, levelinfo_maxwidth, levelinfo[i].name, getstring(u, levelinfo[i].desc));
			}
		}
		else
			mod_help_cmd(s_ChanServ, u, CHANSERV, cmd);

		return MOD_CONT;
	}

	void OnSyntaxError(User *u)
	{
		notice_help(s_ChanServ, u, CHAN_HELP);
		moduleDisplayHelp(s_ChanServ, u);
		if (CSExpire >= 86400)
			notice_help(s_ChanServ, u, CHAN_HELP_EXPIRES, CSExpire / 86400);
		if (u->nc && u->nc->IsServicesOper())
			notice_help(s_ChanServ, u, CHAN_SERVADMIN_HELP);
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
		this->AddCommand(CHANSERV, new CommandCSHelp(), MOD_UNIQUE);
	}
};

MODULE_INIT("cs_help", CSHelp)
