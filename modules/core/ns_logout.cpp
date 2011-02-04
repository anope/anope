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

class CommandNSLogout : public Command
{
 public:
	CommandNSLogout() : Command("LOGOUT", 0, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &nick = !params.empty() ? params[0] : "";
		const Anope::string &param = params.size() > 1 ? params[1] : "";

		User *u2;
		if (!u->Account()->IsServicesOper() && !nick.empty())
			this->OnSyntaxError(source, "");
		else if (!(u2 = (!nick.empty() ? finduser(nick) : u)))
			source.Reply(LanguageString::NICK_X_NOT_IN_USE, nick.c_str());
		else if (!nick.empty() && u2->Account() && !u2->Account()->IsServicesOper())
			source.Reply(_("You can't logout %s because they are a Services Operator."), nick.c_str());
		else
		{
			if (!nick.empty() && !param.empty() && param.equals_ci("REVALIDATE"))
				validate_user(u2);

			u2->isSuperAdmin = 0; /* Dont let people logout and remain a SuperAdmin */
			Log(LOG_COMMAND, u, this) << "to logout " << u2->nick;

			/* Remove founder status from this user in all channels */
			if (!nick.empty())
				source.Reply(_("Nick %s has been logged out."), nick.c_str());
			else
				source.Reply(_("Your nick has been logged out."));

			ircdproto->SendAccountLogout(u2, u2->Account());
			u2->RemoveMode(NickServ, UMODE_REGISTERED);
			ircdproto->SendUnregisteredNick(u2);

			u2->Logout();

			/* Send out an event */
			FOREACH_MOD(I_OnNickLogout, OnNickLogout(u2));
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		if (u->Account() && u->Account()->IsServicesOper())
			source.Reply(_("Syntax: \002LOGOUT [\037nickname\037 [REVALIDATE]]\002\n"
					" \n"
					"Without a parameter, reverses the effect of the \002IDENTIFY\002 \n"
					"command, i.e. make you not recognized as the real owner of the nick\n"
					"anymore. Note, however, that you won't be asked to reidentify\n"
					"yourself.\n"
					" \n"
					"With a parameter, does the same for the given nick. If you \n"
					"specify REVALIDATE as well, Services will ask the given nick\n"
					"to re-identify. This use limited to \002Services Operators\002."));
		else
			source.Reply(_("Syntax: \002LOGOUT\002\n"
					" \n"
					"This reverses the effect of the \002IDENTIFY\002 command, i.e.\n"
					"make you not recognized as the real owner of the nick\n"
					"anymore. Note, however, that you won't be asked to reidentify\n"
					"yourself."));

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "LOGOUT", _("LOGOUT"));
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    LOGOUT     Reverses the effect of the IDENTIFY command"));
	}
};

class NSLogout : public Module
{
	CommandNSLogout commandnslogout;

 public:
	NSLogout(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnslogout);
	}
};

MODULE_INIT(NSLogout)
