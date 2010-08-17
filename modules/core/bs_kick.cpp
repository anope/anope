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

class CommandBSKick : public Command
{
 public:
	CommandBSKick() : Command("KICK", 3, 6)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Anope::string option = params[1];
		Anope::string value = params[2];
		Anope::string ttb = params.size() > 3 ? params[3] : "";

		ChannelInfo *ci = cs_findchan(chan);

		if (readonly)
			notice_lang(Config->s_BotServ, u, BOT_KICK_DISABLED);
		else if (chan.empty() || option.empty() || value.empty())
			syntax_error(Config->s_BotServ, u, "KICK", BOT_KICK_SYNTAX);
		else if (!value.equals_ci("ON") && !value.equals_ci("OFF"))
			syntax_error(Config->s_BotServ, u, "KICK", BOT_KICK_SYNTAX);
		else if (!check_access(u, ci, CA_SET) && !u->Account()->HasPriv("botserv/administration"))
			notice_lang(Config->s_BotServ, u, ACCESS_DENIED);
		else if (!ci->bi)
			notice_help(Config->s_BotServ, u, BOT_NOT_ASSIGNED);
		else
		{
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
							Alog(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_BADWORDS];
							/* reset the value back to 0 - TSL */
							ci->ttb[TTB_BADWORDS] = 0;
							notice_lang(Config->s_BotServ, u, BOT_KICK_BAD_TTB, ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_BADWORDS] = 0;
					ci->botflags.SetFlag(BS_KICK_BADWORDS);
					if (ci->ttb[TTB_BADWORDS])
						notice_lang(Config->s_BotServ, u, BOT_KICK_BADWORDS_ON_BAN, ci->ttb[TTB_BADWORDS]);
					else
						notice_lang(Config->s_BotServ, u, BOT_KICK_BADWORDS_ON);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_BADWORDS);
					notice_lang(Config->s_BotServ, u, BOT_KICK_BADWORDS_OFF);
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
							Alog(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_BOLDS];
							ci->ttb[TTB_BOLDS] = 0;
							notice_lang(Config->s_BotServ, u, BOT_KICK_BAD_TTB, ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_BOLDS] = 0;
					ci->botflags.SetFlag(BS_KICK_BOLDS);
					if (ci->ttb[TTB_BOLDS])
						notice_lang(Config->s_BotServ, u, BOT_KICK_BOLDS_ON_BAN, ci->ttb[TTB_BOLDS]);
					else
						notice_lang(Config->s_BotServ, u, BOT_KICK_BOLDS_ON);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_BOLDS);
					notice_lang(Config->s_BotServ, u, BOT_KICK_BOLDS_OFF);
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
							Alog(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_CAPS];
							ci->ttb[TTB_CAPS] = 0;
							notice_lang(Config->s_BotServ, u, BOT_KICK_BAD_TTB, ttb.c_str());
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
						notice_lang(Config->s_BotServ, u, BOT_KICK_CAPS_ON_BAN, ci->capsmin, ci->capspercent, ci->ttb[TTB_CAPS]);
					else
						notice_lang(Config->s_BotServ, u, BOT_KICK_CAPS_ON, ci->capsmin, ci->capspercent);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_CAPS);
					notice_lang(Config->s_BotServ, u, BOT_KICK_CAPS_OFF);
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
							Alog(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_COLORS];
							ci->ttb[TTB_COLORS] = 0;
							notice_lang(Config->s_BotServ, u, BOT_KICK_BAD_TTB, ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_COLORS] = 0;
					ci->botflags.SetFlag(BS_KICK_COLORS);
					if (ci->ttb[TTB_COLORS])
						notice_lang(Config->s_BotServ, u, BOT_KICK_COLORS_ON_BAN, ci->ttb[TTB_COLORS]);
					else
						notice_lang(Config->s_BotServ, u, BOT_KICK_COLORS_ON);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_COLORS);
					notice_lang(Config->s_BotServ, u, BOT_KICK_COLORS_OFF);
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
							Alog(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_FLOOD];
							ci->ttb[TTB_FLOOD] = 0;
							notice_lang(Config->s_BotServ, u, BOT_KICK_BAD_TTB, ttb.c_str());
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
						notice_lang(Config->s_BotServ, u, BOT_KICK_FLOOD_ON_BAN, ci->floodlines, ci->floodsecs, ci->ttb[TTB_FLOOD]);
					else
						notice_lang(Config->s_BotServ, u, BOT_KICK_FLOOD_ON, ci->floodlines, ci->floodsecs);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_FLOOD);
					notice_lang(Config->s_BotServ, u, BOT_KICK_FLOOD_OFF);
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
							Alog(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_REPEAT];
							ci->ttb[TTB_REPEAT] = 0;
							notice_lang(Config->s_BotServ, u, BOT_KICK_BAD_TTB, ttb.c_str());
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
						notice_lang(Config->s_BotServ, u, BOT_KICK_REPEAT_ON_BAN, ci->repeattimes, ci->ttb[TTB_REPEAT]);
					else
						notice_lang(Config->s_BotServ, u, BOT_KICK_REPEAT_ON, ci->repeattimes);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_REPEAT);
					notice_lang(Config->s_BotServ, u, BOT_KICK_REPEAT_OFF);
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
							Alog(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_REVERSES];
							ci->ttb[TTB_REVERSES] = 0;
							notice_lang(Config->s_BotServ, u, BOT_KICK_BAD_TTB, ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_REVERSES] = 0;
					ci->botflags.SetFlag(BS_KICK_REVERSES);
					if (ci->ttb[TTB_REVERSES])
						notice_lang(Config->s_BotServ, u, BOT_KICK_REVERSES_ON_BAN, ci->ttb[TTB_REVERSES]);
					else
						notice_lang(Config->s_BotServ, u, BOT_KICK_REVERSES_ON);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_REVERSES);
					notice_lang(Config->s_BotServ, u, BOT_KICK_REVERSES_OFF);
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
							Alog(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_UNDERLINES];
							ci->ttb[TTB_UNDERLINES] = 0;
							notice_lang(Config->s_BotServ, u, BOT_KICK_BAD_TTB, ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_UNDERLINES] = 0;
					ci->botflags.SetFlag(BS_KICK_UNDERLINES);
					if (ci->ttb[TTB_UNDERLINES])
						notice_lang(Config->s_BotServ, u, BOT_KICK_UNDERLINES_ON_BAN, ci->ttb[TTB_UNDERLINES]);
					else
						notice_lang(Config->s_BotServ, u, BOT_KICK_UNDERLINES_ON);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_UNDERLINES);
					notice_lang(Config->s_BotServ, u, BOT_KICK_UNDERLINES_OFF);
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
							Alog(LOG_DEBUG) << "remainder of ttb " << error << " ttb " << ci->ttb[TTB_ITALICS];
							ci->ttb[TTB_ITALICS] = 0;
							notice_lang(Config->s_BotServ, u, BOT_KICK_BAD_TTB, ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_ITALICS] = 0;
					ci->botflags.SetFlag(BS_KICK_ITALICS);
					if (ci->ttb[TTB_ITALICS])
						notice_lang(Config->s_BotServ, u, BOT_KICK_ITALICS_ON_BAN, ci->ttb[TTB_ITALICS]);
					else
						notice_lang(Config->s_BotServ, u, BOT_KICK_ITALICS_ON);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_ITALICS);
					notice_lang(Config->s_BotServ, u, BOT_KICK_ITALICS_OFF);
				}
			}
			else
				notice_help(Config->s_BotServ, u, BOT_KICK_UNKNOWN, option.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		if (subcommand.empty())
			notice_help(Config->s_BotServ, u, BOT_HELP_KICK);
		else if (subcommand.equals_ci("BADWORDS"))
			notice_help(Config->s_BotServ, u, BOT_HELP_KICK_BADWORDS);
		else if (subcommand.equals_ci("BOLDS"))
			notice_help(Config->s_BotServ, u, BOT_HELP_KICK_BOLDS);
		else if (subcommand.equals_ci("CAPS"))
			notice_help(Config->s_BotServ, u, BOT_HELP_KICK_CAPS);
		else if (subcommand.equals_ci("COLORS"))
			notice_help(Config->s_BotServ, u, BOT_HELP_KICK_COLORS);
		else if (subcommand.equals_ci("FLOOD"))
			notice_help(Config->s_BotServ, u, BOT_HELP_KICK_FLOOD);
		else if (subcommand.equals_ci("REPEAT"))
			notice_help(Config->s_BotServ, u, BOT_HELP_KICK_REPEAT);
		else if (subcommand.equals_ci("REVERSES"))
			notice_help(Config->s_BotServ, u, BOT_HELP_KICK_REVERSES);
		else if (subcommand.equals_ci("UNDERLINES"))
			notice_help(Config->s_BotServ, u, BOT_HELP_KICK_UNDERLINES);
		else if (subcommand.equals_ci("ITALICS"))
			notice_help(Config->s_BotServ, u, BOT_HELP_KICK_ITALICS);
		else
			return false;

		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_BotServ, u, "KICK", BOT_KICK_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_BotServ, u, BOT_HELP_CMD_KICK);
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
	}
};

MODULE_INIT(BSKick)
