/* BotServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#include "module.h"
#include "modules/bs_info.h"

class CommandBSInfo : public Command
{
	EventHandlers<Event::ServiceBotEvent> &onbotinfo;

 public:
	CommandBSInfo(Module *creator, EventHandlers<Event::ServiceBotEvent> &event) : Command(creator, "botserv/info", 1, 1), onbotinfo(event)
	{
		this->SetSyntax(_("{\037channel\037 | \037nickname\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &query = params[0];

		ServiceBot *bi = ServiceBot::Find(query, true);
		ChanServ::Channel *ci = ChanServ::Find(query);
		InfoFormatter info(source.nc);

		if (bi)
		{
			source.Reply(_("Information for bot \002%s\002:"), bi->nick.c_str());
			info[_("Mask")] = bi->GetIdent() + "@" + bi->host;
			info[_("Real name")] = bi->realname;
			info[_("Created")] = Anope::strftime(bi->bi->GetCreated(), source.GetAccount());
			info[_("Options")] = bi->bi->GetOperOnly() ? _("Private") : _("None");
			info[_("Used on")] = stringify(bi->GetChannelCount()) + " channel(s)";

			this->onbotinfo(&Event::ServiceBotEvent::OnServiceBot, source, bi, ci, info);

			std::vector<Anope::string> replies;
			info.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);

			if (source.HasPriv("botserv/administration"))
			{
				Anope::string buf;
				for (ChanServ::Channel *ci2 : bi->GetChannels())
					buf += " " + ci2->GetName();
				source.Reply(buf);
			}

		}
		else if (ci)
		{
			if (!source.AccessFor(ci).HasPriv("INFO") && !source.HasPriv("botserv/administration"))
			{
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "INFO", ci->GetName());
				return;
			}

			source.Reply(_("Information for channel \002{0}\002:"), ci->GetName());
			info[_("Bot nick")] = ci->GetBot() ? ci->GetBot()->nick : _("not assigned yet");

			Anope::string enabled = Language::Translate(source.nc, _("Enabled"));
			Anope::string disabled = Language::Translate(source.nc, _("Disabled"));

			this->onbotinfo(&Event::ServiceBotEvent::OnServiceBot, source, bi, ci, info);

			std::vector<Anope::string> replies;
			info.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
		else
			source.Reply(_("\002{0}\002 is not a valid bot or registered channel."), query);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Allows you to see {0} information about a channel or a bot."
		               " If the parameter is a channel, then you'll get information such as enabled kickers."
		               " If the parameter is a bot nickname, you'll get information about a bot, such as creation time and number of channels it is on."),
		               source.service->nick);
		return true;
	}

	const Anope::string GetDesc(CommandSource &source) const override
	{
		return Anope::printf(Language::Translate(source.GetAccount(), _("Allows you to see %s information about a channel or a bot")), source.service->nick.c_str());
	}
};

class BSInfo : public Module
{
	CommandBSInfo commandbsinfo;
	EventHandlers<Event::ServiceBotEvent> onbotinfo;

 public:
	BSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandbsinfo(this, onbotinfo), onbotinfo(this, "OnServiceBot")
	{

	}
};

MODULE_INIT(BSInfo)
