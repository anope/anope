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
		if (Config.ForceForbidReason && reason.empty())
		{
			this->OnSyntaxError(u, "");
			return MOD_CONT;
		}

		if (readonly)
			notice_lang(Config.s_NickServ, u, READ_ONLY_MODE);
		if (!ircdproto->IsNickValid(nick))
		{
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, nick.c_str());
			return MOD_CONT;
		}
		if ((na = findnick(nick)))
		{
			if (Config.NSSecureAdmins && na->nc->IsServicesOper())
			{
				notice_lang(Config.s_NickServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}
			delete na;
		}
		NickCore *nc = new NickCore(nick);
		nc->SetFlag(NI_FORBIDDEN);
		na = new NickAlias(nick, nc);
		if (na)
		{
			na->SetFlag(NS_FORBIDDEN);
			na->last_usermask = u->nick;
			if (!reason.empty())
				na->last_realname = reason;

			User *curr = finduser(na->nick);

			if (curr)
			{
				notice_lang(Config.s_NickServ, curr, FORCENICKCHANGE_NOW);
				curr->Collide(na);
			}

			if (ircd->sqline)
			{
				XLine x(na->nick, !reason.empty() ? reason : "Forbidden");
				ircdproto->SendSQLine(&x);
			}

			if (Config.WallForbid)
				ircdproto->SendGlobops(NickServ, "\2%s\2 used FORBID on \2%s\2", u->nick.c_str(), nick.c_str());

			Alog() << Config.s_NickServ << ": " << u->nick << " set FORBID for nick " << nick;
			notice_lang(Config.s_NickServ, u, NICK_FORBID_SUCCEEDED, nick.c_str());

			FOREACH_MOD(I_OnNickForbidden, OnNickForbidden(na));
		}
		else
		{
			Alog() << Config.s_NickServ << ": Valid FORBID for " << nick << " by " << u->nick << " failed";
			notice_lang(Config.s_NickServ, u, NICK_FORBID_FAILED, nick.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_SERVADMIN_HELP_FORBID);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_NickServ, u, "FORBID", Config.ForceForbidReason ? NICK_FORBID_SYNTAX_REASON : NICK_FORBID_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_FORBID);
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
