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

class CommandNSIdentify : public Command
{
 public:
	CommandNSIdentify() : Command("IDENTIFY", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
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
				source.Reply(NICK_IS_PREREG);
			else
				source.Reply(NICK_NOT_REGISTERED);
		}
		else if (na->HasFlag(NS_FORBIDDEN))
			source.Reply(NICK_X_FORBIDDEN, na->nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			source.Reply(NICK_X_SUSPENDED, na->nick.c_str());
		/* You can now identify for other nicks without logging out first,
		 * however you can not identify again for the group you're already
		 * identified as
		 */
		else if (u->Account() && u->Account() == na->nc)
			source.Reply(NICK_ALREADY_IDENTIFIED);
		else
		{
			int res = enc_check_password(pass, na->nc->pass);
			if (!res)
			{
				Log(LOG_COMMAND, u, this) << "and failed to identify";
				source.Reply(PASSWORD_INCORRECT);
				if (bad_password(u))
					return MOD_STOP;
			}
			else if (res == -1)
				source.Reply(NICK_IDENTIFY_FAILED);
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

				FOREACH_MOD(I_OnNickIdentify, OnNickIdentify(u));

				Log(LOG_COMMAND, u, this) << "and identified for account " << u->Account()->display;
				source.Reply(NICK_IDENTIFY_SUCCEEDED);
				if (ircd->vhost)
					do_on_id(u);
				if (Config->NSModeOnID)
					do_setmodes(u);

				if (Config->NSForceEmail && u->Account() && u->Account()->email.empty())
				{
					source.Reply(NICK_IDENTIFY_EMAIL_REQUIRED);
					source.Reply(NICK_IDENTIFY_EMAIL_HOWTO);
				}

				if (u->IsIdentified())
					check_memos(u);
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(NickServ, NICK_HELP_IDENTIFY);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(NickServ, u, "IDENTIFY", NICK_IDENTIFY_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(NickServ, NICK_HELP_CMD_IDENTIFY);
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
