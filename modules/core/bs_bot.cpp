/* BotServ core functions
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

class CommandBSBot : public Command
{
 private:
	CommandReturn DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &nick = params[1];
		const Anope::string &user = params[2];
		const Anope::string &host = params[3];
		const Anope::string &real = params[4];
		BotInfo *bi;

		if (findbot(nick))
		{
			source.Reply(BOT_BOT_ALREADY_EXISTS, nick.c_str());
			return MOD_CONT;
		}

		if (nick.length() > Config->NickLen)
		{
			source.Reply(BOT_BAD_NICK);
			return MOD_CONT;
		}

		if (user.length() > Config->UserLen)
		{
			source.Reply(BOT_LONG_IDENT, Config->UserLen);
			return MOD_CONT;
		}

		if (host.length() > Config->HostLen)
		{
			source.Reply(BOT_LONG_HOST, Config->HostLen);
			return MOD_CONT;
		}

		/* Check the nick is valid re RFC 2812 */
		if (isdigit(nick[0]) || nick[0] == '-')
		{
			source.Reply(BOT_BAD_NICK);
			return MOD_CONT;
		}

		for (unsigned i = 0, end = nick.length(); i < end && i < Config->NickLen; ++i)
			if (!isvalidnick(nick[i]))
			{
				source.Reply(BOT_BAD_NICK);
				return MOD_CONT;
			}

		/* check for hardcored ircd forbidden nicks */
		if (!ircdproto->IsNickValid(nick))
		{
			source.Reply(BOT_BAD_NICK);
			return MOD_CONT;
		}

		/* Check the host is valid re RFC 2812 */
		if (!isValidHost(host, 3))
		{
			source.Reply(BOT_BAD_HOST);
			return MOD_CONT;
		}

		for (unsigned i = 0, end = user.length(); i < end && i < Config->UserLen; ++i)
			if (!isalnum(user[i]))
			{
				source.Reply(BOT_BAD_IDENT, Config->UserLen);
				return MOD_CONT;
			}

		/* We check whether the nick is registered, and inform the user
		* if so. You need to drop the nick manually before you can use
		* it as a bot nick from now on -GD
		*/
		if (findnick(nick))
		{
			source.Reply(NICK_ALREADY_REGISTERED, nick.c_str());
			return MOD_CONT;
		}

		if (!(bi = new BotInfo(nick, user, host, real)))
		{
			// XXX this cant happen?
			source.Reply(BOT_BOT_CREATION_FAILED);
			return MOD_CONT;
		}

		Log(LOG_ADMIN, source.u, this) << "ADD " << bi->GetMask() << " " << bi->realname;

		source.Reply(BOT_BOT_ADDED, bi->nick.c_str(), bi->GetIdent().c_str(), bi->host.c_str(), bi->realname.c_str());

		FOREACH_MOD(I_OnBotCreate, OnBotCreate(bi));
		return MOD_CONT;
	}

	CommandReturn DoChange(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &oldnick = params[1];
		const Anope::string &nick = params.size() > 2 ? params[2] : "";
		const Anope::string &user = params.size() > 3 ? params[3] : "";
		const Anope::string &host = params.size() > 4 ? params[4] : "";
		const Anope::string &real = params.size() > 5 ? params[5] : "";
		BotInfo *bi;

		if (oldnick.empty() || nick.empty())
		{
			this->OnSyntaxError(source.u, "CHANGE");
			return MOD_CONT;
		}

		if (!(bi = findbot(oldnick)))
		{
			source.Reply(BOT_DOES_NOT_EXIST, oldnick.c_str());
			return MOD_CONT;
		}

		if (!oldnick.equals_ci(nick) && nickIsServices(oldnick, false))
		{
			source.Reply(BOT_DOES_NOT_EXIST, oldnick.c_str());
			return MOD_CONT;
		}

		if (nick.length() > Config->NickLen)
		{
			source.Reply(BOT_BAD_NICK);
			return MOD_CONT;
		}

		if (!user.empty() && user.length() > Config->UserLen)
		{
			source.Reply(BOT_LONG_IDENT, Config->UserLen);
			return MOD_CONT;
		}

		if (!host.empty() && host.length() > Config->HostLen)
		{
			source.Reply(BOT_LONG_HOST, Config->HostLen);
			return MOD_CONT;
		}

		if (!oldnick.equals_ci(nick) && nickIsServices(nick, false))
		{
			source.Reply(BOT_DOES_NOT_EXIST, oldnick.c_str());
			return MOD_CONT;
		}

		/* Checks whether there *are* changes.
		* Case sensitive because we may want to change just the case.
		* And we must finally check that the nick is not already
		* taken by another bot.
		*/
		if (nick.equals_cs(bi->nick) && (!user.empty() ? user.equals_cs(bi->GetIdent()) : 1) && (!host.empty() ? host.equals_cs(bi->host) : 1) && (!real.empty() ? real.equals_cs(bi->realname) : 1))
		{
			source.Reply(BOT_BOT_ANY_CHANGES);
			return MOD_CONT;
		}

		/* Check the nick is valid re RFC 2812 */
		if (isdigit(nick[0]) || nick[0] == '-')
		{
			source.Reply(BOT_BAD_NICK);
			return MOD_CONT;
		}

		for (unsigned i = 0, end = nick.length(); i < end && i < Config->NickLen; ++i)
			if (!isvalidnick(nick[i]))
			{
				source.Reply(BOT_BAD_NICK);
				return MOD_CONT;
			}

		/* check for hardcored ircd forbidden nicks */
		if (!ircdproto->IsNickValid(nick))
		{
			source.Reply(BOT_BAD_NICK);
			return MOD_CONT;
		}

		if (!host.empty() && !isValidHost(host, 3))
		{
			source.Reply(BOT_BAD_HOST);
			return MOD_CONT;
		}

		if (!user.empty())
			for (unsigned i = 0, end = user.length(); i < end && i < Config->UserLen; ++i)
				if (!isalnum(user[i]))
				{
					source.Reply(BOT_BAD_IDENT, Config->UserLen);
					return MOD_CONT;
				}

		if (!nick.equals_ci(bi->nick) && findbot(nick))
		{
			source.Reply(BOT_BOT_ALREADY_EXISTS, nick.c_str());
			return MOD_CONT;
		}

		if (!nick.equals_ci(bi->nick))
		{
			/* We check whether the nick is registered, and inform the user
			* if so. You need to drop the nick manually before you can use
			* it as a bot nick from now on -GD
			*/
			if (findnick(nick))
			{
				source.Reply(NICK_ALREADY_REGISTERED, nick.c_str());
				return MOD_CONT;
			}

			/* The new nick is really different, so we remove the Q line for
			the old nick. */
			if (ircd->sqline)
			{
				XLine x(bi->nick);
				ircdproto->SendSQLineDel(&x);
			}

			/* We check whether user with this nick is online, and kill it if so */
			EnforceQlinedNick(nick, Config->s_BotServ);
		}

		if (!user.empty())
			ircdproto->SendQuit(bi, "Quit: Be right back");
		else
		{
			ircdproto->SendChangeBotNick(bi, nick);
			XLine x(bi->nick, "Reserved for services");
			ircdproto->SendSQLine(&x);
		}

		if (!nick.equals_cs(bi->nick))
			bi->SetNewNick(nick);

		if (!user.empty() && !user.equals_cs(bi->GetIdent()))
			bi->SetIdent(user);
		if (!host.empty() && !host.equals_cs(bi->host))
			bi->host = host;
		if (!real.empty() && !real.equals_cs(bi->realname))
			bi->realname = real;

		if (!user.empty())
		{
			ircdproto->SendClientIntroduction(bi, ircd->pseudoclient_mode);
			XLine x(bi->nick, "Reserved for services");
			ircdproto->SendSQLine(&x);
			bi->RejoinAll();
		}

		source.Reply(BOT_BOT_CHANGED, oldnick.c_str(), bi->nick.c_str(), bi->GetIdent().c_str(), bi->host.c_str(), bi->realname.c_str());
		Log(LOG_ADMIN, source.u, this) << "CHANGE " << oldnick << " to " << bi->GetMask() << " " << bi->realname;

		FOREACH_MOD(I_OnBotChange, OnBotChange(bi));
		return MOD_CONT;
	}

	CommandReturn DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &nick = params[1];
		BotInfo *bi;

		if (nick.empty())
		{
			this->OnSyntaxError(source.u, "DEL");
			return MOD_CONT;
		}

		if (!(bi = findbot(nick)))
		{
			source.Reply(BOT_DOES_NOT_EXIST, nick.c_str());
			return MOD_CONT;
		}

		if (nickIsServices(nick, false))
		{
			source.Reply(BOT_DOES_NOT_EXIST, nick.c_str());
			return MOD_CONT;
		}

		FOREACH_MOD(I_OnBotDelete, OnBotDelete(bi));

		ircdproto->SendQuit(bi, "Quit: Help! I'm being deleted by %s!", source.u->nick.c_str());
		XLine x(bi->nick);
		ircdproto->SendSQLineDel(&x);

		Log(LOG_ADMIN, source.u, this) << "DEL " << bi->nick;

		source.Reply(BOT_BOT_DELETED, nick.c_str());
		delete bi;
		return MOD_CONT;
	}
 public:
	CommandBSBot() : Command("BOT", 1, 6)
	{
		this->SetFlag(CFLAG_STRIP_CHANNEL);
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &cmd = params[0];
		User *u = source.u;

		if (readonly)
		{
			source.Reply(BOT_BOT_READONLY);
			return MOD_CONT;
		}

		if (cmd.equals_ci("ADD"))
		{
			// ADD nick user host real - 5
			if (!u->Account()->HasCommand("botserv/bot/add"))
			{
				source.Reply(ACCESS_DENIED);
				return MOD_CONT;
			}

			if (params.size() < 5)
			{
				this->OnSyntaxError(u, "ADD");
				return MOD_CONT;
			}

			std::vector<Anope::string> tempparams = params;
			// ADD takes less params than CHANGE, so we need to take 6 if given and append it with a space to 5.
			if (tempparams.size() >= 6)
				tempparams[4] = tempparams[4] + " " + tempparams[5];

			return this->DoAdd(source, tempparams);
		}
		else if (cmd.equals_ci("CHANGE"))
		{
			// CHANGE oldn newn user host real - 6
			// but only oldn and newn are required
			if (!u->Account()->HasCommand("botserv/bot/change"))
			{
				source.Reply(ACCESS_DENIED);
				return MOD_CONT;
			}

			if (params.size() < 3)
			{
				this->OnSyntaxError(u, "CHANGE");
				return MOD_CONT;
			}

			return this->DoChange(source, params);
		}
		else if (cmd.equals_ci("DEL"))
		{
			// DEL nick
			if (!u->Account()->HasCommand("botserv/bot/del"))
			{
				source.Reply(ACCESS_DENIED);
				return MOD_CONT;
			}

			if (params.size() < 1)
			{
				this->OnSyntaxError(u, "DEL");
				return MOD_CONT;
			}

			return this->DoDel(source, params);
		}
		else
			this->OnSyntaxError(u, "");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(BotServ, BOT_SERVADMIN_HELP_BOT);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(BotServ, u, "BOT", BOT_BOT_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(BotServ, BOT_HELP_CMD_BOT);
	}
};

class BSBot : public Module
{
	CommandBSBot commandbsbot;

 public:
	BSBot(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(BotServ, &commandbsbot);
	}
};

MODULE_INIT(BSBot)
