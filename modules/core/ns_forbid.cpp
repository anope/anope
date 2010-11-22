/* NickServ core functions
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

class CommandNSForbid : public Command
{
 public:
	CommandNSForbid() : Command("FORBID", 1, 2, "nickserv/forbid")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickAlias *na;
		Anope::string nick = params[0];
		Anope::string reason = params.size() > 1 ? params[1] : "";

		/* Assumes that permission checking has already been done. */
		if (Config->ForceForbidReason && reason.empty())
		{
			this->OnSyntaxError(u, "");
			return MOD_CONT;
		}

		if (readonly)
			u->SendMessage(NickServ, READ_ONLY_MODE);
		if (!ircdproto->IsNickValid(nick))
		{
			u->SendMessage(NickServ, NICK_X_FORBIDDEN, nick.c_str());
			return MOD_CONT;
		}
		if ((na = findnick(nick)))
		{
			if (Config->NSSecureAdmins && na->nc->IsServicesOper())
			{
				u->SendMessage(NickServ, ACCESS_DENIED);
				return MOD_CONT;
			}
			delete na;
		}
		NickCore *nc = new NickCore(nick);
		nc->SetFlag(NI_FORBIDDEN);
		na = new NickAlias(nick, nc);
		na->SetFlag(NS_FORBIDDEN);
		na->last_usermask = u->nick;
		if (!reason.empty())
			na->last_realname = reason;

		User *curr = finduser(na->nick);

		if (curr)
		{
			curr->SendMessage(NickServ, FORCENICKCHANGE_NOW);
			curr->Collide(na);
		}

		if (ircd->sqline)
		{
			XLine x(na->nick, !reason.empty() ? reason : "Forbidden");
			ircdproto->SendSQLine(&x);
		}

		if (Config->WallForbid)
			ircdproto->SendGlobops(NickServ, "\2%s\2 used FORBID on \2%s\2", u->nick.c_str(), nick.c_str());

		Log(LOG_ADMIN, u, this) << "to forbid nick " << nick;
		u->SendMessage(NickServ, NICK_FORBID_SUCCEEDED, nick.c_str());

		FOREACH_MOD(I_OnNickForbidden, OnNickForbidden(na));

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(NickServ, NICK_SERVADMIN_HELP_FORBID);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(NickServ, u, "FORBID", Config->ForceForbidReason ? NICK_FORBID_SYNTAX_REASON : NICK_FORBID_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(NickServ, NICK_HELP_CMD_FORBID);
	}
};

class NSForbid : public Module
{
	CommandNSForbid commandnsforbid;

 public:
	NSForbid(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsforbid);
	}
};

MODULE_INIT(NSForbid)
