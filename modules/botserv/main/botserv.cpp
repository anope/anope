/* BotServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/botserv.h"
#include "modules/help.h"

class BotServCore : public Module, public BotServ::BotServService
	, public EventHook<Event::SetCorrectModes>
	, public EventHook<Event::BotAssign>
	, public EventHook<Event::JoinChannel>
	, public EventHook<Event::LeaveChannel>
	, public EventHook<Event::Help>
	, public EventHook<Event::ChannelModeSet>
	, public EventHook<Event::ChanRegistered>
	, public EventHook<Event::UserKicked>
	, public EventHook<Event::CreateBot>
{
	Reference<ServiceBot> BotServ;
	ExtensibleRef<bool> inhabit;

 public:
	BotServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR)
		, BotServ::BotServService(this)
		, EventHook<Event::SetCorrectModes>(this)
		, EventHook<Event::BotAssign>(this)
		, EventHook<Event::JoinChannel>(this)
		, EventHook<Event::LeaveChannel>(this)
		, EventHook<Event::Help>(this)
		, EventHook<Event::ChannelModeSet>(this)
		, EventHook<Event::ChanRegistered>(this)
		, EventHook<Event::UserKicked>(this)
		, EventHook<Event::CreateBot>(this)
		, inhabit("inhabit")
	{
	}

	void OnReload(Configuration::Conf *conf) override
	{
		const Anope::string &bsnick = conf->GetModule(this)->Get<Anope::string>("client");
		BotServ = ServiceBot::Find(bsnick, true);
	}

	void OnSetCorrectModes(User *user, Channel *chan, ChanServ::AccessGroup &access, bool &give_modes, bool &take_modes) override
	{
		/* Do not allow removing bot modes on our service bots */
		if (chan->ci && chan->ci->GetBot() == user)
		{
			const Anope::string &botmodes = Config->GetModule(this)->Get<Anope::string>("botmodes");
			for (unsigned i = 0; i < botmodes.length(); ++i)
				chan->SetMode(chan->ci->GetBot(), ModeManager::FindChannelModeByChar(botmodes[i]), chan->ci->GetBot()->GetUID());
		}
	}

	void OnBotAssign(User *sender, ChanServ::Channel *ci, ServiceBot *bi) override
	{
		printf("on bot assign !\n");
		if (ci->c && ci->c->users.size() >= Config->GetModule(this)->Get<unsigned>("minusers"))
		{
			ChannelStatus status(Config->GetModule(this)->Get<Anope::string>("botmodes"));
			bi->Join(ci->c, &status);
		}
	}

	void OnJoinChannel(User *user, Channel *c) override
	{
		if (!Config || !IRCD)
			return;

		ServiceBot *bi = user->server == Me ? dynamic_cast<ServiceBot *>(user) : NULL;
		if (bi && Config->GetModule(this)->Get<bool>("smartjoin"))
		{
			std::vector<Anope::string> bans = c->GetModeList("BAN");

			/* We check for bans */
			for (unsigned int i = 0; i < bans.size(); ++i)
			{
				Entry ban("BAN", bans[i]);
				if (ban.Matches(user))
					c->RemoveMode(NULL, "BAN", ban.GetMask());
			}

			Anope::string Limit;
			unsigned limit = 0;
			try
			{
				if (c->GetParam("LIMIT", Limit))
					limit = convertTo<unsigned>(Limit);
			}
			catch (const ConvertException &) { }

			/* Should we be invited? */
			if (c->HasMode("INVITE") || (limit && c->users.size() >= limit))
			{
				ChannelMode *cm = ModeManager::FindChannelModeByName("OP");
				char symbol = cm ? anope_dynamic_static_cast<ChannelModeStatus *>(cm)->symbol : 0;
				IRCD->SendNotice(bi, (symbol ? Anope::string(symbol) : "") + c->name, "{0} invited {1} into the channel.", user->nick, user->nick);
			}

			ModeManager::ProcessModes();
		}

		if (user->server != Me && c->ci && c->ci->GetBot())
		{
			/**
			 * We let the bot join even if it was an ignored user, as if we don't,
			 * and the ignored user doesn't just leave, the bot will never
			 * make it into the channel, leaving the channel botless even for
			 * legit users - Rob
			 **/
			/* This is before the user has joined the channel, so check usercount + 1 */
			if (c->users.size() + 1 >= Config->GetModule(this)->Get<unsigned>("minusers") && !c->FindUser(c->ci->GetBot()))
			{
				ChannelStatus status(Config->GetModule(this)->Get<Anope::string>("botmodes"));
				c->ci->GetBot()->Join(c, &status);
			}
		}
	}

	void OnLeaveChannel(User *u, Channel *c) override
	{
		/* Channel is persistent, it shouldn't be deleted and the service bot should stay */
		if (c->ci && c->ci->IsPersist())
			return;
	
		/* Channel is syncing from a netburst, don't destroy it as more users are probably wanting to join immediately
		 * We also don't part the bot here either, if necessary we will part it after the sync
		 */
		if (c->syncing)
			return;

		/* Additionally, do not delete this channel if ChanServ/a BotServ bot is inhabiting it */
		if (inhabit && inhabit->HasExt(c))
			return;

		if (c->ci)
		{
			ServiceBot *bot = c->ci->GetBot();

			/* This is called prior to removing the user from the channnel, so c->users.size() - 1 should be safe */
			if (bot && u != bot && c->users.size() - 1 <= Config->GetModule(this)->Get<unsigned>("minusers") && c->FindUser(bot))
				bot->Part(c);
		}
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty())
			return EVENT_CONTINUE;

		if (source.c)
		{
			source.Reply(_("\002{0}\002 allows you to execute \"fantasy\" commands in the channel.\n"
					"Fantasy commands are commands that can be executed from messaging a\n"
					"channel, and provide a more convenient way to execute commands. Commands that\n"
					"require a channel as a parameter will automatically have that parameter\n"
					"given.\n"), source.service->nick);
			const Anope::string &fantasycharacters = Config->GetModule("fantasy")->Get<Anope::string>("fantasycharacter", "!");
			if (!fantasycharacters.empty())
				source.Reply(_(" \n"
						"Fantasy commands may be prefixed with one of the following characters: {0}\n"), fantasycharacters);
			source.Reply(_(" \n"
					"Available commands are:"));
		}
		else if (*source.service == BotServ)
		{
			source.Reply(_("\002{0}\002 allows you to have a bot on your own channel.\n"
				"It has been created for users that can't host or\n"
				"configure a bot, or for use on networks that don't\n"
				"allow user bots. Available commands are listed\n"
				"below; to use them, type \002{1}{2} \037command\037\002. For\n"
				"more information on a specific command, type\n"
				"\002{3}{4} {5} \037command\037\002.\n"),
				BotServ->nick, Config->StrictPrivmsg, BotServ->nick,
				Config->StrictPrivmsg, BotServ->nick, source.GetCommand());
		}

		return EVENT_CONTINUE;
	}

	void OnPostHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *BotServ)
			return;

		source.Reply(_(" \n"
			"Bot will join a channel whenever there is at least\n"
			"\002%d\002 user(s) on it."), Config->GetModule(this)->Get<unsigned>("minusers"));
		const Anope::string &fantasycharacters = Config->GetModule("fantasy")->Get<Anope::string>("fantasycharacter", "!");
		if (!fantasycharacters.empty())
			source.Reply(_("Additionally, if fantasy is enabled fantasy commands\n"
				"can be executed by prefixing the command name with\n"
				"one of the following characters: %s"), fantasycharacters.c_str());
	}

	EventReturn OnChannelModeSet(Channel *c, const MessageSource &source, ChannelMode *mode, const Anope::string &param) override
	{
		if (source.GetUser() && !source.GetBot() && Config->GetModule(this)->Get<bool>("smartjoin") && mode->name == "BAN" && c->ci && c->ci->GetBot() && c->FindUser(c->ci->GetBot()))
		{
			ServiceBot *bi = c->ci->GetBot();

			Entry ban("BAN", param);
			if (ban.Matches(bi))
				c->RemoveMode(bi, "BAN", param);
		}

		return EVENT_CONTINUE;
	}

	void OnChanRegistered(ChanServ::Channel *ci) override
	{
		/* Set default bot flags */
		spacesepstream sep(Config->GetModule(this)->Get<Anope::string>("defaults", "greet fantasy"));
		for (Anope::string token; sep.GetToken(token);)
			ci->SetS<bool>(token, true);
	}

	void OnUserKicked(const MessageSource &source, User *target, const Anope::string &channel, ChannelStatus &status, const Anope::string &kickmsg) override
	{
		ServiceBot *bi = ServiceBot::Find(target->GetUID());
		if (bi)
			/* Bots get rejoined */
			bi->Join(channel, &status);
	}

	void OnCreateBot(ServiceBot *bi) override
	{
		if (bi->botmodes.empty())
			bi->botmodes = Config->GetModule(this)->Get<Anope::string>("botumodes");
	}
};

MODULE_INIT(BotServCore)

