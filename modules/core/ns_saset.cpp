/* NickServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandNSSASet : public Command
{
	typedef std::map<Anope::string, Command *, std::less<ci::string> > subcommand_map;
	subcommand_map subcommands;

 public:
	CommandNSSASet() : Command("SASET", 2, 4)
	{
	}

	~CommandNSSASet()
	{
		this->subcommands.clear();
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &nick = params[0];
		const Anope::string &cmd = params[1];

		if (readonly)
		{
			source.Reply(NICK_SET_DISABLED);
			return MOD_CONT;
		}

		NickAlias *na = findnick(nick);
		if (!na)
			source.Reply(NICK_SASET_BAD_NICK, nick.c_str());
		else if (na->HasFlag(NS_FORBIDDEN))
			source.Reply(NICK_X_FORBIDDEN, na->nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			source.Reply(NICK_X_SUSPENDED, na->nick.c_str());
		else
		{
			Command *c = this->FindCommand(params[1]);

			if (c)
			{
				Anope::string cmdparams = na->nick;
				for (std::vector<Anope::string>::const_iterator it = params.begin() + 2; it != params.end(); ++it)
					cmdparams += " " + *it;
				/* Don't log the whole message for saset password */
				if (c->name != "PASSWORD")
					Log(LOG_ADMIN, u, this) << params[1] << " " << cmdparams;
				else
					Log(LOG_ADMIN, u, this) << params[1] << " for " << params[0];
				mod_run_cmd(NickServ, u, c, params[1], cmdparams, false);
			}
			else
				source.Reply(NICK_SASET_UNKNOWN_OPTION, cmd.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		if (subcommand.empty())
		{
			source.Reply(NICK_HELP_SASET_HEAD);
			for (subcommand_map::iterator it = this->subcommands.begin(), it_end = this->subcommands.end(); it != it_end; ++it)
				it->second->OnServHelp(source);
			source.Reply(NICK_HELP_SASET_TAIL);
			return true;
		}
		else
		{
			Command *c = this->FindCommand(subcommand);

			if (c)
				return c->OnHelp(source, subcommand);
		}

		return false;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SASET", NICK_SASET_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_SASET);
	}

	bool AddSubcommand(Command *c)
	{
		return this->subcommands.insert(std::make_pair(c->name, c)).second;
	}

	bool DelSubcommand(Command *c)
	{
		return this->subcommands.erase(c->name);
	}

	Command *FindCommand(const Anope::string &subcommand)
	{
		subcommand_map::const_iterator it = this->subcommands.find(subcommand);

		if (it != this->subcommands.end())
			return it->second;

		return NULL;
	}
};

class CommandNSSASetDisplay : public Command
{
 public:
	CommandNSSASetDisplay() : Command("DISPLAY", 2, 2, "nickserv/saset/display")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		NickAlias *setter_na = findnick(params[0]);
		if (!setter_na)
			throw CoreException("NULL na in CommandNSSASetDisplay");
		NickCore *nc = setter_na->nc;

		NickAlias *na = findnick(params[1]);
		if (!na || na->nc != nc)
		{
			source.Reply(NICK_SASET_DISPLAY_INVALID, nc->display.c_str());
			return MOD_CONT;
		}

		change_core_display(nc, params[1]);
		source.Reply(NICK_SET_DISPLAY_CHANGED, nc->display.c_str());
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(NICK_HELP_SASET_DISPLAY);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SASET", NICK_SASET_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_SASET_DISPLAY);
	}
};

class CommandNSSASetPassword : public Command
{
 public:
	CommandNSSASetPassword() : Command("PASSWORD", 2, 2, "nickserv/saset/password")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickAlias *setter_na = findnick(params[0]);
		if (!setter_na)
			throw CoreException("NULL na in CommandNSSASetPassword");
		NickCore *nc = setter_na->nc;

		size_t len = params[1].length();

		if (Config->NSSecureAdmins && u->Account() != nc && nc->IsServicesOper())
		{
			source.Reply(ACCESS_DENIED);
			return MOD_CONT;
		}
		else if (nc->display.equals_ci(params[1]) || (Config->StrictPasswords && len < 5))
		{
			source.Reply(MORE_OBSCURE_PASSWORD);
			return MOD_CONT;
		}
		else if (len > Config->PassLen)
		{
			source.Reply(PASSWORD_TOO_LONG);
			return MOD_CONT;
		}

		if (enc_encrypt(params[1], nc->pass))
		{
			Log(NickServ) << "Failed to encrypt password for " << nc->display << " (saset)";
			source.Reply(NICK_SASET_PASSWORD_FAILED, nc->display.c_str());
			return MOD_CONT;
		}

		Anope::string tmp_pass;
		if (enc_decrypt(nc->pass, tmp_pass) == 1)
			source.Reply(NICK_SASET_PASSWORD_CHANGED_TO, nc->display.c_str(), tmp_pass.c_str());
		else
			source.Reply(NICK_SASET_PASSWORD_CHANGED, nc->display.c_str());

		if (Config->WallSetpass)
			ircdproto->SendGlobops(NickServ, "\2%s\2 used SASET PASSWORD on \2%s\2", u->nick.c_str(), nc->display.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(NICK_HELP_SASET_PASSWORD);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET", NICK_SASET_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_SASET_PASSWORD);
	}
};

class NSSASet : public Module
{
	CommandNSSASet commandnssaset;
	CommandNSSASetDisplay commandnssasetdisplay;
	CommandNSSASetPassword commandnssasetpassword;

 public:
	NSSASet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnssaset);

		commandnssaset.AddSubcommand(&commandnssasetdisplay);
		commandnssaset.AddSubcommand(&commandnssasetpassword);
	}
};

MODULE_INIT(NSSASet)
