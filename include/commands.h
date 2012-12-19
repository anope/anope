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
#include "channels.h"

enum CommandFlag
{
	/* Command allow unidentified users to use it */
	CFLAG_ALLOW_UNREGISTERED,
	
	/* Command's first parameter is a channel name */
	CFLAG_STRIP_CHANNEL,

	/* Command requires a user to execute */
	CFLAG_REQUIRE_USER
};

/* Used in BotInfo::commands */
struct CommandInfo
{
	typedef Anope::map<CommandInfo> map;

	/* Service name of the command */
	Anope::string name;
	/* Permission required to execute the command */
	Anope::string permission;
};

/* Where the replies from commands go to. User inheits from this and is the normal
 * source of a CommandReply
 */
struct CommandReply
{
	virtual ~CommandReply() { }
	virtual void SendMessage(const BotInfo *source, const Anope::string &msg) = 0;
};

/* The source for a command */
class CoreExport CommandSource
{
	/* The nick executing the command */
	Anope::string nick;
	/* User executing the command, may be NULL */
	Reference<User> u;
 public:
	/* The account executing the command */
	Reference<NickCore> nc;
	/* Where the reply should go */
	CommandReply *reply;
	/* Channel the command was executed on (fantasy) */
	Reference<Channel> c;
	/* The service this command is on */
	Reference<BotInfo> service;
	/* The actual name of the command being executed */
	Anope::string command;
	/* The permission of the command being executed */
	Anope::string permission;

	CommandSource(const Anope::string &n, User *user, NickCore *core, CommandReply *reply, BotInfo *bi);

	const Anope::string &GetNick() const;
	User *GetUser();
	NickCore *GetAccount();
	AccessGroup AccessFor(ChannelInfo *ci);
	bool IsFounder(ChannelInfo *ci);

	void Reply(const char *message, ...);
	void Reply(const Anope::string &message);

	bool HasCommand(const Anope::string &cmd);
	bool HasPriv(const Anope::string &cmd);
	bool IsServicesOper();
	bool IsOper();
};

/** Every services command is a class, inheriting from Command.
 */
class CoreExport Command : public Service, public Flags<CommandFlag>
{
	Anope::string desc;
	std::vector<Anope::string> syntax;

 public:
 	/* Maximum paramaters accepted by this command */
	size_t max_params;
	/* Minimum parameters required to use this command */
	size_t min_params;

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

extern CoreExport void RunCommand(CommandSource &source, const Anope::string &message);

#endif // COMMANDS_H
