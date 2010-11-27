/* OperServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandOSSVSNick : public Command
{
 public:
	CommandOSSVSNick() : Command("SVSNICK", 2, 2, "operserv/svsnick")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &nick = params[0];
		Anope::string newnick = params[1];
		User *u2;

		NickAlias *na;

		/* Truncate long nicknames to NICKMAX-2 characters */
		if (newnick.length() > Config->NickLen)
		{
			source.Reply(NICK_X_TRUNCATED, newnick.c_str(), Config->NickLen, newnick.c_str());
			newnick = params[1].substr(0, Config->NickLen);
		}

		/* Check for valid characters */
		if (newnick[0] == '-' || isdigit(newnick[0]))
		{
			source.Reply(NICK_X_ILLEGAL, newnick.c_str());
			return MOD_CONT;
		}
		for (unsigned i = 0, end = newnick.length(); i < end; ++i)
			if (!isvalidnick(newnick[i]))
			{
				source.Reply(NICK_X_ILLEGAL, newnick.c_str());
				return MOD_CONT;
			}

		/* Check for a nick in use or a forbidden/suspended nick */
		if (!(u2 = finduser(nick)))
			source.Reply(NICK_X_NOT_IN_USE, nick.c_str());
		else if (finduser(newnick))
			source.Reply(NICK_X_IN_USE, newnick.c_str());
		else if ((na = findnick(newnick)) && na->HasFlag(NS_FORBIDDEN))
			source.Reply(NICK_X_FORBIDDEN, newnick.c_str());
		else
		{
			source.Reply(OPER_SVSNICK_NEWNICK, nick.c_str(), newnick.c_str());
			ircdproto->SendGlobops(OperServ, "%s used SVSNICK to change %s to %s", u->nick.c_str(), nick.c_str(), newnick.c_str());
			ircdproto->SendForceNickChange(u2, newnick, Anope::CurTime);
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(OPER_HELP_SVSNICK);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SVSNICK", OPER_SVSNICK_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(OPER_HELP_CMD_SVSNICK);
	}
};

class OSSVSNick : public Module
{
	CommandOSSVSNick commandossvsnick;

 public:
	OSSVSNick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		if (!ircd->svsnick)
			throw ModuleException("Your IRCd does not support SVSNICK");

		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandossvsnick);
	}
};

MODULE_INIT(OSSVSNick)
