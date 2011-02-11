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

class CommandNSSetEmail : public Command
{
 public:
	CommandNSSetEmail(const Anope::string &spermission = "") : Command("EMAIL", 1, 2, spermission)
	{
		this->SetDesc("Associate an E-mail address with your nickname");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickAlias *na = findnick(params[0]);
		if (!na)
			throw CoreException("NULL na in CommandNSSetEmail");
		NickCore *nc = na->nc;

		Anope::string param = params.size() > 1 ? params[1] : "";

		if (param.empty() && Config->NSForceEmail)
		{
			source.Reply(_("You cannot unset the e-mail on this network."));
			return MOD_CONT;
		}
		else if (Config->NSSecureAdmins && u->Account() != nc && nc->IsServicesOper())
		{
			source.Reply(LanguageString::ACCESS_DENIED);
			return MOD_CONT;
		}
		else if (!param.empty() && !MailValidate(param))
		{
			source.Reply(LanguageString::MAIL_X_INVALID, param.c_str());
			return MOD_CONT;
		}

		if (!param.empty())
		{
			nc->email = param;
			source.Reply(_("E-mail address for \002%s\002 changed to \002%s\002."), nc->display.c_str(), param.c_str());
		}
		else
		{
			nc->email.clear();
			source.Reply(_("E-mail address for \002%s\002 unset."), nc->display.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SET EMAIL \037address\037\002\n"
				" \n"
				"Associates the given E-mail address with your nickname.\n"
				"This address will be displayed whenever someone requests\n"
				"information on the nickname with the \002INFO\002 command."));
		return true;
	}
};

class CommandNSSASetEmail : public CommandNSSetEmail
{
 public:
	CommandNSSASetEmail() : CommandNSSetEmail("nickserv/saset/email")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SASET \037nickname\037 EMAIL \037address\037\002\n"
				" \n"
				"Associates the given E-mail address with the nickname."));
		return true;
	}
};

class NSSetEmail : public Module
{
	CommandNSSetEmail commandnssetemail;
	CommandNSSASetEmail commandnssasetemail;

 public:
	NSSetEmail(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandnssetemail);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandnssasetemail);
	}

	~NSSetEmail()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand(&commandnssetemail);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand(&commandnssasetemail);
	}
};

MODULE_INIT(NSSetEmail)
