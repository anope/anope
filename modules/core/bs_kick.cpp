/* BotServ core functions
 *
 * (C) 2003-2011 Anope Team
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
	CommandBSKick() : Command("KICK", 3, 6)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		const Anope::string &option = params[1];
		const Anope::string &value = params[2];
		const Anope::string &ttb = params.size() > 3 ? params[3] : "";

		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (readonly)
			source.Reply(BOT_KICK_DISABLED);
		else if (chan.empty() || option.empty() || value.empty())
			SyntaxError(source, "KICK", BOT_KICK_SYNTAX);
		else if (!value.equals_ci("ON") && !value.equals_ci("OFF"))
			SyntaxError(source, "KICK", BOT_KICK_SYNTAX);
		else if (!check_access(u, ci, CA_SET) && !u->Account()->HasPriv("botserv/administration"))
			source.Reply(ACCESS_DENIED);
		else if (!ci->bi)
			source.Reply(BOT_NOT_ASSIGNED);
		else
		{
			bool override = !check_access(u, ci, CA_SET);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << option << " " << value;

			if (option.equals_ci("BADWORDS"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						Anope::string error;
						ci->ttb[TTB_BADWORDS] = convertTo<int16>(ttb, error, false);
						/* Only error if errno returns ERANGE or EINVAL or we are less then 0 - TSL */
						if (!error.empty() || ci->ttb[TTB_BADWORDS] < 0)
						{
							/* leaving the debug behind since we might want to know what these are */
							Log(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_BADWORDS];
							/* reset the value back to 0 - TSL */
							ci->ttb[TTB_BADWORDS] = 0;
							source.Reply(BOT_KICK_BAD_TTB, ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_BADWORDS] = 0;
					ci->botflags.SetFlag(BS_KICK_BADWORDS);
					if (ci->ttb[TTB_BADWORDS])
						source.Reply(BOT_KICK_BADWORDS_ON_BAN, ci->ttb[TTB_BADWORDS]);
					else
						source.Reply(BOT_KICK_BADWORDS_ON);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_BADWORDS);
					source.Reply(BOT_KICK_BADWORDS_OFF);
				}
			}
			else if (option.equals_ci("BOLDS"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						Anope::string error;
						ci->ttb[TTB_BOLDS] = convertTo<int16>(ttb, error, false);
						if (!error.empty() || ci->ttb[TTB_BOLDS] < 0)
						{
							Log(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_BOLDS];
							ci->ttb[TTB_BOLDS] = 0;
							source.Reply(BOT_KICK_BAD_TTB, ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_BOLDS] = 0;
					ci->botflags.SetFlag(BS_KICK_BOLDS);
					if (ci->ttb[TTB_BOLDS])
						source.Reply(BOT_KICK_BOLDS_ON_BAN, ci->ttb[TTB_BOLDS]);
					else
						source.Reply(BOT_KICK_BOLDS_ON);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_BOLDS);
					source.Reply(BOT_KICK_BOLDS_OFF);
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
						Anope::string error;
						ci->ttb[TTB_CAPS] = convertTo<int16>(ttb, error, false);
						if (!error.empty() || ci->ttb[TTB_CAPS] < 0)
						{
							Log(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_CAPS];
							ci->ttb[TTB_CAPS] = 0;
							source.Reply(BOT_KICK_BAD_TTB, ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_CAPS] = 0;

					if (min.empty())
						ci->capsmin = 10;
					else
						ci->capsmin = min.is_number_only() ? convertTo<int16>(min) : 10;
					if (ci->capsmin < 1)
						ci->capsmin = 10;

					if (percent.empty())
						ci->capspercent = 25;
					else
						ci->capspercent = percent.is_number_only() ? convertTo<int16>(percent) : 25;
					if (ci->capspercent < 1 || ci->capspercent > 100)
						ci->capspercent = 25;

					ci->botflags.SetFlag(BS_KICK_CAPS);
					if (ci->ttb[TTB_CAPS])
						source.Reply(BOT_KICK_CAPS_ON_BAN, ci->capsmin, ci->capspercent, ci->ttb[TTB_CAPS]);
					else
						source.Reply(BOT_KICK_CAPS_ON, ci->capsmin, ci->capspercent);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_CAPS);
					source.Reply(BOT_KICK_CAPS_OFF);
				}
			}
			else if (option.equals_ci("COLORS"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						Anope::string error;
						ci->ttb[TTB_COLORS] = convertTo<int16>(ttb, error, false);
						if (!error.empty() || ci->ttb[TTB_COLORS] < 0)
						{
							Log(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_COLORS];
							ci->ttb[TTB_COLORS] = 0;
							source.Reply(BOT_KICK_BAD_TTB, ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_COLORS] = 0;
					ci->botflags.SetFlag(BS_KICK_COLORS);
					if (ci->ttb[TTB_COLORS])
						source.Reply(BOT_KICK_COLORS_ON_BAN, ci->ttb[TTB_COLORS]);
					else
						source.Reply(BOT_KICK_COLORS_ON);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_COLORS);
					source.Reply(BOT_KICK_COLORS_OFF);
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
						Anope::string error;
						ci->ttb[TTB_FLOOD] = convertTo<int16>(ttb, error, false);
						if (!error.empty() || ci->ttb[TTB_FLOOD] < 0)
						{
							Log(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_FLOOD];
							ci->ttb[TTB_FLOOD] = 0;
							source.Reply(BOT_KICK_BAD_TTB, ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_FLOOD] = 0;

					if (lines.empty())
						ci->floodlines = 6;
					else
						ci->floodlines = lines.is_number_only() ? convertTo<int16>(lines) : 6;
					if (ci->floodlines < 2)
						ci->floodlines = 6;

					if (secs.empty())
						ci->floodsecs = 10;
					else
						ci->floodsecs = secs.is_number_only() ? convertTo<int16>(secs) : 10;
					if (ci->floodsecs < 1 || ci->floodsecs > Config->BSKeepData)
						ci->floodsecs = 10;

					ci->botflags.SetFlag(BS_KICK_FLOOD);
					if (ci->ttb[TTB_FLOOD])
						source.Reply(BOT_KICK_FLOOD_ON_BAN, ci->floodlines, ci->floodsecs, ci->ttb[TTB_FLOOD]);
					else
						source.Reply(BOT_KICK_FLOOD_ON, ci->floodlines, ci->floodsecs);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_FLOOD);
					source.Reply(BOT_KICK_FLOOD_OFF);
				}
			}
			else if (option.equals_ci("REPEAT"))
			{
				if (value.equals_ci("ON"))
				{
					Anope::string times = params.size() > 4 ? params[4] : "";

					if (!ttb.empty())
					{
						Anope::string error;
						ci->ttb[TTB_REPEAT] = convertTo<int16>(ttb, error, false);
						if (!error.empty() || ci->ttb[TTB_REPEAT] < 0)
						{
							Log(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_REPEAT];
							ci->ttb[TTB_REPEAT] = 0;
							source.Reply(BOT_KICK_BAD_TTB, ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_REPEAT] = 0;

					if (times.empty())
						ci->repeattimes = 3;
					else
						ci->repeattimes = times.is_number_only() ? convertTo<int16>(times) : 3;
					if (ci->repeattimes < 2)
						ci->repeattimes = 3;

					ci->botflags.SetFlag(BS_KICK_REPEAT);
					if (ci->ttb[TTB_REPEAT])
						source.Reply(BOT_KICK_REPEAT_ON_BAN, ci->repeattimes, ci->ttb[TTB_REPEAT]);
					else
						source.Reply(BOT_KICK_REPEAT_ON, ci->repeattimes);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_REPEAT);
					source.Reply(BOT_KICK_REPEAT_OFF);
				}
			}
			else if (option.equals_ci("REVERSES"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						Anope::string error;
						ci->ttb[TTB_REVERSES] = convertTo<int16>(ttb, error, false);
						if (!error.empty() || ci->ttb[TTB_REVERSES] < 0)
						{
							Log(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_REVERSES];
							ci->ttb[TTB_REVERSES] = 0;
							source.Reply(BOT_KICK_BAD_TTB, ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_REVERSES] = 0;
					ci->botflags.SetFlag(BS_KICK_REVERSES);
					if (ci->ttb[TTB_REVERSES])
						source.Reply(BOT_KICK_REVERSES_ON_BAN, ci->ttb[TTB_REVERSES]);
					else
						source.Reply(BOT_KICK_REVERSES_ON);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_REVERSES);
					source.Reply(BOT_KICK_REVERSES_OFF);
				}
			}
			else if (option.equals_ci("UNDERLINES"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						Anope::string error;
						ci->ttb[TTB_UNDERLINES] = convertTo<int16>(ttb, error, false);
						if (!error.empty() || ci->ttb[TTB_UNDERLINES] < 0)
						{
							Log(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_UNDERLINES];
							ci->ttb[TTB_UNDERLINES] = 0;
							source.Reply(BOT_KICK_BAD_TTB, ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_UNDERLINES] = 0;
					ci->botflags.SetFlag(BS_KICK_UNDERLINES);
					if (ci->ttb[TTB_UNDERLINES])
						source.Reply(BOT_KICK_UNDERLINES_ON_BAN, ci->ttb[TTB_UNDERLINES]);
					else
						source.Reply(BOT_KICK_UNDERLINES_ON);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_UNDERLINES);
					source.Reply(BOT_KICK_UNDERLINES_OFF);
				}
			}
			else if (option.equals_ci("ITALICS"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						Anope::string error;
						ci->ttb[TTB_ITALICS] = convertTo<int16>(ttb, error, false);
						if (!error.empty() || ci->ttb[TTB_ITALICS] < 0)
						{
							Log(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_ITALICS];
							ci->ttb[TTB_ITALICS] = 0;
							source.Reply(BOT_KICK_BAD_TTB, ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_ITALICS] = 0;
					ci->botflags.SetFlag(BS_KICK_ITALICS);
					if (ci->ttb[TTB_ITALICS])
						source.Reply(BOT_KICK_ITALICS_ON_BAN, ci->ttb[TTB_ITALICS]);
					else
						source.Reply(BOT_KICK_ITALICS_ON);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_ITALICS);
					source.Reply(BOT_KICK_ITALICS_OFF);
				}
			}
			else if (option.equals_ci("AMSGS"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						Anope::string error;
						ci->ttb[TTB_AMSGS] = convertTo<int16>(ttb, error, false);
						if (!error.empty() || ci->ttb[TTB_AMSGS] < 0)
						{
							Log(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_ITALICS];
							ci->ttb[TTB_AMSGS] = 0;
							source.Reply(BOT_KICK_AMSGS_ON_BAN, ci->ttb[TTB_AMSGS]);
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_AMSGS] = 0;
					ci->botflags.SetFlag(BS_KICK_AMSGS);
					if (ci->ttb[TTB_AMSGS])
						source.Reply(BOT_KICK_AMSGS_ON_BAN, ci->ttb[TTB_AMSGS]);
					else
						source.Reply(BOT_KICK_AMSGS_ON);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_AMSGS);
					source.Reply(BOT_KICK_AMSGS_OFF);
				}
			}
			else
				source.Reply(BOT_KICK_UNKNOWN, option.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		if (subcommand.empty())
			source.Reply(BOT_HELP_KICK);
		else if (subcommand.equals_ci("BADWORDS"))
			source.Reply(BOT_HELP_KICK_BADWORDS);
		else if (subcommand.equals_ci("BOLDS"))
			source.Reply(BOT_HELP_KICK_BOLDS);
		else if (subcommand.equals_ci("CAPS"))
			source.Reply(BOT_HELP_KICK_CAPS);
		else if (subcommand.equals_ci("COLORS"))
			source.Reply(BOT_HELP_KICK_COLORS);
		else if (subcommand.equals_ci("FLOOD"))
			source.Reply(BOT_HELP_KICK_FLOOD);
		else if (subcommand.equals_ci("REPEAT"))
			source.Reply(BOT_HELP_KICK_REPEAT);
		else if (subcommand.equals_ci("REVERSES"))
			source.Reply(BOT_HELP_KICK_REVERSES);
		else if (subcommand.equals_ci("UNDERLINES"))
			source.Reply(BOT_HELP_KICK_UNDERLINES);
		else if (subcommand.equals_ci("ITALICS"))
			source.Reply(BOT_HELP_KICK_ITALICS);
		else if (subcommand.equals_ci("AMSGS"))
			source.Reply(BOT_HELP_KICK_AMSGS);
		else
			return false;

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "KICK", BOT_KICK_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(BOT_HELP_CMD_KICK);
	}
};

