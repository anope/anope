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
#include "modules/nickserv.h"

static bool SendRegmail(User *u, const NickServ::Nick *na, BotInfo *bi);

class CommandNSConfirm : public Command
{
 public:
	CommandNSConfirm(Module *creator) : Command(creator, "nickserv/confirm", 1, 2)
	{
		this->SetDesc(_("Confirm a passcode"));
		this->SetSyntax(_("\037passcode\037"));
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &passcode = params[0];

		if (source.nc && !source.nc->HasExt("UNCONFIRMED") && source.HasPriv("nickserv/confirm"))
		{
			NickServ::Nick *na = NickServ::FindNick(passcode);
			if (na == NULL)
				source.Reply(_("\002{0}\002 isn't registered."), passcode);
			else if (na->nc->HasExt("UNCONFIRMED") == false)
				source.Reply(_("\002{0}\002 is already confirmed."), na->nick);
			else
			{
				na->nc->Shrink<bool>("UNCONFIRMED");
				Log(LOG_ADMIN, source, this) << "to confirm nick " << na->nick << " (" << na->nc->display << ")";
				source.Reply(_("\002{0}\002 has been confirmed."), na->nick);
			}
		}
		else if (source.nc)
		{
			Anope::string *code = source.nc->GetExt<Anope::string>("passcode");
			if (code != NULL && *code == passcode)
			{
				NickServ::Account *nc = source.nc;
				nc->Shrink<Anope::string>("passcode");
				Log(LOG_COMMAND, source, this) << "to confirm their email";
				source.Reply(_("Your email address of \002{0}\002 has been confirmed."), source.nc->email);
				nc->Shrink<bool>("UNCONFIRMED");

				if (source.GetUser())
				{
					NickServ::Nick *na = NickServ::FindNick(source.GetNick());
					if (na)
					{
						IRCD->SendLogin(source.GetUser(), na);
						if (!Config->GetModule("nickserv")->Get<bool>("nonicknameownership") && na->nc == source.GetAccount() && !na->nc->HasExt("UNCONFIRMED"))
							source.GetUser()->SetMode(source.service, "REGISTERED");
					}
				}
			}
			else
				source.Reply(_("Invalid passcode."));
		}
		else
			source.Reply(_("Invalid passcode."));

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("This command is used by several commands as a way to confirm changes made to your account.\n"
		               "\n"
		               "This is most commonly used to confirm your email address once you register or change it.\n"
		               "\n"
		               "This is also used after when resetting your password to force identify you to your account so you may change your password."));
		if (source.HasPriv("nickserv/confirm"))
			source.Reply(_("Additionally, Services Operators with the \037nickserv/confirm\037 permission can\n"
				"replace \037passcode\037 with a users nick to force validate them."));
		return true;
	}
};

class CommandNSRegister : public Command
{
 public:
	CommandNSRegister(Module *creator) : Command(creator, "nickserv/register", 1, 2)
	{
		this->SetDesc(_("Register a nickname"));
		if (Config->GetModule("nickserv")->Get<bool>("forceemail", "yes"))
			this->SetSyntax(_("\037password\037 \037email\037"));
		else
			this->SetSyntax(_("\037password\037 \037[email]\037"));
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();
		Anope::string u_nick = source.GetNick();
		size_t nicklen = u_nick.length();
		Anope::string pass = params[0];
		Anope::string email = params.size() > 1 ? params[1] : "";
		const Anope::string &nsregister = Config->GetModule(this->owner)->Get<const Anope::string>("registration");

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, nickname registration is temporarily disabled."));
			return;
		}

		if (nsregister.equals_ci("disable"))
		{
			source.Reply(_("Registration is currently disabled."));
			return;
		}

		time_t nickregdelay = Config->GetModule(this->owner)->Get<time_t>("nickregdelay");
		time_t reg_delay = Config->GetModule("nickserv")->Get<time_t>("regdelay");
		if (u && !u->HasMode("OPER") && nickregdelay && Anope::CurTime - u->timestamp < nickregdelay)
		{
			source.Reply(_("You must have been using this nick for at least %d seconds to register."), nickregdelay);
			return;
		}

		/* Prevent "Guest" nicks from being registered. -TheShadow */

		/* Guest nick can now have a series of between 1 and 7 digits.
		 *   --lara
		 */
		const Anope::string &guestnick = Config->GetModule("nickserv")->Get<const Anope::string>("guestnickprefix", "Guest");
		if (nicklen <= guestnick.length() + 7 && nicklen >= guestnick.length() + 1 && !u_nick.find_ci(guestnick) && u_nick.substr(guestnick.length()).find_first_not_of("1234567890") == Anope::string::npos)
		{
			source.Reply(_("\002{0}\002 may not be registered."), u_nick);
			return;
		}

