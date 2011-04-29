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
#include "chanserv.h"

class CommandCSStatus : public Command
{
 public:
	CommandCSStatus() : Command("STATUS", 2, 2, "chanserv/status")
	{
		this->SetDesc(_("Returns the current access level of a user on a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		const Anope::string &nick = params[1];

		User *u2 = finduser(nick);
		ChanAccess *u2_access = ci->GetAccess(u2);
		if (u2)
			source.Reply(_("STATUS %s %s %d"), ci->name.c_str(), u2->nick.c_str(), u2_access ? u2_access->level : 0);
		else /* !u2 */
			source.Reply(_("STATUS ERROR Nick %s not online"), nick.c_str());
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002STATUS \037channel\037 \037nickname\037\002\n"
				" \n"
				"Returns the current access level of the given nick on the\n"
				"given channel.  The reply is of the form:\n"
				" \n"
				"    STATUS \037channel\037 \037nickname\037 \037access-level\037\n"
				" \n"
				"If an error occurs, the reply will be in the form:\n"
				" \n"
				"    STATUS ERROR \037error-message\037"));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "STATUS", _("STATUS \037channel\037 \037item\037"));
	}
};

class CSStatus : public Module
{
	CommandCSStatus commandcsstatus;

 public:
	CSStatus(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		this->AddCommand(chanserv->Bot(), &commandcsstatus);
	}
};

MODULE_INIT(CSStatus)
