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
#include "nickserv.h"

class CommandNSForbid : public Command
{
 public:
	CommandNSForbid() : Command("FORBID", 1, 2, "nickserv/forbid")
	{
		this->SetDesc(_("Prevents a nickname from being registered"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &nick = params[0];
		const Anope::string &reason = params.size() > 1 ? params[1] : "";

		/* Assumes that permission checking has already been done. */
		if (Config->ForceForbidReason && reason.empty())
		{
			this->OnSyntaxError(source, "");
			return MOD_CONT;
		}

		if (readonly)
			source.Reply(_(READ_ONLY_MODE));
		if (!ircdproto->IsNickValid(nick))
		{
			source.Reply(_(NICK_X_FORBIDDEN), nick.c_str());
			return MOD_CONT;
		}

		NickAlias *na = findnick(nick);
		if (na)
		{
			if (Config->NSSecureAdmins && na->nc->IsServicesOper())
			{
				source.Reply(_(ACCESS_DENIED));
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
			curr->SendMessage(nickserv->Bot(), _(FORCENICKCHANGE_NOW));
			curr->Collide(na);
		}

		if (ircd->sqline)
		{
			XLine x(na->nick, !reason.empty() ? reason : "Forbidden");
			ircdproto->SendSQLine(NULL, &x);
		}

		Log(LOG_ADMIN, u, this) << "to forbid nick " << nick;
		source.Reply(_("Nick \002%s\002 is now forbidden."), nick.c_str());

		FOREACH_MOD(I_OnNickForbidden, OnNickForbidden(na));

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002FORBID \037nickname\037 [\037reason\037]\002\n"
				" \n"
				"Disallows a nickname from being registered or used by\n"
				"anyone.  May be cancelled by dropping the nick.\n"
				" \n"
				"On certain networks, reason is required."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "FORBID", Config->ForceForbidReason ? _("FORBID \037nickname\037 \037reason\037") : _("FORBID \037nickname\037 [\037reason\037]"));
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

		if (!nickserv)
			throw ModuleException("NickServ is not loaded!");

		this->AddCommand(nickserv->Bot(), &commandnsforbid);
	}
};

MODULE_INIT(NSForbid)
