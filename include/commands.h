/* Declarations for command data.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifndef COMMAND_H
#define COMMAND_H

#include "services.h"

class Module;
class BotInfo;
class Command;

typedef std::map<Anope::string, Command *, std::less<ci::string> > CommandMap;

/** The return value from commands.
 */
enum CommandReturn
{
	MOD_CONT,
	MOD_STOP
};

extern CoreExport Command *FindCommand(BotInfo *bi, const Anope::string &cmd);
extern CoreExport void mod_help_cmd(BotInfo *bi, User *u, ChannelInfo *ci, const Anope::string &cmd);
extern CoreExport void mod_run_cmd(BotInfo *bi, User *u, const Anope::string &message, ChannelInfo *ci);
extern CoreExport void mod_run_cmd(BotInfo *bi, User *u, Command *c, const Anope::string &command, const Anope::string &message, ChannelInfo *ci);

enum CommandFlag
{
	CFLAG_ALLOW_UNREGISTERED,
	CFLAG_ALLOW_FORBIDDEN,
	CFLAG_ALLOW_SUSPENDED,
	CFLAG_ALLOW_UNREGISTEREDCHANNEL,
	CFLAG_STRIP_CHANNEL,
	CFLAG_DISABLE_FANTASY
};

const Anope::string CommandFlagStrings[] = {
	"CFLAG_ALLOW_UNREGISTERED",
	"CFLAG_ALLOW_FORBIDDEN",
	"CFLAG_ALLOW_SUSPENDED",
	"CFLAG_ALLOW_UNREGISTEREDCHANNEL",
	"CFLAG_STRIP_CHANNEL",
	"CFLAG_DISABLE_FANTASY",
	""
};

/* The source for a command */
struct CoreExport CommandSource
{
	/* User executing the command */
	User *u;
	/* Channel (if applicable) */
	ChannelInfo *ci;
	/* The service this command is on */
	BotInfo *owner;
	/* The service the reply should come from, *not* necessarily the service the command is on */
	BotInfo *service;
	/* Whether or not this was a fantasy command */
	bool fantasy;

	std::list<Anope::string> reply;

	~CommandSource();

	void Reply(LanguageString message, ...);
	void Reply(const char *message, ...);
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
	Anope::string name;
	/* Permission needed to use this comand */
	Anope::string permission;

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
	Command(const Anope::string &sname, size_t min_params, size_t max_params = 0, const Anope::string &spermission = "");

	virtual ~Command();

	/** Execute this command.
	 * @param source The source
	 * @param params Command parameters
	 */
	virtual CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params) = 0;

	/** Called when HELP is requsted for the client this command is on.
	 * @param source The source
	 */
	virtual void OnServHelp(CommandSource &source);

	/** Requested when the user is requesting help on this command. Help on this command should be sent to the user.
	 * @param source The source
	 * @param subcommand The subcommand the user is requesting help on, or an empty string. (e.g. /ns help set foo bar lol gives a subcommand of "FOO BAR LOL")
	 * @return true if help was provided to the user, false otherwise.
	 */
	virtual bool OnHelp(CommandSource &source, const Anope::string &subcommand);

	/** Requested when the user provides bad syntax to this command (not enough params, etc).
	 * @param source The source
	 * @param subcommand The subcommand the user tried to use
	 */
	virtual void OnSyntaxError(CommandSource &source, const Anope::string &subcommand);

	/** Set which command permission (e.g. chanserv/forbid) is required for this command.
	 * @param reststr The permission required to successfully execute this command
	 */
	void SetPermission(const Anope::string &reststr);

	/** Add a subcommand to this command
	 * @param creator The creator of the subcommand
	 * @param c The command
	 */
	virtual bool AddSubcommand(Module *creator, Command *c);

	/** Delete a subcommand from this command
	 * @param c The command
	 */
	virtual bool DelSubcommand(Command *c);

	/** Find a subcommand
	 * @param name The subcommand name
	 * @return The subcommand
	 */
	virtual Command *FindSubcommand(const Anope::string &subcommand);
};

#endif // COMMANDS_H
