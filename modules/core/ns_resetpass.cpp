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

static bool SendResetEmail(User *u, NickAlias *na);

class CommandNSResetPass : public Command
{
 public:
	CommandNSResetPass() : Command("RESETPASS", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Helps you reset lost passwords"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickAlias *na;

		if (Config->RestrictMail && (!u->Account() || !u->Account()->HasCommand("nickserv/resetpass")))
			source.Reply(_(ACCESS_DENIED));
		else if (!(na = findnick(params[0])))
			source.Reply(_(NICK_X_NOT_REGISTERED), params[0].c_str());
		else if (na->HasFlag(NS_FORBIDDEN))
			source.Reply(_(NICK_X_FORBIDDEN), na->nick.c_str());
		else
		{
			if (SendResetEmail(u, na))
			{
				Log(LOG_COMMAND, u, this) << "for " << na->nick << " (group: " << na->nc->display << ")";
				source.Reply(_("Password reset email for \002%s\002 has been sent."), na->nick.c_str());
			}
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002RESETPASS \037nickname\037\002\n"
				"Sends a code key to the nickname with instructions on how to\n"
				"reset their password."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "RESETPASS", _("RESETPASS \037nickname\037\002"));
	}
};

class NSResetPass : public Module
{
	CommandNSResetPass commandnsresetpass;

 public:
	NSResetPass(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		if (!Config->UseMail)
			throw ModuleException("Not using mail.");

		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsresetpass);

		ModuleManager::Attach(I_OnPreCommand, this);
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		if (command->service == NickServ && command->name.equals_ci("CONFIRM") && params.size() > 1)
		{
			NickAlias *na = findnick(params[0]);

			time_t t;
			Anope::string c;
			if (na && na->nc->GetExtRegular("ns_resetpass_code", c) && na->nc->GetExtRegular("ns_resetpass_time", t))
			{
				const Anope::string &passcode = params[1];
				if (t < Anope::CurTime - 3600)
				{
					na->nc->Shrink("ns_resetpass_code");
					na->nc->Shrink("ns_resetpass_time");
					source.Reply(_("Your password reset request has expired."));
				}
				else if (passcode.equals_cs(c))
				{
					na->nc->Shrink("ns_resetpass_code");
					na->nc->Shrink("ns_resetpass_time");

					NickAlias *this_na = findnick(u->nick);

					if (this_na && this_na == na)
					{
						u->UpdateHost();
						na->last_realname = u->realname;
						na->last_seen = Anope::CurTime;
						u->SetMode(NickServ, UMODE_REGISTERED);
					}

					u->Login(na->nc);
					ircdproto->SendAccountLogin(u, u->Account());
					ircdproto->SetAutoIdentificationToken(u);
					na->nc->UnsetFlag(NI_UNCONFIRMED);
					FOREACH_MOD(I_OnNickIdentify, OnNickIdentify(u));

					Log(LOG_COMMAND, u, &commandnsresetpass) << "confirmed RESETPASS to forcefully identify to " << na->nick;
					source.Reply(_("You are now identified for your nick. Change your password using \"%R%s SET PASSWORD \002newpassword\002\" now."), Config->s_NickServ.c_str());

					if (ircd->vhost)
						do_on_id(u);
					if (Config->NSModeOnID)
						do_setmodes(u);
					check_memos(u);
				}
				else
					return EVENT_CONTINUE;

				return EVENT_STOP;
			}
		}

		return EVENT_CONTINUE;
	}
};

static bool SendResetEmail(User *u, NickAlias *na)
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
		passcode += chars[1 + static_cast<int>((static_cast<float>(max - min)) * getrandom16() / 65536.0) + min];

	Anope::string subject = Anope::printf(GetString(na->nc, "Reset password request for %s"), na->nick.c_str());
	Anope::string message = Anope::printf(GetString(na->nc,
	"Hi,\n"
	" \n"
	"You have requested to have the password for %s reset.\n"
	"To reset your password, type %R%s CONFIRM %s %s\n"
	" \n"
	"If you don't know why this mail was sent to you, please ignore it silently.\n"
	" \n"
	"%s administrators."), na->nick.c_str(), Config->s_NickServ.c_str(), na->nick.c_str(), passcode.c_str(), Config->NetworkName.c_str());

	if (Config->UseStrictPrivMsg)
		message = message.replace_all_cs("%R", "/");
	else
		message = message.replace_all_cs("%R", "/msg ");

	na->nc->Extend("ns_resetpass_code", new ExtensibleItemRegular<Anope::string>(passcode));
	na->nc->Extend("ns_resetpass_time", new ExtensibleItemRegular<time_t>(Anope::CurTime));

	return Mail(u, na->nc, NickServ, subject, message);
}

MODULE_INIT(NSResetPass)
