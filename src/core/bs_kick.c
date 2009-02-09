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

void myBotServHelp(User * u);
class CommandBSKick : public Command
{
 public:
	CommandBSKick() : Command("KICK", 3, 4)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *option = params[1].c_str();
		const char *value = params[2].c_str();
		const char *ttb = params[3].c_str();

		ChannelInfo *ci;

		if (readonly)
			notice_lang(s_BotServ, u, BOT_KICK_DISABLED);
		else if (!chan || !option || !value)
			syntax_error(s_BotServ, u, "KICK", BOT_KICK_SYNTAX);
		else if (stricmp(value, "ON") && stricmp(value, "OFF"))
			syntax_error(s_BotServ, u, "KICK", BOT_KICK_SYNTAX);
		else if (!(ci = cs_findchan(chan)))
			notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
		else if (ci->flags & CI_VERBOTEN)
			notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
		else if (!is_services_admin(u) && !check_access(u, ci, CA_SET))
			notice_lang(s_BotServ, u, ACCESS_DENIED);
		else if (!ci->bi)
			notice_help(s_BotServ, u, BOT_NOT_ASSIGNED);
		else {
			if (!stricmp(option, "BADWORDS")) {
				if (!stricmp(value, "ON")) {
					if (ttb) {
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
					ci->botflags |= BS_KICK_BADWORDS;
					if (ci->ttb[TTB_BADWORDS])
						notice_lang(s_BotServ, u, BOT_KICK_BADWORDS_ON_BAN,
									ci->ttb[TTB_BADWORDS]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_BADWORDS_ON);
				} else {
					ci->botflags &= ~BS_KICK_BADWORDS;
					notice_lang(s_BotServ, u, BOT_KICK_BADWORDS_OFF);
				}
			} else if (!stricmp(option, "BOLDS")) {
				if (!stricmp(value, "ON")) {
					if (ttb) {
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
					ci->botflags |= BS_KICK_BOLDS;
					if (ci->ttb[TTB_BOLDS])
						notice_lang(s_BotServ, u, BOT_KICK_BOLDS_ON_BAN,
									ci->ttb[TTB_BOLDS]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_BOLDS_ON);
				} else {
					ci->botflags &= ~BS_KICK_BOLDS;
					notice_lang(s_BotServ, u, BOT_KICK_BOLDS_OFF);
				}
			} else if (!stricmp(option, "CAPS")) {
				if (!stricmp(value, "ON")) {
					char *min = strtok(NULL, " ");
					char *percent = strtok(NULL, " ");

					if (ttb) {
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

					ci->botflags |= BS_KICK_CAPS;
					if (ci->ttb[TTB_CAPS])
						notice_lang(s_BotServ, u, BOT_KICK_CAPS_ON_BAN,
									ci->capsmin, ci->capspercent,
									ci->ttb[TTB_CAPS]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_CAPS_ON,
									ci->capsmin, ci->capspercent);
				} else {
					ci->botflags &= ~BS_KICK_CAPS;
					notice_lang(s_BotServ, u, BOT_KICK_CAPS_OFF);
				}
			} else if (!stricmp(option, "COLORS")) {
				if (!stricmp(value, "ON")) {
					if (ttb) {
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
					ci->botflags |= BS_KICK_COLORS;
					if (ci->ttb[TTB_COLORS])
						notice_lang(s_BotServ, u, BOT_KICK_COLORS_ON_BAN,
									ci->ttb[TTB_COLORS]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_COLORS_ON);
				} else {
					ci->botflags &= ~BS_KICK_COLORS;
					notice_lang(s_BotServ, u, BOT_KICK_COLORS_OFF);
				}
			} else if (!stricmp(option, "FLOOD")) {
				if (!stricmp(value, "ON")) {
					char *lines = strtok(NULL, " ");
					char *secs = strtok(NULL, " ");

					if (ttb) {
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

					ci->botflags |= BS_KICK_FLOOD;
					if (ci->ttb[TTB_FLOOD])
						notice_lang(s_BotServ, u, BOT_KICK_FLOOD_ON_BAN,
									ci->floodlines, ci->floodsecs,
									ci->ttb[TTB_FLOOD]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_FLOOD_ON,
									ci->floodlines, ci->floodsecs);
				} else {
					ci->botflags &= ~BS_KICK_FLOOD;
					notice_lang(s_BotServ, u, BOT_KICK_FLOOD_OFF);
				}
			} else if (!stricmp(option, "REPEAT")) {
				if (!stricmp(value, "ON")) {
					char *times = strtok(NULL, " ");

					if (ttb) {
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

					ci->botflags |= BS_KICK_REPEAT;
					if (ci->ttb[TTB_REPEAT])
						notice_lang(s_BotServ, u, BOT_KICK_REPEAT_ON_BAN,
									ci->repeattimes, ci->ttb[TTB_REPEAT]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_REPEAT_ON,
									ci->repeattimes);
				} else {
					ci->botflags &= ~BS_KICK_REPEAT;
					notice_lang(s_BotServ, u, BOT_KICK_REPEAT_OFF);
				}
			} else if (!stricmp(option, "REVERSES")) {
				if (!stricmp(value, "ON")) {
					if (ttb) {
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
					ci->botflags |= BS_KICK_REVERSES;
					if (ci->ttb[TTB_REVERSES])
						notice_lang(s_BotServ, u, BOT_KICK_REVERSES_ON_BAN,
									ci->ttb[TTB_REVERSES]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_REVERSES_ON);
				} else {
					ci->botflags &= ~BS_KICK_REVERSES;
					notice_lang(s_BotServ, u, BOT_KICK_REVERSES_OFF);
				}
			} else if (!stricmp(option, "UNDERLINES")) {
				if (!stricmp(value, "ON")) {
					if (ttb) {
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
					ci->botflags |= BS_KICK_UNDERLINES;
					if (ci->ttb[TTB_UNDERLINES])
						notice_lang(s_BotServ, u, BOT_KICK_UNDERLINES_ON_BAN,
									ci->ttb[TTB_UNDERLINES]);
					else
						notice_lang(s_BotServ, u, BOT_KICK_UNDERLINES_ON);
				} else {
					ci->botflags &= ~BS_KICK_UNDERLINES;
					notice_lang(s_BotServ, u, BOT_KICK_UNDERLINES_OFF);
				}
			} else
				notice_help(s_BotServ, u, BOT_KICK_UNKNOWN, option);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		// XXX: This is not a good way to handle it, we need to accept params for HELP.
		notice_help(s_BotServ, u, BOT_HELP_KICK);
		notice_help(s_BotServ, u, BOT_HELP_KICK_BADWORDS);
		notice_help(s_BotServ, u, BOT_HELP_KICK_BOLDS);
		notice_help(s_BotServ, u, BOT_HELP_KICK_CAPS);
		notice_help(s_BotServ, u, BOT_HELP_KICK_COLORS);
		notice_help(s_BotServ, u, BOT_HELP_KICK_FLOOD);
		notice_help(s_BotServ, u, BOT_HELP_KICK_REPEAT);
		notice_help(s_BotServ, u, BOT_HELP_KICK_REVERSES);
		notice_help(s_BotServ, u, BOT_HELP_KICK_UNDERLINES);
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
		this->AddCommand(BOTSERV, new CommandBSKick(), MOD_UNIQUE);

		this->SetBotHelp(myBotServHelp);
	}
};


/**
 * Add the help response to Anopes /bs help output.
 * @param u The user who is requesting help
 **/
void myBotServHelp(User * u)
{
	notice_lang(s_BotServ, u, BOT_HELP_CMD_KICK);
}


MODULE_INIT("bs_kick", BSKick)
