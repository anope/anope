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

class CommandNSInfo : public Command
{
 private:
	// cannot be const, as it is modified
	void CheckOptStr(Anope::string &buf, NickCoreFlag opt, const Anope::string &str, NickCore *nc, bool reverse_logic = false)
	{
		if (reverse_logic ? !nc->HasFlag(opt) : nc->HasFlag(opt))
		{
			if (!buf.empty())
				buf += ", ";

			buf += str;
		}
	}
 public:
	CommandNSInfo() : Command("INFO", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc("Displays information about a given nickname");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &nick = params[0];
		NickAlias *na = findnick(nick);
		bool has_auspex = u->IsIdentified() && u->Account()->HasPriv("nickserv/auspex");

		if (!na)
		{
			if (nickIsServices(nick, true))
				source.Reply(_("Nick \002%s\002 is part of this Network's Services."), nick.c_str());
			else
				source.Reply(_(NICK_X_NOT_REGISTERED), nick.c_str());
		}
		else if (na->HasFlag(NS_FORBIDDEN))
		{
			if (u->HasMode(UMODE_OPER) && !na->last_usermask.empty())
				source.Reply(_(NICK_X_FORBIDDEN_OPER), nick.c_str(), na->last_usermask.c_str(), !na->last_realname.empty() ? na->last_realname.c_str() : _(NO_REASON));
			else
				source.Reply(_(NICK_X_FORBIDDEN), nick.c_str());
		}
		else
		{
			bool nick_online = false, show_hidden = false;

			/* Is the real owner of the nick we're looking up online? -TheShadow */
			User *u2 = finduser(na->nick);
			if (u2 && u2->Account() == na->nc)
				nick_online = true;

			if (has_auspex || (u->Account() && na->nc == u->Account()))
				show_hidden = true;

			source.Reply(_("%s is %s"), na->nick.c_str(), na->last_realname.c_str());

			if (na->nc->IsServicesOper() && (show_hidden || !na->nc->HasFlag(NI_HIDE_STATUS)))
				source.Reply(_("%s is a services operator of type %s."), na->nick.c_str(), na->nc->ot->GetName().c_str());

			if (nick_online)
			{
				if (show_hidden || !na->nc->HasFlag(NI_HIDE_MASK))
					source.Reply(_("   Is online from: %s"), na->last_usermask.c_str());
				else
					source.Reply(_("%s is currently online."), na->nick.c_str());
			}
			else
			{
				if (show_hidden || !na->nc->HasFlag(NI_HIDE_MASK))
					source.Reply(_("Last seen address: %s"), na->last_usermask.c_str());
			}

			source.Reply(_("  Time registered: %s"), do_strftime(na->time_registered).c_str());

			if (!nick_online)
			{
				source.Reply(_("   Last seen time: %s"), do_strftime(na->last_seen).c_str());
			}

			if (!na->last_quit.empty() && (show_hidden || !na->nc->HasFlag(NI_HIDE_QUIT)))
				source.Reply(_("Last quit message: %s"), na->last_quit.c_str());

			if (!na->nc->email.empty() && (show_hidden || !na->nc->HasFlag(NI_HIDE_EMAIL)))
				source.Reply(_("   E-mail address: %s"), na->nc->email.c_str());

			if (show_hidden)
			{
				if (!Config->s_HostServ.empty() && ircd->vhost && na->hostinfo.HasVhost())
				{
					if (ircd->vident && !na->hostinfo.GetIdent().empty())
						source.Reply(_("            vhost: %s@%s"), na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str());
					else
						source.Reply(_("            vhost: %s"), na->hostinfo.GetHost().c_str());
				}
				if (!na->nc->greet.empty())
					source.Reply(_("    Greet message: %s"), na->nc->greet.c_str());

				Anope::string optbuf;

				CheckOptStr(optbuf, NI_KILLPROTECT, _("Protection"), na->nc);
				CheckOptStr(optbuf, NI_SECURE, _("Security"), na->nc);
				CheckOptStr(optbuf, NI_PRIVATE, _("Private"), na->nc);
				CheckOptStr(optbuf, NI_MSG, _("Message mode"), na->nc);
				CheckOptStr(optbuf, NI_AUTOOP, _("Auto-op"), na->nc);

				source.Reply(_(NICK_INFO_OPTIONS), optbuf.empty() ? _("None") : optbuf.c_str());

				if (na->nc->HasFlag(NI_SUSPENDED))
				{
					if (!na->last_quit.empty())
						source.Reply(_("This nickname is currently suspended, reason: %s"), na->last_quit.c_str());
					else
						source.Reply(_("This nickname is currently suspended"));
				}

				if (na->nc->HasFlag(NI_UNCONFIRMED) == false)
				{
					if (na->HasFlag(NS_NO_EXPIRE))
						source.Reply(_("This nickname will not expire."));
					else
						source.Reply(_("Expires on: %s"), do_strftime(na->last_seen + Config->NSExpire).c_str());
				}
				else
					source.Reply(_("Expires on: %s"), do_strftime(na->time_registered + Config->NSUnconfirmedExpire).c_str());
			}

			FOREACH_MOD(I_OnNickInfo, OnNickInfo(source, na, show_hidden));

			if (na->nc->HasFlag(NI_UNCONFIRMED))
				source.Reply(_("This nickname is unconfirmed."));
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002INFO \037nickname\037\002\n"
				" \n"
				"Displays information about the given nickname, such as\n"
				"the nick's owner, last seen address and time, and nick\n"
				"options."));

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "INFO", _("INFO \037nick\037"));
	}
};

class NSInfo : public Module
{
	CommandNSInfo commandnsinfo;

 public:
	NSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsinfo);
	}
};

MODULE_INIT(NSInfo)
