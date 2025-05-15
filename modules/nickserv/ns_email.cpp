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
	bool clean = false;
	unsigned maxemails = 0;

	/* strip dots from username, and remove anything after the first + */
	Anope::string CleanMail(const Anope::string &email)
	{
		size_t host = email.find('@');
		if (host == Anope::string::npos)
			return email;

		Anope::string username = email.substr(0, host);
		username = username.replace_all_cs(".", "");

		size_t sz = username.find('+');
		if (sz != Anope::string::npos)
			username = username.substr(0, sz);

		Anope::string cleaned = username + email.substr(host);
		Log(LOG_DEBUG) << "cleaned " << email << " to " << cleaned;
		return cleaned;
	}

	unsigned CountEmail(const Anope::string &email, NickCore *unc)
	{
		unsigned count = 0;

		if (email.empty())
			return 0;

		Anope::string cleanemail = clean ? CleanMail(email) : email;

		for (const auto &[_, nc] : *NickCoreList)
		{
			Anope::string cleannc = clean ? CleanMail(nc->email) : nc->email;

			if (unc != nc && cleanemail.equals_ci(cleannc))
				++count;
		}

		return count;
	}

	bool CheckLimitReached(CommandSource &source, const Anope::string &email, bool ignoreself)
	{
		if (!maxemails || email.empty())
			return false;

		if (CountEmail(email, ignoreself ? source.GetAccount() : NULL) < maxemails)
			return false;

		source.Reply(maxemails, N_("The email address \002%s\002 has reached its usage limit of %u user.", "The email address \002%s\002 has reached its usage limit of %u users."), email.c_str(), maxemails);
		return true;
	}
}

