/* NickServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

static bool SendResetEmail(User *u, const NickAlias *na, BotInfo *bi);

class CommandNSResetPass final
	: public Command
{
public:
	CommandNSResetPass(Module *creator) : Command(creator, "nickserv/resetpass", 2, 2)
	{
		this->SetDesc(_("Helps you reset lost passwords"));
		this->SetSyntax(_("\037nickname\037 \037email\037"));
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const NickAlias *na;

		if (!(na = NickAlias::Find(params[0])))
			source.Reply(NICK_X_NOT_REGISTERED, params[0].c_str());
		else if (na->nc->HasExt("NS_SUSPENDED"))
			source.Reply(NICK_X_SUSPENDED, na->nc->display.c_str());
		else if (!na->nc->email.equals_ci(params[1]))
			source.Reply(_("Incorrect email address."));
		else
		{
			if (SendResetEmail(source.GetUser(), na, source.service))
			{
				Log(LOG_COMMAND, source, this) << "for " << na->nick << " (group: " << na->nc->display << ")";
				source.Reply(_("Password reset email for \002%s\002 has been sent."), na->nick.c_str());
			}
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sends a passcode to the nickname with instructions on how to\n"
				"reset their password.  Email must be the email address associated\n"
				"to the nickname."));
		return true;
	}
};

struct ResetInfo final
{
	Anope::string code;
	time_t time;
};

class NSResetPass final
	: public Module
{
	CommandNSResetPass commandnsresetpass;
	PrimitiveExtensibleItem<ResetInfo> reset;

public:
	NSResetPass(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnsresetpass(this), reset(this, "reset")
	{
		if (!Config->GetBlock("mail").Get<bool>("usemail"))
			throw ModuleException("Not using mail.");
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) override
	{
		if (command->name == "nickserv/confirm" && params.size() > 1)
		{
			if (Anope::ReadOnly)
			{
				source.Reply(READ_ONLY_MODE);
				return EVENT_STOP;
			}

			NickAlias *na = NickAlias::Find(params[0]);

			ResetInfo *ri = na ? reset.Get(na->nc) : NULL;
			if (na && ri)
			{
				NickCore *nc = na->nc;
				if (nc->HasExt("NS_SUSPENDED"))
				{
					source.Reply(NICK_X_SUSPENDED, nc->display.c_str());
					return EVENT_STOP;
				}

				const Anope::string &passcode = params[1];
				if (ri->time < Anope::CurTime - 3600)
				{
					reset.Unset(nc);
					source.Reply(_("Your password reset request has expired."));
				}
				else if (passcode.equals_cs(ri->code))
				{
					reset.Unset(nc);
					nc->Shrink<bool>("UNCONFIRMED");

					Log(LOG_COMMAND, source, &commandnsresetpass) << "to confirm RESETPASS and forcefully identify as " << na->nick;

					if (source.GetUser())
					{
						source.GetUser()->Identify(na);
					}

					source.Reply(_("You are now identified for your nick. Change your password now."));
				}
				else
					return EVENT_CONTINUE;

				return EVENT_STOP;
			}
		}

		return EVENT_CONTINUE;
	}
};

static bool SendResetEmail(User *u, const NickAlias *na, BotInfo *bi)
{
	auto *ri = na->nc->Extend<ResetInfo>("reset");
	ri->code = Anope::Random(Config->GetBlock("options").Get<size_t>("codelength", 15));
	ri->time = Anope::CurTime;

	Anope::map<Anope::string> vars = {
		{ "nick",    na->nick                                                                },
		{ "network", Config->GetBlock("networkinfo").Get<const Anope::string>("networkname") },
		{ "code",    ri->code                                                                },
	};

	auto subject = Anope::Template(Language::Translate(na->nc, Config->GetBlock("mail").Get<const Anope::string>("reset_subject").c_str()), vars);
	auto message = Anope::Template(Language::Translate(na->nc, Config->GetBlock("mail").Get<const Anope::string>("reset_message").c_str()), vars);

	return Mail::Send(u, na->nc, bi, subject, message);
}

MODULE_INIT(NSResetPass)