		if (!IRCD->IsNickValid(u_nick))
		{
			source.Reply(_("\002{0}\002 may not be registered."), u_nick);
			return;
		}

		if (Config->GetModule("nickserv")->Get<bool>("restrictopernicks"))
			for (unsigned i = 0; i < Oper::opers.size(); ++i)
			{
				Oper *o = Oper::opers[i];

				if (!source.IsOper() && u_nick.find_ci(o->name) != Anope::string::npos)
				{
					source.Reply(_("\002{0}\002 may not be registered."), u_nick);
					return;
				}
			}

		if (Config->GetModule("nickserv")->Get<bool>("forceemail", "yes") && email.empty())
			this->OnSyntaxError(source, "");
		else if (u && Anope::CurTime < u->lastnickreg + reg_delay)
			source.Reply(_("Please wait \002{0}\002 seconds before using the {1} command again."), (u->lastnickreg + reg_delay) - Anope::CurTime, source.command);
		else if (NickServ::FindNick(u_nick) != NULL)
			source.Reply(_("\002{0}\002 is already registered."), u_nick);
		else if (pass.equals_ci(u_nick) || (Config->GetBlock("options")->Get<bool>("strictpasswords") && pass.length() < 5))
			source.Reply(_("Please try again with a more obscure password. Passwords should be at least five characters long, should not be something easily guessed"
			               " (e.g. your real name or your nickname), and cannot contain the space or tab characters."));
		else if (pass.length() > Config->GetModule("nickserv")->Get<unsigned>("passlen", "32"))
			source.Reply(_("Your password is too long, it can not contain more than \002{0}\002 characters."), Config->GetModule("nickserv")->Get<unsigned>("passlen", "32"));
		else if (!email.empty() && !Mail::Validate(email))
			source.Reply(_("\002{0}\002 is not a valid e-mail address."), email);
		else
		{
			NickServ::Account *nc = NickServ::service->CreateAccount(u_nick);
			NickServ::Nick *na = NickServ::service->CreateNick(u_nick, nc);
			Anope::Encrypt(pass, nc->pass);
			if (!email.empty())
				nc->email = email;

			if (u)
			{
				na->last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
				na->last_realname = u->realname;
			}
			else
				na->last_realname = source.GetNick();

			Log(LOG_COMMAND, source, this) << "to register " << na->nick << " (email: " << (!na->nc->email.empty() ? na->nc->email : "none") << ")";

			if (NickServ::Event::OnNickRegister)
				NickServ::Event::OnNickRegister(&NickServ::Event::NickRegister::OnNickRegister, source.GetUser(), na, pass);

			if (na->nc->GetAccessCount())
				source.Reply(_("\002{0}\002 has been registered under your hostmask: \002{1}\002"), u_nick, na->nc->GetAccess(0));
			else
				source.Reply(_("\002{0}\002 has been registered."), u_nick);

			Anope::string tmp_pass;
			if (Anope::Decrypt(na->nc->pass, tmp_pass) == 1)
				source.Reply(_("Your password is \002{0}\002 - remember this for later use."), tmp_pass);

			if (nsregister.equals_ci("admin"))
			{
				nc->Extend<bool>("UNCONFIRMED");
				// User::Identify() called below will notify the user that their registration is pending
			}
			else if (nsregister.equals_ci("mail"))
			{
				if (!email.empty())
				{
					nc->Extend<bool>("UNCONFIRMED");
					SendRegmail(NULL, na, source.service);
				}
			}

			if (u)
			{
				u->Identify(na);
				u->lastnickreg = Anope::CurTime;
			}
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Registers your nickname. Once your nickname is registered, you will be able to use most features of services, including owning and managing channels."
		               "Make sure you remember the password - you'll need it to identify yourself later. Your email address will only be used if you forget your password."));

		if (!Config->GetModule("nickserv")->Get<bool>("forceemail", "yes"))
		{
			source.Reply(" ");
			source.Reply(_("The \037email\037 parameter is optional and will set the email\n"
					"for your nick immediately.\n"
					"Your privacy is respected; this e-mail won't be given to\n"
					"any third-party person. You may also wish to \002SET HIDE\002 it\n"
					"after registering if it isn't the default setting already."));
		}

		if (!Config->GetModule("nickserv")->Get<bool>("nonicknameownership"))
		{
			source.Reply(" ");
			source.Reply(_("This command also creates a new group for your nickname, which will allow you to group other nicknames later, which share the same configuration, the same set of memos and the same channel privileges."));
		}
		return true;
	}
};

