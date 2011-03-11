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

class CommandNSStatus : public Command
{
 public:
	CommandNSStatus() : Command("STATUS", 0, 16)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Returns the owner status of the given nickname"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &nick = !params.empty() ? params[0] : u->nick;
		NickAlias *na = findnick(nick);
		spacesepstream sep(nick);
		Anope::string nickbuf;

		while (sep.GetToken(nickbuf))
		{
			User *u2 = finduser(nickbuf);
			if (!u2) /* Nick is not online */
				source.Reply(_("STATUS %s %d %s"), nickbuf.c_str(), 0, "");
			else if (u2->IsIdentified() && na && na->nc == u2->Account()) /* Nick is identified */
				source.Reply(_("STATUS %s %d %s"), nickbuf.c_str(), 3, u2->Account()->display.c_str());
			else if (u2->IsRecognized()) /* Nick is recognised, but NOT identified */
				source.Reply(_("STATUS %s %d %s"), nickbuf.c_str(), 2, u2->Account() ? u2->Account()->display.c_str() : "");
			else if (!na) /* Nick is online, but NOT a registered */
				source.Reply(_("STATUS %s %d %s"), nickbuf.c_str(), 0, "");
			else
				/* Nick is not identified for the nick, but they could be logged into an account,
				 * so we tell the user about it
				 */
				source.Reply(_("STATUS %s %d %s"), nickbuf.c_str(), 1, u2->Account() ? u2->Account()->display.c_str() : "");
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002STATUS \037nickname\037...\002\n"
				" \n"
				"Returns whether the user using the given nickname is\n"
				"recognized as the owner of the nickname. The response has\n"
				"this format:\n"
				" \n"
				"    \037nickname\037 \037status-code\037 \037account\037\n"
				" \n"
				"where \037nickname\037 is the nickname sent with the command,\n"
				"\037status-code\037 is one of the following, and \037account\037\n"
				"is the account they are logged in as.\n"
				" \n"
				"    0 - no such user online \002or\002 nickname not registered\n"
				"    1 - user not recognized as nickname's owner\n"
				"    2 - user recognized as owner via access list only\n"
				"    3 - user recognized as owner via password identification\n"
				" \n"
				"Up to sixteen nicknames may be sent with each command; the\n"
				"rest will be ignored. If no nickname is given, your status\n"
				"will be returned."));
		return true;
	}
};

class NSStatus : public Module
{
	CommandNSStatus commandnsstatus;

 public:
	NSStatus(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsstatus);
	}
};

MODULE_INIT(NSStatus)
