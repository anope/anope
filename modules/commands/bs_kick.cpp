/* BotServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#include "module.h"

static Module *me;

class CommandBSKick : public Command
{
 public:
	CommandBSKick(Module *creator) : Command(creator, "botserv/kick", 0)
	{
		this->SetDesc(_("Configures kickers"));
		this->SetSyntax(_("\037option\037 \037channel\037 {\037ON|OFF\037} [\037settings\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Configures bot kickers.  \037option\037 can be one of:"));

		Anope::string this_name = source.command;
		for (CommandInfo::map::const_iterator it = source.service->commands.begin(), it_end = source.service->commands.end(); it != it_end; ++it)
		{
			const Anope::string &c_name = it->first;
			const CommandInfo &info = it->second;

			if (c_name.find_ci(this_name + " ") == 0)
			{
				ServiceReference<Command> command("Command", info.name);
				if (command)
				{
					source.command = c_name;
					command->OnServHelp(source);
				}
			}
		}

		source.Reply(_("Type \002%s%s HELP %s \037option\037\002 for more information\n"
				"on a specific option.\n"
				" \n"
				"Note: access to this command is controlled by the\n"
				"level SET."), Config->StrictPrivmsg.c_str(), source.service->nick.c_str(), this_name.c_str());

		return true;
	}
};

class CommandBSKickBase : public Command
{
 public:
	CommandBSKickBase(Module *creator, const Anope::string &cname, int minarg, int maxarg) : Command(creator, cname, minarg, maxarg)
	{
	}

	virtual void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override = 0;

	virtual bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override = 0;

 protected:
	bool CheckArguments(CommandSource &source, const std::vector<Anope::string> &params, ChannelInfo* &ci) anope_override
	{
		const Anope::string &chan = params[0];
		const Anope::string &option = params[1];

		ci = ChannelInfo::Find(chan);

		if (Anope::ReadOnly)
			source.Reply(_("Sorry, kicker configuration is temporarily disabled."));
		else if (ci == NULL)
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
		else if (option.empty())
			this->OnSyntaxError(source, "");
		else if (!option.equals_ci("ON") && !option.equals_ci("OFF"))
			this->OnSyntaxError(source, "");
		else if (!source.AccessFor(ci).HasPriv("SET") && !source.HasPriv("botserv/administration"))
			source.Reply(ACCESS_DENIED);
		else if (!ci->bi)
			source.Reply(BOT_NOT_ASSIGNED);
		else
			return true;

		return false;
	}

	void Process(CommandSource &source, ChannelInfo *ci, const Anope::string &param, const Anope::string &ttb, size_t ttb_idx, const Anope::string &optname, const Anope::string &mdname)
	{
		if (param.equals_ci("ON"))
		{
			if (!ttb.empty())
			{
				int16_t i;

				try
				{
					i = convertTo<int16_t>(ttb);
					if (i < 0)
						throw ConvertException();
				}
				catch (const ConvertException &)
				{
					source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
					return;
				}

				ci->ttb[ttb_idx] = i;
			}
			else
				ci->ttb[ttb_idx] = 0;

			ci->ExtendMetadata(mdname);
			if (ci->ttb[ttb_idx])
				source.Reply(_("Bot will now kick for \002%s\002, and will place a ban\n"
						"after %d kicks for the same user."), optname.c_str(), ci->ttb[ttb_idx]);
			else
				source.Reply(_("Bot will now kick for \002%s\002."), optname.c_str());

			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enable the " << optname << "kicker";
		}
		else if (param.equals_ci("OFF"))
		{
			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to disable the " << optname << "kicker";

			ci->Shrink(mdname);
			source.Reply(_("Bot won't kick for \002%s\002 anymore."), optname.c_str());
		}
		else
			this->OnSyntaxError(source, "");
	}
};

class CommandBSKickAMSG : public CommandBSKickBase
{
 public:
	CommandBSKickAMSG(Module *creator) : CommandBSKickBase(creator, "botserv/kick/amsg", 2, 3)
	{
		this->SetDesc(_("Configures AMSG kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci;
		if (CheckArguments(source, params, ci))
			Process(source, ci, params[1], params.size() > 2 ? params[2] : "", TTB_AMSGS, "AMSG", "BS_KICK_AMSGS");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		source.Reply(_("Sets the AMSG kicker on or off. When enabled, the bot will\n"
				"kick users who send the same message to multiple channels\n"
				"where BotServ bots are.\n"
				"\037ttb\037 is the number of times a user can be kicked\n"
				"before they get banned. Don't give ttb to disable\n"
				"the ban system once activated."));
		return true;
	}
};

class CommandBSKickBadwords : public CommandBSKickBase
{
 public:
	CommandBSKickBadwords(Module *creator) : CommandBSKickBase(creator, "botserv/kick/badwords", 2, 3)
	{
		this->SetDesc(_("Configures badwords kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci;
		if (CheckArguments(source, params, ci))
			Process(source, ci, params[1], params.size() > 2 ? params[2] : "", TTB_BADWORDS, "badwords", "BS_KICK_BADWORDS");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		source.Reply(_("Sets the bad words kicker on or off. When enabled, this\n"
				"option tells the bot to kick users who say certain words\n"
				"on the channels.\n"
				"You can define bad words for your channel using the\n"
				"\002BADWORDS\002 command. Type \002%s%s HELP BADWORDS\002 for\n"
				"more information.\n"
				"ttb is the number of times a user can be kicked\n"
				"before it get banned. Don't give ttb to disable\n"
				"the ban system once activated."), Config->StrictPrivmsg.c_str(), source.service->nick.c_str());
		return true;
	}
};

class CommandBSKickBolds : public CommandBSKickBase
{
 public:
	CommandBSKickBolds(Module *creator) : CommandBSKickBase(creator, "botserv/kick/bolds", 2, 3)
	{
		this->SetDesc(_("Configures badwords kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci;
		if (CheckArguments(source, params, ci))
			Process(source, ci, params[1], params.size() > 2 ? params[2] : "", TTB_BOLDS, "bolds", "BS_KICK_BOLDS");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		source.Reply(_("Sets the bolds kicker on or off. When enabled, this\n"
				"option tells the bot to kick users who use bolds.\n"
				"ttb is the number of times a user can be kicked\n"
				"before it get banned. Don't give ttb to disable\n"
				"the ban system once activated."));
		return true;
	}
};

class CommandBSKickCaps : public CommandBSKickBase
{
 public:
	CommandBSKickCaps(Module *creator) : CommandBSKickBase(creator, "botserv/kick/caps", 2, 5)
	{
		this->SetDesc(_("Configures caps kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037 [\037min\037 [\037percent\037]]]\002"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci;
		if (!CheckArguments(source, params, ci))
			return;

		if (params[1].equals_ci("ON"))
		{
			const Anope::string &ttb = params.size() > 2 ? params[2] : "",
				&min = params.size() > 3 ? params[3] : "",
				&percent = params.size() > 4 ? params[4] : "";

			if (!ttb.empty())
			{
				try
				{
					ci->ttb[TTB_CAPS] = convertTo<int16_t>(ttb);
					if (ci->ttb[TTB_CAPS] < 0)
						throw ConvertException();
				}
				catch (const ConvertException &)
				{
					ci->ttb[TTB_CAPS] = 0;
					source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
					return;
				}
			}
			else
				ci->ttb[TTB_CAPS] = 0;

			ci->capsmin = 10;
			try
			{
				ci->capsmin = convertTo<int16_t>(min);
			}
			catch (const ConvertException &) { }
			if (ci->capsmin < 1)
				ci->capsmin = 10;

			ci->capspercent = 25;
			try
			{
				ci->capspercent = convertTo<int16_t>(percent);
			}
			catch (const ConvertException &) { }
			if (ci->capspercent < 1 || ci->capspercent > 100)
				ci->capspercent = 25;

			ci->ExtendMetadata("BS_KICK_CAPS");
			if (ci->ttb[TTB_CAPS])
				source.Reply(_("Bot will now kick for \002caps\002 (they must constitute at least\n"
						"%d characters and %d%% of the entire message), and will\n"
						"place a ban after %d kicks for the same user."), ci->capsmin, ci->capspercent, ci->ttb[TTB_CAPS]);
			else
				source.Reply(_("Bot will now kick for \002caps\002 (they must constitute at least\n"
						"%d characters and %d%% of the entire message)."), ci->capsmin, ci->capspercent);
		}
		else
		{
			ci->Shrink("BS_KICK_CAPS");
			source.Reply(_("Bot won't kick for \002caps\002 anymore."));
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		source.Reply(_("Sets the caps kicker on or off. When enabled, this\n"
				"option tells the bot to kick users who are talking in\n"
				"CAPS.\n"
				"The bot kicks only if there are at least \002min\002 caps\n"
				"and they constitute at least \002percent\002%% of the total\n"
				"text line (if not given, it defaults to 10 characters\n"
				"and 25%%).\n"
				"ttb is the number of times a user can be kicked\n"
				"before it get banned. Don't give ttb to disable\n"
				"the ban system once activated."));
		return true;
	}
};

class CommandBSKickColors : public CommandBSKickBase
{
 public:
	CommandBSKickColors(Module *creator) : CommandBSKickBase(creator, "botserv/kick/colors", 2, 3)
	{
		this->SetDesc(_("Configures color kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci;
		if (CheckArguments(source, params, ci))
			Process(source, ci, params[1], params.size() > 2 ? params[2] : "", TTB_COLORS, "colors", "BS_KICK_COLORS");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		source.Reply(_("Sets the colors kicker on or off. When enabled, this\n"
				"option tells the bot to kick users who use colors.\n"
				"ttb is the number of times a user can be kicked\n"
				"before it get banned. Don't give ttb to disable\n"
				"the ban system once activated."));
		return true;
	}
};

class CommandBSKickFlood : public CommandBSKickBase
{
 public:
	CommandBSKickFlood(Module *creator) : CommandBSKickBase(creator, "botserv/kick/flood", 2, 5)
	{
		this->SetDesc(_("Configures flood kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037 [\037ln\037 [\037secs\037]]]\002"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci;
		if (!CheckArguments(source, params, ci))
			return;

		if (params[1].equals_ci("ON"))
		{
			const Anope::string &ttb = params.size() > 2 ? params[2] : "",
						&lines = params.size() > 3 ? params[3] : "",
						&secs = params.size() > 4 ? params[4] : "";

			if (!ttb.empty())
			{
				int16_t i;

				try
				{
					i = convertTo<int16_t>(ttb);
					if (i < 1)
						throw ConvertException();
				}
				catch (const ConvertException &)
				{
					source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
					return;
				}

				ci->ttb[TTB_FLOOD] = i;
			}
			else
				ci->ttb[TTB_FLOOD] = 0;

			ci->floodlines = 6;
			try
			{
				ci->floodlines = convertTo<int16_t>(lines);
			}
			catch (const ConvertException &) { }
			if (ci->floodlines < 2)
				ci->floodlines = 6;

			ci->floodsecs = 10;
			try
			{
				ci->floodsecs = convertTo<int16_t>(secs);
			}
			catch (const ConvertException &) { }
			if (ci->floodsecs < 1)
				ci->floodsecs = 10;
			if (ci->floodsecs > Config->GetModule(me)->Get<time_t>("keepdata"))
				ci->floodsecs = Config->GetModule(me)->Get<time_t>("keepdata");

			ci->ExtendMetadata("BS_KICK_FLOOD");
			if (ci->ttb[TTB_FLOOD])
				source.Reply(_("Bot will now kick for \002flood\002 (%d lines in %d seconds\n"
						"and will place a ban after %d kicks for the same user."), ci->floodlines, ci->floodsecs, ci->ttb[TTB_FLOOD]);
			else
				source.Reply(_("Bot will now kick for \002flood\002 (%d lines in %d seconds)."), ci->floodlines, ci->floodsecs);
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->Shrink("BS_KICK_FLOOD");
			source.Reply(_("Bot won't kick for \002flood\002 anymore."));
		}
		else
			this->OnSyntaxError(source, params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		source.Reply(_("Sets the flood kicker on or off. When enabled, this\n"
				"option tells the bot to kick users who are flooding\n"
				"the channel using at least \002ln\002 lines in \002secs\002 seconds\n"
				"(if not given, it defaults to 6 lines in 10 seconds).\n"
				" \n"
				"ttb is the number of times a user can be kicked\n"
				"before it get banned. Don't give ttb to disable\n"
				"the ban system once activated."));
		return true;
	}
};

class CommandBSKickItalics : public CommandBSKickBase
{
 public:
	CommandBSKickItalics(Module *creator) : CommandBSKickBase(creator, "botserv/kick/italics", 2, 3)
	{
		this->SetDesc(_("Configures italics kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci;
		if (CheckArguments(source, params, ci))
			Process(source, ci, params[1], params.size() > 2 ? params[2] : "", TTB_ITALICS, "italics", "BS_KICK_ITALICS");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		source.Reply(_("Sets the italics kicker on or off. When enabled, this\n"
				"option tells the bot to kick users who use italics.\n"
				"ttb is the number of times a user can be kicked\n"
				"before it get banned. Don't give ttb to disable\n"
				"the ban system once activated."));
		return true;
	}
};

class CommandBSKickRepeat : public CommandBSKickBase
{
 public:
	CommandBSKickRepeat(Module *creator) : CommandBSKickBase(creator, "botserv/kick/repeat", 2, 4)
	{
		this->SetDesc(_("Configures repeat kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037 [\037num\037]]\002"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci;
		if (!CheckArguments(source, params, ci))
			return;

		if (params[1].equals_ci("ON"))
		{
			const Anope::string &ttb = params[2],
						&times = params.size() > 3 ? params[3] : "";

			if (!ttb.empty())
			{
				int16_t i;

				try
				{
					i = convertTo<int16_t>(ttb);
					if (i < 0)
						throw ConvertException();
				}
				catch (const ConvertException &)
				{
					source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
					return;
				}

				ci->ttb[TTB_REPEAT] = i;
			}
			else
				ci->ttb[TTB_REPEAT] = 0;

			ci->repeattimes = 3;
			try
			{
				ci->repeattimes = convertTo<int16_t>(times);
			}
			catch (const ConvertException &) { }
			if (ci->repeattimes < 2)
				ci->repeattimes = 3;

			ci->ExtendMetadata("BS_KICK_REPEAT");
			if (ci->ttb[TTB_REPEAT])
				source.Reply(_("Bot will now kick for \002repeats\002 (users that say the\n"
						"same thing %d times), and will place a ban after %d\n"
						"kicks for the same user."), ci->repeattimes, ci->ttb[TTB_REPEAT]);
			else
				source.Reply(_("Bot will now kick for \002repeats\002 (users that say the\n"
					"same thing %d times)."), ci->repeattimes);
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->Shrink("BS_KICK_REPEAT");
			source.Reply(_("Bot won't kick for \002repeats\002 anymore."));
		}
		else
			this->OnSyntaxError(source, params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		source.Reply(_("Sets the repeat kicker on or off. When enabled, this\n"
				"option tells the bot to kick users who are repeating\n"
				"themselves \002num\002 times (if num is not given, it\n"
				"defaults to 3).\n"
				"ttb is the number of times a user can be kicked\n"
				"before it get banned. Don't give ttb to disable\n"
				"the ban system once activated."));
		return true;
	}
};

class CommandBSKickReverses : public CommandBSKickBase
{
 public:
 	CommandBSKickReverses(Module *creator) : CommandBSKickBase(creator, "botserv/kick/reverses", 2, 3)
	{
		this->SetDesc(_("Configures reverses kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci;
		if (CheckArguments(source, params, ci))
			Process(source, ci, params[1], params.size() > 2 ? params[2] : "", TTB_ITALICS, "italics", "BS_KICK_ITALICS");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		source.Reply(_("Sets the reverses kicker on or off. When enabled, this\n"
				"option tells the bot to kick users who use reverses.\n"
				"ttb is the number of times a user can be kicked\n"
				"before it get banned. Don't give ttb to disable\n"
				"the ban system once activated."));
		return true;
	}
};

class CommandBSKickUnderlines : public CommandBSKickBase
{
 public:
	CommandBSKickUnderlines(Module *creator) : CommandBSKickBase(creator, "botserv/kick/underlines", 2, 3)
	{
		this->SetDesc(_("Configures underlines kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci;
		if (CheckArguments(source, params, ci))
			Process(source, ci, params[1], params.size() > 2 ? params[2] : "", TTB_ITALICS, "italics", "BS_KICK_ITALICS");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		source.Reply(_("Sets the underlines kicker on or off. When enabled, this\n"
			"option tells the bot to kick users who use underlines.\n"
			"ttb is the number of times a user can be kicked\n"
			"before it get banned. Don't give ttb to disable\n"
			"the ban system once activated."));
		return true;
	}
};

struct BanData : ExtensibleItem
{
	struct Data
	{
		Anope::string mask;
		time_t last_use;
		int16_t ttb[TTB_SIZE];

		Data()
		{
			this->Clear();
		}

		void Clear()
		{
			last_use = 0;
			for (int i = 0; i < TTB_SIZE; ++i)
				this->ttb[i] = 0;
		}
	};

 private:
 	typedef std::map<Anope::string, Data, ci::less> data_type;
	data_type data_map;

 public:
	Data &get(const Anope::string &key)
	{
		return this->data_map[key];
	}

	bool empty() const
	{
		return this->data_map.empty();
	}

	void purge()
	{
		time_t keepdata = Config->GetModule(me)->Get<time_t>("keepdata");
		for (data_type::iterator it = data_map.begin(), it_end = data_map.end(); it != it_end;)
		{
			const Anope::string &user = it->first;
			Data &bd = it->second;
			++it;

			if (Anope::CurTime - bd.last_use > keepdata)
				data_map.erase(user);
		}
	}
};

struct UserData : ExtensibleItem
{
	UserData()
	{
		this->Clear();
	}

	void Clear()
	{
		last_use = last_start = Anope::CurTime;
		lines = times = 0;
		lastline.clear();
	}

	/* Data validity */
	time_t last_use;

	/* for flood kicker */
	int16_t lines;
	time_t last_start;

	/* for repeat kicker */
	Anope::string lasttarget;
	int16_t times;

	Anope::string lastline;
};


