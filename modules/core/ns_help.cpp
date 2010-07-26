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

class CommandNSHelp : public Command
{
 public:
	CommandNSHelp() : Command("HELP", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string cmd = params[0];

		if (cmd.equals_ci("SET LANGUAGE"))
		{
			int i;
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_LANGUAGE);
			for (i = 0; i < NUM_LANGS && langlist[i] >= 0; ++i)
				u->SendMessage(Config.s_NickServ, "    %2d) %s", i + 1, langnames[langlist[i]]);
		}
		else
			mod_help_cmd(NickServ, u, cmd);

		return MOD_CONT;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP);
		for (CommandMap::const_iterator it = NickServ->Commands.begin(), it_end = NickServ->Commands.end(); it != it_end; ++it)
			if (!Config.HidePrivilegedCommands || it->second->permission.empty() || (u->Account() && u->Account()->HasCommand(it->second->permission)))
				it->second->OnServHelp(u);
		if (u->Account() && u->Account()->IsServicesOper())
			notice_help(Config.s_NickServ, u, NICK_SERVADMIN_HELP);
		if (Config.NSExpire >= 86400)
			notice_help(Config.s_NickServ, u, NICK_HELP_EXPIRES, Config.NSExpire / 86400);
		notice_help(Config.s_NickServ, u, NICK_HELP_FOOTER);
	}
};

class NSHelp : public Module
{
 public:
	NSHelp(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, new CommandNSHelp());
	}
};

MODULE_INIT(NSHelp)
