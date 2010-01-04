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
	int retVal = MOD_CONT;
	ChannelInfo *ci;
	EventReturn MOD_RESULT;

	FOREACH_RESULT(I_OnPreCommandRun, OnPreCommandRun(service, u, cmd, c));
	if (MOD_RESULT == EVENT_STOP)
		return;
	
	if (!c)
	{
		notice_lang(service, u, UNKNOWN_COMMAND_HELP, cmd, service);
		return;
	}

	if (!c->HasFlag(CFLAG_ALLOW_UNREGISTERED))
	{
		// Command requires registered users only
		if (!nick_identified(u))
		{
			notice_lang(service, u, NICK_IDENTIFY_REQUIRED, Config.s_NickServ);
			alog("Access denied for unregistered user %s with service %s and command %s", u->nick.c_str(), service, cmd);
			return;
		}
	}

	std::vector<ci::string> params;
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
		params.push_back(curparam.c_str());
	}

	if (params.size() < c->MinParams)
	{
		c->OnSyntaxError(u, !params.empty() ? params[params.size() - 1] : "");
		return;
	}

	FOREACH_RESULT(I_OnPreCommand, OnPreCommand(u, c->service, c->name.c_str(), params));
	if (MOD_RESULT == EVENT_STOP)
		return;

	if (params.size() > 0 && !c->HasFlag(CFLAG_STRIP_CHANNEL) && (cmdTable == CHANSERV || cmdTable == BOTSERV))
	{
		if (ircdproto->IsChannelValid(params[0].c_str()))
		{
			if ((ci = cs_findchan(params[0].c_str())))
			{
				if ((ci->HasFlag(CI_FORBIDDEN)) && (!c->HasFlag(CFLAG_ALLOW_FORBIDDEN)))
				{
					notice_lang(service, u, CHAN_X_FORBIDDEN, ci->name.c_str());
					alog("Access denied for user %s with service %s and command %s because of FORBIDDEN channel %s",
						u->nick.c_str(), service, cmd, ci->name.c_str());
					return;
				}
				else if ((ci->HasFlag(CI_SUSPENDED)) && (!c->HasFlag(CFLAG_ALLOW_SUSPENDED)))
				{
					notice_lang(service, u, CHAN_X_FORBIDDEN, ci->name.c_str());
					alog("Access denied for user %s with service %s and command %s because of SUSPENDED channel %s",
						u->nick.c_str(), service, cmd, ci->name.c_str());
					return;
				}
			}
			else if (!c->HasFlag(CFLAG_ALLOW_UNREGISTEREDCHANNEL))
			{
				notice_lang(service, u, CHAN_X_NOT_REGISTERED, params[0].c_str());
				return;
			}
		}
		/* A user not giving a channel name for a param that should be a channel */
		else
		{
			notice_lang(service, u, CHAN_X_INVALID, params[0].c_str());
			return;
		}
	}

	// If the command requires a permission, and they aren't registered or don't have the required perm, DENIED
	if (!c->permission.empty())
	{
		if (!u->nc->HasCommand(c->permission))
		{
			notice_lang(service, u, ACCESS_DENIED);
			alog("Access denied for user %s with service %s and command %s", u->nick.c_str(), service, cmd);
			return;
		}

	}

	retVal = c->Execute(u, params);

	if (retVal == MOD_CONT)
	{
		FOREACH_MOD(I_OnPostCommand, OnPostCommand(u, c->service, c->name.c_str(), params));
	}
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
void mod_help_cmd(char *service, User * u, CommandHash * cmdTable[], const char *cmd)
{
	spacesepstream tokens(cmd);
	std::string token;
	tokens.GetToken(token);

	Command *c = findCommand(cmdTable, token.c_str());

	ci::string subcommand = tokens.StreamEnd() ? "" : tokens.GetRemaining().c_str();

	if (!c || !c->OnHelp(u, subcommand))
		notice_lang(service, u, NO_HELP_AVAILABLE, cmd);
	else
	{
		u->SendMessage(service, " ");

		/* Inform the user what permission is required to use the command */
		if (!c->permission.empty())
			notice_lang(service, u, COMMAND_REQUIRES_PERM, c->permission.c_str());

		/* User isn't identified and needs to be to use this command */
		if (!c->HasFlag(CFLAG_ALLOW_UNREGISTERED) && !nick_identified(u))
			notice_lang(service, u, COMMAND_IDENTIFY_REQUIRED);
		/* User doesn't have the proper permission to use this command */
		else if (!c->permission.empty() && (!u->nc || (!u->nc->HasCommand(c->permission))))
			notice_lang(service, u, COMMAND_CANNOT_USE);
		/* User can use this command */
		else
			notice_lang(service, u, COMMAND_CAN_USE);
	}
}

/*************************************************************************/