class BanDataPurger : public Timer
{
 public:
	BanDataPurger(Module *o) : Timer(o, 300, Anope::CurTime, true) { }

	void Tick(time_t) anope_override
	{
		Log(LOG_DEBUG) << "bs_main: Running bandata purger";

		for (channel_map::iterator it = ChannelList.begin(), it_end = ChannelList.end(); it != it_end; ++it)
		{
			Channel *c = it->second;

			BanData *bd = c->GetExt<BanData *>("bs_main_bandata");
			if (bd != NULL)
			{
				bd->purge();
				if (bd->empty())
					c->Shrink("bs_main_bandata");
			}
		}
	}
};

class BSKick : public Module
{
	CommandBSKick commandbskick;
	CommandBSKickAMSG commandbskickamsg;
	CommandBSKickBadwords commandbskickbadwords;
	CommandBSKickBolds commandbskickbolds;
	CommandBSKickCaps commandbskickcaps;
	CommandBSKickColors commandbskickcolors;
	CommandBSKickFlood commandbskickflood;
	CommandBSKickItalics commandbskickitalics;
	CommandBSKickRepeat commandbskickrepeat;
	CommandBSKickReverses commandbskickreverse;
	CommandBSKickUnderlines commandbskickunderlines;

	BanDataPurger purger;

