
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

class CommandNSRecover : public Command
{
 private:
	void DoRecover(CommandSource &source, User *u, NickAlias *na, const Anope::string &nick)
	{
		u->SendMessage(source.owner, FORCENICKCHANGE_NOW);

		if (u->Account() == na->nc)
		{
			ircdproto->SendLogout(u);
			u->RemoveMode(findbot(Config->NickServ), UMODE_REGISTERED);
		}

		u->Collide(na);

		/* Convert Config->NSReleaseTimeout seconds to string format */
		Anope::string relstr = duration(Config->NSReleaseTimeout);
		source.Reply(NICK_RECOVERED, Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str(), nick.c_str(), relstr.c_str());
	}

 public:
	CommandNSRecover(Module *creator) : Command(creator, "nickserv/recover", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Kill another user who has taken your nick"));
		this->SetSyntax(_("\037nickname\037 [\037password\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &nick = params[0];
		const Anope::string &pass = params.size() > 1 ? params[1] : "";

		NickAlias *na;
		User *u2;
		if (!(u2 = finduser(nick)))
			source.Reply(NICK_X_NOT_IN_USE, nick.c_str());
		else if (!(na = findnick(u2->nick)))
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			source.Reply(NICK_X_SUSPENDED, na->nick.c_str());
		else if (nick.equals_ci(u->nick))
			source.Reply(_("You can't recover yourself!"));
		else if (!pass.empty())
		{
			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnCheckAuthentication, OnCheckAuthentication(this, &source, params, na->nc->display, pass));
			if (MOD_RESULT == EVENT_STOP)
				return;

			if (MOD_RESULT == EVENT_ALLOW)
			{
				this->DoRecover(source, u2, na, nick);
			}
			else
			{
				source.Reply(ACCESS_DENIED);
				Log(LOG_COMMAND, u, this) << "with invalid password for " << nick;
				bad_password(u);
			}
		}
		else
		{
			if (u->Account() == na->nc || (!na->nc->HasFlag(NI_SECURE) && is_on_access(u, na->nc)) ||
					(!u->fingerprint.empty() && na->nc->FindCert(u->fingerprint)))
			{
				this->DoRecover(source, u2, na, nick);
			}
			else
				source.Reply(ACCESS_DENIED);
		}
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
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
