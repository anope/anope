// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"

static bool SendResetEmail(User *u, const NickAlias *na, BotInfo *bi);

class CommandNSResetPass final
	: public Command
{
public:
	CommandNSResetPass(Module *creator) : Command(creator, "nickserv/resetpass", 2, 2)
	{
		this->SetDesc(_("Send an email to recover access to an account"));
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
				Log(LOG_COMMAND, source, this) << "for " << na->nick << " (account: " << na->nc->display << ")";
				source.Reply(_("Password reset email for \002%s\002 has been sent."), na->nick.c_str());
			}
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Sends a confirmation code to the nickname's email address with instructions on "
			"how to reset their password. \037email\037 must be the email address associated "
			"with the account."
		));
		return true;
	}
};

struct ResetInfo final
{
	Anope::string code;
	time_t time;
};

class CommandNSConfirmResetPass final
	: public Command
{
private:
	PrimitiveExtensibleItem<ResetInfo> &reset;

public:
	CommandNSConfirmResetPass(Module *creator, PrimitiveExtensibleItem<ResetInfo> &r)
		: Command(creator, "nickserv/confirm/resetpass", 1, 2)
		, reset(r)
	{
		this->AllowUnregistered(true);
		this->SetDesc(_("Confirm a previous password reset"));
		this->SetSyntax(_("[\037nickname\037] \037code\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		Anope::string code, nick;
		if (params.size() > 1)
		{
			nick = params[0];
			code = params[1];
		}
		else
		{
			code = params[0];
			nick = source.GetNick();
		}

		auto *na = NickAlias::Find(nick);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
			return;
		}

		NickCore *nc = na->nc;
		if (nc->HasExt("NS_SUSPENDED"))
		{
			source.Reply(NICK_X_SUSPENDED, na->nick.c_str());
			return;
		}

		auto *ri = reset.Get(nc);
		if (!ri)
		{
			source.Reply(_("There is no password reset confirmation pending for %s."),
				na->nick.c_str());
			return;
		}
		if (!code.equals_cs(ri->code))
		{
			source.Reply(_("The password reset code you specified for %s is incorrect."),
				na->nick.c_str());
			return;
		}

		auto resetexpire = Config->GetModule(owner).Get<time_t>("resetexpire", "1d");
		if (ri->time < Anope::CurTime - resetexpire)
		{
			reset.Unset(nc);
			source.Reply(_("The password reset request for %s has expired."),
				na->nick.c_str());
			return;
		}

		reset.Unset(nc);

		if (Config->GetModule("nickserv").Get<const Anope::string>("registration").equals_ci("mail"))
			nc->Shrink<bool>("UNCONFIRMED");

		if (source.GetUser())
			source.GetUser()->Identify(na);

		Log(LOG_COMMAND, source, this) << "to reset their password and forcibly identify as " << na->nick;
		source.Reply(_("You are now identified as %s. Change your password now using %s."),
			na->nick.c_str(), source.service->GetQueryCommand("nickserv/set/password").c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		auto resetexpire = Config->GetModule(owner).Get<time_t>("resetexpire", "1d");

		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Confirms a password reset and identifies you to the specified account. You have "
				"%s after requesting a reset to do this before your request expires. Once you "
				"have done this you can set the password using %s."
			),
			Anope::Duration(resetexpire, source.GetAccount()).c_str(),
			source.service->GetQueryCommand("nickserv/set/password").c_str()
		);
		return true;
	}
};

class NSResetPass final
	: public Module
{
private:
	CommandNSConfirmResetPass commandnsconfirmpassword;
	CommandNSResetPass commandnsresetpass;
	PrimitiveExtensibleItem<ResetInfo> reset;

public:
	NSResetPass(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnsconfirmpassword(this, reset)
		, commandnsresetpass(this)
		, reset(this, "reset")
	{
		if (!Config->GetBlock("mail").Get<bool>("usemail"))
			throw ModuleException("Not using mail.");
	}
};

static bool SendResetEmail(User *u, const NickAlias *na, BotInfo *bi)
{
	auto *ri = na->nc->Extend<ResetInfo>("reset");
	ri->code = Anope::Random(Config->GetBlock("options").Get<size_t>("codelength", "15"));
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
