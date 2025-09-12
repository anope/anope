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

namespace
{
	Anope::string *GetCode(NickCore *nc)
	{
		auto *code = nc->GetExt<Anope::string>("passcode");
		if (!code)
		{
			code = nc->Extend<Anope::string>("passcode");
			*code = Anope::Random(Config->GetBlock("options").Get<size_t>("codelength", "15"));
		}
		return code;
	}
}

static bool SendRegmail(User *u, const NickAlias *na, BotInfo *bi);

static ServiceReference<NickServService> nickserv("NickServService", "NickServ");

class CommandNSRegister final
	: public Command
{
public:
	CommandNSRegister(Module *creator) : Command(creator, "nickserv/register", 1, 2)
	{
		this->SetDesc(_("Register a nickname"));
		if (Config->GetModule("nickserv").Get<bool>("forceemail", "yes"))
			this->SetSyntax(_("\037password\037 \037email\037"));
		else
			this->SetSyntax(_("\037password\037 \037[email]\037"));
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();
		Anope::string u_nick = source.GetNick();
		Anope::string pass = params[0];
		Anope::string email = params.size() > 1 ? params[1] : "";
		const Anope::string &nsregister = Config->GetModule(this->owner).Get<const Anope::string>("registration");

		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		if (nsregister.equals_ci("disable"))
		{
			source.Reply(_("Registration is currently disabled."));
			return;
		}

		time_t nickregdelay = Config->GetModule(this->owner).Get<time_t>("nickregdelay");
		time_t reg_delay = Config->GetModule("nickserv").Get<time_t>("regdelay");
		if (u && !u->HasMode("OPER") && nickregdelay && Anope::CurTime - u->timestamp < nickregdelay)
		{
			auto waitperiod = (u->timestamp + nickregdelay) -  Anope::CurTime;
			source.Reply(_("You must wait %s before registering your nick."),
				Anope::Duration(waitperiod, source.GetAccount()).c_str());
			return;
		}

		if (nickserv && nickserv->IsGuestNick(u_nick))
		{
			source.Reply(NICK_CANNOT_BE_REGISTERED, u_nick.c_str());
			return;
		}

		if (!IRCD->IsNickValid(u_nick))
		{
			source.Reply(NICK_CANNOT_BE_REGISTERED, u_nick.c_str());
			return;
		}

		if (BotInfo::Find(u_nick, true))
		{
			source.Reply(NICK_CANNOT_BE_REGISTERED, u_nick.c_str());
			return;
		}

		if (Config->GetModule("nickserv").Get<bool>("restrictopernicks"))
		{
			for (auto *o : Oper::opers)
			{
				if (!source.IsOper() && u_nick.find_ci(o->name) != Anope::string::npos)
				{
					source.Reply(NICK_CANNOT_BE_REGISTERED, u_nick.c_str());
					return;
				}
			}
		}

		unsigned int minpasslen = Config->GetModule("nickserv").Get<unsigned>("minpasslen", "10");
		unsigned int maxpasslen = Config->GetModule("nickserv").Get<unsigned>("maxpasslen", "50");

		if (Config->GetModule("nickserv").Get<bool>("forceemail", "yes") && email.empty())
			this->OnSyntaxError(source, "");
		else if (u && Anope::CurTime < u->lastnickreg + reg_delay)
		{
			auto waitperiod = (unsigned long)(u->lastnickreg + reg_delay) - Anope::CurTime;
			source.Reply(_("Please wait %s before using the REGISTER command again."),
				Anope::Duration(waitperiod, source.GetAccount()).c_str());
		}
		else if (NickAlias::Find(u_nick) != NULL)
			source.Reply(NICK_ALREADY_REGISTERED, u_nick.c_str());
		else if (pass.equals_ci(u_nick))
			source.Reply(MORE_OBSCURE_PASSWORD);
		else if (pass.length() < minpasslen)
			source.Reply(PASSWORD_TOO_SHORT, minpasslen);
		else if (pass.length() > maxpasslen)
			source.Reply(PASSWORD_TOO_LONG, maxpasslen);
		else if (!email.empty() && !Mail::Validate(email))
			source.Reply(MAIL_X_INVALID, email.c_str());
		else
		{
			Anope::string encpass;
			if (!Anope::Encrypt(pass, encpass))
			{
				source.Reply(_("Accounts can not be registered right now. Please try again later."));
				return;
			}

			auto *nc = new NickCore(u_nick);
			auto *na = new NickAlias(u_nick, nc);
			if (!email.empty())
				nc->email = email;
			nc->pass = encpass;

			if (u)
				na->last_userhost = u->GetIdent() + "@" + u->GetDisplayedHost();

			Log(LOG_COMMAND, source, this) << "to register " << na->nick << " (email: " << (!na->nc->email.empty() ? na->nc->email : "none") << ")";

			source.Reply(_("Nickname \002%s\002 registered."), u_nick.c_str());
			if (nsregister.equals_ci("admin") || nsregister.equals_ci("code"))
			{
				nc->Extend<bool>("UNCONFIRMED");
			}
			else if (nsregister.equals_ci("mail"))
			{
				if (!email.empty())
				{
					nc->Extend<bool>("UNCONFIRMED");
					SendRegmail(NULL, na, source.service);
				}
			}

			FOREACH_MOD(OnNickRegister, (source.GetUser(), na, pass));

			if (u)
			{
				// This notifies the user that if their registration is unconfirmed
				u->Identify(na);
				u->lastnickreg = Anope::CurTime;
			}
			else if (nc->HasExt("UNCONFIRMED"))
			{
				if (nsregister.equals_ci("admin"))
					source.Reply(_("All new accounts must be validated by an administrator. Please wait for your registration to be confirmed."));
				else if (nsregister.equals_ci("code"))
				{
					const auto *code = GetCode(na->nc);
					source.Reply(_("Your account is not confirmed. To confirm it, type \002%s\002."),
						source.service->GetQueryCommand("nickserv/confirm/register", *code).c_str());
				}
				else if (nsregister.equals_ci("mail"))
					source.Reply(_("Your account is not confirmed. To confirm it, follow the instructions that were emailed to you."));
			}
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		unsigned int minpasslen = Config->GetModule("nickserv").Get<unsigned>("minpasslen", "10");
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Registers your nickname in the %s database. Once "
				"your nick is registered, you can use the \002SET\002 and \002ACCESS\002 "
				"commands to configure your nick's settings as you like "
				"them. Make sure you remember the password you use when "
				"registering - you'll need it to make changes to your nick "
				"later. (Note that \002case matters!\002 \037ANOPE\037, \037Anope\037, and "
				"\037anope\037 are all different passwords!) "
				"\n\n"
				"Guidelines on choosing passwords:"
				"\n\n"
				"Passwords should not be easily guessable. For example, "
				"using your real name as a password is a bad idea. Using "
				"your nickname as a password is a much worse idea ;) and, "
				"in fact, %s will not allow it. Also, short "
				"passwords are vulnerable to trial-and-error searches, so "
				"you should choose a password at least %u characters long. "
				"Finally, the space character cannot be used in passwords."
			),
			source.service->nick.c_str(),
			source.service->nick.c_str(),
			minpasslen);

		if (!Config->GetModule("nickserv").Get<bool>("forceemail", "yes"))
		{
			source.Reply(" ");
			source.Reply(_(
				"The \037email\037 parameter is optional and will set the email address for your "
				"nick immediately. You may also wish to \002SET\032HIDE\002 it after registering "
				"if it isn't the default setting already."
			));
		}

		if (!Config->GetModule("nickserv").Get<bool>("nonicknameownership"))
		{
			source.Reply(" ");
			source.Reply(_(
				"You can associate multiple nicknames with your account. All nicknames will "
				"share the same configuration, set of memos, and channel privileges."
			));
		}

		return true;
	}
};