class BSKick : public Module
{
	CommandBSKick commandbskick;

 public:
	BSKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(BotServ, &commandbskick);

		ModuleManager::Attach(I_OnPrivmsg, this);
	}

	EventReturn OnPrivmsg(User *u, ChannelInfo *ci, Anope::string &msg, bool &Allow)
	{
		Anope::string m, ch;
		time_t time;

		if (u->GetExtRegular("bs_kick_lastmsg", m) && u->GetExtRegular("bs_kick_lasttime", time) && u->GetExtRegular("bs_kick_lastchan", ch))
		{
			if (time == Anope::CurTime && m == msg && ch != ci->name)
			{
				for (UChannelList::iterator it = u->chans.begin(); it != u->chans.end();)
				{
					Channel *c = (*it)->chan;
					++it;

					if (c->ci != NULL && c->ci->botflags.HasFlag(BS_KICK_AMSGS))
					{
						check_ban(c->ci, u, TTB_AMSGS);
						bot_kick(c->ci, u, BOT_REASON_AMSGS);
					}
				}

				return EVENT_CONTINUE;
			}
		}
		
		u->Extend("bs_kick_lastmsg", new ExtensibleItemRegular<Anope::string>(msg));
		u->Extend("bs_kick_lasttime", new ExtensibleItemRegular<time_t>(Anope::CurTime));
		u->Extend("bs_kick_lastchan", new ExtensibleItemRegular<Anope::string>(ci->name));

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(BSKick)
