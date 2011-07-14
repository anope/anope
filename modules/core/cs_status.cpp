/* ChanServ core functions
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

class CommandCSStatus : public Command
{
 public:
	CommandCSStatus(Module *creator) : Command(creator, "chanserv/status", 2, 2, "chanserv/status")
	{
		this->SetDesc(_("Returns the current access level of a user on a channel"));
		this->SetSyntax(_("\037channel\037 \037item\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &nick = params[1];

		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		User *u2 = finduser(nick);
		ChanAccess *u2_access = ci->GetAccess(u2);
		if (u2)
			source.Reply(_("STATUS %s %s %d"), ci->name.c_str(), u2->nick.c_str(), u2_access ? u2_access->level : 0);
		else /* !u2 */
			source.Reply(_("STATUS ERROR Nick %s not online"), nick.c_str());
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Returns the current access level of the given nick on the\n"
				"given channel.  The reply is of the form:\n"
				" \n"
				"    STATUS \037channel\037 \037nickname\037 \037access-level\037\n"
				" \n"
				"If an error occurs, the reply will be in the form:\n"
				" \n"
				"    STATUS ERROR \037error-message\037"));
		return true;
	}
};

class CSStatus : public Module
{
	CommandCSStatus commandcsstatus;

 public:
	CSStatus(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcsstatus(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandcsstatus);
	}
};

MODULE_INIT(CSStatus)
