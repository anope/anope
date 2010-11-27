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

static bool SendRegmail(User *u, NickRequest *nr);

class CommandNSConfirm : public Command
{
 protected:
	CommandReturn ActuallyConfirmNick(CommandSource &source, NickRequest *nr, bool force)
	{
		User *u = source.u;
		NickAlias *na = new NickAlias(nr->nick, new NickCore(nr->nick));
		Anope::string tmp_pass;

		na->nc->pass = nr->password;

		na->nc->memos.memomax = Config->MSMaxMemos;

		if (force)
		{
			na->last_usermask = "*@*";
			na->last_realname = "unknown";
		}
		else
		{
			Anope::string last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
			na->last_usermask = last_usermask;
			na->last_realname = u->realname;
			if (Config->NSAddAccessOnReg)
				na->nc->AddAccess(create_mask(u));
		}

		na->time_registered = na->last_seen = Anope::CurTime;
		na->nc->language = Config->NSDefLanguage;
		if (!nr->email.empty())
			na->nc->email = nr->email;

		if (!force)
		{
			u->Login(na->nc);
			Log(LOG_COMMAND, u, this) << "to register " << u->nick << " (email: " << (!nr->email.empty() ? nr->email : "none") << ")";
			if (Config->NSAddAccessOnReg)
				source.Reply(NICK_REGISTERED, u->nick.c_str(), na->nc->GetAccess(0).c_str());
			else
				source.Reply(NICK_REGISTERED_NO_MASK, u->nick.c_str());
			delete nr;

			ircdproto->SendAccountLogin(u, u->Account());
			ircdproto->SetAutoIdentificationToken(u);

			if (enc_decrypt(na->nc->pass, tmp_pass) == 1)
				source.Reply(NICK_PASSWORD_IS, tmp_pass.c_str());

			u->lastnickreg = Anope::CurTime;
		}
		else
		{
			Log(LOG_COMMAND, u, this) << "to confirm " << u->nick << " (email: " << (!nr->email.empty() ? nr->email : "none") << ")";
			source.Reply(NICK_FORCE_REG, nr->nick.c_str());
			User *user = finduser(nr->nick);
			/* Delrequest must be called before validate_user */
			delete nr;
			if (user)
				validate_user(user);
		}

		FOREACH_MOD(I_OnNickRegister, OnNickRegister(na));

		return MOD_CONT;
	}

	CommandReturn DoConfirm(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		Anope::string passcode = !params.empty() ? params[0] : "";

		NickRequest *nr = findrequestnick(u->nick);

		if (Config->NSEmailReg)
		{
			if (passcode.empty())
			{
				this->OnSyntaxError(source, "");
				return MOD_CONT;
			}

			if (!nr)
			{
				if (u->Account() && u->Account()->HasPriv("nickserv/confirm"))
				{
					/* If an admin, their nick is obviously already regged, so look at the passcode to get the nick
					   of the user they are trying to validate, and push that user through regardless of passcode */
					nr = findrequestnick(passcode);
					if (nr)
					{
						ActuallyConfirmNick(source, nr, true);
						return MOD_CONT;
					}
				}
				source.Reply(NICK_CONFIRM_NOT_FOUND, Config->s_NickServ.c_str());

				return MOD_CONT;
			}

			if (!nr->passcode.equals_cs(passcode))
			{
				source.Reply(NICK_CONFIRM_INVALID);
				return MOD_CONT;
			}
		}

		if (!nr)
		{
			source.Reply(NICK_REGISTRATION_FAILED);
			return MOD_CONT;
		}

		ActuallyConfirmNick(source, nr, false);
		return MOD_CONT;
	}

 public:
	CommandNSConfirm(const Anope::string &cmdn, int min, int max) : Command(cmdn, min, max)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoConfirm(source, params);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		source.Reply(NICK_HELP_CONFIRM);
		if (u->Account() && u->Account()->HasPriv("nickserv/confirm"))
			source.Reply(NICK_HELP_CONFIRM_OPER);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(NICK_CONFIRM_INVALID);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_CONFIRM);
	}
};

