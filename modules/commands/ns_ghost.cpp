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

class NSGhostRequest : public IdentifyRequest
{
	CommandSource source;
	Command *cmd;
 
 public:
	NSGhostRequest(Module *o, CommandSource &src, Command *c, const Anope::string &user, const Anope::string &pass) : IdentifyRequest(o, user, pass), source(src), cmd(c) { }

	void OnSuccess() anope_override
	{
		if (!source.GetUser() || !source.service)
			return;

		User *user = source.GetUser();
		if (!user->IsIdentified())
			source.Reply(_("You may not ghost an unidentified user, use RECOVER instead."));
		else
		{
			Log(LOG_COMMAND, source, cmd) << "for " << GetAccount();
			Anope::string buf = "GHOST command used by " + source.GetNick();
			user->Kill(source.service->nick, buf);
			source.Reply(_("Ghost with your nick has been killed."));
		}
	}

	void OnFail() anope_override
	{
		source.Reply(ACCESS_DENIED);
		if (!GetPassword().empty())
		{
			Log(LOG_COMMAND, source, cmd) << "with an invalid password for " << GetAccount();
			if (source.GetUser())
				bad_password(source.GetUser());
		}
	}
};

class CommandNSGhost : public Command
{
 public:
	CommandNSGhost(Module *creator) : Command(creator, "nickserv/ghost", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Disconnects a \"ghost\" IRC session using your nick"));
		this->SetSyntax("\037nickname\037 [\037password\037]");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &nick = params[0];
		const Anope::string &pass = params.size() > 1 ? params[1] : "";

		User *user = finduser(nick);
		const NickAlias *na = findnick(nick);

		if (!user)
			source.Reply(NICK_X_NOT_IN_USE, nick.c_str());
		else if (user->server == Me)
			source.Reply(_("\2%s\2 is a services enforcer."), user->nick.c_str());
		else if (!na)
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			source.Reply(NICK_X_SUSPENDED, na->nick.c_str());
		else if (nick.equals_ci(source.GetNick()))
			source.Reply(_("You can't ghost yourself!"));
		else
		{
			bool ok = false;
			if (source.GetAccount() == na->nc)
				ok = true;
			else if (!na->nc->HasFlag(NI_SECURE) && source.GetUser() && is_on_access(source.GetUser(), na->nc))
				ok = true;
			else if (source.GetUser() && !source.GetUser()->fingerprint.empty() && na->nc->FindCert(source.GetUser()->fingerprint))
				ok = true;

			if (ok == false && !pass.empty())
			{
				NSGhostRequest *req = new NSGhostRequest(owner, source, this, na->nc->display, pass);
				FOREACH_MOD(I_OnCheckAuthentication, OnCheckAuthentication(source.GetUser(), req));
				req->Dispatch();
			}
			else
			{
				NSGhostRequest req(owner, source, this, na->nc->display, pass);

				if (ok)
					req.OnSuccess();
				else
					req.OnFail();
			}
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Terminates a \"ghost\" IRC session using your nick. A\n"
				"ghost\" session is one which is not actually connected,\n"
				"but which the IRC server believes is still online for one\n"
				"reason or another. Typically, this happens if your\n"
				"computer crashes or your Internet or modem connection\n"
				"goes down while you're on IRC.\n"
				" \n"
				"In order to use the \002GHOST\002 command for a nick, your\n"
				"current address as shown in /WHOIS must be on that nick's\n"
				"access list, you must be identified and in the group of\n"
				"that nick, or you must supply the correct password for\n"
				"the nickname."));
		return true;
	}
};

class NSGhost : public Module
{
	CommandNSGhost commandnsghost;

 public:
	NSGhost(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnsghost(this)
	{
		this->SetAuthor("Anope");

		if (Config->NoNicknameOwnership)
			throw ModuleException(modname + " can not be used with options:nonicknameownership enabled");
	}
};

MODULE_INIT(NSGhost)
