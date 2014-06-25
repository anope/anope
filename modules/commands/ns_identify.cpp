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

class NSIdentifyRequestListener : public NickServ::IdentifyRequestListener
{
	CommandSource source;
	Command *cmd;

 public:
	NSIdentifyRequestListener(CommandSource &s, Command *c) : source(s), cmd(c) { }

	void OnSuccess(NickServ::IdentifyRequest *req) override
	{
		if (!source.GetUser())
			return;

		User *u = source.GetUser();
		NickServ::Nick *na = NickServ::FindNick(req->GetAccount());

		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), req->GetAccount());
			return;
		}

		if (u->IsIdentified())
			Log(LOG_COMMAND, source, cmd) << "to log out of account " << u->Account()->display;

		Log(LOG_COMMAND, source, cmd) << "and identified for account " << na->nc->display;
		source.Reply(_("Password accepted - you are now recognized as \002{0}\002."), na->nc->display);
		u->Identify(na);
	}

	void OnFail(NickServ::IdentifyRequest *req) override
	{
		if (!source.GetUser())
			return;

		bool accountexists = NickServ::FindNick(req->GetAccount()) != NULL;
		Log(LOG_COMMAND, source, cmd) << "and failed to identify to" << (accountexists ? " " : " nonexistent ") << "account " << req->GetAccount();
		if (accountexists)
		{
			source.Reply(_("Password incorrect."));
			source.GetUser()->BadPassword();
		}
		else
			source.Reply("\002{0}\002 isn't registered.", req->GetAccount());
	}
};

class CommandNSIdentify : public Command
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

		NickServ::Nick *na = NickServ::FindNick(nick);
		if (na && na->nc->HasExt("NS_SUSPENDED"))
		{
			source.Reply(_("\002{0}\002 is suspended."), na->nick);
			return;
		}

		if (u->Account() && na && u->Account() == na->nc)
		{
			source.Reply(_("You are already identified."));
			return;
		}

		NickServ::IdentifyRequest *req = NickServ::service->CreateIdentifyRequest(new NSIdentifyRequestListener(source, this), owner, na ? na->nc->display : nick, pass);
		Event::OnCheckAuthentication(&Event::CheckAuthentication::OnCheckAuthentication, u, req);
		req->Dispatch();
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Logs you in to account \037account\037. If no \037account\037 is given, your current nickname is used."
		               " Many commands require you to authenticate with this command before you use them. \037password\037 should be the same one you registered with."));
		return true;
	}
};

class NSIdentify : public Module
{
	CommandNSIdentify commandnsidentify;

 public:
	NSIdentify(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnsidentify(this)
	{

	}
};

MODULE_INIT(NSIdentify)
