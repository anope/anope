/* Routines for looking up commands in a *Serv command list.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "hashcomp.h"

Command *FindCommand(BotInfo *bi, const Anope::string &name)
{
	if (!bi || bi->Commands.empty() || name.empty())
		return NULL;

	CommandMap::iterator it = bi->Commands.find(name);

	if (it != bi->Commands.end())
		return it->second;

	return NULL;
}

void mod_run_cmd(BotInfo *bi, User *u, const Anope::string &fullmessage, ChannelInfo *ci)
{
	if (!bi || !u)
		return;
	
	spacesepstream sep(fullmessage);
	Anope::string command, message;

	if (!sep.GetToken(command))
		return;
	message = sep.GetRemaining();
	
	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnPreCommandRun, OnPreCommandRun(u, bi, command, message, ci));
	if (MOD_RESULT == EVENT_STOP)
		return;
	
	Command *c = FindCommand(bi, command);

	mod_run_cmd(bi, u, c, command, message, ci);
}

void mod_run_cmd(BotInfo *bi, User *u, Command *c, const Anope::string &command, const Anope::string &message, ChannelInfo *ci)
{
	if (!bi || !u)
		return;

	if (!c)
	{
		u->SendMessage(bi, UNKNOWN_COMMAND_HELP, command.c_str(), bi->nick.c_str());
		return;
	}

	// Command requires registered users only
	if (!c->HasFlag(CFLAG_ALLOW_UNREGISTERED) && !u->IsIdentified())
	{
		u->SendMessage(bi, NICK_IDENTIFY_REQUIRED, Config->s_NickServ.c_str());
		Log(LOG_COMMAND, "denied", bi) << "Access denied for unregistered user " << u->GetMask() << " with command " << command;
		return;
	}

	std::vector<Anope::string> params;
	Anope::string curparam, endparam;
	spacesepstream sep(message);
	while (sep.GetToken(curparam))
	{
		// - 1 because params[0] corresponds with a maxparam of 1.
		if (params.size() >= c->MaxParams - 1)
			endparam += curparam + " ";
		else
			params.push_back(curparam);
	}

	if (!endparam.empty())
	{
		// Remove trailing space
		endparam.erase(endparam.length() - 1);

		// Add it
		params.push_back(endparam);
	}

	bool fantasy = ci != NULL;
	if (params.size() > 0 && !c->HasFlag(CFLAG_STRIP_CHANNEL) && (bi == ChanServ || bi == BotServ))
	{
		if (ircdproto->IsChannelValid(params[0]))
		{
			ci = cs_findchan(params[0]);
			if (ci)
			{
				if (ci->HasFlag(CI_FORBIDDEN) && !c->HasFlag(CFLAG_ALLOW_FORBIDDEN))
				{
					u->SendMessage(bi, CHAN_X_FORBIDDEN, ci->name.c_str());
					Log(LOG_COMMAND, "denied", bi) << "Access denied for user " << u->GetMask() << " with command " << command << " because of FORBIDDEN channel " << ci->name;
					return;
				}
				else if (ci->HasFlag(CI_SUSPENDED) && !c->HasFlag(CFLAG_ALLOW_SUSPENDED))
				{
					u->SendMessage(bi, CHAN_X_FORBIDDEN, ci->name.c_str());
					Log(LOG_COMMAND, "denied", bi) << "Access denied for user " << u->GetMask() << " with command " << command << " because of SUSPENDED channel " << ci->name;
					return;
				}
			}
			else if (!c->HasFlag(CFLAG_ALLOW_UNREGISTEREDCHANNEL))
			{
				u->SendMessage(bi, CHAN_X_NOT_REGISTERED, params[0].c_str());
				return;
			}
		}
		/* A user not giving a channel name for a param that should be a channel */
		else
		{
			u->SendMessage(bi, CHAN_X_INVALID, params[0].c_str());
			return;
		}
	}

	CommandSource source;
	source.u = u;
	source.ci = ci;
	source.owner = bi;
	source.service = fantasy && ci ? ci->bi : c->service;
	source.fantasy = fantasy;

	if (params.size() < c->MinParams)
	{
		c->OnSyntaxError(source, !params.empty() ? params[params.size() - 1] : "");
		return;
	}

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnPreCommand, OnPreCommand(source, c, params));
	if (MOD_RESULT == EVENT_STOP)
		return;

	// If the command requires a permission, and they aren't registered or don't have the required perm, DENIED
	if (!c->permission.empty() && !u->Account()->HasCommand(c->permission))
	{
		u->SendMessage(bi, ACCESS_DENIED);
		Log(LOG_COMMAND, "denied", bi) << "Access denied for user " << u->GetMask() << " with command " << command;
		return;
	}

	CommandReturn ret = c->Execute(source, params);

	if (ret == MOD_CONT)
	{
		FOREACH_MOD(I_OnPostCommand, OnPostCommand(source, c, params));
	}
}

/**
 * Prints the help message for a given command.
 * @param bi Client the command is on
 * @param u User
 * @param ci Optional channel the command was executed one (fantasy)
 * @param cmd Command
 */
void mod_help_cmd(BotInfo *bi, User *u, ChannelInfo *ci, const Anope::string &cmd)
{
	if (!bi || !u || cmd.empty())
		return;

	spacesepstream tokens(cmd);
	Anope::string token;
	tokens.GetToken(token);

	Command *c = FindCommand(bi, token);
	Anope::string subcommand = tokens.StreamEnd() ? "" : tokens.GetRemaining();

	CommandSource source;
	source.u = u;
	source.ci = ci;
	source.owner = bi;
	source.service = ci ? ci->bi : bi;
	source.fantasy = ci != NULL;

	if (!c || (Config->HidePrivilegedCommands && !c->permission.empty() && (!u->Account() || !u->Account()->HasCommand(c->permission))) || !c->OnHelp(source, subcommand))
		u->SendMessage(bi, NO_HELP_AVAILABLE, cmd.c_str());
	else
	{
		u->SendMessage(bi, " ");

		/* Inform the user what permission is required to use the command */
		if (!c->permission.empty())
			u->SendMessage(bi, COMMAND_REQUIRES_PERM, c->permission.c_str());

		/* User isn't identified and needs to be to use this command */
		if (!c->HasFlag(CFLAG_ALLOW_UNREGISTERED) && !u->IsIdentified())
			u->SendMessage(bi, COMMAND_IDENTIFY_REQUIRED);
		/* User doesn't have the proper permission to use this command */
		else if (!c->permission.empty() && (!u->Account() || !u->Account()->HasCommand(c->permission)))
			u->SendMessage(bi, COMMAND_CANNOT_USE);
		/* User can use this command */
		else
			u->SendMessage(bi, COMMAND_CAN_USE);
	}
}
