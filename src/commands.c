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
	Command *current;

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
		if (!u->na || !u->na->nc)
		{
			// XXX: we should have a new string for this
			notice_lang(service, u, ACCESS_DENIED);
			alog("Access denied for unregistered user %s with service %s and command %s", u->nick, service, cmd);
			return;
		}
	}
	else
	{
		// Check whether or not access string is empty
	}

/*
XXX: priv checking
	if (c->has_priv != NULL && !c->has_priv(u))
	{
		notice_lang(service, u, ACCESS_DENIED);
		alog("Access denied for %s with service %s and command %s", u->nick, service, cmd);
		return;
	}
  */
  
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

	retVal = c->Execute(u, params);
	if (retVal == MOD_CONT)
	{
		current = c->next;
		while (current && retVal == MOD_CONT)
		{
			retVal = current->Execute(u, params);
			current = current->next;
		}
	}
}

/*************************************************************************/

/**
 * Output the 'Limited to' line for the given command
 * @param service Services Client
 * @param u User Struct
 * @param c Command Struct
 * @return void
 *
void do_help_limited(char *service, User * u, Command * c)
{
	if (c->has_priv == is_services_oper)
		notice_lang(service, u, HELP_LIMIT_SERV_OPER);
	else if (c->has_priv == is_services_admin)
		notice_lang(service, u, HELP_LIMIT_SERV_ADMIN);
	else if (c->has_priv == is_services_root)
		notice_lang(service, u, HELP_LIMIT_SERV_ROOT);
	else if (c->has_priv == is_oper)
		notice_lang(service, u, HELP_LIMIT_IRC_OPER);
	else if (c->has_priv == is_host_setter)
		notice_lang(service, u, HELP_LIMIT_HOST_SETTER);
	else if (c->has_priv == is_host_remover)
		notice_lang(service, u, HELP_LIMIT_HOST_REMOVER);
}
*/

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
	Command *c = findCommand(cmdTable, cmd);
	Command *current;
	bool has_had_help = false;
	int cont = MOD_CONT;
	const char *p1 = NULL, *p2 = NULL, *p3 = NULL, *p4 = NULL;

	for (current = c; (current) && (cont == MOD_CONT);
		 current = current->next) {
		p1 = current->help_param1;
		p2 = current->help_param2;
		p3 = current->help_param3;
		p4 = current->help_param4;
		has_had_help = current->OnHelp(u, ""); // XXX: this needs finishing to actually pass a subcommand
	}
	if (has_had_help == false) {
		notice_lang(service, u, NO_HELP_AVAILABLE, cmd);
	}
	//else {
	//	do_help_limited(service, u, c);
	//}
}

/*************************************************************************/
