/* NickServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

class CommandNSIdentify : public Command
{
 public:
	CommandNSIdentify(const std::string &cname) : Command(cname, 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		std::string pass = params[0].c_str();
		NickAlias *na;
		NickRequest *nr;
		int res;

		if (!(na = findnick(u->nick.c_str())))
		{
			if ((nr = findrequestnick(u->nick.c_str())))
				notice_lang(Config.s_NickServ, u, NICK_IS_PREREG);
			else
				notice_lang(Config.s_NickServ, u, NICK_NOT_REGISTERED);
		}
		else if (na->HasFlag(NS_FORBIDDEN))
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
		else if (na->nc->HasFlag(NI_SUSPENDED))
			notice_lang(Config.s_NickServ, u, NICK_X_SUSPENDED, na->nick);
		/* You can now identify for other nicks without logging out first,
		 * however you can not identify again for the group you're already
		 * identified as
		 */
		else if (u->Account() && u->Account() == na->nc)
			notice_lang(Config.s_NickServ, u, NICK_ALREADY_IDENTIFIED);
		else if (!(res = enc_check_password(pass, na->nc->pass)))
		{
			Alog() << Config.s_NickServ << ": Failed IDENTIFY for " << u->nick << "!" << u->GetIdent() << "@" << u->host;
			notice_lang(Config.s_NickServ, u, PASSWORD_INCORRECT);
			if (bad_password(u))
				return MOD_STOP;
		}
		else if (res == -1)
			notice_lang(Config.s_NickServ, u, NICK_IDENTIFY_FAILED);
		else
		{
			if (u->IsIdentified())
			{
				Alog() << Config.s_NickServ << ": " << u->GetMask() << " logged out of account " << u->Account()->display;
			}

			if (na->last_realname)
				delete [] na->last_realname;
			na->last_realname = sstrdup(u->realname);
			na->last_seen = time(NULL);

			u->Login(na->nc);
			ircdproto->SendAccountLogin(u, u->Account());
			ircdproto->SetAutoIdentificationToken(u);

			u->UpdateHost();

			FOREACH_MOD(I_OnNickIdentify, OnNickIdentify(u));

			Alog() << Config.s_NickServ << ": " << u->GetMask() << " identified for account " << u->Account()->display;
			notice_lang(Config.s_NickServ, u, NICK_IDENTIFY_SUCCEEDED);
			if (ircd->vhost)
				do_on_id(u);
			if (Config.NSModeOnID)
				do_setmodes(u);

			if (Config.NSForceEmail && u->Account() && !u->Account()->email)
			{
				notice_lang(Config.s_NickServ, u, NICK_IDENTIFY_EMAIL_REQUIRED);
				notice_help(Config.s_NickServ, u, NICK_IDENTIFY_EMAIL_HOWTO);
			}

			if (u->IsIdentified())
				check_memos(u);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_IDENTIFY);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_NickServ, u, "IDENTIFY", NICK_IDENTIFY_SYNTAX);
	}
};

class NSIdentify : public Module
{
 public:
	NSIdentify(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSIdentify("IDENTIFY"));
		this->AddCommand(NICKSERV, new CommandNSIdentify("ID"));

		ModuleManager::Attach(I_OnNickServHelp, this);
	}
	void OnNickServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_IDENTIFY);
	}
};

MODULE_INIT(NSIdentify)
