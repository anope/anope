/* NickServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandNSGetPass : public Command
{
 public:
	CommandNSGetPass(Module *creator) : Command(creator, "nickserv/getpass", 1, 1)
	{
		this->SetDesc(_("Retrieve the password for a nickname"));
		this->SetSyntax(_("\037account\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &nick = params[0];
		Anope::string tmp_pass;
		NickServ::Nick *na = NickServ::FindNick(nick);

		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), nick);
			return;
		}

		if (Config->GetModule("nickserv")->Get<bool>("secureadmins", "yes") && na->GetAccount()->IsServicesOper())
		{
			source.Reply(_("You may not get the password of other Services Operators."));
			return;
		}

		if (!Anope::Decrypt(na->GetAccount()->GetPassword(), tmp_pass))
		{
			source.Reply(_("The \002{0}\002 command is unavailable because encryption is in use."), source.command);
			return;
		}

		Log(LOG_ADMIN, source, this) << "for " << na->GetNick();
		source.Reply(_("Password of \002{0}\02 is \002%s\002."), na->GetNick(), tmp_pass);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Returns the password for the given account. This command may not be available if password encryption is in use."));
		return true;
	}
};

class NSGetPass : public Module
{
	CommandNSGetPass commandnsgetpass;

 public:
	NSGetPass(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandnsgetpass(this)
	{

		Anope::string tmp_pass = "plain:tmp";
		if (!Anope::Decrypt(tmp_pass, tmp_pass))
			throw ModuleException("Incompatible with the encryption module being used");

	}
};

MODULE_INIT(NSGetPass)
