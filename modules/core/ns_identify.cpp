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

class CommandNSIdentify : public Command
{
 public:
	CommandNSIdentify() : Command("IDENTIFY", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc("Identify yourself with your password");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &nick = params.size() == 2 ? params[0] : u->nick;
		Anope::string pass = params[params.size() - 1];

		NickAlias *na = findnick(nick), *this_na = findnick(u->nick);
		if (!na)
		{
			NickRequest *nr = findrequestnick(nick);
			if (nr)
				source.Reply(_(NICK_IS_PREREG));
			else
				source.Reply(_(NICK_NOT_REGISTERED));
		}
		else if (na->HasFlag(NS_FORBIDDEN))
			source.Reply(_(NICK_X_FORBIDDEN), na->nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			source.Reply(_(NICK_X_SUSPENDED), na->nick.c_str());
		/* You can now identify for other nicks without logging out first,
		 * however you can not identify again for the group you're already
		 * identified as
		 */
		else if (u->Account() && u->Account() == na->nc)
			source.Reply(_("You are already identified."));
		else
		{
			int res = enc_check_password(pass, na->nc->pass);
			if (!res)
			{
				Log(LOG_COMMAND, u, this) << "and failed to identify";
				source.Reply(_(PASSWORD_INCORRECT));
				if (bad_password(u))
					return MOD_STOP;
			}
			else if (res == -1)
				source.Reply(_("Sorry, identification failed."));
			else
			{
				if (u->IsIdentified())
					Log(LOG_COMMAND, "nickserv/identify") << "to log out of account " << u->Account()->display;

				na->last_realname = u->realname;
				na->last_seen = Anope::CurTime;

				u->Login(na->nc);
				ircdproto->SendAccountLogin(u, u->Account());
				ircdproto->SetAutoIdentificationToken(u);

				if (this_na && this_na->nc == na->nc)
					u->SetMode(NickServ, UMODE_REGISTERED);

				u->UpdateHost();

				Log(LOG_COMMAND, u, this) << "and identified for account " << u->Account()->display;
				source.Reply(_("Password accepted - you are now recognized."));
				if (ircd->vhost)
					do_on_id(u);
				if (Config->NSModeOnID)
					do_setmodes(u);

				FOREACH_MOD(I_OnNickIdentify, OnNickIdentify(u));

				if (Config->NSForceEmail && u->Account() && u->Account()->email.empty())
				{
					source.Reply(_("You must now supply an e-mail for your nick.\n"
						"This e-mail will allow you to retrieve your password in\n"
						"case you forget it."));
					source.Reply(_("Type \002%R%s SET EMAIL \037e-mail\037\002 in order to set your e-mail.\n"
						"Your privacy is respected; this e-mail won't be given to\n"
					"any third-party person."), NickServ->nick.c_str());
				}

				if (u->IsIdentified())
					check_memos(u);
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002IDENTIFY [account] \037password\037\002\n"
				" \n"
				"Tells %s that you are really the owner of this\n"
				"nick.  Many commands require you to authenticate yourself\n"
				"with this command before you use them.  The password\n"
				"should be the same one you sent with the \002REGISTER\002\n"
				"command."), NickServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "IDENTIFY", _("IDENTIFY [account] \037password\037"));
	}
};

class NSIdentify : public Module
{
	CommandNSIdentify commandnsidentify;

 public:
	NSIdentify(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsidentify);
	}
};

MODULE_INIT(NSIdentify)
