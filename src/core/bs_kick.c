/* BotServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

int do_kickcmd(User * u);

class CommandBSKick : public Command
{
 public:
	CommandBSKick() : Command("KICK", 3, 6)
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		ci::string option = params[1];
		ci::string value = params[2];
		const char *ttb = params.size() > 3 ? params[3].c_str() : NULL;

		ChannelInfo *ci = cs_findchan(chan);

		if (readonly)
			notice_lang(s_BotServ, u, BOT_KICK_DISABLED);
		else if (!chan || option.empty() || value.empty())
			syntax_error(s_BotServ, u, "KICK", BOT_KICK_SYNTAX);
		else if (value != "ON" && value != "OFF")
			syntax_error(s_BotServ, u, "KICK", BOT_KICK_SYNTAX);
		else if (!check_access(u, ci, CA_SET) && !u->nc->HasPriv("botserv/administration"))
			notice_lang(s_BotServ, u, ACCESS_DENIED);
		else if (!ci->bi)
			notice_help(s_BotServ, u, BOT_NOT_ASSIGNED);
		else {
			if (option == "BADWORDS") {
				if (value == "ON") {
					if (ttb) {
						errno = 0;
						ci->ttb[TTB_BADWORDS] =
							strtol(ttb, NULL, 10);
						/* Only error if errno returns ERANGE or EINVAL or we are less then 0 - TSL */
						if (errno == ERANGE || errno == EINVAL
							|| ci->ttb[TTB_BADWORDS] < 0) {
							/* leaving the debug behind since we might want to know what these are */
							if (debug) {
								alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_BADWORDS]);
							}
							/* reset the value back to 0 - TSL */
							ci->ttb[TTB_BADWORDS] = 0;
							notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
							return MOD_CONT;
						}
					} else {
						ci->ttb[TTB_BADWORDS] = 0;
					}
					ci->botflags.SetFlag(BS_KICK_BADWORDS);
					if (ci->ttb[TTB_BADWORDS])
						notice_lang(s_BotServ, u, BOT_KICK_BADWORDS_ON_BAN,
									ci->ttb[TTB_BADWORDS]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_BADWORDS_ON);
				} else {
					ci->botflags.UnsetFlag(BS_KICK_BADWORDS);
					notice_lang(s_BotServ, u, BOT_KICK_BADWORDS_OFF);
				}
			} else if (option == "BOLDS") {
				if (value == "ON") {
					if (ttb) {
						errno = 0;
						ci->ttb[TTB_BOLDS] = strtol(ttb, NULL, 10);
						if (errno == ERANGE || errno == EINVAL
							|| ci->ttb[TTB_BOLDS] < 0) {
							if (debug) {
								alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_BOLDS]);
							}
							ci->ttb[TTB_BOLDS] = 0;
							notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
							return MOD_CONT;
						}
					} else
						ci->ttb[TTB_BOLDS] = 0;
					ci->botflags.SetFlag(BS_KICK_BOLDS);
					if (ci->ttb[TTB_BOLDS])
						notice_lang(s_BotServ, u, BOT_KICK_BOLDS_ON_BAN,
									ci->ttb[TTB_BOLDS]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_BOLDS_ON);
				} else {
					ci->botflags.UnsetFlag(BS_KICK_BOLDS);
					notice_lang(s_BotServ, u, BOT_KICK_BOLDS_OFF);
				}
			} else if (option == "CAPS") {
				if (value == "ON") {
					const char *min = params.size() > 4 ? params[4].c_str() : NULL;
					const char *percent = params.size() > 5 ? params[5].c_str() : NULL;

					if (ttb) {
						errno = 0;
						ci->ttb[TTB_CAPS] = strtol(ttb, NULL, 10);
						if (errno == ERANGE || errno == EINVAL
							|| ci->ttb[TTB_CAPS] < 0) {
							if (debug) {
								alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_CAPS]);
							}
							ci->ttb[TTB_CAPS] = 0;
							notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
							return MOD_CONT;
						}
					} else
						ci->ttb[TTB_CAPS] = 0;

					if (!min)
						ci->capsmin = 10;
					else
						ci->capsmin = atol(min);
					if (ci->capsmin < 1)
						ci->capsmin = 10;

					if (!percent)
						ci->capspercent = 25;
					else
						ci->capspercent = atol(percent);
					if (ci->capspercent < 1 || ci->capspercent > 100)
						ci->capspercent = 25;

					ci->botflags.SetFlag(BS_KICK_CAPS);
					if (ci->ttb[TTB_CAPS])
						notice_lang(s_BotServ, u, BOT_KICK_CAPS_ON_BAN,
									ci->capsmin, ci->capspercent,
									ci->ttb[TTB_CAPS]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_CAPS_ON,
									ci->capsmin, ci->capspercent);
				} else {
					ci->botflags.UnsetFlag(BS_KICK_CAPS);
					notice_lang(s_BotServ, u, BOT_KICK_CAPS_OFF);
				}
			} else if (option == "COLORS") {
				if (value == "ON") {
					if (ttb) {
						errno = 0;
						ci->ttb[TTB_COLORS] = strtol(ttb, NULL, 10);
						if (errno == ERANGE || errno == EINVAL
							|| ci->ttb[TTB_COLORS] < 0) {
							if (debug) {
								alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_COLORS]);
							}
							ci->ttb[TTB_COLORS] = 0;
							notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
							return MOD_CONT;
						}
					} else
						ci->ttb[TTB_COLORS] = 0;
					ci->botflags.SetFlag(BS_KICK_COLORS);
					if (ci->ttb[TTB_COLORS])
						notice_lang(s_BotServ, u, BOT_KICK_COLORS_ON_BAN,
									ci->ttb[TTB_COLORS]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_COLORS_ON);
				} else {
					ci->botflags.UnsetFlag(BS_KICK_COLORS);
					notice_lang(s_BotServ, u, BOT_KICK_COLORS_OFF);
				}
			} else if (option == "FLOOD") {
				if (value == "ON") {
					const char *lines = params.size() > 4 ? params[4].c_str() : NULL;
					const char *secs = params.size() > 5 ? params[5].c_str() : NULL;

					if (ttb) {
						errno = 0;
						ci->ttb[TTB_FLOOD] = strtol(ttb, NULL, 10);
						if (errno == ERANGE || errno == EINVAL
							|| ci->ttb[TTB_FLOOD] < 0) {
							if (debug) {
								alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_FLOOD]);
							}
							ci->ttb[TTB_FLOOD] = 0;
							notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
							return MOD_CONT;
						}
					} else
						ci->ttb[TTB_FLOOD] = 0;

					if (!lines)
						ci->floodlines = 6;
					else
						ci->floodlines = atol(lines);
					if (ci->floodlines < 2)
						ci->floodlines = 6;

					if (!secs)
						ci->floodsecs = 10;
					else
						ci->floodsecs = atol(secs);
					if (ci->floodsecs < 1 || ci->floodsecs > BSKeepData)
						ci->floodsecs = 10;

					ci->botflags.SetFlag(BS_KICK_FLOOD);
					if (ci->ttb[TTB_FLOOD])
						notice_lang(s_BotServ, u, BOT_KICK_FLOOD_ON_BAN,
									ci->floodlines, ci->floodsecs,
									ci->ttb[TTB_FLOOD]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_FLOOD_ON,
									ci->floodlines, ci->floodsecs);
				} else {
					ci->botflags.UnsetFlag(BS_KICK_FLOOD);
					notice_lang(s_BotServ, u, BOT_KICK_FLOOD_OFF);
				}
			} else if (option == "REPEAT") {
				if (value == "ON") {
					const char *times = params.size() > 4 ? params[4].c_str() : NULL;

					if (ttb) {
						errno = 0;
						ci->ttb[TTB_REPEAT] = strtol(ttb, NULL, 10);
						if (errno == ERANGE || errno == EINVAL
							|| ci->ttb[TTB_REPEAT] < 0) {
							if (debug) {
								alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_REPEAT]);
							}
							ci->ttb[TTB_REPEAT] = 0;
							notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
							return MOD_CONT;
						}
					} else
						ci->ttb[TTB_REPEAT] = 0;

					if (!times)
						ci->repeattimes = 3;
					else
						ci->repeattimes = atol(times);
					if (ci->repeattimes < 2)
						ci->repeattimes = 3;

					ci->botflags.SetFlag(BS_KICK_REPEAT);
					if (ci->ttb[TTB_REPEAT])
						notice_lang(s_BotServ, u, BOT_KICK_REPEAT_ON_BAN,
									ci->repeattimes, ci->ttb[TTB_REPEAT]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_REPEAT_ON,
									ci->repeattimes);
				} else {
					ci->botflags.UnsetFlag(BS_KICK_REPEAT);
					notice_lang(s_BotServ, u, BOT_KICK_REPEAT_OFF);
				}
			} else if (option == "REVERSES") {
				if (value == "ON") {
					if (ttb) {
						errno = 0;
						ci->ttb[TTB_REVERSES] =
							strtol(ttb, NULL, 10);
						if (errno == ERANGE || errno == EINVAL
							|| ci->ttb[TTB_REVERSES] < 0) {
							if (debug) {
								alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_REVERSES]);
							}
							ci->ttb[TTB_REVERSES] = 0;
							notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
							return MOD_CONT;
						}
					} else
						ci->ttb[TTB_REVERSES] = 0;
					ci->botflags.SetFlag(BS_KICK_REVERSES);
					if (ci->ttb[TTB_REVERSES])
						notice_lang(s_BotServ, u, BOT_KICK_REVERSES_ON_BAN,
									ci->ttb[TTB_REVERSES]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_REVERSES_ON);
				} else {
					ci->botflags.UnsetFlag(BS_KICK_REVERSES);
					notice_lang(s_BotServ, u, BOT_KICK_REVERSES_OFF);
				}
			} else if (option == "UNDERLINES") {
				if (value == "ON") {
					if (ttb) {
						errno = 0;
						ci->ttb[TTB_UNDERLINES] =
							strtol(ttb, NULL, 10);
						if (errno == ERANGE || errno == EINVAL
							|| ci->ttb[TTB_UNDERLINES] < 0) {
							if (debug) {
								alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_UNDERLINES]);
							}
							ci->ttb[TTB_UNDERLINES] = 0;
							notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
							return MOD_CONT;
						}
					} else
						ci->ttb[TTB_UNDERLINES] = 0;
					ci->botflags.SetFlag(BS_KICK_UNDERLINES);
					if (ci->ttb[TTB_UNDERLINES])
						notice_lang(s_BotServ, u, BOT_KICK_UNDERLINES_ON_BAN,
									ci->ttb[TTB_UNDERLINES]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_UNDERLINES_ON);
				} else {
					ci->botflags.UnsetFlag(BS_KICK_UNDERLINES);
					notice_lang(s_BotServ, u, BOT_KICK_UNDERLINES_OFF);
				}
			} else
				notice_help(s_BotServ, u, BOT_KICK_UNKNOWN, option.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		if (subcommand.empty())
			notice_help(s_BotServ, u, BOT_HELP_KICK);
		else if (subcommand == "BADWORDS")
			notice_help(s_BotServ, u, BOT_HELP_KICK_BADWORDS);
		else if (subcommand == "BOLDS")
			notice_help(s_BotServ, u, BOT_HELP_KICK_BOLDS);
		else if (subcommand == "CAPS")
			notice_help(s_BotServ, u, BOT_HELP_KICK_CAPS);
		else if (subcommand == "COLORS")
			notice_help(s_BotServ, u, BOT_HELP_KICK_COLORS);
		else if (subcommand == "FLOOD")
			notice_help(s_BotServ, u, BOT_HELP_KICK_FLOOD);
		else if (subcommand == "REPEAT")
			notice_help(s_BotServ, u, BOT_HELP_KICK_REPEAT);
		else if (subcommand == "REVERSES")
			notice_help(s_BotServ, u, BOT_HELP_KICK_REVERSES);
		else if (subcommand == "UNDERLINES")
			notice_help(s_BotServ, u, BOT_HELP_KICK_UNDERLINES);
		else
			return false;

		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_BotServ, u, "KICK", BOT_KICK_SYNTAX);
	}
};

class BSKick : public Module
{
 public:
	BSKick(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(BOTSERV, new CommandBSKick());

		ModuleManager::Attach(I_OnBotServHelp, this);
	}
	void OnBotServHelp(User *u)
	{
		notice_lang(s_BotServ, u, BOT_HELP_CMD_KICK);
	}
};

MODULE_INIT(BSKick)
