
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

class NSRecoverRequest : public IdentifyRequest
{
	CommandSource source;
	Command *cmd;
	dynamic_reference<NickAlias> na;
	dynamic_reference<User> u;
 
 public:
	NSRecoverRequest(Module *m, CommandSource &src, Command *c, User *user, NickAlias *n, const Anope::string &pass) : IdentifyRequest(m, n->nc->display, pass), source(src), cmd(c), na(n), u(user) { }

	void OnSuccess() anope_override
	{
		if (!na || !u)
			return;

		u->SendMessage(source.service, FORCENICKCHANGE_NOW);

		if (u->Account() == na->nc)
		{
			ircdproto->SendLogout(u);
			u->RemoveMode(findbot(Config->NickServ), UMODE_REGISTERED);
		}

		u->Collide(na);

		/* Convert Config->NSReleaseTimeout seconds to string format */
		Anope::string relstr = duration(Config->NSReleaseTimeout);
		source.Reply(NICK_RECOVERED, Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str(), na->nick.c_str(), relstr.c_str());
	}

	void OnFail() anope_override
	{
		if (!na || !u)
			return;

		source.Reply(ACCESS_DENIED);
		if (!GetPassword().empty())
		{
			Log(LOG_COMMAND, source, cmd) << "with invalid password for " << na->nick;
			bad_password(u);
		}
	}
};

class CommandNSRecover : public Command
{
 public:
	CommandNSRecover(Module *creator) : Command(creator, "nickserv/recover", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Kill another user who has taken your nick"));
		this->SetSyntax(_("\037nickname\037 [\037password\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{

		const Anope::string &nick = params[0];
		const Anope::string &pass = params.size() > 1 ? params[1] : "";

		NickAlias *na;
		User *u2;
		if (!(u2 = finduser(nick)))
			source.Reply(NICK_X_NOT_IN_USE, nick.c_str());
		else if (u2->server == Me)
			source.Reply(_("\2%s\2 has already been recovered."), u2->nick.c_str());
		else if (!(na = findnick(u2->nick)))
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			source.Reply(NICK_X_SUSPENDED, na->nick.c_str());
		else if (nick.equals_ci(source.GetNick()))
			source.Reply(_("You can't recover yourself!"));
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
				NSRecoverRequest *req = new NSRecoverRequest(owner, source, this, u2, na, pass);
				FOREACH_MOD(I_OnCheckAuthentication, OnCheckAuthentication(source.GetUser(), req));
				req->Dispatch();
			}
			else
			{
				NSRecoverRequest req(owner, source, this, u2, na, pass);

				if (ok)
					req.OnSuccess();
				else
					req.OnFail();
			}
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		/* Convert Config->NSReleaseTimeout seconds to string format */
		Anope::string relstr = duration(Config->NSReleaseTimeout);

		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to recover your nickname if someone else has\n"
				"taken it; this does the same thing that %s does\n"
				"automatically if someone tries to use a kill-protected\n"
				"nick.\n"
				" \n"
				"When you give this command, %s will bring a fake\n"
				"user online with the same nickname as the user you're\n"
				"trying to recover your nick from.  This causes the IRC\n"
				"servers to disconnect the other user.  This fake user will\n"
				"remain online for %s to ensure that the other\n"
				"user does not immediately reconnect; after that time, you\n"
				"can reclaim your nick.  Alternatively, use the \002RELEASE\002\n"
				"command (\002%s%s HELP RELEASE\002) to get the nick\n"
				"back sooner.\n"
				" \n"
				"In order to use the \002RECOVER\002 command for a nick, your\n"
				"current address as shown in /WHOIS must be on that nick's\n"
				"access list, you must be identified and in the group of\n"
				"that nick, or you must supply the correct password for\n"
				"the nickname."), Config->NickServ.c_str(), Config->NickServ.c_str(), relstr.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str());

		return true;
	}
};

class NSRecover : public Module
{
	CommandNSRecover commandnsrecover;

 public:
	NSRecover(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnsrecover(this)
	{
		this->SetAuthor("Anope");

		if (Config->NoNicknameOwnership)
			throw ModuleException(modname + " can not be used with options:nonicknameownership enabled");
	}
};

MODULE_INIT(NSRecover)
