/* BotServ core functions
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

class CommandBSBot : public Command
{
 private:
	CommandReturn DoAdd(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[1].c_str();
		const char *user = params[2].c_str();
		const char *host = params[3].c_str();
		const char *real = params[4].c_str();
		const char *ch = NULL;
		BotInfo *bi;

		if (findbot(nick))
		{
			notice_lang(Config.s_BotServ, u, BOT_BOT_ALREADY_EXISTS, nick);
			return MOD_CONT;
		}

		if (strlen(nick) > Config.NickLen)
		{
			notice_lang(Config.s_BotServ, u, BOT_BAD_NICK);
			return MOD_CONT;
		}

		if (strlen(user) > Config.UserLen)
		{
			notice_lang(Config.s_BotServ, u, BOT_LONG_IDENT, Config.UserLen);
			return MOD_CONT;
		}

		if (strlen(user) > Config.HostLen)
		{
			notice_lang(Config.s_BotServ, u, BOT_LONG_HOST, Config.HostLen);
			return MOD_CONT;
		}

		/* Check the nick is valid re RFC 2812 */
		if (isdigit(nick[0]) || nick[0] == '-')
		{
			notice_lang(Config.s_BotServ, u, BOT_BAD_NICK);
			return MOD_CONT;
		}

		for (ch = nick; *ch && (ch - nick) < Config.NickLen; ch++)
		{
			if (!isvalidnick(*ch))
			{
				notice_lang(Config.s_BotServ, u, BOT_BAD_NICK);
				return MOD_CONT;
			}
		}

		/* check for hardcored ircd forbidden nicks */
		if (!ircdproto->IsNickValid(nick))
		{
			notice_lang(Config.s_BotServ, u, BOT_BAD_NICK);
			return MOD_CONT;
		}

		/* Check the host is valid re RFC 2812 */
		if (!isValidHost(host, 3))
		{
			notice_lang(Config.s_BotServ, u, BOT_BAD_HOST);
			return MOD_CONT;
		}

		for (ch = user; *ch && (ch - user) < Config.UserLen; ch++)
		{
			if (!isalnum(*ch))
			{
				notice_lang(Config.s_BotServ, u, BOT_BAD_IDENT, Config.UserLen);
				return MOD_CONT;
			}
		}


		/* We check whether the nick is registered, and inform the user
		* if so. You need to drop the nick manually before you can use
		* it as a bot nick from now on -GD
		*/
		if (findnick(nick))
		{
			notice_lang(Config.s_BotServ, u, NICK_ALREADY_REGISTERED, nick);
			return MOD_CONT;
		}

		if (!(bi = new BotInfo(nick, user, host, real)))
		{
			notice_lang(Config.s_BotServ, u, BOT_BOT_CREATION_FAILED);
			return MOD_CONT;
		}

		/* We check whether user with this nick is online, and kill it if so */
		EnforceQlinedNick(nick, Config.s_BotServ);

		notice_lang(Config.s_BotServ, u, BOT_BOT_ADDED, bi->nick.c_str(), bi->user.c_str(), bi->host.c_str(), bi->real.c_str());

		FOREACH_MOD(I_OnBotCreate, OnBotCreate(bi));
		return MOD_CONT;
	}

	CommandReturn DoChange(User *u, const std::vector<ci::string> &params)
	{
		const char *oldnick = params[1].c_str();
		const char *nick = params.size() > 2 ? params[2].c_str() : NULL;
		const char *user = params.size() > 3 ? params[3].c_str() : NULL;
		const char *host = params.size() > 4 ? params[4].c_str() : NULL;
		const char *real = params.size() > 5 ? params[5].c_str() : NULL;
		const char *ch = NULL;
		BotInfo *bi;

		if (!oldnick || !nick)
		{
			this->OnSyntaxError(u, "CHANGE");
			return MOD_CONT;
		}

		if (!(bi = findbot(oldnick)))
		{
			notice_lang(Config.s_BotServ, u, BOT_DOES_NOT_EXIST, oldnick);
			return MOD_CONT;
		}

		if (stricmp(oldnick, nick) && nickIsServices(oldnick, 0))
		{
			notice_lang(Config.s_BotServ, u, BOT_DOES_NOT_EXIST, oldnick);
			return MOD_CONT;
		}

		if (strlen(nick) > Config.NickLen)
		{
			notice_lang(Config.s_BotServ, u, BOT_BAD_NICK);
			return MOD_CONT;
		}

		if (user && strlen(user) > Config.UserLen)
		{
			notice_lang(Config.s_BotServ, u, BOT_LONG_IDENT, Config.UserLen);
			return MOD_CONT;
		}

		if (host && strlen(host) > Config.HostLen)
		{
			notice_lang(Config.s_BotServ, u, BOT_LONG_HOST, Config.HostLen);
			return MOD_CONT;
		}

		if (stricmp(oldnick, nick) && nickIsServices(nick, 0))
		{
			notice_lang(Config.s_BotServ, u, BOT_DOES_NOT_EXIST, oldnick);
			return MOD_CONT;
		}

		/* Checks whether there *are* changes.
		* Case sensitive because we may want to change just the case.
		* And we must finally check that the nick is not already
		* taken by another bot.
		*/
		if (bi->nick == nick && (user ? bi->user == user : 1) && (host ? bi->host == host : 1) && (real ? bi->real == real : 1))
		{
			notice_lang(Config.s_BotServ, u, BOT_BOT_ANY_CHANGES);
			return MOD_CONT;
		}

		/* Check the nick is valid re RFC 2812 */
		if (isdigit(nick[0]) || nick[0] == '-')
		{
			notice_lang(Config.s_BotServ, u, BOT_BAD_NICK);
			return MOD_CONT;
		}

		for (ch = nick; *ch && (ch - nick) < Config.NickLen; ch++)
		{
			if (!isvalidnick(*ch))
			{
				notice_lang(Config.s_BotServ, u, BOT_BAD_NICK);
				return MOD_CONT;
			}
		}

		/* check for hardcored ircd forbidden nicks */
		if (!ircdproto->IsNickValid(nick))
		{
			notice_lang(Config.s_BotServ, u, BOT_BAD_NICK);
			return MOD_CONT;
		}

		if (host && !isValidHost(host, 3))
		{
			notice_lang(Config.s_BotServ, u, BOT_BAD_HOST);
			return MOD_CONT;
		}

		if (user)
		{
			for (ch = user; *ch && (ch - user) < Config.UserLen; ch++)
			{
				if (!isalnum(*ch))
				{
					notice_lang(Config.s_BotServ, u, BOT_BAD_IDENT, Config.UserLen);
					return MOD_CONT;
				}
			}
		}

		ci::string ci_bi_nick(bi->nick.c_str());
		if (ci_bi_nick != nick && findbot(nick))
		{
			notice_lang(Config.s_BotServ, u, BOT_BOT_ALREADY_EXISTS, nick);
			return MOD_CONT;
		}

		if (ci_bi_nick != nick)
		{
			/* We check whether the nick is registered, and inform the user
			* if so. You need to drop the nick manually before you can use
			* it as a bot nick from now on -GD
			*/
			if (findnick(nick))
			{
				notice_lang(Config.s_BotServ, u, NICK_ALREADY_REGISTERED, nick);
				return MOD_CONT;
			}

			/* The new nick is really different, so we remove the Q line for
			the old nick. */
			if (ircd->sqline)
			{
				ircdproto->SendSQLineDel(bi->nick);
			}

			/* We check whether user with this nick is online, and kill it if so */
			EnforceQlinedNick(nick, Config.s_BotServ);
		}

		if (user)
			ircdproto->SendQuit(bi, "Quit: Be right back");
		else {
			ircdproto->SendChangeBotNick(bi, nick);
			ircdproto->SendSQLine(bi->nick, "Reserved for services");
		}

		if (bi->nick != nick)
			bi->ChangeNick(nick);

		if (user && bi->user != user)
			bi->user = user;
		if (host && bi->host != host)
			bi->host = host;
		if (real && bi->real != real)
			bi->real = real;

		if (user)
		{
			if (ircd->ts6) {
				// This isn't the nicest way to do this, unfortunately.
				bi->uid = ts6_uid_retrieve();
			}
			ircdproto->SendClientIntroduction(bi->nick, bi->user, bi->host, bi->real, ircd->pseudoclient_mode, bi->uid);
			ircdproto->SendSQLine(bi->nick, "Reserved for services");
			bi->RejoinAll();
		}

		notice_lang(Config.s_BotServ, u, BOT_BOT_CHANGED, oldnick, bi->nick.c_str(), bi->user.c_str(), bi->host.c_str(), bi->real.c_str());

		FOREACH_MOD(I_OnBotChange, OnBotChange(bi));
		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[1].c_str();
		BotInfo *bi;

		if (!nick)
		{
			this->OnSyntaxError(u, "DEL");
			return MOD_CONT;
		}

		if (!(bi = findbot(nick)))
		{
			notice_lang(Config.s_BotServ, u, BOT_DOES_NOT_EXIST, nick);
			return MOD_CONT;
		}

		if (nickIsServices(nick, 0))
		{
			notice_lang(Config.s_BotServ, u, BOT_DOES_NOT_EXIST, nick);
			return MOD_CONT;
		}

		FOREACH_MOD(I_OnBotDelete, OnBotDelete(bi));

		ircdproto->SendQuit(bi, "Quit: Help! I'm being deleted by %s!", u->nick.c_str());
		ircdproto->SendSQLineDel(bi->nick);

		delete bi;
		notice_lang(Config.s_BotServ, u, BOT_BOT_DELETED, nick);
		return MOD_CONT;
	}
 public:
	CommandBSBot() : Command("BOT", 1, 6)
	{
		this->SetFlag(CFLAG_STRIP_CHANNEL);
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ci::string cmd = params[0];

		if (readonly)
		{
			notice_lang(Config.s_BotServ, u, BOT_BOT_READONLY);
			return MOD_CONT;
		}

		if (cmd == "ADD")
		{
			// ADD nick user host real - 5
			if (!u->Account()->HasCommand("botserv/bot/add"))
			{
				notice_lang(Config.s_BotServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}

			if (params.size() < 5)
			{
				this->OnSyntaxError(u, "ADD");
				return MOD_CONT;
			}

			std::vector<ci::string> tempparams = params;
			// ADD takes less params than CHANGE, so we need to take 6 if given and append it with a space to 5.
			if (tempparams.size() >= 6)
				tempparams[4] = tempparams[4] + " " + tempparams[5];

			return this->DoAdd(u, tempparams);
		}
		else if (cmd == "CHANGE")
		{
			// CHANGE oldn newn user host real - 6
			// but only oldn and newn are required
			if (!u->Account()->HasCommand("botserv/bot/change"))
			{
				notice_lang(Config.s_BotServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}

			if (params.size() < 3)
			{
				this->OnSyntaxError(u, "CHANGE");
				return MOD_CONT;
			}

			return this->DoChange(u, params);
		}
		else if (cmd == "DEL")
		{
			// DEL nick
			if (!u->Account()->HasCommand("botserv/bot/del"))
			{
				notice_lang(Config.s_BotServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}

			if (params.size() < 1)
			{
				this->OnSyntaxError(u, "DEL");
				return MOD_CONT;
			}

			return this->DoDel(u, params);
		}
		else
			this->OnSyntaxError(u, "");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_lang(Config.s_BotServ, u, BOT_SERVADMIN_HELP_BOT);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_BotServ, u, "BOT", BOT_BOT_SYNTAX);
	}
};

class BSBot : public Module
{
 public:
	BSBot(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);
		this->AddCommand(BOTSERV, new CommandBSBot());

		ModuleManager::Attach(I_OnBotServHelp, this);
	}
	void OnBotServHelp(User *u)
	{
		notice_lang(Config.s_BotServ, u, BOT_HELP_CMD_BOT);
	}
};

MODULE_INIT(BSBot)
