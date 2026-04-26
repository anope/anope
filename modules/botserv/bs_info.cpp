// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"

class CommandBSInfo final
	: public Command
{
private:
	static void send_bot_channels(std::vector<Anope::string> &buffers, const BotInfo *bi)
	{
		Anope::string buf;
		for (const auto &[_, ci] : *RegisteredChannelList)
		{
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

public:
	CommandBSInfo(Module *creator) : Command(creator, "botserv/info", 1, 1)
	{
		this->SetSyntax(_("{\037channel\037 | \037nickname\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &query = params[0];

		BotInfo *bi = BotInfo::Find(query, true);
		ChannelInfo *ci = ChannelInfo::Find(query);
		InfoFormatter info(source.nc);

		if (bi)
		{
			source.Reply(_("Information about bot \002%s\002:"), bi->nick.c_str());
			info[_("Mask")] = bi->GetIdent() + "@" + bi->host;
			info[_("Real name")] = bi->realname;
			info[_("Created")] = Anope::strftime(bi->created, source.GetAccount());
			info[_("Options")] = bi->oper_only ? _("Private") : _("None");
			info[_("Used on")] = Anope::Format(source.Translate(bi->GetChannelCount(), N_("%u channel", "%u channels")), bi->GetChannelCount());

			FOREACH_MOD(OnBotInfo, (source, bi, ci, info));
			info.SendTo(source);

			if (source.HasPriv("botserv/administration"))
			{
				std::vector<Anope::string> buf;
				this->send_bot_channels(buf, bi);
				for (const auto &line : buf)
					source.Reply(line);
			}

		}
		else if (ci)
		{
			if (!source.AccessFor(ci).HasPriv("INFO") && !source.HasPriv("botserv/administration"))
			{
				source.Reply(ACCESS_DENIED);
				return;
			}

			source.Reply(CHAN_INFO_HEADER, ci->name.c_str());
			info[_("Bot nick")] = ci->bi ? ci->bi->nick : _("not assigned yet");

			Anope::string enabled = source.Translate(_("Enabled"));
			Anope::string disabled = source.Translate(_("Disabled"));

			FOREACH_MOD(OnBotInfo, (source, bi, ci, info));
			info.SendTo(source);
		}
		else
			source.Reply(_("\002%s\002 is not a valid bot or registered channel."), query.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Allows you to see %s information about a channel or a bot. "
			"If the parameter is a channel, then you'll get information "
			"such as enabled kickers. If the parameter is a nick, "
			"you'll get information about a bot, such as creation "
			"time or number of channels it is on."
		), source.service->nick.c_str());

		ExampleWrapper()
			.AddEntry("#example", _(
				"Shows information about the bot assigned to \035#example\035 and its kickers and "
				"options."
			))
			.AddEntry("ChanServ", _(
				"Shows information about the \035ChanServ\035 bot."
			))
			.SendTo(source);

		return true;
	}

	Anope::string GetDesc(CommandSource &source) const override
	{
		return Anope::Format(source.Translate(_("Allows you to see %s information about a channel or a bot")), source.service->nick.c_str());
	}
};

class BSInfo final
	: public Module
{
	CommandBSInfo commandbsinfo;

public:
	BSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandbsinfo(this)
	{

	}
};

MODULE_INIT(BSInfo)
