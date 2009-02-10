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

class CommandOSRaw : public Command
{
 public:
	CommandOSRaw() : Command("RAW", 1, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *text = params[0].c_str();
		send_cmd(NULL, "%s", text);
		if (WallOSRaw)
		{
			std::string kw;
			spacesepstream textsep(text);
			while (textsep.GetString(kw) && kw[0] == ':');
			ircdproto->SendGlobops(s_OperServ, "\2%s\2 used RAW command for \2%s\2", u->nick, !kw.empty() ? kw.c_str() : "\2non RFC compliant message\2");
		}
		alog("%s used RAW command for %s", u->nick, text);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_root(u))
			return false;

		notice_lang(s_OperServ, u, OPER_HELP_RAW);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "RAW", OPER_RAW_SYNTAX);
	}
};

class OSRaw : public Module
{
 public:
	OSRaw(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(THIRD);

		c = createCommand("RAW", do_raw, is_services_root, OPER_HELP_RAW, -1, -1, -1, -1);
		this->AddCommand(OPERSERV, c, MOD_UNIQUE);

		if (DisableRaw)
			throw ModuleException("os_raw: Not loading because you probably shouldn't be loading me");
	}
};

MODULE_INIT("os_raw", OSRaw)
