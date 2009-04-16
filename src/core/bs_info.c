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

class CommandBSInfo : public Command
{
 private:
	void send_bot_channels(User * u, BotInfo * bi)
	{
		int i;
		ChannelInfo *ci;
		char buf[307], *end;

		*buf = 0;
		end = buf;

		for (i = 0; i < 256; i++) {
			for (ci = chanlists[i]; ci; ci = ci->next) {
				if (ci->bi == bi) {
					if (strlen(buf) + strlen(ci->name) > 300) {
						u->SendMessage(s_BotServ, "%s", buf);
						*buf = 0;
						end = buf;
					}
					end +=
						snprintf(end, sizeof(buf) - (end - buf), " %s ",
								 ci->name);
				}
			}
		}

		if (*buf)
			u->SendMessage(s_BotServ, "%s", buf);
		return;
	}
 public:
	CommandBSInfo() : Command("INFO", 1, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		BotInfo *bi;
		ChannelInfo *ci;
		const char *query = params[0].c_str();

		int need_comma = 0;
		char buf[BUFSIZE], *end;
		const char *commastr = getstring(u, COMMA_SPACE);

		if ((bi = findbot(query)))
		{
			struct tm *tm;

			notice_lang(s_BotServ, u, BOT_INFO_BOT_HEADER, bi->nick);
			notice_lang(s_BotServ, u, BOT_INFO_BOT_MASK, bi->user, bi->host);
			notice_lang(s_BotServ, u, BOT_INFO_BOT_REALNAME, bi->real);
			tm = localtime(&bi->created);
			strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
			notice_lang(s_BotServ, u, BOT_INFO_BOT_CREATED, buf);
			notice_lang(s_BotServ, u, BOT_INFO_BOT_OPTIONS,
						getstring(u,
									(bi->
									 flags & BI_PRIVATE) ? BOT_INFO_OPT_PRIVATE :
									BOT_INFO_OPT_NONE));
			notice_lang(s_BotServ, u, BOT_INFO_BOT_USAGE, bi->chancount);

			if (u->nc->HasPriv("botserv/administration"))
				this->send_bot_channels(u, bi);
		}
		else if ((ci = cs_findchan(query)))
		{
			if (!is_founder(u, ci) && !u->nc->HasPriv("botserv/administration"))
			{
				notice_lang(s_BotServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}

			notice_lang(s_BotServ, u, BOT_INFO_CHAN_HEADER, ci->name);
			if (ci->bi)
				notice_lang(s_BotServ, u, BOT_INFO_CHAN_BOT, ci->bi->nick);
			else
				notice_lang(s_BotServ, u, BOT_INFO_CHAN_BOT_NONE);

			if (ci->botflags & BS_KICK_BADWORDS) {
				if (ci->ttb[TTB_BADWORDS])
					notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BADWORDS_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_BADWORDS]);
				else
					notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BADWORDS,
								getstring(u, BOT_INFO_ACTIVE));
			} else
				notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BADWORDS,
							getstring(u, BOT_INFO_INACTIVE));
			if (ci->botflags & BS_KICK_BOLDS) {
				if (ci->ttb[TTB_BOLDS])
					notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BOLDS_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_BOLDS]);
				else
					notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BOLDS,
								getstring(u, BOT_INFO_ACTIVE));
			} else
				notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BOLDS,
							getstring(u, BOT_INFO_INACTIVE));
			if (ci->botflags & BS_KICK_CAPS) {
				if (ci->ttb[TTB_CAPS])
					notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_CAPS_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_CAPS], ci->capsmin,
								ci->capspercent);
				else
					notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_CAPS_ON,
								getstring(u, BOT_INFO_ACTIVE), ci->capsmin,
								ci->capspercent);
			} else
				notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_CAPS_OFF,
							getstring(u, BOT_INFO_INACTIVE));
			if (ci->botflags & BS_KICK_COLORS) {
				if (ci->ttb[TTB_COLORS])
					notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_COLORS_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_COLORS]);
				else
					notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_COLORS,
								getstring(u, BOT_INFO_ACTIVE));
			} else
				notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_COLORS,
							getstring(u, BOT_INFO_INACTIVE));
			if (ci->botflags & BS_KICK_FLOOD) {
				if (ci->ttb[TTB_FLOOD])
					notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_FLOOD_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_FLOOD], ci->floodlines,
								ci->floodsecs);
				else
					notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_FLOOD_ON,
								getstring(u, BOT_INFO_ACTIVE),
								ci->floodlines, ci->floodsecs);
			} else
				notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_FLOOD_OFF,
							getstring(u, BOT_INFO_INACTIVE));
			if (ci->botflags & BS_KICK_REPEAT) {
				if (ci->ttb[TTB_REPEAT])
					notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REPEAT_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_REPEAT], ci->repeattimes);
				else
					notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REPEAT_ON,
								getstring(u, BOT_INFO_ACTIVE),
								ci->repeattimes);
			} else
				notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REPEAT_OFF,
							getstring(u, BOT_INFO_INACTIVE));
			if (ci->botflags & BS_KICK_REVERSES) {
				if (ci->ttb[TTB_REVERSES])
					notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REVERSES_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_REVERSES]);
				else
					notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REVERSES,
								getstring(u, BOT_INFO_ACTIVE));
			} else
				notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REVERSES,
							getstring(u, BOT_INFO_INACTIVE));
			if (ci->botflags & BS_KICK_UNDERLINES) {
				if (ci->ttb[TTB_UNDERLINES])
					notice_lang(s_BotServ, u,
								BOT_INFO_CHAN_KICK_UNDERLINES_BAN,
								getstring(u, BOT_INFO_ACTIVE),
								ci->ttb[TTB_UNDERLINES]);
				else
					notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_UNDERLINES,
								getstring(u, BOT_INFO_ACTIVE));
			} else
				notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_UNDERLINES,
							getstring(u, BOT_INFO_INACTIVE));

			end = buf;
			*end = 0;
			if (ci->botflags & BS_DONTKICKOPS) {
				end += snprintf(end, sizeof(buf) - (end - buf), "%s",
								getstring(u, BOT_INFO_OPT_DONTKICKOPS));
				need_comma = 1;
			}
			if (ci->botflags & BS_DONTKICKVOICES) {
				end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
								need_comma ? commastr : "",
								getstring(u, BOT_INFO_OPT_DONTKICKVOICES));
				need_comma = 1;
			}
			if (ci->botflags & BS_FANTASY) {
				end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
								need_comma ? commastr : "",
								getstring(u, BOT_INFO_OPT_FANTASY));
				need_comma = 1;
			}
			if (ci->botflags & BS_GREET) {
				end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
								need_comma ? commastr : "",
								getstring(u, BOT_INFO_OPT_GREET));
				need_comma = 1;
			}
			if (ci->botflags & BS_NOBOT) {
				end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
								need_comma ? commastr : "",
								getstring(u, BOT_INFO_OPT_NOBOT));
				need_comma = 1;
			}
			if (ci->botflags & BS_SYMBIOSIS) {
				end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
								need_comma ? commastr : "",
								getstring(u, BOT_INFO_OPT_SYMBIOSIS));
				need_comma = 1;
			}
			notice_lang(s_BotServ, u, BOT_INFO_CHAN_OPTIONS,
						*buf ? buf : getstring(u, BOT_INFO_OPT_NONE));

		} else
			notice_lang(s_BotServ, u, BOT_INFO_NOT_FOUND, query);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_BotServ, u, BOT_HELP_INFO);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_BotServ, u, "INFO", BOT_INFO_SYNTAX);
	}
};

class BSInfo : public Module
{
 public:
	BSInfo(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(BOTSERV, new CommandBSInfo(), MOD_UNIQUE);
	}
	void BotServHelp(User *u)
	{
		notice_lang(s_BotServ, u, BOT_HELP_CMD_INFO);
	}
};

MODULE_INIT("bs_info", BSInfo)
