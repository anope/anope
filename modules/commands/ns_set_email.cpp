/* NickServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

static bool SendConfirmMail(User *u, const BotInfo *bi)
{
	int chars[] = {
		' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
		'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
		'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
		'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
		'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
	};
	int idx, min = 1, max = 62;
	Anope::string code;
	for (idx = 0; idx < 9; ++idx)
		code += chars[1 + static_cast<int>((static_cast<float>(max - min)) * static_cast<uint16_t>(rand()) / 65536.0) + min];
	
	u->Account()->Extend("ns_set_email_passcode", new ExtensibleItemClass<Anope::string>(code));

	Anope::string subject = Config->MailEmailchangeSubject;
	Anope::string message = Config->MailEmailchangeMessage;

	subject = subject.replace_all_cs("%e", u->Account()->email);
	subject = subject.replace_all_cs("%N", Config->NetworkName);
	subject = subject.replace_all_cs("%c", code);

	message = message.replace_all_cs("%e", u->Account()->email);
	message = message.replace_all_cs("%N", Config->NetworkName);
	message = message.replace_all_cs("%c", code);

	return Mail(u, u->Account(), bi, subject, message);
}

class CommandNSSetEmail : public Command
{
 public:
	CommandNSSetEmail(Module *creator, const Anope::string &cname = "nickserv/set/email", size_t min = 0) : Command(creator, cname, min, min + 1)
	{
		this->SetDesc(_("Associate an E-mail address with your nickname"));
		this->SetSyntax(_("\037address\037"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		const NickAlias *na = findnick(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		if (param.empty() && Config->NSForceEmail)
		{
			source.Reply(_("You cannot unset the e-mail on this network."));
			return;
		}
		else if (Config->NSSecureAdmins && source.nc != nc && nc->IsServicesOper())
		{
			source.Reply(_("You may not change the email of other services operators."));
			return;
		}
		else if (!param.empty() && !MailValidate(param))
		{
			source.Reply(MAIL_X_INVALID, param.c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (!param.empty() && Config->NSConfirmEmailChanges && !source.IsServicesOper())
		{
			source.nc->Extend("ns_set_email", new ExtensibleItemClass<Anope::string>(param));
			Anope::string old = source.nc->email;
			source.nc->email = param;
			if (SendConfirmMail(source.GetUser(), source.owner))
				source.Reply(_("A confirmation email has been sent to \002%s\002. Follow the instructions in it to change your email address."), param.c_str());
			source.nc->email = old;
		}
		else
		{
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
		}

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, params.size() ? params[0] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Associates the given E-mail address with your nickname.\n"
				"This address will be displayed whenever someone requests\n"
				"information on the nickname with the \002INFO\002 command."));
		return true;
	}
};

class CommandNSSASetEmail : public CommandNSSetEmail
{
 public:
	CommandNSSASetEmail(Module *creator) : CommandNSSetEmail(creator, "nickserv/saset/email", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 \037address\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, params[0], params.size() > 1 ? params[1] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Associates the given E-mail address with the nickname."));
		return true;
	}
};

class NSSetEmail : public Module
{
	CommandNSSetEmail commandnssetemail;
	CommandNSSASetEmail commandnssasetemail;

 public:
	NSSetEmail(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssetemail(this), commandnssasetemail(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::Attach(I_OnPreCommand, this);

	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) anope_override
	{
		NickCore *uac = source.nc;
		if (command->name == "nickserv/confirm" && !params.empty() && uac)
		{
			Anope::string *new_email = uac->GetExt<ExtensibleItemClass<Anope::string> *>("ns_set_email"), *passcode = uac->GetExt<ExtensibleItemClass<Anope::string> *>("ns_set_email_passcode");
			if (new_email && passcode)
			{
				if (params[0] == *passcode)
				{
					uac->email = *new_email;
					Log(LOG_COMMAND, source, command) << "to confirm their email address change to " << uac->email;
					source.Reply(_("Your email address has been changed to \002%s\002."), uac->email.c_str());
					uac->Shrink("ns_set_email");
					uac->Shrink("ns_set_email_passcode");
					return EVENT_STOP;
				}
			}
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(NSSetEmail)