class CommandNSRegister : public CommandNSConfirm
{
 public:
	CommandNSRegister() : CommandNSConfirm("REGISTER", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickRequest *nr = NULL, *anr = NULL;
		NickAlias *na;
		size_t prefixlen = Config->NSGuestNickPrefix.length();
		size_t nicklen = u->nick.length();
		Anope::string pass = params[0];
		Anope::string email = params.size() > 1 ? params[1] : "";
		Anope::string passcode;
		int idx, min = 1, max = 62;
		int chars[] = {
			' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
			'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
			'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
			'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
			'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
		};
		std::list<std::pair<Anope::string, Anope::string> >::iterator it, it_end;

		if (readonly)
		{
			source.Reply(NICK_REGISTRATION_DISABLED);
			return MOD_CONT;
		}

		if (!is_oper(u) && Config->NickRegDelay && Anope::CurTime - u->my_signon < Config->NickRegDelay)
		{
			source.Reply(NICK_REG_DELAY, Config->NickRegDelay);
			return MOD_CONT;
		}

		if ((anr = findrequestnick(u->nick)))
		{
			source.Reply(NICK_REQUESTED);
			return MOD_CONT;
		}

		/* Prevent "Guest" nicks from being registered. -TheShadow */

		/* Guest nick can now have a series of between 1 and 7 digits.
		 *   --lara
		 */
		if (nicklen <= prefixlen + 7 && nicklen >= prefixlen + 1 && !u->nick.find_ci(Config->NSGuestNickPrefix) && !u->nick.substr(prefixlen).find_first_not_of("1234567890"))
		{
			source.Reply(NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
			return MOD_CONT;
		}

		if (!ircdproto->IsNickValid(u->nick))
		{
			source.Reply(NICK_X_FORBIDDEN, u->nick.c_str());
			return MOD_CONT;
		}

		if (Config->RestrictOperNicks)
			for (it = Config->Opers.begin(), it_end = Config->Opers.end(); it != it_end; ++it)
			{
				Anope::string nick = it->first;

				if (u->nick.find_ci(nick) != Anope::string::npos && !is_oper(u))
				{
					source.Reply(NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
					return MOD_CONT;
				}
		}

		if (Config->NSForceEmail && email.empty())
			this->OnSyntaxError(source, "");
		else if (Anope::CurTime < u->lastnickreg + Config->NSRegDelay)
			source.Reply(NICK_REG_PLEASE_WAIT, (u->lastnickreg + Config->NSRegDelay) - Anope::CurTime);
		else if ((na = findnick(u->nick)))
		{
			/* i.e. there's already such a nick regged */
			if (na->HasFlag(NS_FORBIDDEN))
			{
				Log(NickServ) << u->GetMask() << " tried to register FORBIDden nick " << u->nick;
				source.Reply(NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
			}
			else
				source.Reply(NICK_ALREADY_REGISTERED, u->nick.c_str());
		}
		else if (pass.equals_ci(u->nick) || (Config->StrictPasswords && pass.length() < 5))
			source.Reply(MORE_OBSCURE_PASSWORD);
		else if (pass.length() > Config->PassLen)
			source.Reply(PASSWORD_TOO_LONG);
		else if (!email.empty() && !MailValidate(email))
			source.Reply(MAIL_X_INVALID, email.c_str());
		else
		{
			for (idx = 0; idx < 9; ++idx)
				passcode += chars[1 + static_cast<int>((static_cast<float>(max - min)) * getrandom16() / 65536.0) + min];
			nr = new NickRequest(u->nick);
			nr->passcode = passcode;
			enc_encrypt(pass, nr->password);
			if (!email.empty())
				nr->email = email;
			nr->requested = Anope::CurTime;
			FOREACH_MOD(I_OnMakeNickRequest, OnMakeNickRequest(nr));
			if (Config->NSEmailReg)
			{
				if (SendRegmail(u, nr))
				{
					source.Reply(NICK_ENTER_REG_CODE, email.c_str(), Config->s_NickServ.c_str(), Config->s_NickServ.c_str());
					Log(LOG_COMMAND, u, this) << "send registration verification code to " << nr->email;
				}
				else
				{
					Log(LOG_COMMAND, u, this) << "unable to send registration verification mail";
					source.Reply(NICK_REG_UNABLE);
					delete nr;
					return MOD_CONT;
				}
			}
			else
			{
				std::vector<Anope::string> empty_params;
				return this->DoConfirm(source, empty_params);
			}
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(NICK_HELP_REGISTER);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		if (Config->NSForceEmail)
			SyntaxError(source, "REGISTER", NICK_REGISTER_SYNTAX_EMAIL);
		else
			SyntaxError(source, "REGISTER", NICK_REGISTER_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_REGISTER);
	}
};

class CommandNSResend : public Command
{
 public:
	CommandNSResend() : Command("RESEND", 0, 0)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickRequest *nr = NULL;
		if (Config->NSEmailReg)
		{
			if ((nr = findrequestnick(u->nick)))
			{
				if (Anope::CurTime < nr->lastmail + Config->NSResendDelay)
				{
					source.Reply(MAIL_LATER);
					return MOD_CONT;
				}
				if (!SendRegmail(u, nr))
				{
					nr->lastmail = Anope::CurTime;
					source.Reply(NICK_REG_RESENT, nr->email.c_str());
					Log(LOG_COMMAND, u, this) << "resend registration verification code for " << nr->nick;
				}
				else
				{
					Log(LOG_COMMAND, u, this) << "unable to resend registration verification code for " << nr->nick;
					return MOD_CONT;
				}
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(NICK_HELP_RESEND);
		return true;
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_RESEND);
	}
};

class NSRegister : public Module
{
	CommandNSRegister commandnsregister;
	CommandNSConfirm commandnsconfirm;
	CommandNSResend commandnsrsend;

 public:
	NSRegister(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), commandnsconfirm("CONFIRM", 1, 1)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsregister);
		this->AddCommand(NickServ, &commandnsconfirm);
		this->AddCommand(NickServ, &commandnsrsend);
	}
};

static bool SendRegmail(User *u, NickRequest *nr)
{
	char subject[BUFSIZE], message[BUFSIZE];

	snprintf(subject, sizeof(subject), GetString(NICK_REG_MAIL_SUBJECT).c_str(), nr->nick.c_str());
	snprintf(message, sizeof(message), GetString(NICK_REG_MAIL).c_str(), nr->nick.c_str(), Config->NetworkName.c_str(), Config->s_NickServ.c_str(), nr->passcode.c_str(), Config->NetworkName.c_str());

	return Mail(u, nr, NickServ, subject, message);
}

MODULE_INIT(NSRegister)
