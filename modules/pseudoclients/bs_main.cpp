/* BotServ core functions
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

class BotServCore : public Module
{
	Channel *fantasy_channel;
 public:
	BotServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), fantasy_channel(NULL)
	{
		this->SetAuthor("Anope");

		BotInfo *BotServ = findbot(Config->BotServ);
		if (BotServ == NULL)
			throw ModuleException("No bot named " + Config->BotServ);

		Implementation i[] = { I_OnPrivmsg, I_OnPreCommand, I_OnJoinChannel, I_OnLeaveChannel,
					I_OnPreHelp, I_OnPostHelp, I_OnChannelModeSet };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

	}

	void OnPrivmsg(User *u, Channel *c, Anope::string &msg)
	{
		if (!u || !c || !c->ci || !c->ci->bi || msg.empty())
			return;

		/* Answer to ping if needed */
		if (msg.substr(0, 6).equals_ci("\1PING ") && msg[msg.length() - 1] == '\1')
		{
			Anope::string ctcp = msg;
			ctcp.erase(ctcp.begin());
			ctcp.erase(ctcp.length() - 1);
			ircdproto->SendCTCP(c->ci->bi, u->nick, "%s", ctcp.c_str());
		}


		Anope::string realbuf = msg;

		bool was_action = false;
		if (realbuf.substr(0, 8).equals_ci("\1ACTION ") && realbuf[realbuf.length() - 1] == '\1')
		{
			realbuf.erase(0, 8);
			realbuf.erase(realbuf.length() - 1);
			was_action = true;
		}
	
		if (realbuf.empty())
			return;

		/* Fantaisist commands */
		if (c->ci->botflags.HasFlag(BS_FANTASY) && realbuf[0] == Config->BSFantasyCharacter[0] && !was_action)
		{
			/* Strip off the fantasy character */
			realbuf.erase(realbuf.begin());

			size_t space = realbuf.find(' ');
			Anope::string command, rest;
			if (space == Anope::string::npos)
				command = realbuf;
			else
			{
				command = realbuf.substr(0, space);
				rest = realbuf.substr(space + 1);
			}

			BotInfo *bi = findbot(Config->ChanServ);
			if (bi == NULL)
				bi = findbot(Config->BotServ);
			if (bi == NULL || !bi->commands.count(command))
				return;

			if (c->ci->HasPriv(u, CA_FANTASIA))
			{

				this->fantasy_channel = c;
				bi->OnMessage(u, realbuf);
				this->fantasy_channel = NULL;
	
				FOREACH_MOD(I_OnBotFantasy, OnBotFantasy(command, u, c->ci, rest));
			}
			else
			{
				FOREACH_MOD(I_OnBotNoFantasyAccess, OnBotNoFantasyAccess(command, u, c->ci, rest));
			}
		}
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params)
	{
		if (this->fantasy_channel != NULL)
		{
			if (!command->HasFlag(CFLAG_STRIP_CHANNEL))
				params.insert(params.begin(), this->fantasy_channel->name);
			source.c = this->fantasy_channel;
		}

		return EVENT_CONTINUE;
	}

	void OnJoinChannel(User *user, Channel *c)
	{
		if (c->ci && c->ci->bi)
		{
			/**
			 * We let the bot join even if it was an ignored user, as if we don't,
			 * and the ignored user doesnt just leave, the bot will never
			 * make it into the channel, leaving the channel botless even for
			 * legit users - Rob
			 **/
			if (c->users.size() >= Config->BSMinUsers && !c->FindUser(c->ci->bi))
				c->ci->bi->Join(c, &Config->BotModeList);
			/* Only display the greet if the main uplink we're connected
			 * to has synced, or we'll get greet-floods when the net
			 * recovers from a netsplit. -GD
			 */
			if (c->FindUser(c->ci->bi) && c->ci->botflags.HasFlag(BS_GREET) && user->Account() && !user->Account()->greet.empty() && c->ci->HasPriv(user, CA_GREET) && user->server->IsSynced())
			{
				ircdproto->SendPrivmsg(c->ci->bi, c->name, "[%s] %s", user->Account()->display.c_str(), user->Account()->greet.c_str());
				c->ci->bi->lastmsg = Anope::CurTime;
			}
		}
	}

	void OnLeaveChannel(User *u, Channel *c)
	{
		/* Channel is persistent, it shouldn't be deleted and the service bot should stay */
		if (c->HasFlag(CH_PERSIST) || (c->ci && c->ci->HasFlag(CI_PERSIST)))
			return;
	
		/* Channel is syncing from a netburst, don't destroy it as more users are probably wanting to join immediatly
		 * We also don't part the bot here either, if necessary we will part it after the sync
		 */
		if (c->HasFlag(CH_SYNCING))
			return;

		/* Additionally, do not delete this channel if ChanServ/a BotServ bot is inhabiting it */
		if (c->HasFlag(CH_INHABIT))
			return;

		if (c->ci && c->ci->bi && c->users.size() - 1 <= Config->BSMinUsers && c->FindUser(c->ci->bi))
		{
			bool persist = c->HasFlag(CH_PERSIST);
			c->SetFlag(CH_PERSIST);
			c->ci->bi->Part(c->ci->c);
			if (!persist)
				c->UnsetFlag(CH_PERSIST);
		}
	}

	void OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!params.empty() || source.owner->nick != Config->BotServ)
			return;
		source.Reply(_("\002%s\002 allows you to have a bot on your own channel.\n"
			"It has been created for users that can't host or\n"
			"configure a bot, or for use on networks that don't\n"
			"allow user bots. Available commands are listed \n"
			"below; to use them, type \002%s%s \037command\037\002. For\n"
			"more information on a specific command, type\n"
			"\002%s%s %s \037command\037\002.\n "),
			Config->BotServ.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->BotServ.c_str(),
			Config->UseStrictPrivMsgString.c_str(), Config->BotServ.c_str(), source.command.c_str());
	}

	void OnPostHelp(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!params.empty() || source.owner->nick != Config->BotServ)
			return;
		source.Reply(_(" \n"
			"Bot will join a channel whenever there is at least\n"
			"\002%d\002 user(s) on it. Additionally, all %s commands\n"
			"can be used if fantasy is enabled by prefixing the command\n"
			"name with a %c."), Config->BSMinUsers, Config->ChanServ.c_str(), Config->BSFantasyCharacter[0]);
	}

	EventReturn OnChannelModeSet(Channel *c, ChannelModeName Name, const Anope::string &param)
	{
		if (Config->BSSmartJoin && Name == CMODE_BAN && c->ci && c->ci->bi && c->FindUser(c->ci->bi))
		{
			BotInfo *bi = c->ci->bi;

			Entry ban(CMODE_BAN, param);
			if (ban.Matches(bi))
				c->RemoveMode(bi, CMODE_BAN, param);
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(BotServCore)