	BanData::Data &GetBanData(User *u, Channel *c)
	{
		BanData *bd = c->GetExt<BanData *>("bs_main_bandata");
		if (bd == NULL)
		{
			bd = new BanData();
			c->Extend("bs_main_bandata", bd);
		}

		return bd->get(u->GetMask());
	}

	UserData *GetUserData(User *u, Channel *c)
	{
		ChanUserContainer *uc = c->FindUser(u);
		if (uc == NULL)
			return NULL;

		UserData *ud = uc->GetExt<UserData *>("bs_main_userdata");
		if (ud == NULL)
		{
			ud = new UserData();
			uc->Extend("bs_main_userdata", ud);
		}

		return ud;
       }

	void check_ban(ChannelInfo *ci, User *u, int ttbtype)
	{
		/* Don't ban ulines */
		if (u->server->IsULined())
			return;

		BanData::Data &bd = this->GetBanData(u, ci->c);

		++bd.ttb[ttbtype];
		if (ci->ttb[ttbtype] && bd.ttb[ttbtype] >= ci->ttb[ttbtype])
		{
			/* Should not use == here because bd.ttb[ttbtype] could possibly be > ci->ttb[ttbtype]
			 * if the TTB was changed after it was not set (0) before and the user had already been
			 * kicked a few times. Bug #1056 - Adam */

			bd.ttb[ttbtype] = 0;

			Anope::string mask = ci->GetIdealBan(u);

			ci->c->SetMode(NULL, "BAN", mask);
			FOREACH_MOD(I_OnBotBan, OnBotBan(u, ci, mask));
		}
	}

