/* Declarations for command data.
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifndef COMMAND_H
#define COMMAND_H

#include "service.h"
#include "anope.h"

enum CommandFlag
{
	CFLAG_ALLOW_UNREGISTERED,
	CFLAG_STRIP_CHANNEL
};

const Anope::string CommandFlagStrings[] = {
	"CFLAG_ALLOW_UNREGISTERED",
	"CFLAG_STRIP_CHANNEL",
	""
};

struct CommandInfo
{
	Anope::string name;
	Anope::string permission;
};

struct CommandReply
{
	virtual void SendMessage(const BotInfo *source, const Anope::string &msg) = 0;
};

/* The source for a command */
class CoreExport CommandSource
{
	/* The nick executing the command */
	Anope::string nick;
	/* User executing the command, may be NULL */
	User *u;
 public:
	/* The account executing the command */
	NickCore *nc;
	/* Where the reply should go */
	CommandReply *reply;
	/* Channel the command was executed on (fantasy) */
	Channel *c;
	/* The service this command is on */
	BotInfo *owner;
	/* The service the reply should come from, *not* necessarily the service the command is on */
	BotInfo *service;
	/* The actual name of the command being executed */
	Anope::string command;
	/* The permission of the command being executed */
	Anope::string permission;

	CommandSource(const Anope::string &n, User *user, NickCore *core, CommandReply *reply);

	const Anope::string &GetNick() const;
	User *GetUser() const;
	AccessGroup AccessFor(ChannelInfo *ci) const;
	bool IsFounder(ChannelInfo *ci) const;

	void Reply(const char *message, ...);
	void Reply(const Anope::string &message);

	bool HasCommand(const Anope::string &cmd);
	bool HasPriv(const Anope::string &cmd);
	bool IsServicesOper() const;
	bool IsOper() const;
};

/** Every services command is a class, inheriting from Command.
 */
class CoreExport Command : public Service, public Flags<CommandFlag>
{
	Anope::string desc;
	std::vector<Anope::string> syntax;

 public:
 	/* Maximum paramaters accepted by this command */
	size_t MaxParams;
	/* Minimum parameters required to use this command */
	size_t MinParams;

	/* Module which owns us */
	Module *module;

	/** Create a new command.
	 * @param owner The owner of the command
	 * @param sname The command name
	 * @param min_params The minimum number of parameters the parser will require to execute this command
	 * @param max_params The maximum number of parameters the parser will create, after max_params, all will be combined into the last argument.
	 * NOTE: If max_params is not set (default), there is no limit to the max number of params.
	 */
	Command(Module *owner, const Anope::string &sname, size_t min_params, size_t max_params = 0);

	virtual ~Command();

 protected:
	void SetDesc(const Anope::string &d);

	void ClearSyntax();
	void SetSyntax(const Anope::string &s);
	void SendSyntax(CommandSource &);
	void SendSyntax(CommandSource &, const Anope::string &syntax);
 public:
 	/** Get the command description
	 * @return The commands description
	 */
 	const Anope::string &GetDesc() const;

	/** Execute this command.
	 * @param source The source
	 * @param params Command parameters
	 */
	virtual void Execute(CommandSource &source, const std::vector<Anope::string> &params) = 0;

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
};

extern void RunCommand(CommandSource &source, const Anope::string &message);

#endif // COMMANDS_H
