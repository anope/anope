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
	void send_bot_channels(std::vector<Anope::string> &buffers, BotInfo *bi)
	{
		Anope::string buf;
		for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
		{
			ChannelInfo *ci = it->second;

			if (ci->bi == bi)
			{
				buf += " " + ci->name + " ";
				if (buf.length() > 300)
				{
					buffers.push_back(buf);
					buf.clear();
				}
			}
		}
		if (!buf.empty())
			buffers.push_back(buf);
	}

	void CheckOptStr(Anope::string &buf, BotServFlag flag, const char *option, Flags<BotServFlag> &flags, NickCore *nc)
	{
		if (flags.HasFlag(flag))
		{
			if (!buf.empty())
				buf += ", ";
			buf += translate(nc, option);
		}
	}

 public:
	CommandBSInfo(Module *creator) : Command(creator, "botserv/info", 1, 1)
	{
		this->SetDesc(_("Allows you to see BotServ information about a channel or a bot"));
		this->SetSyntax(_("\002INFO {\037chan\037 | \037nick\037}\002"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &query = params[0];

		User *u = source.u;
		BotInfo *bi = findbot(query);
		ChannelInfo *ci;
		InfoFormatter info(u);

		if (bi)
		{
			source.Reply(_("Information for bot \002%s\002:"), bi->nick.c_str());
			info[_("Mask")] = bi->GetIdent() + "@" + bi->host;
			info[_("Real name")] = bi->realname;
			info[_("Created")] = do_strftime(bi->created);
			info[_("Options")] = bi->HasFlag(BI_PRIVATE) ? _("Private") : _("None");
			info[_("Used on")] = stringify(bi->chancount) + " channel(s)";

			std::vector<Anope::string> replies;
			info.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);

			if (u->HasPriv("botserv/administration"))
			{
				std::vector<Anope::string> buf;
				this->send_bot_channels(buf, bi);
				for (unsigned i = 0; i < buf.size(); ++i)
					source.Reply(buf[i]);
			}

		}
		else if ((ci = cs_findchan(query)))
		{
			if (!ci->AccessFor(u).HasPriv("FOUNDER") && !u->HasPriv("botserv/administration"))
			{
				source.Reply(ACCESS_DENIED);
				return;
			}

			source.Reply(CHAN_INFO_HEADER, ci->name.c_str());
			info[_("Bot nick")] = ci->bi ? ci->bi->nick : "not assigned yet";

			Anope::string enabled = translate(u, _("Enabled"));
			Anope::string disabled = translate(u, _("Disabled"));

			if (ci->botflags.HasFlag(BS_KICK_BADWORDS))
			{
				if (ci->ttb[TTB_BADWORDS])
					info[_("Bad words kicker")] = Anope::printf("%s (%d kick(s) to ban)", enabled.c_str(), ci->ttb[TTB_BADWORDS]);
				else
					info[_("Bad words kicker")] = enabled;
			}
			else
				info[_("Bad words kicker")] = disabled;

			if (ci->botflags.HasFlag(BS_KICK_BOLDS))
			{
				if (ci->ttb[TTB_BOLDS])
					info[_("Bolds kicker")] = Anope::printf("%s (%d kick(s) to ban)", enabled.c_str(), ci->ttb[TTB_BOLDS]);
				else
					info[_("Bolds kicker")] = enabled;
			}
			else
				info[_("Bolds kicker")] = disabled;

			if (ci->botflags.HasFlag(BS_KICK_CAPS))
			{
				if (ci->ttb[TTB_CAPS])
					info[_("Caps kicker")] = Anope::printf(_("%s (%d kick(s) to ban; minimum %d/%d%%"), enabled.c_str(), ci->ttb[TTB_CAPS], ci->capsmin, ci->capspercent);
				else
					info[_("Caps kicker")] = Anope::printf(_("%s (minimum %d/%d%%)"), enabled.c_str(), ci->capsmin, ci->capspercent);
			}
			else
				info[_("Caps kicker")] = disabled;

			if (ci->botflags.HasFlag(BS_KICK_COLORS))
			{
				if (ci->ttb[TTB_COLORS])
					info[_("Colors kicker")] = Anope::printf(_("%s (%d kick(s) to ban)"), enabled.c_str(), ci->ttb[TTB_COLORS]);
				else
					info[_("Colors kicker")] = enabled;
			}
			else
				info[_("Colors kicker")] = disabled;

			if (ci->botflags.HasFlag(BS_KICK_FLOOD))
			{
				if (ci->ttb[TTB_FLOOD])
					info[_("Flood kicker")] = Anope::printf(_("%s (%d kick(s) to ban; %d lines in %ds"), enabled.c_str(), ci->ttb[TTB_FLOOD], ci->floodlines, ci->floodsecs);
				else
					info[_("Flood kicker")] = Anope::printf(_("%s (%d lines in %ds)"), enabled.c_str(), ci->floodlines, ci->floodsecs);
			}
			else
				info[_("Flood kicker")] = disabled;

			if (ci->botflags.HasFlag(BS_KICK_REPEAT))
			{
				if (ci->ttb[TTB_REPEAT])
					info[_("Repeat kicker")] = Anope::printf(_("%s (%d kick(s) to ban; %d times)"), enabled.c_str(), ci->ttb[TTB_REPEAT], ci->repeattimes);
				else
					info[_("Repeat kicker")] = Anope::printf(_("%s (%d times)"), enabled.c_str(), ci->repeattimes);
			}
			else
				info[_("Repeat kicker")] = disabled;

			if (ci->botflags.HasFlag(BS_KICK_REVERSES))
			{
				if (ci->ttb[TTB_REVERSES])
					info[_("Reverses kicker")] = Anope::printf(_("%s (%d kick(s) to ban)"), enabled.c_str(), ci->ttb[TTB_REVERSES]);
				else
					info[_("Reverses kicker")] = enabled;
			}
			else
				info[_("Reverses kicker")] = disabled;

			if (ci->botflags.HasFlag(BS_KICK_UNDERLINES))
			{
				if (ci->ttb[TTB_UNDERLINES])
					info[_("Underlines kicker")] = Anope::printf(_("%s (%d kick(s) to ban)"), enabled.c_str(), ci->ttb[TTB_UNDERLINES]);
				else
					info[_("Underlines kicker")] = enabled;
			}
			else
				info[_("Underlines kicker")] = disabled;

                        if (ci->botflags.HasFlag(BS_KICK_ITALICS))
			{
				if (ci->ttb[TTB_ITALICS])
					info[_("Italics kicker")] = Anope::printf(_("%s (%d kick(s) to ban)"), enabled.c_str(), ci->ttb[TTB_ITALICS]);
				else
					info[_("Italics kicker")] = enabled;
			}
			else
				info[_("Italics kicker")] = disabled;

			if (ci->botflags.HasFlag(BS_KICK_AMSGS))
			{
				if (ci->ttb[TTB_AMSGS])
					info[_("AMSG kicker")] = Anope::printf(_("%s (%d kick(s) to ban)"), enabled.c_str(), ci->ttb[TTB_AMSGS]);
				else
					info[_("AMSG kicker")] = enabled;
			}
			else
				info[_("AMSG kicker")] = disabled;

			Anope::string flags;
			CheckOptStr(flags, BS_DONTKICKOPS, _("Ops protection"), ci->botflags, u->Account());
			CheckOptStr(flags, BS_DONTKICKVOICES, _("Voices protection"), ci->botflags, u->Account());
			CheckOptStr(flags, BS_FANTASY, _("Fantasy"), ci->botflags, u->Account());
			CheckOptStr(flags, BS_GREET, _("Greet"), ci->botflags, u->Account());
			CheckOptStr(flags, BS_NOBOT, _("No bot"), ci->botflags, u->Account());

			info[_("Options")] = flags.empty() ? _("None") : flags;

			std::vector<Anope::string> replies;
			info.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
		else
			source.Reply(_("\002%s\002 is not a valid bot or registered channel."), query.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to see %s information about a channel or a bot.\n"
				"If the parameter is a channel, then you'll get information\n"
				"such as enabled kickers. If the parameter is a nick,\n"
				"you'll get information about a bot, such as creation\n"
				"time or number of channels it is on."), source.owner->nick.c_str());
		return true;
	}
};

class BSInfo : public Module
{
	CommandBSInfo commandbsinfo;

 public:
	BSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandbsinfo(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(BSInfo)