	void bot_kick(ChannelInfo *ci, User *u, const char *message, ...)
	{
		va_list args;
		char buf[1024];

		if (!ci || !ci->bi || !ci->c || !u || u->server->IsULined() || !ci->c->FindUser(u))
			return;

		Anope::string fmt = Language::Translate(u, message);
		va_start(args, message);
		vsnprintf(buf, sizeof(buf), fmt.c_str(), args);
		va_end(args);

		ci->c->Kick(ci->bi, u, "%s", buf);
	}

 public:
	BSKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandbskick(this),
		commandbskickamsg(this), commandbskickbadwords(this), commandbskickbolds(this), commandbskickcaps(this),
		commandbskickcolors(this), commandbskickflood(this), commandbskickitalics(this), commandbskickrepeat(this),
		commandbskickreverse(this), commandbskickunderlines(this),

		purger(this)
	{
		me = this;

		Implementation i[] = { I_OnPrivmsg };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	~BSKick()
	{
		for (channel_map::const_iterator cit = ChannelList.begin(), cit_end = ChannelList.end(); cit != cit_end; ++cit)
		{
			Channel *c = cit->second;
			for (Channel::ChanUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
				it->second->Shrink("bs_main_userdata");
			c->Shrink("bs_main_bandata");
		}
	}

	void OnPrivmsg(User *u, Channel *c, Anope::string &msg) anope_override
	{
		/* Now we can make kicker stuff. We try to order the checks
		 * from the fastest one to the slowest one, since there's
		 * no need to process other kickers if a user is kicked before
		 * the last kicker check.
		 *
		 * But FIRST we check whether the user is protected in any
		 * way.
		 */
		ChannelInfo *ci = c->ci;
		if (ci == NULL)
			return;

		if (ci->AccessFor(u).HasPriv("NOKICK"))
			return;
		else if (ci->HasExt("BS_DONTKICKOPS") && (c->HasUserStatus(u, "HALFOP") || c->HasUserStatus(u, "OP") || c->HasUserStatus(u, "PROTECT") || c->HasUserStatus(u, "OWNER")))
			return;
		else if (ci->HasExt("BS_DONTKICKVOICES") && c->HasUserStatus(u, "VOICE"))
			return;

		Anope::string realbuf = msg;

		/* If it's a /me, cut the CTCP part because the ACTION will cause
		 * problems with the caps or badwords kicker
		 */
		if (realbuf.substr(0, 8).equals_ci("\1ACTION ") && realbuf[realbuf.length() - 1] == '\1')
		{
			realbuf.erase(0, 8);
			realbuf.erase(realbuf.length() - 1);
		}

		if (realbuf.empty())
			return;

		/* Bolds kicker */
		if (ci->HasExt("BS_KICK_BOLDS") && realbuf.find(2) != Anope::string::npos)
		{
			check_ban(ci, u, TTB_BOLDS);
			bot_kick(ci, u, _("Don't use bolds on this channel!"));
			return;
		}

		/* Color kicker */
		if (ci->HasExt("BS_KICK_COLORS") && realbuf.find(3) != Anope::string::npos)
		{
			check_ban(ci, u, TTB_COLORS);
			bot_kick(ci, u, _("Don't use colors on this channel!"));
			return;
		}

		/* Reverses kicker */
		if (ci->HasExt("BS_KICK_REVERSES") && realbuf.find(22) != Anope::string::npos)
		{
			check_ban(ci, u, TTB_REVERSES);
			bot_kick(ci, u, _("Don't use reverses on this channel!"));
			return;
		}

		/* Italics kicker */
		if (ci->HasExt("BS_KICK_ITALICS") && realbuf.find(29) != Anope::string::npos)
		{
			check_ban(ci, u, TTB_ITALICS);
			bot_kick(ci, u, _("Don't use italics on this channel!"));
			return;
		}

		/* Underlines kicker */
		if (ci->HasExt("BS_KICK_UNDERLINES") && realbuf.find(31) != Anope::string::npos)
		{
			check_ban(ci, u, TTB_UNDERLINES);
			bot_kick(ci, u, _("Don't use underlines on this channel!"));
			return;
		}

		/* Caps kicker */
		if (ci->HasExt("BS_KICK_CAPS") && realbuf.length() >= static_cast<unsigned>(ci->capsmin))
		{
			int i = 0, l = 0;

			for (unsigned j = 0, end = realbuf.length(); j < end; ++j)
			{
				if (isupper(realbuf[j]))
					++i;
				else if (islower(realbuf[j]))
					++l;
			}

			/* i counts uppercase chars, l counts lowercase chars. Only
			 * alphabetic chars (so islower || isupper) qualify for the
			 * percentage of caps to kick for; the rest is ignored. -GD
			 */

			if ((i || l) && i >= ci->capsmin && i * 100 / (i + l) >= ci->capspercent)
			{
				check_ban(ci, u, TTB_CAPS);
				bot_kick(ci, u, _("Turn caps lock OFF!"));
				return;
			}
		}

		/* Bad words kicker */
		if (ci->HasExt("BS_KICK_BADWORDS"))
		{
			bool mustkick = false;

			/* Normalize the buffer */
			Anope::string nbuf = Anope::NormalizeBuffer(realbuf);
			bool casesensitive = Config->GetModule("botserv")->Get<bool>("casesensitive");

			for (unsigned i = 0, end = ci->GetBadWordCount(); i < end; ++i)
			{
				const BadWord *bw = ci->GetBadWord(i);

				if (bw->type == BW_ANY && ((casesensitive && nbuf.find(bw->word) != Anope::string::npos) || (!casesensitive && nbuf.find_ci(bw->word) != Anope::string::npos)))
					mustkick = true;
				else if (bw->type == BW_SINGLE)
				{
					size_t len = bw->word.length();

					if ((casesensitive && bw->word.equals_cs(nbuf)) || (!casesensitive && bw->word.equals_ci(nbuf)))
						mustkick = true;
					else if (nbuf.find(' ') == len && ((casesensitive && bw->word.equals_cs(nbuf)) || (!casesensitive && bw->word.equals_ci(nbuf))))
						mustkick = true;
					else
					{
						if (nbuf.rfind(' ') == nbuf.length() - len - 1 && ((casesensitive && nbuf.find(bw->word) == nbuf.length() - len) || (!casesensitive && nbuf.find_ci(bw->word) == nbuf.length() - len)))
							mustkick = true;
						else
						{
							Anope::string wordbuf = " " + bw->word + " ";

							if ((casesensitive && nbuf.find(wordbuf) != Anope::string::npos) || (!casesensitive && nbuf.find_ci(wordbuf) != Anope::string::npos))
								mustkick = true;
						}
					}
				}
				else if (bw->type == BW_START)
				{
					size_t len = bw->word.length();

					if ((casesensitive && nbuf.substr(0, len).equals_cs(bw->word)) || (!casesensitive && nbuf.substr(0, len).equals_ci(bw->word)))
						mustkick = true;
					else
					{
						Anope::string wordbuf = " " + bw->word;

						if ((casesensitive && nbuf.find(wordbuf) != Anope::string::npos) || (!casesensitive && nbuf.find_ci(wordbuf) != Anope::string::npos))
							mustkick = true;
					}
				}
				else if (bw->type == BW_END)
				{
					size_t len = bw->word.length();

					if ((casesensitive && nbuf.substr(nbuf.length() - len).equals_cs(bw->word)) || (!casesensitive && nbuf.substr(nbuf.length() - len).equals_ci(bw->word)))
						mustkick = true;
					else
					{
						Anope::string wordbuf = bw->word + " ";

						if ((casesensitive && nbuf.find(wordbuf) != Anope::string::npos) || (!casesensitive && nbuf.find_ci(wordbuf) != Anope::string::npos))
							mustkick = true;
					}
				}

				if (mustkick)
				{
					check_ban(ci, u, TTB_BADWORDS);
					if (Config->GetModule(me)->Get<bool>("gentlebadwordreason"))
						bot_kick(ci, u, _("Watch your language!"));
					else
						bot_kick(ci, u, _("Don't use the word \"%s\" on this channel!"), bw->word.c_str());

					return;
				}
			} /* for */
		} /* if badwords */

		UserData *ud = GetUserData(u, c);

		if (ud)
		{
			/* Flood kicker */
			if (ci->HasExt("BS_KICK_FLOOD"))
			{
				if (Anope::CurTime - ud->last_start > ci->floodsecs)
				{
					ud->last_start = Anope::CurTime;
					ud->lines = 0;
				}

				++ud->lines;
				if (ud->lines >= ci->floodlines)
				{
					check_ban(ci, u, TTB_FLOOD);
					bot_kick(ci, u, _("Stop flooding!"));
					return;
				}
			}

			/* Repeat kicker */
			if (ci->HasExt("BS_KICK_REPEAT"))
			{
				if (!ud->lastline.equals_ci(realbuf))
					ud->times = 0;
				else
					++ud->times;

				if (ud->times >= ci->repeattimes)
				{
					check_ban(ci, u, TTB_REPEAT);
					bot_kick(ci, u, _("Stop repeating yourself!"));
					return;
				}
			}

			if (ud->lastline.equals_ci(realbuf) && !ud->lasttarget.empty() && !ud->lasttarget.equals_ci(ci->name))
			{
				for (User::ChanUserList::iterator it = u->chans.begin(); it != u->chans.end();)
				{
					Channel *chan = it->second->chan;
					++it;

					if (chan->ci && chan->ci->HasExt("BS_KICK_AMSGS") && !chan->ci->AccessFor(u).HasPriv("NOKICK"))
					{
						check_ban(chan->ci, u, TTB_AMSGS);
						bot_kick(chan->ci, u, _("Don't use AMSGs!"));
					}
				}
			}

			ud->lasttarget = ci->name;
			ud->lastline = realbuf;
		}
	}
};

MODULE_INIT(BSKick)
