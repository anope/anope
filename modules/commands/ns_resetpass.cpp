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

static bool SendResetEmail(User *u, const NickAlias *na, const BotInfo *bi);

class CommandNSResetPass : public Command
{
 public:
	CommandNSResetPass(Module *creator) : Command(creator, "nickserv/resetpass", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Helps you reset lost passwords"));
		this->SetSyntax(_("\037nickname\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const NickAlias *na;

		if (Config->RestrictMail && !source.HasCommand("nickserv/resetpass"))
			source.Reply(ACCESS_DENIED);
		else if (!(na = NickAlias::Find(params[0])))
			source.Reply(NICK_X_NOT_REGISTERED, params[0].c_str());
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

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sends a code key to the nickname with instructions on how to\n"
				"reset their password."));
		return true;
	}
};

struct ResetInfo : ExtensibleItem
{
	Anope::string code;
	time_t time;
};

class NSResetPass : public Module
{
	CommandNSResetPass commandnsresetpass;

 public:
	NSResetPass(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnsresetpass(this)
	{
		this->SetAuthor("Anope");

		if (!Config->UseMail)
			throw ModuleException("Not using mail.");


		ModuleManager::Attach(I_OnPreCommand, this);
	}

	~NSResetPass()
	{
		for (nickcore_map::const_iterator it = NickCoreList->begin(), it_end = NickCoreList->end(); it != it_end; ++it)
			it->second->Shrink("ns_resetpass");
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) anope_override
	{
		if (command->name == "nickserv/confirm" && params.size() > 1)
		{
			NickAlias *na = NickAlias::Find(params[0]);

			ResetInfo *ri = na ? na->nc->GetExt<ResetInfo *>("ns_resetpass") : NULL;
			if (na && ri)
			{
				NickCore *nc = na->nc;
				const Anope::string &passcode = params[1];
				if (ri->time < Anope::CurTime - 3600)
				{
					nc->Shrink("ns_resetpass");
					source.Reply(_("Your password reset request has expired."));
				}
				else if (passcode.equals_cs(ri->code))
				{
					nc->Shrink("ns_resetpass");

					Log(LOG_COMMAND, source, &commandnsresetpass) << "confirmed RESETPASS to forcefully identify to " << na->nick;

					nc->UnsetFlag(NI_UNCONFIRMED);

					if (source.GetUser())
					{
						source.GetUser()->Identify(na);
						source.Reply(_("You are now identified for your nick. Change your password now."));
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

static bool SendResetEmail(User *u, const NickAlias *na, const BotInfo *bi)
{
	int min = 1, max = 62;
	int chars[] = {
		' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
		'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
		'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
		'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
		'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
	};

	Anope::string passcode;
	int idx;
	for (idx = 0; idx < 20; ++idx)
		passcode += chars[1 + static_cast<int>((static_cast<float>(max - min)) * static_cast<uint16_t>(rand()) / 65536.0) + min];

	Anope::string subject = Language::Translate(na->nc, Config->MailResetSubject.c_str());
	Anope::string message = Language::Translate(na->nc, Config->MailResetMessage.c_str());

	subject = subject.replace_all_cs("%n", na->nick);
	subject = subject.replace_all_cs("%N", Config->NetworkName);
	subject = subject.replace_all_cs("%c", passcode);

	message = message.replace_all_cs("%n", na->nick);
	message = message.replace_all_cs("%N", Config->NetworkName);
	message = message.replace_all_cs("%c", passcode);

	ResetInfo *ri = new ResetInfo;
	ri->code = passcode;
	ri->time = Anope::CurTime;
	NickCore *nc = na->nc;
	nc->Extend("ns_resetpass", ri);

	return Mail::Send(u, nc, bi, subject, message);
}

MODULE_INIT(NSResetPass)
