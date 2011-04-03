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
 public:
	CommandNSRecover() : Command("RECOVER", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Kill another user who has taken your nick"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &nick = params[0];
		const Anope::string &pass = params.size() > 1 ? params[1] : "";

		NickAlias *na;
		User *u2;
		if (!(u2 = finduser(nick)))
			source.Reply(_(NICK_X_NOT_IN_USE), nick.c_str());
		else if (!(na = findnick(u2->nick)))
			source.Reply(_(NICK_X_NOT_REGISTERED), nick.c_str());
		else if (na->HasFlag(NS_FORBIDDEN))
			source.Reply(_(NICK_X_FORBIDDEN), na->nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			source.Reply(_(NICK_X_SUSPENDED), na->nick.c_str());
		else if (nick.equals_ci(u->nick))
			source.Reply(_("You can't recover yourself!"));
		else if (!pass.empty())
		{
			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnCheckAuthentication, OnCheckAuthentication(u, this, params, na->nc->display, pass));
			if (MOD_RESULT == EVENT_STOP)
				return MOD_CONT;

			if (MOD_RESULT == EVENT_ALLOW)
			{
				u2->SendMessage(NickServ, _(FORCENICKCHANGE_NOW));
				u2->Collide(na);

				/* Convert Config->NSReleaseTimeout seconds to string format */
				Anope::string relstr = duration(Config->NSReleaseTimeout);

				source.Reply(_(NICK_RECOVERED), Config->UseStrictPrivMsgString.c_str(), Config->s_NickServ.c_str(), nick.c_str(), relstr.c_str());
			}
			else
			{
				source.Reply(_(ACCESS_DENIED));
				Log(LOG_COMMAND, u, this) << "with invalid password for " << nick;
				if (bad_password(u))
					return MOD_STOP;
			}
		}
		else
		{
			if (u->Account() == na->nc || (!na->nc->HasFlag(NI_SECURE) && is_on_access(u, na->nc)) ||
					(!u->fingerprint.empty() && na->nc->FindCert(u->fingerprint)))
			{
				u2->SendMessage(NickServ, _(FORCENICKCHANGE_NOW));
				u2->Collide(na);

				/* Convert Config->NSReleaseTimeout seconds to string format */
				Anope::string relstr = duration(Config->NSReleaseTimeout);

				source.Reply(_(NICK_RECOVERED), Config->UseStrictPrivMsgString.c_str(), Config->s_NickServ.c_str(), nick.c_str(), relstr.c_str());
			}
			else
				source.Reply(_(ACCESS_DENIED));
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		/* Convert Config->NSReleaseTimeout seconds to string format */
		Anope::string relstr = duration(Config->NSReleaseTimeout);

		source.Reply(_("Syntax: \002RECOVER \037nickname\037 [\037password\037]\002\n"
				" \n"
				"Allows you to recover your nickname if someone else has\n"
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
				"the nickname."), NickServ->nick.c_str(), NickServ->nick.c_str(), relstr.c_str(), Config->UseStrictPrivMsgString.c_str(), NickServ->nick.c_str());

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "RECOVER", _("RECOVER \037nickname\037 [\037password\037]"));
	}
};

class NSRecover : public Module
{
	CommandNSRecover commandnsrecover;

 public:
	NSRecover(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsrecover);
	}
};

MODULE_INIT(NSRecover)
