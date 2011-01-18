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

class CommandBSInfo : public Command
{
 private:
	void send_bot_channels(CommandSource &source, BotInfo *bi)
	{
		Anope::string buf;
		for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
		{
			ChannelInfo *ci = it->second;

			if (ci->bi == bi)
			{
				if (buf.length() + ci->name.length() > 300)
				{
					source.Reply("%s", buf.c_str());
					buf.clear();
				}
				buf += " " + ci->name + " ";
			}
		}

		if (!buf.empty())
			source.Reply("%s", buf.c_str());
		return;
	}
 public:
	CommandBSInfo() : Command("INFO", 1, 1)
	{
		this->SetFlag(CFLAG_STRIP_CHANNEL);
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &query = params[0];

		bool need_comma = false;
		char buf[BUFSIZE], *end;

		User *u = source.u;
		BotInfo *bi = findbot(query);
		ChannelInfo *ci;
		if (bi)
		{
			source.Reply(BOT_INFO_BOT_HEADER, bi->nick.c_str());
			source.Reply(BOT_INFO_BOT_MASK, bi->GetIdent().c_str(), bi->host.c_str());
			source.Reply(BOT_INFO_BOT_REALNAME, bi->realname.c_str());
			source.Reply(BOT_INFO_BOT_CREATED, do_strftime(bi->created).c_str());
			source.Reply(BOT_INFO_BOT_OPTIONS, GetString(u, (bi->HasFlag(BI_PRIVATE) ? NICK_INFO_OPT_PRIVATE : NICK_INFO_OPT_NONE)).c_str());
			source.Reply(BOT_INFO_BOT_USAGE, bi->chancount);

			if (u->Account()->HasPriv("botserv/administration"))
				this->send_bot_channels(source, bi);
		}
		else if ((ci = cs_findchan(query)))
		{
			if (!check_access(u, ci, CA_FOUNDER) && !u->Account()->HasPriv("botserv/administration"))
			{
				source.Reply(ACCESS_DENIED);
				return MOD_CONT;
			}

			source.Reply(CHAN_INFO_HEADER, ci->name.c_str());
			if (ci->bi)
				source.Reply(BOT_INFO_CHAN_BOT, ci->bi->nick.c_str());
			else
				source.Reply(BOT_INFO_CHAN_BOT_NONE);

			if (ci->botflags.HasFlag(BS_KICK_BADWORDS))
			{
				if (ci->ttb[TTB_BADWORDS])
					source.Reply(BOT_INFO_CHAN_KICK_BADWORDS_BAN, GetString(u, BOT_INFO_ACTIVE).c_str(), ci->ttb[TTB_BADWORDS]);
				else
					source.Reply(BOT_INFO_CHAN_KICK_BADWORDS, GetString(u, BOT_INFO_ACTIVE).c_str());
			}
			else
				source.Reply(BOT_INFO_CHAN_KICK_BADWORDS, GetString(u, BOT_INFO_INACTIVE).c_str());
			if (ci->botflags.HasFlag(BS_KICK_BOLDS))
			{
				if (ci->ttb[TTB_BOLDS])
					source.Reply(BOT_INFO_CHAN_KICK_BOLDS_BAN, GetString(u, BOT_INFO_ACTIVE).c_str(), ci->ttb[TTB_BOLDS]);
				else
					source.Reply(BOT_INFO_CHAN_KICK_BOLDS, GetString(u, BOT_INFO_ACTIVE).c_str());
			}
			else
				source.Reply(BOT_INFO_CHAN_KICK_BOLDS, GetString(u, BOT_INFO_INACTIVE).c_str());
			if (ci->botflags.HasFlag(BS_KICK_CAPS))
			{
				if (ci->ttb[TTB_CAPS])
					source.Reply(BOT_INFO_CHAN_KICK_CAPS_BAN, GetString(u, BOT_INFO_ACTIVE).c_str(), ci->ttb[TTB_CAPS], ci->capsmin, ci->capspercent);
				else
					source.Reply(BOT_INFO_CHAN_KICK_CAPS_ON, GetString(u, BOT_INFO_ACTIVE).c_str(), ci->capsmin, ci->capspercent);
			}
			else
				source.Reply(BOT_INFO_CHAN_KICK_CAPS_OFF, GetString(u, BOT_INFO_INACTIVE).c_str());
			if (ci->botflags.HasFlag(BS_KICK_COLORS))
			{
				if (ci->ttb[TTB_COLORS])
					source.Reply(BOT_INFO_CHAN_KICK_COLORS_BAN, GetString(u, BOT_INFO_ACTIVE).c_str(), ci->ttb[TTB_COLORS]);
				else
					source.Reply(BOT_INFO_CHAN_KICK_COLORS, GetString(u, BOT_INFO_ACTIVE).c_str());
			}
			else
				source.Reply(BOT_INFO_CHAN_KICK_COLORS, GetString(u, BOT_INFO_INACTIVE).c_str());
			if (ci->botflags.HasFlag(BS_KICK_FLOOD))
			{
				if (ci->ttb[TTB_FLOOD])
					source.Reply(BOT_INFO_CHAN_KICK_FLOOD_BAN, GetString(u, BOT_INFO_ACTIVE).c_str(), ci->ttb[TTB_FLOOD], ci->floodlines, ci->floodsecs);
				else
					source.Reply(BOT_INFO_CHAN_KICK_FLOOD_ON, GetString(u, BOT_INFO_ACTIVE).c_str(), ci->floodlines, ci->floodsecs);
			}
			else
				source.Reply(BOT_INFO_CHAN_KICK_FLOOD_OFF, GetString(u, BOT_INFO_INACTIVE).c_str());
			if (ci->botflags.HasFlag(BS_KICK_REPEAT))
			{
				if (ci->ttb[TTB_REPEAT])
					source.Reply(BOT_INFO_CHAN_KICK_REPEAT_BAN, GetString(u, BOT_INFO_ACTIVE).c_str(), ci->ttb[TTB_REPEAT], ci->repeattimes);
				else
					source.Reply(BOT_INFO_CHAN_KICK_REPEAT_ON, GetString(u, BOT_INFO_ACTIVE).c_str(), ci->repeattimes);
			}
			else
				source.Reply(BOT_INFO_CHAN_KICK_REPEAT_OFF, GetString(u, BOT_INFO_INACTIVE).c_str());
			if (ci->botflags.HasFlag(BS_KICK_REVERSES))
			{
				if (ci->ttb[TTB_REVERSES])
					source.Reply(BOT_INFO_CHAN_KICK_REVERSES_BAN, GetString(u, BOT_INFO_ACTIVE).c_str(), ci->ttb[TTB_REVERSES]);
				else
					source.Reply(BOT_INFO_CHAN_KICK_REVERSES, GetString(u, BOT_INFO_ACTIVE).c_str());
			}
			else
				source.Reply(BOT_INFO_CHAN_KICK_REVERSES, GetString(u, BOT_INFO_INACTIVE).c_str());
			if (ci->botflags.HasFlag(BS_KICK_UNDERLINES))
			{
				if (ci->ttb[TTB_UNDERLINES])
					source.Reply(BOT_INFO_CHAN_KICK_UNDERLINES_BAN, GetString(u, BOT_INFO_ACTIVE).c_str(), ci->ttb[TTB_UNDERLINES]);
				else
					source.Reply(BOT_INFO_CHAN_KICK_UNDERLINES, GetString(u, BOT_INFO_ACTIVE).c_str());
			}
			else
				source.Reply(BOT_INFO_CHAN_KICK_UNDERLINES, GetString(u, BOT_INFO_INACTIVE).c_str());
                        if (ci->botflags.HasFlag(BS_KICK_ITALICS))
			{
				if (ci->ttb[TTB_ITALICS])
					source.Reply(BOT_INFO_CHAN_KICK_ITALICS_BAN, GetString(u, BOT_INFO_ACTIVE).c_str(), ci->ttb[TTB_ITALICS]);
				else
					source.Reply(BOT_INFO_CHAN_KICK_ITALICS, GetString(u, BOT_INFO_ACTIVE).c_str());
			}
			else
				source.Reply(BOT_INFO_CHAN_KICK_ITALICS, GetString(u, BOT_INFO_INACTIVE).c_str());
			if (ci->botflags.HasFlag(BS_KICK_AMSGS))
			{
				if (ci->ttb[TTB_AMSGS])
					source.Reply(BOT_INFO_CHAN_KICK_AMSGS_BAN, GetString(u, BOT_INFO_ACTIVE).c_str(), ci->ttb[TTB_AMSGS]);
				else
					source.Reply(BOT_INFO_CHAN_KICK_AMSGS, GetString(u, BOT_INFO_ACTIVE).c_str());
			}
			else
				source.Reply(BOT_INFO_CHAN_KICK_AMSGS, GetString(u, BOT_INFO_INACTIVE).c_str());

			if (ci->botflags.HasFlag(BS_MSG_PRIVMSG))
				source.Reply(BOT_INFO_CHAN_MSG, "PRIVMSG");
			else if (ci->botflags.HasFlag(BS_MSG_NOTICE))
				source.Reply(BOT_INFO_CHAN_MSG, "NOTICE");
			else if (ci->botflags.HasFlag(BS_MSG_NOTICEOPS))
				source.Reply(BOT_INFO_CHAN_MSG, "NOTICEOPS");
			
			end = buf;
			*end = 0;
			if (ci->botflags.HasFlag(BS_DONTKICKOPS))
			{
				end += snprintf(end, sizeof(buf) - (end - buf), "%s", GetString(u, BOT_INFO_OPT_DONTKICKOPS).c_str());
				need_comma = true;
			}
			if (ci->botflags.HasFlag(BS_DONTKICKVOICES))
			{
				end += snprintf(end, sizeof(buf) - (end - buf), "%s%s", need_comma ? ", " : "", GetString(u, BOT_INFO_OPT_DONTKICKVOICES).c_str());
				need_comma = true;
			}
			if (ci->botflags.HasFlag(BS_FANTASY))
			{
				end += snprintf(end, sizeof(buf) - (end - buf), "%s%s", need_comma ? ", " : "", GetString(u, BOT_INFO_OPT_FANTASY).c_str());
				need_comma = true;
			}
			if (ci->botflags.HasFlag(BS_GREET))
			{
				end += snprintf(end, sizeof(buf) - (end - buf), "%s%s", need_comma ? ", " : "", GetString(u, BOT_INFO_OPT_GREET).c_str());
				need_comma = true;
			}
			if (ci->botflags.HasFlag(BS_NOBOT))
			{
				end += snprintf(end, sizeof(buf) - (end - buf), "%s%s", need_comma ? ", " : "", GetString(u, BOT_INFO_OPT_NOBOT).c_str());
				need_comma = true;
			}
			if (ci->botflags.HasFlag(BS_SYMBIOSIS))
			{
				end += snprintf(end, sizeof(buf) - (end - buf), "%s%s", need_comma ? ", " : "", GetString(u, BOT_INFO_OPT_SYMBIOSIS).c_str());
				need_comma = true;
			}
			source.Reply(BOT_INFO_CHAN_OPTIONS, *buf ? buf : GetString(u, BOT_INFO_OPT_NONE).c_str());
		}
		else
			source.Reply(BOT_INFO_NOT_FOUND, query.c_str());
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(BOT_HELP_INFO);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "INFO", BOT_INFO_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(BOT_HELP_CMD_INFO);
	}
};

class BSInfo : public Module
{
	CommandBSInfo commandbsinfo;

 public:
	BSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(BotServ, &commandbsinfo);
	}
};

MODULE_INIT(BSInfo)
