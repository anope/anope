/* Declarations for command data.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include "services.h"

class Module;
class BotInfo;
class Command;

typedef std::map<ci::string, Command *> CommandMap;

/** The return value from commands.
 *  */
enum CommandReturn
{
	MOD_CONT,
	MOD_STOP
};

extern CoreExport Command *FindCommand(BotInfo *bi, const ci::string &cmd);
extern CoreExport void mod_help_cmd(BotInfo *bi, User *u, const ci::string &cmd);
extern CoreExport void mod_run_cmd(BotInfo *bi, User *u, const ci::string &cmd);

enum CommandFlag
{
	CFLAG_ALLOW_UNREGISTERED,
	CFLAG_ALLOW_FORBIDDEN,
	CFLAG_ALLOW_SUSPENDED,
	CFLAG_ALLOW_UNREGISTEREDCHANNEL,
	CFLAG_STRIP_CHANNEL,
	CFLAG_DISABLE_FANTASY
};

/** Every services command is a class, inheriting from Command.
 */
class CoreExport Command : public Flags<CommandFlag>
{
 public:
 	/* Maximum paramaters accepted by this command */
	size_t MaxParams;
	/* Minimum parameters required to use this command */
	size_t MinParams;
	/* Command name */
	ci::string name;
	/* Permission needed to use this comand */
	std::string permission;

	/* Module which owns us */
	Module *module;
	/* Service this command is on */
	BotInfo *service;

	/** Create a new command.
	 * @param sname The command name
	 * @param min_params The minimum number of parameters the parser will require to execute this command
	 * @param max_params The maximum number of parameters the parser will create, after max_params, all will be combined into the last argument.
	 * NOTE: If max_params is not set (default), there is no limit to the max number of params.
	 */
	Command(const ci::string &sname, size_t min_params, size_t max_params = 0, const std::string &spermission = "");

	virtual ~Command();

	/** Execute this command.
	 * @param u The user executing the command.
	 */
	virtual CommandReturn Execute(User *u, const std::vector<ci::string> &);

	/** Requested when the user is requesting help on this command. Help on this command should be sent to the user.
	 * @param u The user requesting help
	 * @param subcommand The subcommand the user is requesting help on, or an empty string. (e.g. /ns help set foo bar lol gives a subcommand of 
"FOO BAR LOL")
	 * @return true if help was provided to the user, false otherwise.
	 */
	virtual bool OnHelp(User *u, const ci::string &subcommand);

	/** Requested when the user provides bad syntax to this command (not enough params, etc).
	 * @param u The user executing the command.
	 * @param subcommand The subcommand the user tried to use
	 */
	virtual void OnSyntaxError(User *u, const ci::string &subcommand);

	/** Set which command permission (e.g. chanserv/forbid) is required for this command.
	 * @param reststr The permission required to successfully execute this command
	 */
	void SetPermission(const std::string &reststr);
};

#endif