class CommandNSConfirmRegister final
	: public Command
{
public:
	CommandNSConfirmRegister(Module *creator)
		: Command(creator, "nickserv/confirm/register", 1, 2)
	{
		this->SetDesc(_("Confirm a previous account registration"));
		this->SetSyntax(_("\037code\037"));
		this->SetSyntax(_("@\037nickname\037"), [](auto &source) { return source.HasPriv("nickserv/confirm/register"); });
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		auto has_priv = source.HasPriv("nickserv/confirm/register");

		Anope::string code;
		NickAlias *na;
		if (params[0] == '@')
		{
			if (!has_priv)
			{
				source.Reply(ACCESS_DENIED);
				return;
			}

			auto nick = params[0].substr(0);
			na = NickAlias::Find(nick);
			if (!na)
			{
				source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
				return;
			}
		}
		else
		{
			code = params[0];
			na = source.GetAccount()->na;
		}

		NickCore *nc = na->nc;
		if (nc->HasExt("NS_SUSPENDED"))
		{
			source.Reply(NICK_X_SUSPENDED, na->nick.c_str());
			return;
		}

		auto *passcode = nc->GetExt<Anope::string>("passcode");
		if (!passcode)
		{
			source.Reply(_("There is no registration confirmation pending for %s."),
				na->nick.c_str());
			return;
		}
		if (has_priv || !code.equals_cs(*passcode))
		{
			source.Reply(_("The registration confirmation code you specified for %s is incorrect."),
				na->nick.c_str());
			return;
		}

		na->nc->Shrink<bool>("UNCONFIRMED");
		FOREACH_MOD(OnNickConfirm, (source.GetUser(), nc));

		auto nonicknameownership = Config->GetModule("nickserv").Get<bool>("nonicknameownership");
		for (auto *u : nc->users)
		{
			IRCD->SendLogin(u, na);

			if (!nonicknameownership)
				continue;

			const auto &aliases = *nc->aliases;
			auto it = std::find_if(aliases.begin(), aliases.end(), [&u](const auto *na) {
				return na->nick.equals_ci(u->nick);
			});
			if (it != aliases.end())
				u->SetMode(source.service, "REGISTERED"); // nick is in the group
		}

		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to confirm the registration of " << nc->display;
		source.Reply(_("Nick \002%s\002 has been confirmed."), na->nick.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		auto unconfirmedexpire = Config->GetModule(owner).Get<time_t>("unconfirmedexpire", "1d");

		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Confirms an account registration. You have %s after registration to do this "
				"before your registration expires."
			),
			Anope::Duration(unconfirmedexpire, source.GetAccount()).c_str());

		if (source.HasPriv("nickserv/confirm/register"))
		{
			source.Reply(" ");
			source.Reply(_(
				"Additionally, Services Operators with the \037nickserv/confirm/register\037 "
				"permission can specify @\037nickname\037 instead of \037code\037 to force "
				"confirm another user's account registration."
			));
		}
		return true;
	}
};

