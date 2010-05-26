/* Routines for looking up commands in a *Serv command list.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#include "services.h"
#include "modules.h"
#include "language.h"
#include "hashcomp.h"

Command *FindCommand(BotInfo *bi, const ci::string &name)
{
	if (!bi || bi->Commands.empty() || name.empty())
		return NULL;
	
	std::map<ci::string, Command *>::iterator it = bi->Commands.find(name);

	if (it != bi->Commands.end())
		return it->second;

	return NULL;
}

void mod_run_cmd(BotInfo *bi, User *u, const ci::string &cmd)
{
	if (!bi || !u || cmd.empty())
		return;

	Command *c = FindCommand(bi, cmd);
	int retVal = MOD_CONT;
	ChannelInfo *ci;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnPreCommandRun, OnPreCommandRun(bi, u, cmd, c));
	if (MOD_RESULT == EVENT_STOP)
		return;

	if (!c)
	{
		notice_lang(bi->nick, u, UNKNOWN_COMMAND_HELP, cmd.c_str(), bi->nick.c_str());
		return;
	}

	if (!c->HasFlag(CFLAG_ALLOW_UNREGISTERED))
	{
		// Command requires registered users only
		if (!u->IsIdentified())
		{
			notice_lang(bi->nick, u, NICK_IDENTIFY_REQUIRED, Config.s_NickServ);
			Alog() << "Access denied for unregistered user " << u->nick << " with service " << bi->nick << " and command " << cmd;
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

	FOREACH_RESULT(I_OnPreCommand, OnPreCommand(u, c->service, c->name, params));
	if (MOD_RESULT == EVENT_STOP)
		return;

	if (params.size() > 0 && !c->HasFlag(CFLAG_STRIP_CHANNEL) && (bi == ChanServ || bi == BotServ))
	{
		if (ircdproto->IsChannelValid(params[0].c_str()))
		{
			if ((ci = cs_findchan(params[0])))
			{
				if ((ci->HasFlag(CI_FORBIDDEN)) && (!c->HasFlag(CFLAG_ALLOW_FORBIDDEN)))
				{
					notice_lang(bi->nick, u, CHAN_X_FORBIDDEN, ci->name.c_str());
					Alog() << "Access denied for user " << u->nick << " with service " << bi->nick
						<< " and command " << cmd << " because of FORBIDDEN channel " << ci->name;
					return;
				}
				else if ((ci->HasFlag(CI_SUSPENDED)) && (!c->HasFlag(CFLAG_ALLOW_SUSPENDED)))
				{
					notice_lang(bi->nick, u, CHAN_X_FORBIDDEN, ci->name.c_str());
					Alog() << "Access denied for user " << u->nick << " with service " << bi->nick
						<<" and command " << cmd << " because of SUSPENDED channel " << ci->name;
					return;
				}
			}
			else if (!c->HasFlag(CFLAG_ALLOW_UNREGISTEREDCHANNEL))
			{
				notice_lang(bi->nick, u, CHAN_X_NOT_REGISTERED, params[0].c_str());
				return;
			}
		}
		/* A user not giving a channel name for a param that should be a channel */
		else
		{
			notice_lang(bi->nick, u, CHAN_X_INVALID, params[0].c_str());
			return;
		}
	}

	// If the command requires a permission, and they aren't registered or don't have the required perm, DENIED
	if (!c->permission.empty())
	{
		if (!u->Account()->HasCommand(c->permission))
		{
			notice_lang(bi->nick, u, ACCESS_DENIED);
			Alog() << "Access denied for user " << u->nick << " with service " << bi->nick << " and command " << cmd;
			return;
		}

	}

	retVal = c->Execute(u, params);

	if (retVal == MOD_CONT)
	{
		FOREACH_MOD(I_OnPostCommand, OnPostCommand(u, c->service, c->name.c_str(), params));
	}
}

/**
 * Prints the help message for a given command.
 * @param services Services Client
 * @param u User Struct
 * @param Command Hash Table
 * @param cmd Command
 * @return void
 */
void mod_help_cmd(BotInfo *bi, User *u, const ci::string &cmd)
{
	if (!bi || !u || cmd.empty())
		return;
	
	spacesepstream tokens(cmd);
	ci::string token;
	tokens.GetToken(token);

	Command *c = FindCommand(bi, token);

	ci::string subcommand = tokens.StreamEnd() ? "" : tokens.GetRemaining().c_str();

	if (!c || !c->OnHelp(u, subcommand))
		notice_lang(bi->nick, u, NO_HELP_AVAILABLE, cmd.c_str());
	else
	{
		u->SendMessage(bi->nick, " ");

		/* Inform the user what permission is required to use the command */
		if (!c->permission.empty())
			notice_lang(bi->nick, u, COMMAND_REQUIRES_PERM, c->permission.c_str());

		/* User isn't identified and needs to be to use this command */
		if (!c->HasFlag(CFLAG_ALLOW_UNREGISTERED) && !u->IsIdentified())
			notice_lang(bi->nick, u, COMMAND_IDENTIFY_REQUIRED);
		/* User doesn't have the proper permission to use this command */
		else if (!c->permission.empty() && (!u->Account() || (!u->Account()->HasCommand(c->permission))))
			notice_lang(bi->nick, u, COMMAND_CANNOT_USE);
		/* User can use this command */
		else
			notice_lang(bi->nick, u, COMMAND_CAN_USE);
	}
}

