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
	std::map<ci::string, Command *> subcommands;
 public:
	CommandNSSASet(const ci::string &cname) : Command(cname, 2, 4)
	{
	}

	~CommandNSSASet()
	{
		for (std::map<ci::string, Command *>::const_iterator it = this->subcommands.begin(), it_end = this->subcommands.end(); it != it_end; ++it)
			delete it->second;
		this->subcommands.clear();
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		ci::string cmd = params[1];

		if (readonly)
		{
			notice_lang(Config.s_NickServ, u, NICK_SASET_DISABLED);
			return MOD_CONT;
		}

		NickAlias *na = findnick(nick);
		if (!na)
			notice_lang(Config.s_NickServ, u, NICK_SASET_BAD_NICK, nick);
		else if (na->HasFlag(NS_FORBIDDEN))
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
		else if (na->nc->HasFlag(NI_SUSPENDED))
			notice_lang(Config.s_NickServ, u, NICK_X_SUSPENDED, na->nick);
		else
		{
			Command *c = this->FindCommand(params[1]);

			if (c)
			{
				ci::string cmdparams = na->nc->display;
				for (std::vector<ci::string>::const_iterator it = params.begin() + 1; it != params.end(); ++it)
					cmdparams += " " + *it;
				mod_run_cmd(NickServ, u, c, params[1], cmdparams);
			}
			else
				notice_lang(Config.s_NickServ, u, NICK_SASET_UNKNOWN_OPTION, cmd.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		if (subcommand.empty())
		{
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_HEAD);
			for (std::map<ci::string, Command *>::iterator it = this->subcommands.begin(), it_end = this->subcommands.end(); it != it_end; ++it)
				it->second->OnServHelp(u);
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_TAIL);
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

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_NickServ, u, "SASET", NICK_SASET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SASET);
	}

	bool AddSubcommand(Command *c)
	{
		return this->subcommands.insert(std::make_pair(c->name, c)).second;
	}

	bool DelSubcommand(const ci::string &command)
	{
		return this->subcommands.erase(command);
	}

	Command *FindCommand(const ci::string &subcommand)
	{
		std::map<ci::string, Command *>::const_iterator it = this->subcommands.find(subcommand);

		if (it != this->subcommands.end())
			return it->second;

		return NULL;
	}
};

class CommandNSSASetDisplay : public Command
{
 public:
	CommandNSSASetDisplay(const ci::string &cname) : Command(cname, 2, 2, "nickserv/saset/display")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		assert(nc);

		NickAlias *na = findnick(params[1]);
		if (!na || na->nc != nc)
		{
			notice_lang(Config.s_NickServ, u, NICK_SASET_DISPLAY_INVALID, nc->display);
			return MOD_CONT;
		}

		change_core_display(nc, params[1].c_str());
		notice_lang(Config.s_NickServ, u, NICK_SASET_DISPLAY_CHANGED, nc->display);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SASET_DISPLAY);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		// XXX
		syntax_error(Config.s_NickServ, u, "SASET", NICK_SASET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SASET_DISPLAY);
	}
};

class CommandNSSASetPassword : public Command
{
 public:
	CommandNSSASetPassword(const ci::string &cname) : Command(cname, 2, 2, "nickserv/saset/password")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		assert(nc);

		size_t len = params[1].size();

		if (Config.NSSecureAdmins && u->Account() != nc && nc->IsServicesOper())
		{
			notice_lang(Config.s_NickServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}
		else if (nc->display == params[1] || (Config.StrictPasswords && len < 5))
		{
			notice_lang(Config.s_NickServ, u, MORE_OBSCURE_PASSWORD);
			return MOD_CONT;
		}
		else if (len > Config.PassLen)
		{
			notice_lang(Config.s_NickServ, u, PASSWORD_TOO_LONG);
			return MOD_CONT;
		}

		std::string buf = params[1].c_str();
		if (enc_encrypt(buf, nc->pass))
		{
			Alog() << Config.s_NickServ << ": Failed to encrypt password for " << nc->display << " (saset)";
			notice_lang(Config.s_NickServ, u, NICK_SASET_PASSWORD_FAILED, nc->display);
			return MOD_CONT;
		}

		std::string tmp_pass;
		if (enc_decrypt(nc->pass, tmp_pass) == 1)
			notice_lang(Config.s_NickServ, u, NICK_SASET_PASSWORD_CHANGED_TO, nc->display, tmp_pass.c_str());
		else
			notice_lang(Config.s_NickServ, u, NICK_SASET_PASSWORD_CHANGED, nc->display);

		Alog() << Config.s_NickServ << ": " << u->GetMask() << " used SASET PASSWORD on " << nc->display << " (e-mail: "<< (nc->email ? nc->email : "none")  << ")";

		if (Config.WallSetpass)
			ircdproto->SendGlobops(NickServ, "\2%s\2 used SASET PASSWORD on \2%s\2", u->nick.c_str(), nc->display);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SASET_PASSWORD);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_NickServ, u, "SASET", NICK_SASET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SASET_PASSWORD);
	}
};

class NSSASet : public Module
{
public:
	NSSASet(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = new CommandNSSASet("SASET");
		this->AddCommand(NickServ, c);
		c->AddSubcommand(new CommandNSSASetDisplay("DISPLAY"));
		c->AddSubcommand(new CommandNSSASetPassword("PASSWORD"));
	}
};

MODULE_INIT(NSSASet)