class CommandNSResend : public Command
{
 public:
	CommandNSResend(Module *creator) : Command(creator, "nickserv/resend", 0, 0)
	{
		this->SetDesc(_("Resend registration confirmation email"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!Config->GetModule(this->owner)->Get<const Anope::string>("registration").equals_ci("mail"))
		{
			source.Reply(_("Access denied."));
			return;
		}

		const NickServ::Nick *na = NickServ::FindNick(source.GetNick());

		if (na == NULL)
			source.Reply(_("Your nickname isn't registered."));
		else if (na->nc != source.GetAccount() || !source.nc->HasExt("UNCONFIRMED"))
			source.Reply(_("Your account is already confirmed."));
		else
		{
			if (Anope::CurTime < source.nc->lastmail + Config->GetModule(this->owner)->Get<time_t>("resenddelay"))
				source.Reply(_("Cannot send mail now; please retry a little later."));
			else if (SendRegmail(source.GetUser(), na, source.service))
			{
				na->nc->lastmail = Anope::CurTime;
				source.Reply(_("Your passcode has been re-sent to \002{0}\002."), na->nc->email);
				Log(LOG_COMMAND, source, this) << "to resend registration verification code";
			}
			else
				Log(this->owner) << "Unable to resend registration verification code for " << source.GetNick();
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (!Config->GetModule(this->owner)->Get<const Anope::string>("registration").equals_ci("mail"))
			return false;

		source.Reply(_("This command will resend you the registration confirmation email."));
		return true;
	}

	void OnServHelp(CommandSource &source) override
	{
		if (Config->GetModule(this->owner)->Get<const Anope::string>("registration").equals_ci("mail"))
			Command::OnServHelp(source);
	}
};

class NSRegister : public Module
	, public EventHook<Event::NickIdentify>
	, public EventHook<NickServ::Event::PreNickExpire>
{
	CommandNSRegister commandnsregister;
	CommandNSConfirm commandnsconfirm;
	CommandNSResend commandnsrsend;

	SerializableExtensibleItem<bool> unconfirmed;
	SerializableExtensibleItem<Anope::string> passcode;

 public:
	NSRegister(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::NickIdentify>("OnNickIdentify")
		, EventHook<NickServ::Event::PreNickExpire>("OnPreNickExpire")
		, commandnsregister(this)
		, commandnsconfirm(this)
		, commandnsrsend(this)
		, unconfirmed(this, "UNCONFIRMED")
		, passcode(this, "passcode")
	{
		if (Config->GetModule(this)->Get<const Anope::string>("registration").equals_ci("disable"))
			throw ModuleException("Module " + this->name + " will not load with registration disabled.");
	}

	void OnNickIdentify(User *u) override
	{
		BotInfo *NickServ;
		if (unconfirmed.HasExt(u->Account()) && (NickServ = Config->GetClient("NickServ")))
		{
			const Anope::string &nsregister = Config->GetModule(this)->Get<const Anope::string>("registration");
			if (nsregister.equals_ci("admin"))
				u->SendMessage(NickServ, _("All new accounts must be validated by an administrator. Please wait for your registration to be confirmed."));
			else
				u->SendMessage(NickServ, _("Your email address is not confirmed. To confirm it, follow the instructions that were emailed to you."));
			const NickServ::Nick *this_na = NickServ::FindNick(u->Account()->display);
			time_t time_registered = Anope::CurTime - this_na->time_registered;
			time_t unconfirmed_expire = Config->GetModule(this)->Get<time_t>("unconfirmedexpire", "1d");
			if (unconfirmed_expire > time_registered)
				u->SendMessage(NickServ, _("Your account will expire, if not confirmed, in %s."), Anope::Duration(unconfirmed_expire - time_registered, u->Account()).c_str());
		}
	}

	void OnPreNickExpire(NickServ::Nick *na, bool &expire) override
	{
		if (unconfirmed.HasExt(na->nc))
		{
			time_t unconfirmed_expire = Config->GetModule(this)->Get<time_t>("unconfirmedexpire", "1d");
			if (unconfirmed_expire && Anope::CurTime - na->time_registered >= unconfirmed_expire)
				expire = true;
		}
	}
};

static bool SendRegmail(User *u, const NickServ::Nick *na, BotInfo *bi)
{
	NickServ::Account *nc = na->nc;

	Anope::string *code = na->nc->GetExt<Anope::string>("passcode");
	if (code == NULL)
	{
		code = na->nc->Extend<Anope::string>("passcode");
		*code = Anope::Random(9);
	}

	Anope::string subject = Language::Translate(na->nc, Config->GetBlock("mail")->Get<const Anope::string>("registration_subject").c_str()),
		message = Language::Translate(na->nc, Config->GetBlock("mail")->Get<const Anope::string>("registration_message").c_str());

	subject = subject.replace_all_cs("%n", na->nick);
	subject = subject.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<const Anope::string>("networkname"));
	subject = subject.replace_all_cs("%c", *code);

	message = message.replace_all_cs("%n", na->nick);
	message = message.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<const Anope::string>("networkname"));
	message = message.replace_all_cs("%c", *code);

	return Mail::Send(u, nc, bi, subject, message);
}

MODULE_INIT(NSRegister)
