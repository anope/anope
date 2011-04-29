/* NickServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"
#include "nickserv.h"

class CommandNSHelp : public Command
{
 public:
	CommandNSHelp() : Command("HELP", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Displays this list and give information about commands"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		mod_help_cmd(nickserv->Bot(), source.u, NULL, params[0]);
		return MOD_CONT;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		source.Reply(_("\002%s\002 allows you to \"register\" a nickname and\n"
				"prevent others from using it. The following\n"
				"commands allow for registration and maintenance of\n"
				"nicknames; to use them, type \002%s%s \037command\037\002.\n"
				"For more information on a specific command, type\n"
				"\002%s%s HELP \037command\037\002."), Config->UseStrictPrivMsgString.c_str(), Config->s_NickServ.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->s_NickServ.c_str(), Config->s_NickServ.c_str());
		for (CommandMap::const_iterator it = nickserv->Bot()->Commands.begin(), it_end = nickserv->Bot()->Commands.end(); it != it_end; ++it)
			if (!Config->HidePrivilegedCommands || it->second->permission.empty() || u->HasCommand(it->second->permission))
				it->second->OnServHelp(source);
		if (u->IsServicesOper())
			source.Reply(_(" \n"
					"Services Operators can also drop any nickname without needing\n"
					"to identify for the nick, and may view the access list for\n"
					"any nickname (\002%s%s ACCESS LIST \037nick\037\002)."),
					Config->UseStrictPrivMsgString.c_str(), Config->s_NickServ.c_str());
		if (Config->NSExpire >= 86400)
			source.Reply(_("Nicknames that are not used anymore are subject to \n"
					"the automatic expiration, i.e. they will be deleted\n"
					"after %d days if not used."), Config->NSExpire / 86400);
		source.Reply(_(" \n"
				"\002NOTICE:\002 This service is intended to provide a way for\n"
				"IRC users to ensure their identity is not compromised.\n"
				"It is \002NOT\002 intended to facilitate \"stealing\" of\n"
				"nicknames or other malicious actions. Abuse of %s\n"
				"will result in, at minimum, loss of the abused\n"
				"nickname(s)."), Config->s_NickServ.c_str());
	}
};

class NSHelp : public Module
{
	CommandNSHelp commandnshelp;

 public:
	NSHelp(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!nickserv)
			throw ModuleException("NickServ is not loaded!");

		this->AddCommand(nickserv->Bot(), &commandnshelp);
	}
};

MODULE_INIT(NSHelp)
