/* BotServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

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
			info[_("Used on")] = Anope::printf(Language::Translate(source.nc, bi->GetChannelCount(), N_("%u channel", "%u channels")), bi->GetChannelCount());

			FOREACH_MOD(OnBotInfo, (source, bi, ci, info));

			std::vector<Anope::string> replies;
			info.Process(replies);

			for (const auto &reply : replies)
				source.Reply(reply);

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

			Anope::string enabled = Language::Translate(source.nc, _("Enabled"));
			Anope::string disabled = Language::Translate(source.nc, _("Disabled"));

			FOREACH_MOD(OnBotInfo, (source, bi, ci, info));

			std::vector<Anope::string> replies;
			info.Process(replies);

			for (const auto &reply : replies)
				source.Reply(reply);
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
		return true;
	}

	const Anope::string GetDesc(CommandSource &source) const override
	{
		return Anope::printf(Language::Translate(source.GetAccount(), _("Allows you to see %s information about a channel or a bot")), source.service->nick.c_str());
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
