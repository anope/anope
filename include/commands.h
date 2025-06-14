/* Declarations for command data.
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

#include "service.h"
#include "anope.h"
#include "channels.h"
#include "textproc.h"

struct CommandGroup final
{
	Anope::string name, description;
};

/* Used in BotInfo::commands */
struct CommandInfo final
{
	typedef Anope::map<CommandInfo> map;

	/* Service name of the command */
	Anope::string name;
	/* Permission required to execute the command */
	Anope::string permission;
	/* Group this command is in */
	Anope::string group;
	/* whether or not to hide this command in help output */
	bool hide = false;
	/* Whether to prepend the channel name (only used with fantasy) */
	bool prepend_channel = false;
	/* Whether to require the FANTASY privilege (only used with fantasy) */
	bool require_privilege = true;
};

/* Where the replies from commands go to. User inherits from this and is the normal
 * source of a CommandReply
 */
struct CoreExport CommandReply
{
	virtual ~CommandReply() = default;
	virtual void SendMessage(BotInfo *source, const Anope::string &msg) = 0;
	virtual void SendMessage(CommandSource &source, const Anope::string &msg);
};

/* The source for a command */
class CoreExport CommandSource final
{
	/* The nick executing the command */
	Anope::string nick;
	/* User executing the command, may be NULL */
	Reference<User> u;
public:
	/* The account executing the command */
	Reference<NickCore> nc;
	/* for web clients */
	Anope::string ip;
	/* Where the reply should go */
	CommandReply *reply;
	/* Channel the command was executed on (fantasy) */
	Reference<Channel> c = nullptr;
	/* The service this command is on */
	Reference<BotInfo> service;
	/* The actual name of the command being executed */
	Anope::string command;
	/* The permission of the command being executed */
	Anope::string permission;
	/* The unique identifier of the executing message. */
	Anope::string msgid;

	CommandSource(const Anope::string &n, User *user, NickCore *core, CommandReply *reply, BotInfo *bi, const Anope::string &m = "");

	const Anope::string &GetNick() const;
	User *GetUser();
	NickCore *GetAccount();
	AccessGroup AccessFor(ChannelInfo *ci);
	bool IsFounder(ChannelInfo *ci);

	void Reply(const char *message, ...) ATTR_FORMAT(2, 3);
	void Reply(int count, const char *singular, const char *plural, ...) ATTR_FORMAT(4, 5);
	void Reply(const Anope::string &message);

	bool HasCommand(const Anope::string &cmd);
	bool HasPriv(const Anope::string &cmd);
	bool IsServicesOper();
	bool IsOper();
};

/** Every services command is a class, inheriting from Command.
 */
class CoreExport Command
	: public Service
{
	Anope::string desc;
	std::vector<std::pair<Anope::string, std::function<bool(CommandSource&)>>> syntax;
	/* Allow unregistered users to use this command */
	bool allow_unregistered = false;
	/* Command requires that a user is executing it */
	bool require_user = false;

public:
	/* Maximum parameters accepted by this command */
	size_t max_params;
	/* Minimum parameters required to use this command */
	size_t min_params;

	/* Module which owns us */
	Module *module;

protected:
	/** Create a new command.
	 * @param owner The owner of the command
	 * @param sname The command name
	 * @param min_params The minimum number of parameters the parser will require to execute this command
	 * @param max_params The maximum number of parameters the parser will create, after max_params, all will be combined into the last argument.
	 * NOTE: If max_params is not set (default), there is no limit to the max number of params.
	 */
	Command(Module *owner, const Anope::string &sname, size_t min_params, size_t max_params = 0);

public:
	virtual ~Command() = default;

protected:
	void SetDesc(const Anope::string &d);

	void ClearSyntax();
	void SetSyntax(const Anope::string &s, const std::function<bool(CommandSource&)> &p = nullptr);
	void SendSyntax(CommandSource &);

	void AllowUnregistered(bool b);
	void RequireUser(bool b);

public:
	inline bool AllowUnregistered() const { return this->allow_unregistered; }
	inline bool RequireUser() const { return this->require_user; }

	/** Get the command description
	 * @param source The source wanting the command description
	 * @return The commands description
	 */
	virtual const Anope::string GetDesc(CommandSource &source) const;

	/** Execute this command.
	 * @param source The source
	 * @param params Command parameters
	 */
	virtual void Execute(CommandSource &source, const std::vector<Anope::string> &params) = 0;

	/** Called when help is requested for the client this command is on.
	 * @param source The source
	 */
	virtual void OnServHelp(CommandSource &source, HelpWrapper &help);

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

	/** Runs a command
	 * @param source The source of the command
	 * @param message The full message to run, the command is at the beginning of the message
	 */
	static bool Run(CommandSource &source, const Anope::string &message);

	bool Run(CommandSource &source, const Anope::string &, const CommandInfo &, std::vector<Anope::string> &params);

	/** Looks up a command name from the service name.
	 * Note that if the same command exists multiple places this will return the first one encountered
	 * @param command_service The command service to lookup, eg, nickserv/register
	 * @param bot If found, is set to the bot the command is on, eg NickServ
	 * @param name If found, is set to the command name, eg REGISTER
	 * @return true if the given command service exists
	 */
	static bool FindCommandFromService(const Anope::string &command_service, BotInfo *&bi, Anope::string &name);
};