class CommandNSResend final
	: public Command
{
public:
	CommandNSResend(Module *creator)
		: Command(creator, "nickserv/resend", 0, 1)
	{
		this->SetDesc(_("Resend registration confirmation email"));
		this->SetSyntax(_("[\037nickname\037]"), [](auto &source) { return source.HasCommand("nickserv/resend"); });
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!Config->GetModule(this->owner).Get<const Anope::string>("registration").equals_ci("mail"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		auto is_oper = false;
		Anope::string nick;
		if (!params.empty() && source.HasCommand("nickserv/resend"))
		{
			nick = params[0];
			is_oper = true;
		}
		else if (source.nc)
			nick = source.GetAccount()->display;
		else
			nick = source.GetNick();

		const auto *na = NickAlias::Find(nick);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
			return;
		}

		NickCore *nc = na->nc;
		if (!nc->HasExt("UNCONFIRMED"))
		{
			source.Reply(_("Nick \002%s\002 is already confirmed."), na->nick.c_str());
			return;
		}

		if (!is_oper && Anope::CurTime < nc->lastmail + Config->GetModule(this->owner).Get<time_t>("resenddelay"))
		{
			source.Reply(_("Cannot send mail now; please retry a little later."));
			return;
		}

		if (!SendRegmail(source.GetUser(), na, source.service))
		{
			Log(this->owner) << "Unable to resend registration confirmation code for " << na->nick;
			return;
		}

		nc->lastmail = Anope::CurTime;
		source.Reply(_("The confirmation code for \002%s\002 has been re-sent to %s."),
			na->nick.c_str(), nc->email.c_str());

		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to resend the registration confirmation code for " << na->nick;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (!Config->GetModule(this->owner).Get<const Anope::string>("registration").equals_ci("mail"))
			return false;

		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command will resend a registration confirmation email."));

		if (source.HasCommand("nickserv/resend"))
		{
			source.Reply(" ");
			source.Reply(_(
				"Additionally, Services Operators with the \037nickserv/resend\037 permission "
				"can specify a nickname to resend a confirmation email for another account."
			));
		}
		return true;
	}

	void OnServHelp(CommandSource &source, HelpWrapper &help) override
	{
		if (Config->GetModule(this->owner).Get<const Anope::string>("registration").equals_ci("mail"))
			Command::OnServHelp(source, help);
	}
};

