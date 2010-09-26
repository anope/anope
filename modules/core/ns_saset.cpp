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

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string nick = params[0];
		Anope::string cmd = params[1];

		if (readonly)
		{
			u->SendMessage(NickServ, NICK_SET_DISABLED);
			return MOD_CONT;
		}

		NickAlias *na = findnick(nick);
		if (!na)
			u->SendMessage(NickServ, NICK_SASET_BAD_NICK, nick.c_str());
		else if (na->HasFlag(NS_FORBIDDEN))
			u->SendMessage(NickServ, NICK_X_FORBIDDEN, na->nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			u->SendMessage(NickServ, NICK_X_SUSPENDED, na->nick.c_str());
		else
		{
			Command *c = this->FindCommand(params[1]);

			if (c)
			{
				Anope::string cmdparams = na->nc->display;
				for (std::vector<Anope::string>::const_iterator it = params.begin() + 2; it != params.end(); ++it)
					cmdparams += " " + *it;
				Log(LOG_ADMIN, u, this) << params[1] << " " << cmdparams;
				mod_run_cmd(NickServ, u, c, params[1], cmdparams);
			}
			else
				u->SendMessage(NickServ, NICK_SASET_UNKNOWN_OPTION, cmd.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		if (subcommand.empty())
		{
			u->SendMessage(NickServ, NICK_HELP_SASET_HEAD);
			for (subcommand_map::iterator it = this->subcommands.begin(), it_end = this->subcommands.end(); it != it_end; ++it)
				it->second->OnServHelp(u);
			u->SendMessage(NickServ, NICK_HELP_SASET_TAIL);
			return true;
		}
		else
		{
			Command *c = this->FindCommand(subcommand);

			if (c)
				return c->OnHelp(u, subcommand);
		}

		return false;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(NickServ, u, "SASET", NICK_SASET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(NickServ, NICK_HELP_CMD_SASET);
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

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		if (!nc)
			throw CoreException("NULL nc in CommandNSSASetDisplay");

		NickAlias *na = findnick(params[1]);
		if (!na || na->nc != nc)
		{
			u->SendMessage(NickServ, NICK_SASET_DISPLAY_INVALID, nc->display.c_str());
			return MOD_CONT;
		}

		change_core_display(nc, params[1]);
		u->SendMessage(NickServ, NICK_SET_DISPLAY_CHANGED, nc->display.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(NickServ, NICK_HELP_SASET_DISPLAY);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		// XXX
		SyntaxError(NickServ, u, "SASET", NICK_SASET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(NickServ, NICK_HELP_CMD_SASET_DISPLAY);
	}
};

class CommandNSSASetPassword : public Command
{
 public:
	CommandNSSASetPassword() : Command("PASSWORD", 2, 2, "nickserv/saset/password")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		if (!nc)
			throw CoreException("NULL nc in CommandNSSASetPassword");

		size_t len = params[1].length();

		if (Config->NSSecureAdmins && u->Account() != nc && nc->IsServicesOper())
		{
			u->SendMessage(NickServ, ACCESS_DENIED);
			return MOD_CONT;
		}
		else if (nc->display.equals_ci(params[1]) || (Config->StrictPasswords && len < 5))
		{
			u->SendMessage(NickServ, MORE_OBSCURE_PASSWORD);
			return MOD_CONT;
		}
		else if (len > Config->PassLen)
		{
			u->SendMessage(NickServ, PASSWORD_TOO_LONG);
			return MOD_CONT;
		}

		if (enc_encrypt(params[1], nc->pass))
		{
			// XXX 
			//Alog() << Config->s_NickServ << ": Failed to encrypt password for " << nc->display << " (saset)";
			u->SendMessage(NickServ, NICK_SASET_PASSWORD_FAILED, nc->display.c_str());
			return MOD_CONT;
		}

		Anope::string tmp_pass;
		if (enc_decrypt(nc->pass, tmp_pass) == 1)
			u->SendMessage(NickServ, NICK_SASET_PASSWORD_CHANGED_TO, nc->display.c_str(), tmp_pass.c_str());
		else
			u->SendMessage(NickServ, NICK_SASET_PASSWORD_CHANGED, nc->display.c_str());

		if (Config->WallSetpass)
			ircdproto->SendGlobops(NickServ, "\2%s\2 used SASET PASSWORD on \2%s\2", u->nick.c_str(), nc->display.c_str());

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(NickServ, NICK_HELP_SASET_PASSWORD);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		SyntaxError(NickServ, u, "SASET", NICK_SASET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(NickServ, NICK_HELP_CMD_SASET_PASSWORD);
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