class CommandNSGetEmail final
	: public Command
{
public:
	CommandNSGetEmail(Module *creator)
		: Command(creator, "nickserv/getemail", 1, 1)
	{
		this->SetDesc(_("Matches and returns all users that registered using given email"));
		this->SetSyntax(_("\037email\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &email = params[0];
		int j = 0;

		Log(LOG_ADMIN, source, this) << "on " << email;

		for (const auto &[_, nc] : *NickCoreList)
		{
			if (!nc->email.empty() && Anope::Match(nc->email, email))
			{
				++j;
				source.Reply(_("Email matched: \002%s\002 (\002%s\002) to \002%s\002."), nc->display.c_str(), nc->email.c_str(), email.c_str());
			}
		}

		if (j <= 0)
		{
			source.Reply(_("No registrations matching \002%s\002 were found."), email.c_str());
			return;
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Returns the matching accounts that used given email."));
		return true;
	}
};

class CommandNSSetEmail
	: public Command
{
	static bool SendConfirmMail(User *u, NickCore *nc, BotInfo *bi, const Anope::string &new_email)
	{
		Anope::string code = Anope::Random(Config->GetBlock("options").Get<size_t>("codelength", "15"));

		std::pair<Anope::string, Anope::string> *n = nc->Extend<std::pair<Anope::string, Anope::string> >("ns_set_email");
		n->first = new_email;
		n->second = code;

		Anope::map<Anope::string> vars = {
			{ "old_email", nc->email                                                               },
			{ "new_email", new_email                                                               },
			{ "account",   nc->display                                                             },
			{ "network",   Config->GetBlock("networkinfo").Get<const Anope::string>("networkname") },
			{ "code",      code                                                                    },
		};

		auto subject = Anope::Template(Config->GetBlock("mail").Get<const Anope::string>("emailchange_subject"), vars);
		auto message = Anope::Template(Config->GetBlock("mail").Get<const Anope::string>("emailchange_message"), vars);

		Anope::string old = nc->email;
		nc->email = new_email;
		bool b = Mail::Send(u, nc, bi, subject, message);
		nc->email = old;
		return b;
	}

public:
	CommandNSSetEmail(Module *creator, const Anope::string &cname = "nickserv/set/email", size_t min = 0) : Command(creator, cname, min, min + 1)
	{
		this->SetDesc(_("Associate an email address with your nickname"));
		this->SetSyntax(_("\037address\037"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		if (nc->HasExt("UNCONFIRMED"))
		{
			source.Reply(_("You may not change the email of an unconfirmed account."));
			return;
		}

		if (param.empty() && Config->GetModule("nickserv").Get<bool>("forceemail", "yes"))
		{
			source.Reply(_("You cannot unset the email on this network."));
			return;
		}
		else if (Config->GetModule("nickserv").Get<bool>("secureadmins", "yes") && source.nc != nc && nc->IsServicesOper())
		{
			source.Reply(_("You may not change the email of other Services Operators."));
			return;
		}
		else if (!param.empty() && !Mail::Validate(param))
		{
			source.Reply(MAIL_X_INVALID, param.c_str());
			return;
		}
		else if (CheckLimitReached(source, param, true))
			return;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetNickOption, MOD_RESULT, (source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		const auto nsmailreg = Config->GetModule("ns_register").Get<const Anope::string>("registration").equals_ci("mail");
		if (!param.empty() && Config->GetModule("nickserv").Get<bool>("confirmemailchanges", nsmailreg ? "yes" : "no") && source.GetAccount() == nc)
		{
			if (SendConfirmMail(source.GetUser(), source.GetAccount(), source.service, param))
			{
				Log(LOG_COMMAND, source, this) << "to request changing the email of " << nc->display << " to " << param;
				source.Reply(_("A confirmation email has been sent to \002%s\002. Follow the instructions in it to change your email address."), param.c_str());
			}
		}
		else
		{
			if (!param.empty())
			{
				Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the email of " << nc->display << " to " << param;
				nc->email = param;
				source.Reply(_("Email address for \002%s\002 changed to \002%s\002."), nc->display.c_str(), param.c_str());
			}
			else
			{
				Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to unset the email of " << nc->display;
				nc->email.clear();
				source.Reply(_("Email address for \002%s\002 unset."), nc->display.c_str());
			}
		}
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params.size() ? params[0] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Associates the given email address with your nickname. "
			"This address will be displayed whenever someone requests "
			"information on the nickname with the \002INFO\002 command."
		));
		return true;
	}
};

class CommandNSSASetEmail final
	: public CommandNSSetEmail
{
public:
	CommandNSSASetEmail(Module *creator) : CommandNSSetEmail(creator, "nickserv/saset/email", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 \037address\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params.size() > 1 ? params[1] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Associates the given email address with the nickname."));
		return true;
	}
};

class NSEmail final
	: public Module
{
private:
	CommandNSGetEmail commandnsgetemail;
	CommandNSSetEmail commandnssetemail;
	CommandNSSASetEmail commandnssasetemail;

	/* email, passcode */
	PrimitiveExtensibleItem<std::pair<Anope::string, Anope::string>> ns_set_email;

public:
	NSEmail(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnsgetemail(this)
		, commandnssetemail(this)
		, commandnssasetemail(this)
		, ns_set_email(this, "ns_set_email")
	{
	}

	void OnReload(Configuration::Conf &conf) override
	{
		const auto &modconf = conf.GetModule(this);
		maxemails = modconf.Get<unsigned>("maxemails");
		clean = modconf.Get<bool>("remove_aliases", "yes");
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) override
	{
		NickCore *uac = source.nc;

		if (command->name == "nickserv/confirm" && !params.empty() && uac)
		{
			std::pair<Anope::string, Anope::string> *n = ns_set_email.Get(uac);
			if (n)
			{
				if (params[0] == n->second)
				{
					uac->email = n->first;
					Log(LOG_COMMAND, source, command) << "to confirm their email address change to " << uac->email;
					source.Reply(_("Your email address has been changed to \002%s\002."), uac->email.c_str());
					ns_set_email.Unset(uac);
					return EVENT_STOP;
				}
			}
		}
		if (!source.IsOper() && command->name == "nickserv/register")
		{
			if (CheckLimitReached(source, params.size() > 1 ? params[1] : "", false))
				return EVENT_STOP;
		}
		else if (!source.IsOper() && command->name == "nickserv/ungroup" && source.GetAccount())
		{
			if (CheckLimitReached(source, source.GetAccount()->email, false))
				return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(NSEmail)
