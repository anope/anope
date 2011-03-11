/* NickServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * A simple call to check for all emails that a user may have registered
 * with. It returns the nicks that match the email you provide. Wild
 * Cards are not excepted. Must use user@email-host.
 */

/*************************************************************************/

#include "module.h"

class CommandNSGetEMail : public Command
{
 public:
	CommandNSGetEMail() : Command("GETEMAIL", 1, 1, "nickserv/getemail")
	{
		this->SetDesc(_("Matches and returns all users that registered using given email"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &email = params[0];
		int j = 0;

		Log(LOG_ADMIN, u, this) << "on " << email;

		for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
		{
			NickCore *nc = it->second;

			if (!nc->email.empty() && nc->email.equals_ci(email))
			{
				++j;
				source.Reply(_("Emails Match \002%s\002 to \002%s\002."), nc->display.c_str(), email.c_str());
			}
		}

		if (j <= 0)
		{
			source.Reply(_("No Emails listed for \002%s\002."), email.c_str());
			return MOD_CONT;
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002GETEMAIL \037user@emailhost\037\002\n"
				"Returns the matching nicks that used given email. \002Note\002 that\n"
				"you can not use wildcards for either user or emailhost. Whenever\n"
				"this command is used, a message including the person who issued\n"
				"the command and the email it was used on will be logged."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "GETMAIL", _("GETEMAIL \002user@email-host\002 No WildCards!!"));
	}
};

class NSGetEMail : public Module
{
	CommandNSGetEMail commandnsgetemail;
 public:
	NSGetEMail(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsgetemail);
	}
};

MODULE_INIT(NSGetEMail)
