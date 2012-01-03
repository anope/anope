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

class CommandNSRelease : public Command
{
 public:
	CommandNSRelease(Module *creator) : Command(creator, "nickserv/release", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Regain custody of your nick after RECOVER"));
		this->SetSyntax(_("\037nickname\037 [\037password\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &nick = params[0];
		Anope::string pass = params.size() > 1 ? params[1] : "";
		NickAlias *na;

		if (!(na = findnick(nick)))
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			source.Reply(NICK_X_SUSPENDED, na->nick.c_str());
		else if (!na->HasFlag(NS_HELD))
			source.Reply(_("Nick \002%s\002 isn't being held."), nick.c_str());
		else if (!pass.empty())
		{
			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnCheckAuthentication, OnCheckAuthentication(this, &source, params, na->nc->display, pass));
			if (MOD_RESULT == EVENT_STOP)
				return;

			if (MOD_RESULT == EVENT_ALLOW)
			{
				Log(LOG_COMMAND, u, this) << "released " << na->nick;
				na->Release();
				source.Reply(_("Services' hold on your nick has been released."));
			}
			else
			{
				source.Reply(ACCESS_DENIED);
				Log(LOG_COMMAND, u, this) << "invalid password for " << nick;
				bad_password(u);
			}
		}
		else
		{
			if (u->Account() == na->nc || (!na->nc->HasFlag(NI_SECURE) && is_on_access(u, na->nc)) ||
					(!u->fingerprint.empty() && na->nc->FindCert(u->fingerprint)))
			{
				na->Release();
				source.Reply(_("Services' hold on your nick has been released."));
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
		source.Reply(_("Instructs %s to remove any hold on your nickname\n"
				"caused by automatic kill protection or use of the \002RECOVER\002\n"
				"command. This holds lasts for %s;\n"
				"This command gets rid of them sooner.\n"
				" \n"
				"In order to use the \002RELEASE\002 command for a nick, your\n"
				"current address as shown in /WHOIS must be on that nick's\n"
				"access list, you must be identified and in the group of\n"
				"that nick, or you must supply the correct password for\n"
				"the nickname."), Config->NickServ.c_str(), relstr.c_str());


		return true;
	}
};

class NSRelease : public Module
{
	CommandNSRelease commandnsrelease;

 public:
	NSRelease(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnsrelease(this)
	{
		this->SetAuthor("Anope");

		if (Config->NoNicknameOwnership)
			throw ModuleException(modname + " can not be used with options:nonicknameownership enabled");
	}
};

MODULE_INIT(NSRelease)
