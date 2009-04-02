/* Routines for looking up commands in a *Serv command list.
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

#include "services.h"
#include "commands.h"
#include "language.h"
#include "hashcomp.h"

/*************************************************************************/

/**
 * Return the Command corresponding to the given name, or NULL if no such
 * command exists.
 * @param list Command struct
 * @param cmd Command to look up
 * @return Command Struct for the given cmd
 */
Command *lookup_cmd(Command * list, char *cmd)
{
	Command *c;

	for (c = list; ; c++) {
		if (stricmp(c->name.c_str(), cmd) == 0) {
			return c;
		}
	}
	return NULL;
}

/*************************************************************************/

/**
 * Run the routine for the given command, if it exists and the user has
 * privilege to do so; if not, print an appropriate error message.
 * @param services Services Client
 * @param u User Struct
 * @param Command Hash Table
 * @param cmd Command
 * @return void
 */
void mod_run_cmd(char *service, User * u, CommandHash * cmdTable[], const char *cmd)
{
	Command *c = findCommand(cmdTable, cmd);
	int retVal = 0;


	if (!c)
	{
		if ((!checkDefCon(DEFCON_SILENT_OPER_ONLY)) || is_oper(u))
		{
			notice_lang(service, u, UNKNOWN_COMMAND_HELP, cmd, service);
		}
		return;
	}

	if ((checkDefCon(DEFCON_OPER_ONLY) || checkDefCon(DEFCON_SILENT_OPER_ONLY)) && !is_oper(u))
	{
		if (!checkDefCon(DEFCON_SILENT_OPER_ONLY))
		{
			notice_lang(service, u, OPER_DEFCON_DENIED);
			return;
		}
	}

	if (!c->HasFlag(CFLAG_ALLOW_UNREGISTERED))
	{
		// Command requires registered users only
		if (!nick_identified(u))
		{
			notice_lang(service, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
			alog("Access denied for unregistered user %s with service %s and command %s", u->nick, service, cmd);
			return;
		}
	}
	else
	{
		// Check whether or not access string is empty
	}

	std::vector<std::string> params;
	std::string curparam;
	char *s = NULL;
	while ((s = strtok(NULL, " ")))
	{
		// - 1 because params[0] corresponds with a maxparam of 1.
		if (params.size() >= (c->MaxParams - 1))
		{
			curparam += s;
			curparam += " ";
		}
		else
		{
			params.push_back(s);
		}
	}

	if (!curparam.empty())
	{
		// Remove trailing space
		curparam.erase(curparam.size() - 1, curparam.size());

		// Add it
		params.push_back(curparam);
	}

	if (params.size() < c->MinParams)
	{
		c->OnSyntaxError(u);
		return;
	}

	EventReturn MOD_RESULT = EVENT_CONTINUE;
	FOREACH_RESULT(I_OnPreCommand, OnPreCommand(u, c->name, params));
	if (MOD_RESULT == EVENT_STOP)
		return;

	retVal = c->Execute(u, params);
}

/*************************************************************************/

/**
 * Prints the help message for a given command.
 * @param services Services Client
 * @param u User Struct
 * @param Command Hash Table
 * @param cmd Command
 * @return void
 */
void mod_help_cmd(char *service, User * u, CommandHash * cmdTable[],
				  const char *cmd)
{
	spacesepstream tokens(cmd);
	std::string token;
	tokens.GetToken(token);

	Command *c = findCommand(cmdTable, token.c_str());

	std::string subcommand = tokens.StreamEnd() ? "" : tokens.GetRemaining();

	if (!c->OnHelp(u, subcommand))
		notice_lang(service, u, NO_HELP_AVAILABLE, cmd);
}

/*************************************************************************/
