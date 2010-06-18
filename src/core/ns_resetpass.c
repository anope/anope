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

class CommandNSResetPass : public Command
{
 public:
	CommandNSResetPass() : Command("RESETPASS", 1, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickAlias *na;

		if (Config.RestrictMail && !u->Account()->HasCommand("nickserv/resetpass"))
			notice_lang(Config.s_NickServ, u, ACCESS_DENIED);
		if (!(na = findnick(params[0].c_str())))
			notice_lang(Config.s_NickServ, u, NICK_X_NOT_REGISTERED, params[0].c_str());
		else if (na->HasFlag(NS_FORBIDDEN))
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
		else
		{
			char buf[BUFSIZE], message[BUFSIZE];
			snprintf(buf, sizeof(buf), getstring(na, NICK_RESETPASS_SUBJECT), na->nick);

			MailInfo *mail = MailBegin(u, na->nc, buf, Config.s_NickServ);
			if (!mail)
				return MOD_CONT;

			char passcode[20];
			int min = 1, max = 62;
			int chars[] = {
				' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
				'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
				'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
				'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
				'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
			};

	 		int idx;
			for (idx = 0; idx < 20; ++idx)
				passcode[idx] = chars[1 + static_cast<int>((static_cast<float>(max - min)) * getrandom16() / 65536.0) + min];
			passcode[idx] = '\0';

			snprintf(message, sizeof(message), getstring(na, NICK_RESETPASS_MESSAGE), na->nick, Config.s_NickServ, passcode, Config.NetworkName);
			fprintf(mail->pipe, "%s", message);
			fprintf(mail->pipe, "\n.\n");
			MailEnd(mail);

			na->nc->Shrink("ns_resetpass_code");
			na->nc->Shrink("ns_resetpass_time");

			na->nc->Extend("ns_resetpass_code", new ExtensibleItemPointerArray<char>(sstrdup(passcode)));
			na->nc->Extend("ns_resetpass_time", new ExtensibleItemRegular<time_t>(time(NULL)));

			Alog() << Config.s_NickServ <<  ": " << u->GetMask() << " used RESETPASS on " << na->nick << " (" << na->nc->display << ")";
			notice_lang(Config.s_NickServ, u, NICK_RESETPASS_COMPLETE, na->nick);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_RESETPASS);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_NickServ, u, "RESETPASS", NICK_RESETPASS_SYNTAX);
	}
};

class NSResetPass : public Module
{
 public:
	NSResetPass(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		if (!Config.UseMail)
			throw ModuleException("Not using mail.");

		this->AddCommand(NICKSERV, new CommandNSResetPass());

		Implementation i[] = { I_OnNickServHelp, I_OnPreCommand };
		ModuleManager::Attach(i, this, 2);
	}

	void OnNickServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_RESETPASS);
	}

	EventReturn OnPreCommand(User *u, const std::string &service, const ci::string &command, const std::vector<ci::string> &params)
	{
		time_t t;
		char *c;

		if (service == Config.s_NickServ && command == "CONFIRM" && !params.empty())
		{
			NickAlias *na = findnick(u->nick);
			
			if (na && na->nc->GetExtArray("ns_resetpass_code", c) && na->nc->GetExtRegular<time_t>("ns_resetpass_time", t))
			{
				if (t < time(NULL) - 3600)
				{
					na->nc->Shrink("ns_resetpass_code");
					na->nc->Shrink("ns_resetpass_time");
					notice_lang(Config.s_NickServ, u, NICK_CONFIRM_EXPIRED);
					return EVENT_STOP;
				}

				std::string passcode = params[0].c_str();
				if (passcode == c)
				{
					na->nc->Shrink("ns_resetpass_code");
					na->nc->Shrink("ns_resetpass_time");

					u->UpdateHost();
					if (na->last_realname)
						delete [] na->last_realname;
					na->last_realname = sstrdup(u->realname);
					na->last_seen = time(NULL);
					u->Login(na->nc);
					ircdproto->SendAccountLogin(u, u->Account());
					ircdproto->SetAutoIdentificationToken(u);
					FOREACH_MOD(I_OnNickIdentify, OnNickIdentify(u));

					Alog() << Config.s_NickServ << ": " << u->GetMask() << " used CONFIRM with RESETPASS to forcefully identify to " << na->nick;
					notice_lang(Config.s_NickServ, u, NICK_CONFIRM_SUCCESS, Config.s_NickServ);

					if (ircd->vhost)
						do_on_id(u);
					if (Config.NSModeOnID)
						do_setmodes(u);
					check_memos(u);
				}
				else
				{
					Alog() << Config.s_NickServ << ": Invalid CONFIRM passcode for " << na->nick << " from " << u->GetMask();
					notice_lang(Config.s_NickServ, u, NICK_CONFIRM_INVALID);
					bad_password(u);
				}

				return EVENT_STOP;
			}
		}
		
		return EVENT_CONTINUE;
	}
};

MODULE_INIT(NSResetPass)
