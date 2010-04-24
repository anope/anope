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

#define MAX_CMD_HASH 1024
#define CMD_HASH(x)       (((x)[0]&31)<<5 | ((x)[1]&31))        /* Will gen a hash from a string :) */

#define HOSTSERV HS_cmdTable /* using HOSTSERV etc. looks nicer than HS_cmdTable for modules */
#define BOTSERV BS_cmdTable
#define MEMOSERV MS_cmdTable
#define NICKSERV NS_cmdTable
#define CHANSERV CS_cmdTable
#define OPERSERV OS_cmdTable

#ifndef _WIN32
#define MDE
#else
#ifndef MODULE_COMPILE
#define MDE __declspec(dllexport)
#else
#define MDE __declspec(dllimport)
#endif
#endif

/** The return value from commands.
 *  */
enum CommandReturn
{
	MOD_CONT,
	MOD_STOP
};

class Command;

extern MDE Command *lookup_cmd(Command *list, char *name);
extern MDE void mod_help_cmd(char *service, User *u, CommandHash *cmdTable[], const char *cmd);
extern MDE void mod_run_cmd(const std::string &service, User *u, CommandHash *cmdTable[], const char *cmd);

struct CommandHash {
	char *name;     /* Name of the command */
	Command *c;     /* Actual command */
	CommandHash *next; /* Next command */
};

extern MDE CommandHash *HOSTSERV[MAX_CMD_HASH];
extern MDE CommandHash  *BOTSERV[MAX_CMD_HASH];
extern MDE CommandHash *MEMOSERV[MAX_CMD_HASH];
extern MDE CommandHash *NICKSERV[MAX_CMD_HASH];
extern MDE CommandHash *CHANSERV[MAX_CMD_HASH];
extern MDE CommandHash *OPERSERV[MAX_CMD_HASH];

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
	size_t MaxParams;
	size_t MinParams;
	ci::string name;
	std::string permission;

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

	/* Module related stuff */
	int core;		  /* Can this command be deleted? */
	char *mod_name; /* Name of the module who owns us, NULL for core's  */
	char *service;  /* Service we provide this command for */
	Command *next;
};

#endif

