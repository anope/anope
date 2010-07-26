/* ChanServ core functions
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

class CommandCSHelp : public Command
{
 public:
	CommandCSHelp() : Command("HELP", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetFlag(CFLAG_STRIP_CHANNEL);
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string cmd = params[0];

		if (cmd.equals_ci("LEVELS DESC"))
		{
			int i;
			notice_help(Config.s_ChanServ, u, CHAN_HELP_LEVELS_DESC);
			if (!levelinfo_maxwidth)
				for (i = 0; levelinfo[i].what >= 0; ++i)
				{
					int len = levelinfo[i].name.length();
					if (len > levelinfo_maxwidth)
						levelinfo_maxwidth = len;
				}
			for (i = 0; levelinfo[i].what >= 0; ++i)
				notice_help(Config.s_ChanServ, u, CHAN_HELP_LEVELS_DESC_FORMAT, levelinfo_maxwidth, levelinfo[i].name.c_str(), getstring(u, levelinfo[i].desc));
		}
		else
			mod_help_cmd(ChanServ, u, cmd);

		return MOD_CONT;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP);
		for (CommandMap::const_iterator it = ChanServ->Commands.begin(); it != ChanServ->Commands.end(); ++it)
			if (!Config.HidePrivilegedCommands || it->second->permission.empty() || (u->Account() && u->Account()->HasCommand(it->second->permission)))
				it->second->OnServHelp(u);
		if (Config.CSExpire >= 86400)
			notice_help(Config.s_ChanServ, u, CHAN_HELP_EXPIRES, Config.CSExpire / 86400);
		if (u->Account() && u->Account()->IsServicesOper())
			notice_help(Config.s_ChanServ, u, CHAN_SERVADMIN_HELP);
	}
};

class CSHelp : public Module
{
 public:
	CSHelp(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, new CommandCSHelp());
	}
};

MODULE_INIT(CSHelp)
