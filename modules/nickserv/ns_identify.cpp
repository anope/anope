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

class NSIdentifyRequest final
	: public IdentifyRequest
{
	CommandSource source;
	Command *cmd;

public:
	NSIdentifyRequest(Module *o, CommandSource &s, Command *c, const Anope::string &acc, const Anope::string &pass)
		: IdentifyRequest(o, acc, pass, s.ip)
		, source(s)
		, cmd(c)
	{
	}

	void OnSuccess() override
	{
		if (!source.GetUser())
			return;

		User *u = source.GetUser();
		NickAlias *na = NickAlias::Find(GetAccount());

		if (!na)
			source.Reply(NICK_X_NOT_REGISTERED, GetAccount().c_str());
		else
		{
			if (u->IsIdentified())
				Log(LOG_COMMAND, source, cmd) << "to log out of account " << u->Account()->display;

			Log(LOG_COMMAND, source, cmd) << "and identified for account " << na->nc->display;
			source.Reply(_("Password accepted - you are now recognized."));
			u->Identify(na);
		}
	}

	void OnFail() override
	{
		if (source.GetUser())
		{
			bool accountexists = NickAlias::Find(GetAccount()) != NULL;
			Log(LOG_COMMAND, source, cmd) << "and failed to identify to" << (accountexists ? " " : " nonexistent ") << "account " << GetAccount();
			if (accountexists)
			{
				source.Reply(PASSWORD_INCORRECT);
				source.GetUser()->BadPassword();
			}
			else
				source.Reply(NICK_X_NOT_REGISTERED, GetAccount().c_str());
		}
	}
};

class CommandNSIdentify final
	: public Command
{
public:
	CommandNSIdentify(Module *creator) : Command(creator, "nickserv/identify", 1, 2)
	{
		this->SetDesc(_("Identify yourself with your password"));
		this->SetSyntax(_("[\037account\037] \037password\037"));
		this->AllowUnregistered(true);
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();

		const Anope::string &nick = params.size() == 2 ? params[0] : u->nick;
		Anope::string pass = params[params.size() - 1];

		NickAlias *na = NickAlias::Find(nick);
		if (na && na->nc->HasExt("NS_SUSPENDED"))
		{
			source.Reply(NICK_X_SUSPENDED, na->nick.c_str());
			return;
		}

		if (u->IsIdentified() && na && u->Account() == na->nc)
		{
			source.Reply(_("You are already identified."));
			return;
		}

		unsigned int maxlogins = Config->GetModule(this->owner).Get<unsigned int>("maxlogins");
		if (na && maxlogins && na->nc->users.size() >= maxlogins)
		{
			source.Reply(_("Account \002%s\002 has already reached the maximum number of simultaneous logins (%u)."), na->nc->display.c_str(), maxlogins);
			return;
		}

		auto *req = new NSIdentifyRequest(owner, source, this, na ? na->nc->display : nick, pass);
		FOREACH_MOD(OnCheckAuthentication, (u, req));
		req->Dispatch();
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Tells %s that you are really the owner of this "
				"nick.  Many commands require you to authenticate yourself "
				"with this command before you use them.  The password "
				"should be the same one you sent with the \002REGISTER\002 "
				"command."
			),
			source.service->nick.c_str());
		return true;
	}
};

class NSIdentify final
	: public Module
{
	CommandNSIdentify commandnsidentify;

public:
	NSIdentify(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnsidentify(this)
	{

	}
};

MODULE_INIT(NSIdentify)
