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

static bool SendResetEmail(User *u, NickServ::Nick *na, ServiceBot *bi);

class CommandNSResetPass : public Command
{
 public:
	CommandNSResetPass(Module *creator) : Command(creator, "nickserv/resetpass", 2, 2)
	{
		this->SetDesc(_("Helps you reset lost passwords"));
		this->SetSyntax(_("\037account\037 \037email\037"));
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		NickServ::Nick *na = NickServ::FindNick(params[0]);

		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), params[0]);
			return;
		}

		if (!na->GetAccount()->GetEmail().equals_ci(params[1]))
		{
			source.Reply(_("Incorrect email address."));
			return;
		}

		if (SendResetEmail(source.GetUser(), na, source.service))
		{
			Log(LOG_COMMAND, source, this) << "for " << na->GetNick() << " (group: " << na->GetAccount()->GetDisplay() << ")";
			source.Reply(_("Password reset email for \002{0}\002 has been sent."), na->GetNick());
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sends a passcode to the email address of \037account\037 with instructions on how to reset their password. \037email\037 must be the email address associated to \037account\037."));
		return true;
	}
};

struct ResetInfo
{
	Anope::string code;
	time_t time;
};

class NSResetPass : public Module
	, public EventHook<Event::PreCommand>
{
	CommandNSResetPass commandnsresetpass;
	ExtensibleItem<ResetInfo> reset;

 public:
	NSResetPass(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::PreCommand>()
		, commandnsresetpass(this), reset(this, "reset")
	{
		if (!Config->GetBlock("mail")->Get<bool>("usemail"))
			throw ModuleException("Not using mail.");
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) override
	{
		if (command->name == "nickserv/confirm" && params.size() > 1)
		{
			if (Anope::ReadOnly)
			{
				source.Reply(_("Services are in read-only mode."));
				return EVENT_STOP;
			}

			NickServ::Nick *na = NickServ::FindNick(params[0]);

			ResetInfo *ri = na ? reset.Get(na->GetAccount()) : NULL;
			if (na && ri)
			{
				NickServ::Account *nc = na->GetAccount();
				const Anope::string &passcode = params[1];
				if (ri->time < Anope::CurTime - 3600)
				{
					reset.Unset(nc);
					source.Reply(_("Your password reset request has expired."));
				}
				else if (passcode.equals_cs(ri->code))
				{
					reset.Unset(nc);
					nc->UnsetS<bool>("UNCONFIRMED");

					Log(LOG_COMMAND, source, &commandnsresetpass) << "confirmed RESETPASS to forcefully identify as " << na->GetNick();

					if (source.GetUser())
					{
						source.GetUser()->Identify(na);
						source.Reply(_("You are now identified for \002{0}\002. Change your password now."), na->GetAccount()->GetDisplay());
					}
				}
				else
					return EVENT_CONTINUE;

				return EVENT_STOP;
			}
		}

		return EVENT_CONTINUE;
	}
};

static bool SendResetEmail(User *u, NickServ::Nick *na, ServiceBot *bi)
{
	Anope::string subject = Language::Translate(na->GetAccount(), Config->GetBlock("mail")->Get<Anope::string>("reset_subject").c_str()),
		message = Language::Translate(na->GetAccount(), Config->GetBlock("mail")->Get<Anope::string>("reset_message").c_str()),
		passcode = Anope::Random(20);

	subject = subject.replace_all_cs("%n", na->GetNick());
	subject = subject.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<Anope::string>("networkname"));
	subject = subject.replace_all_cs("%c", passcode);

	message = message.replace_all_cs("%n", na->GetNick());
	message = message.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<Anope::string>("networkname"));
	message = message.replace_all_cs("%c", passcode);

	na->GetAccount()->Extend<ResetInfo>("reset", ResetInfo{passcode, Anope::CurTime});

	return Mail::Send(u, na->GetAccount(), bi, subject, message);
}

MODULE_INIT(NSResetPass)
