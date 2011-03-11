/* NickServ core functions
 *
 * (C) 2003-2011 Anope Team
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
		this->SetDesc(_("Set SET-options on another nickname"));
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
			source.Reply(_(NICK_SET_DISABLED));
			return MOD_CONT;
		}

		NickAlias *na = findnick(nick);
		if (!na)
			source.Reply(_(NICK_X_NOT_REGISTERED), nick.c_str());
		else if (na->HasFlag(NS_FORBIDDEN))
			source.Reply(_(NICK_X_FORBIDDEN), na->nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			source.Reply(_(NICK_X_SUSPENDED), na->nick.c_str());
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
				mod_run_cmd(NickServ, u, NULL, c, params[1], cmdparams);
			}
			else
				source.Reply(_("Unknown SASET option \002%s\002."), cmd.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		if (subcommand.empty())
		{
			source.Reply(_("Syntax: \002SASET \037nickname\037 \037option\037 \037parameters\037\002.\n"
					" \n"
					"Sets various nickname options. \037option\037 can be one of:"));
			for (subcommand_map::iterator it = this->subcommands.begin(), it_end = this->subcommands.end(); it != it_end; ++it)
				it->second->OnServHelp(source);
			source.Reply(_("Type \002%R%s HELP SASET \037option\037\002 for more information\n"
					"on a specific option. The options will be set on the given\n"
					"\037nickname\037."), NickServ->nick.c_str());
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
		SyntaxError(source, "SASET", _(NICK_SASET_SYNTAX));
	}

	bool AddSubcommand(Module *creator, Command *c)
	{
		c->module = creator;
		c->service = this->service;
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
		this->SetDesc(_("Set the display of the group in Services"));
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
			source.Reply(_(NICK_SASET_DISPLAY_INVALID), nc->display.c_str());
			return MOD_CONT;
		}

		change_core_display(nc, params[1]);
		source.Reply(_(NICK_SET_DISPLAY_CHANGED), nc->display.c_str());
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SASET \037nickname\037 DISPLAY \037new-display\037\002\n"
				" \n"
				"Changes the display used to refer to the nickname group in \n"
				"Services. The new display MUST be a nick of the group."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SASET", _(NICK_SASET_SYNTAX));
	}
};

class CommandNSSASetPassword : public Command
{
 public:
	CommandNSSASetPassword() : Command("PASSWORD", 2, 2, "nickserv/saset/password")
	{
		this->SetDesc(_("Set the nickname password"));
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
			source.Reply(_(ACCESS_DENIED));
			return MOD_CONT;
		}
		else if (nc->display.equals_ci(params[1]) || (Config->StrictPasswords && len < 5))
		{
			source.Reply(_(MORE_OBSCURE_PASSWORD));
			return MOD_CONT;
		}
		else if (len > Config->PassLen)
		{
			source.Reply(_(PASSWORD_TOO_LONG));
			return MOD_CONT;
		}

		if (enc_encrypt(params[1], nc->pass))
		{
			Log(NickServ) << "Failed to encrypt password for " << nc->display << " (saset)";
			source.Reply(_(NICK_SASET_PASSWORD_FAILED), nc->display.c_str());
			return MOD_CONT;
		}

		Anope::string tmp_pass;
		if (enc_decrypt(nc->pass, tmp_pass) == 1)
			source.Reply(_(NICK_SASET_PASSWORD_CHANGED_TO), nc->display.c_str(), tmp_pass.c_str());
		else
			source.Reply(_(NICK_SASET_PASSWORD_CHANGED), nc->display.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SASET \037nickname\037 PASSWORD \037new-password\037\002\n"
				" \n"
				"Changes the password used to identify as the nick's owner."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET", _(NICK_SASET_SYNTAX));
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

		commandnssaset.AddSubcommand(this, &commandnssasetdisplay);
		commandnssaset.AddSubcommand(this, &commandnssasetpassword);
	}
};

MODULE_INIT(NSSASet)
