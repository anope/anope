#include "services.h"
#include "modules.h"

Command::Command(const std::string &sname, size_t min_params, size_t max_params) : MaxParams(max_params), MinParams(min_params), name(sname)
{
	this->has_priv = NULL;
	this->help_param1 = NULL;
	this->help_param2 = NULL;
	this->help_param3 = NULL;
	this->help_param4 = NULL;
	this->core = 0;
	this->next = NULL;
	this->mod_name = NULL;
	this->service = NULL;
	this->all_help = NULL;
	this->regular_help = NULL;
	this->oper_help = NULL;
	this->admin_help = NULL;
	this->root_help = NULL;
}

Command::~Command()
{
	this->has_priv = NULL;
	if (this->mod_name) {
		delete [] this->mod_name;
	}
	if (this->service) {
		delete [] this->service;
	}
	this->next = NULL;
}

/** Execute this command.
 * @param u The user executing the command.
 */
CommandReturn Command::Execute(User *u, std::vector<std::string> &) { return MOD_CONT; }

/** Requested when the user is requesting help on this command. Help on this command should be sent to the user.
 * @param u The user requesting help
 * @param subcommand The subcommand the user is requesting help on, or an empty string. (e.g. /ns help set foo bar lol gives a subcommand of "FOO BAR LOL")
 * @return true if help was provided to the user, false otherwise.
 */
bool Command::OnHelp(User *u, const std::string &subcommand) { return false; }

/** Requested when the user provides bad syntax to this command (not enough params, etc).
 * @param u The user executing the command.
 */
void Command::OnSyntaxError(User *u) { }