class NSRegister final
	: public Module
{
	CommandNSRegister commandnsregister;
	CommandNSConfirmRegister commandnsconfirmregister;
	CommandNSResend commandnsresend;

	SerializableExtensibleItem<bool> unconfirmed;
	SerializableExtensibleItem<Anope::string> passcode;

public:
	NSRegister(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnsregister(this)
		, commandnsconfirmregister(this)
		, commandnsresend(this)
		, unconfirmed(this, "UNCONFIRMED")
		, passcode(this, "passcode")
	{
		if (Config->GetModule(this).Get<const Anope::string>("registration").equals_ci("disable"))
			throw ModuleException("Module " + this->name + " will not load with registration disabled.");
	}

	void OnNickIdentify(User *u) override
	{
		BotInfo *NickServ;
		if (unconfirmed.HasExt(u->Account()) && (NickServ = Config->GetClient("NickServ")))
		{
			const Anope::string &nsregister = Config->GetModule(this).Get<const Anope::string>("registration");
			if (nsregister.equals_ci("admin"))
				u->SendMessage(NickServ, _("All new accounts must be validated by an administrator. Please wait for your registration to be confirmed."));
			else if (nsregister.equals_ci("code"))
			{
				const auto *code = GetCode(u->Account());
				u->SendMessage(NickServ, _("Your account is not confirmed. To confirm it, type \002%s\002."),
					NickServ->GetQueryCommand("nickserv/confirm/register", *code).c_str());
			}
			else if (nsregister.equals_ci("mail"))
				u->SendMessage(NickServ, _("Your account is not confirmed. To confirm it, follow the instructions that were emailed to you."));

			const NickAlias *this_na = u->AccountNick();
			time_t registered = Anope::CurTime - this_na->registered;
			time_t unconfirmed_expire = Config->GetModule(this).Get<time_t>("unconfirmedexpire", "1d");
			if (unconfirmed_expire > registered)
				u->SendMessage(NickServ, _("Your account will expire, if not confirmed, in %s."), Anope::Duration(unconfirmed_expire - registered, u->Account()).c_str());
		}
	}

	void OnPreNickExpire(NickAlias *na, bool &expire) override
	{
		if (unconfirmed.HasExt(na->nc))
		{
			time_t unconfirmed_expire = Config->GetModule(this).Get<time_t>("unconfirmedexpire", "1d");
			if (unconfirmed_expire && Anope::CurTime - na->registered >= unconfirmed_expire)
				expire = true;
		}
	}
};

static bool SendRegmail(User *u, const NickAlias *na, BotInfo *bi)
{
	NickCore *nc = na->nc;
	auto *code = GetCode(na->nc);

	Anope::map<Anope::string> vars = {
		{ "nick",    na->nick                                                                },
		{ "network", Config->GetBlock("networkinfo").Get<const Anope::string>("networkname") },
		{ "code",    *code                                                                   },
	};

	auto subject = Anope::Template(Language::Translate(na->nc, Config->GetBlock("mail").Get<const Anope::string>("registration_subject").c_str()), vars);
	auto message = Anope::Template(Language::Translate(na->nc, Config->GetBlock("mail").Get<const Anope::string>("registration_message").c_str()), vars);

	return Mail::Send(u, nc, bi, subject, message);
}

MODULE_INIT(NSRegister)
