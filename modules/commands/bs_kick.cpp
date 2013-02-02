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
/*************************************************************************/

#include "module.h"

class CommandBSKick : public Command
{
 public:
	CommandBSKick(Module *creator) : Command(creator, "botserv/kick", 3, 6)
	{
		this->SetDesc(_("Configures kickers"));
		this->SetSyntax(_("\037channel\037 \037option\037 {\037ON|OFF\037} [\037settings\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &chan = params[0];
		const Anope::string &option = params[1];
		const Anope::string &value = params[2];
		const Anope::string &ttb = params.size() > 3 ? params[3] : "";

		ChannelInfo *ci = ChannelInfo::Find(params[0]);

		if (Anope::ReadOnly)
			source.Reply(_("Sorry, kicker configuration is temporarily disabled."));
		else if (ci == NULL)
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
		else if (chan.empty() || option.empty() || value.empty())
			this->OnSyntaxError(source, "");
		else if (!value.equals_ci("ON") && !value.equals_ci("OFF"))
			this->OnSyntaxError(source, "");
		else if (!source.AccessFor(ci).HasPriv("SET") && !source.HasPriv("botserv/administration"))
			source.Reply(ACCESS_DENIED);
		else if (!ci->bi)
			source.Reply(BOT_NOT_ASSIGNED);
		else
		{
			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << option << " " << value;

			if (option.equals_ci("BADWORDS"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_BADWORDS] = convertTo<int16_t>(ttb);
							if (ci->ttb[TTB_BADWORDS] < 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							/* reset the value back to 0 - TSL */
							ci->ttb[TTB_BADWORDS] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return;
						}
					}
					else
						ci->ttb[TTB_BADWORDS] = 0;

					ci->ExtendMetadata("BS_KICK_BADWORDS");
					if (ci->ttb[TTB_BADWORDS])
						source.Reply(_("Bot will now kick for \002bad words\002, and will place a ban\n"
								"after %d kicks for the same user. Use the BADWORDS command\n"
								"to add or remove a bad word."), ci->ttb[TTB_BADWORDS]);
					else
						source.Reply(_("Bot will now kick for \002bad words\002. Use the BADWORDS command\n"
								"to add or remove a bad word."));
				}
				else
				{
					ci->Shrink("BS_KICK_BADWORDS");
					source.Reply(_("Bot won't kick for \002bad words\002 anymore."));
				}
			}
			else if (option.equals_ci("BOLDS"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_BOLDS] = convertTo<int16_t>(ttb);
							if (ci->ttb[TTB_BOLDS] < 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_BOLDS] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return;
						}
					}
					else
						ci->ttb[TTB_BOLDS] = 0;
					ci->ExtendMetadata("BS_KICK_BOLDS");
					if (ci->ttb[TTB_BOLDS])
						source.Reply(_("Bot will now kick for \002bolds\002, and will place a ban\n"
								"after %d kicks for the same user."), ci->ttb[TTB_BOLDS]);
					else
						source.Reply(_("Bot will now kick for \002bolds\002."));
				}
				else
				{
					ci->Shrink("BS_KICK_BOLDS");
					source.Reply(_("Bot won't kick for \002bolds\002 anymore."));
				}
			}
			else if (option.equals_ci("CAPS"))
			{
				if (value.equals_ci("ON"))
				{
					Anope::string min = params.size() > 4 ? params[4] : "";
					Anope::string percent = params.size() > 5 ? params[5] : "";

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
			else if (option.equals_ci("COLORS"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_COLORS] = convertTo<int16_t>(ttb);
							if (ci->ttb[TTB_COLORS] < 1)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_COLORS] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return;
						}
					}
					else
						ci->ttb[TTB_COLORS] = 0;

					ci->ExtendMetadata("BS_KICK_COLORS");
					if (ci->ttb[TTB_COLORS])
						source.Reply(_("Bot will now kick for \002colors\002, and will place a ban\n"
								"after %d kicks for the same user."), ci->ttb[TTB_COLORS]);
					else
						source.Reply(_("Bot will now kick for \002colors\002."));
				}
				else
				{
					ci->Shrink("BS_KICK_COLORS");
					source.Reply(_("Bot won't kick for \002colors\002 anymore."));
				}
			}
			else if (option.equals_ci("FLOOD"))
			{
				if (value.equals_ci("ON"))
				{
					Anope::string lines = params.size() > 4 ? params[4] : "";
					Anope::string secs = params.size() > 5 ? params[5] : "";

					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_FLOOD] = convertTo<int16_t>(ttb);
							if (ci->ttb[TTB_FLOOD] < 1)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_FLOOD] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return;
						}
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
					if (ci->floodsecs > Config->BSKeepData)
						ci->floodsecs = Config->BSKeepData;

					ci->ExtendMetadata("BS_KICK_FLOOD");
					if (ci->ttb[TTB_FLOOD])
						source.Reply(_("Bot will now kick for \002flood\002 (%d lines in %d seconds\n"
								"and will place a ban after %d kicks for the same user."), ci->floodlines, ci->floodsecs, ci->ttb[TTB_FLOOD]);
					else
						source.Reply(_("Bot will now kick for \002flood\002 (%d lines in %d seconds)."), ci->floodlines, ci->floodsecs);
				}
				else
				{
					ci->Shrink("BS_KICK_FLOOD");
					source.Reply(_("Bot won't kick for \002flood\002 anymore."));
				}
			}
			else if (option.equals_ci("REPEAT"))
			{
				if (value.equals_ci("ON"))
				{
					Anope::string times = params.size() > 4 ? params[4] : "";

					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_REPEAT] = convertTo<int16_t>(ttb);
							if (ci->ttb[TTB_REPEAT] < 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_REPEAT] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return;
						}
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
				else
				{
					ci->Shrink("BS_KICK_REPEAT");
					source.Reply(_("Bot won't kick for \002repeats\002 anymore."));
				}
			}
			else if (option.equals_ci("REVERSES"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_REVERSES] = convertTo<int16_t>(ttb);
							if (ci->ttb[TTB_REVERSES] < 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_REVERSES] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return;
						}
					}
					else
						ci->ttb[TTB_REVERSES] = 0;
					ci->ExtendMetadata("BS_KICK_REVERSES");
					if (ci->ttb[TTB_REVERSES])
						source.Reply(_("Bot will now kick for \002reverses\002, and will place a ban\n"
								"after %d kicks for the same user."), ci->ttb[TTB_REVERSES]);
					else
						source.Reply(_("Bot will now kick for \002reverses\002."));
				}
				else
				{
					ci->Shrink("BS_KICK_REVERSES");
					source.Reply(_("Bot won't kick for \002reverses\002 anymore."));
				}
			}
			else if (option.equals_ci("UNDERLINES"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_UNDERLINES] = convertTo<int16_t>(ttb);
							if (ci->ttb[TTB_REVERSES] < 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_UNDERLINES] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return;
						}
					}
					else
						ci->ttb[TTB_UNDERLINES] = 0;

					ci->ExtendMetadata("BS_KICK_UNDERLINES");
					if (ci->ttb[TTB_UNDERLINES])
						source.Reply(_("Bot will now kick for \002underlines\002, and will place a ban\n"
								"after %d kicks for the same user."), ci->ttb[TTB_UNDERLINES]);
					else
						source.Reply(_("Bot will now kick for \002underlines\002."));
				}
				else
				{
					ci->Shrink("BS_KICK_UNDERLINES");
					source.Reply(_("Bot won't kick for \002underlines\002 anymore."));
				}
			}
			else if (option.equals_ci("ITALICS"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_ITALICS] = convertTo<int16_t>(ttb);
							if (ci->ttb[TTB_ITALICS] < 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_ITALICS] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return;
						}
					}
					else
						ci->ttb[TTB_ITALICS] = 0;

					ci->ExtendMetadata("BS_KICK_ITALICS");
					if (ci->ttb[TTB_ITALICS])
						source.Reply(_("Bot will now kick for \002italics\002, and will place a ban\n"
								"after %d kicks for the same user."), ci->ttb[TTB_ITALICS]);
					else
						source.Reply(_("Bot will now kick for \002italics\002."));
				}
				else
				{
					ci->Shrink("BS_KICK_ITALICS");
					source.Reply(_("Bot won't kick for \002italics\002 anymore."));
				}
			}
			else if (option.equals_ci("AMSGS"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_AMSGS] = convertTo<int16_t>(ttb);
							if (ci->ttb[TTB_AMSGS] < 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_AMSGS] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return;
						}
					}
					else
						ci->ttb[TTB_AMSGS] = 0;

					ci->ExtendMetadata("BS_KICK_AMSGS");
					if (ci->ttb[TTB_AMSGS])
						source.Reply(_("Bot will now kick for \002amsgs\002, and will place a ban\n"
								"after %d kicks for the same user."), ci->ttb[TTB_AMSGS]);
					else
						source.Reply(_("Bot will now kick for \002amsgs\002"));
				}
				else
				{
					ci->Shrink("BS_KICK_AMSGS");
					source.Reply(_("Bot won't kick for \002amsgs\002 anymore."));
				}
			}
			else
				this->OnSyntaxError(source, "");
		}
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		if (subcommand.empty())
		{
			this->SendSyntax(source);
			source.Reply(" ");
			source.Reply(_("Configures bot kickers.  \037option\037 can be one of:\n"
					" \n"
					"    AMSGS         Sets if the bot kicks for amsgs\n"
					"    BOLDS         Sets if the bot kicks bolds\n"
					"    BADWORDS      Sets if the bot kicks bad words\n"
					"    CAPS          Sets if the bot kicks caps\n"
					"    COLORS        Sets if the bot kicks colors\n"
					"    FLOOD         Sets if the bot kicks flooding users\n"
					"    REPEAT        Sets if the bot kicks users who repeat\n"
					"                       themselves\n"
					"    REVERSES      Sets if the bot kicks reverses\n"
					"    UNDERLINES    Sets if the bot kicks underlines\n"
					"    ITALICS       Sets if the bot kicks italics\n"
					" \n"
					"Type \002%s%s HELP KICK \037option\037\002 for more information\n"
					"on a specific option.\n"
					" \n"
					"Note: access to this command is controlled by the\n"
					"level SET."), Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str());
		}
		else if (subcommand.equals_ci("BADWORDS"))
			source.Reply(_("Syntax: \002\037channel\037 BADWORDS {\037ON|OFF\037} [\037ttb\037]\002\n"
					"Sets the bad words kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who say certain words\n"
					"on the channels.\n"
					"You can define bad words for your channel using the\n"
					"\002BADWORDS\002 command. Type \002%s%s HELP BADWORDS\002 for\n"
					"more information.\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."), Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str());
		else if (subcommand.equals_ci("BOLDS"))
			source.Reply(_("Syntax: \002\037channel\037 BOLDS {\037ON|OFF\037} [\037ttb\037]\002\n"
					"Sets the bolds kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who use bolds.\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("CAPS"))
			source.Reply(_("Syntax: \002\037channel\037 CAPS {\037ON|OFF\037} [\037ttb\037 [\037min\037 [\037percent\037]]]\002\n"
					"Sets the caps kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who are talking in\n"
					"CAPS.\n"
					"The bot kicks only if there are at least \002min\002 caps\n"
					"and they constitute at least \002percent\002%% of the total\n"
					"text line (if not given, it defaults to 10 characters\n"
					"and 25%%).\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("COLORS"))
			source.Reply(_("Syntax: \002\037channel\037 COLORS {\037ON|OFF\037} [\037ttb\037]\002\n"
					"Sets the colors kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who use colors.\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("FLOOD"))
			source.Reply(_("Syntax: \002\037channel\037 FLOOD {\037ON|OFF\037} [\037ttb\037 [\037ln\037 [\037secs\037]]]\002\n"
					"Sets the flood kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who are flooding\n"
					"the channel using at least \002ln\002 lines in \002secs\002 seconds\n"
					"(if not given, it defaults to 6 lines in 10 seconds).\n"
					" \n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("REPEAT"))
			source.Reply(_("Syntax: \002\037#channel\037 REPEAT {\037ON|OFF\037} [\037ttb\037 [\037num\037]]\002\n"
					"Sets the repeat kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who are repeating\n"
					"themselves \002num\002 times (if num is not given, it\n"
					"defaults to 3).\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("REVERSES"))
			source.Reply(_("Syntax: \002\037channel\037 REVERSES {\037ON|OFF\037} [\037ttb\037]\002\n"
					"Sets the reverses kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who use reverses.\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("UNDERLINES"))
			source.Reply(_("Syntax: \002\037channel\037 UNDERLINES {\037ON|OFF\037} [\037ttb\037]\002\n"
					"Sets the underlines kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who use underlines.\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("ITALICS"))
			source.Reply(_("Syntax: \002\037channel\037 ITALICS {\037ON|OFF\037} [\037ttb\037]\002\n"
					"Sets the italics kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who use italics.\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("AMSGS"))
			source.Reply(_("Syntax: \002\037channel\037 AMSGS {\037ON|OFF\037} [\037ttb\037]\002\n"
					"Sets the amsg kicker on or off. When enabled, the bot will\n"
					"kick users who send the same message to multiple channels\n"
					"where BotServ bots are.\n"
					"Ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else
			return false;

		return true;
	}
};

struct BanData : public ExtensibleItem
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
		for (data_type::iterator it = data_map.begin(), it_end = data_map.end(); it != it_end;)
		{
			const Anope::string &user = it->first;
			Data &bd = it->second;
			++it;

			if (Anope::CurTime - bd.last_use > Config->BSKeepData)
				data_map.erase(user);
		}
	}

	void OnDelete() anope_override
	{
		delete this;
	}
};

struct UserData : public ExtensibleItem
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

	void OnDelete() anope_override
	{
		delete this;
	}
};


class BanDataPurger : public CallBack
{
 public:
	BanDataPurger(Module *owner) : CallBack(owner, 300, Anope::CurTime, true) { }

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
	BSKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandbskick(this), purger(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::Attach(I_OnPrivmsg, this);
	}

	~BSKick()
	{
		for (channel_map::const_iterator cit = ChannelList.begin(), cit_end = ChannelList.end(); cit != cit_end; ++cit)
		{
			Channel *c = cit->second;
			for (Channel::ChanUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
				(*it)->Shrink("bs_main_userdata");
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

			for (unsigned i = 0, end = ci->GetBadWordCount(); i < end; ++i)
			{
				const BadWord *bw = ci->GetBadWord(i);

				if (bw->type == BW_ANY && ((Config->BSCaseSensitive && nbuf.find(bw->word) != Anope::string::npos) || (!Config->BSCaseSensitive && nbuf.find_ci(bw->word) != Anope::string::npos)))
					mustkick = true;
				else if (bw->type == BW_SINGLE)
				{
					size_t len = bw->word.length();

					if ((Config->BSCaseSensitive && bw->word.equals_cs(nbuf)) || (!Config->BSCaseSensitive && bw->word.equals_ci(nbuf)))
						mustkick = true;
					else if (nbuf.find(' ') == len && ((Config->BSCaseSensitive && bw->word.equals_cs(nbuf)) || (!Config->BSCaseSensitive && bw->word.equals_ci(nbuf))))
						mustkick = true;
					else
					{
						if (nbuf.rfind(' ') == nbuf.length() - len - 1 && ((Config->BSCaseSensitive && nbuf.find(bw->word) == nbuf.length() - len) || (!Config->BSCaseSensitive && nbuf.find_ci(bw->word) == nbuf.length() - len)))
							mustkick = true;
						else
						{
							Anope::string wordbuf = " " + bw->word + " ";

							if ((Config->BSCaseSensitive && nbuf.find(wordbuf) != Anope::string::npos) || (!Config->BSCaseSensitive && nbuf.find_ci(wordbuf) != Anope::string::npos))
								mustkick = true;
						}
					}
				}
				else if (bw->type == BW_START)
				{
					size_t len = bw->word.length();

					if ((Config->BSCaseSensitive && nbuf.substr(0, len).equals_cs(bw->word)) || (!Config->BSCaseSensitive && nbuf.substr(0, len).equals_ci(bw->word)))
						mustkick = true;
					else
					{
						Anope::string wordbuf = " " + bw->word;

						if ((Config->BSCaseSensitive && nbuf.find(wordbuf) != Anope::string::npos) || (!Config->BSCaseSensitive && nbuf.find_ci(wordbuf) != Anope::string::npos))
							mustkick = true;
					}
				}
				else if (bw->type == BW_END)
				{
					size_t len = bw->word.length();

					if ((Config->BSCaseSensitive && nbuf.substr(nbuf.length() - len).equals_cs(bw->word)) || (!Config->BSCaseSensitive && nbuf.substr(nbuf.length() - len).equals_ci(bw->word)))
						mustkick = true;
					else
					{
						Anope::string wordbuf = bw->word + " ";

						if ((Config->BSCaseSensitive && nbuf.find(wordbuf) != Anope::string::npos) || (!Config->BSCaseSensitive && nbuf.find_ci(wordbuf) != Anope::string::npos))
							mustkick = true;
					}
				}

				if (mustkick)
				{
					check_ban(ci, u, TTB_BADWORDS);
					if (Config->BSGentleBWReason)
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
					Channel *chan = (*it)->chan;
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
