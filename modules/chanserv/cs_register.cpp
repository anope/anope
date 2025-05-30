/* ChanServ core functions
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

class CommandCSRegister final
	: public Command
{
public:
	CommandCSRegister(Module *creator) : Command(creator, "chanserv/register", 1, 2)
	{
		this->SetDesc(_("Register a channel"));
		this->SetSyntax(_("\037channel\037 [\037description\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &chdesc = params.size() > 1 ? params[1] : "";
		unsigned maxregistered = Config->GetModule("chanserv").Get<unsigned>("maxregistered");

		User *u = source.GetUser();
		NickCore *nc = source.nc;
		Channel *c = Channel::Find(params[0]);
		ChannelInfo *ci = ChannelInfo::Find(params[0]);

		if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);
		else if (nc->HasExt("UNCONFIRMED"))
			source.Reply(_("You must confirm your account before you can register a channel."));
		else if (chan[0] == '&')
			source.Reply(_("Local channels cannot be registered."));
		else if (chan[0] != '#')
			source.Reply(CHAN_SYMBOL_REQUIRED);
		else if (!IRCD->IsChannelValid(chan))
			source.Reply(CHAN_X_INVALID, chan.c_str());
		else if (!c && u)
			source.Reply(CHAN_X_NOT_IN_USE, chan.c_str());
		else if (ci)
			source.Reply(_("Channel \002%s\002 is already registered!"), chan.c_str());
		else if (c && u && !c->HasUserStatus(u, "OP"))
			source.Reply(_("You must be a channel operator to register the channel."));
		else if (maxregistered && nc->channelcount >= maxregistered && !source.HasPriv("chanserv/no-register-limit"))
			source.Reply(nc->channelcount > maxregistered ? CHAN_EXCEEDED_CHANNEL_LIMIT : CHAN_REACHED_CHANNEL_LIMIT, maxregistered);
		else
		{
			ci = new ChannelInfo(chan);
			ci->SetFounder(nc);
			ci->desc = chdesc;

			if (c && !c->topic.empty())
			{
				ci->last_topic = c->topic;
				ci->last_topic_setter = c->topic_setter;
				ci->last_topic_time = c->topic_time;
			}
			else
				ci->last_topic_setter = source.service->nick;

			Log(LOG_COMMAND, source, this, ci);
			source.Reply(_("Channel \002%s\002 registered under your account: %s"), chan.c_str(), nc->display.c_str());

			FOREACH_MOD(OnChanRegistered, (ci));

			/* Implement new mode lock */
			if (c)
			{
				c->CheckModes();
				if (u)
					c->SetCorrectModes(u, true);
			}
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Registers a channel in the %s database. In order "
				"to use this command, you must first be a channel operator "
				"on the channel you're trying to register. "
				"The description, which is optional, is a "
				"general description of the channel's purpose. "
				"\n\n"
				"When you register a channel, you are recorded as the "
				"\"founder\" of the channel. The channel founder is allowed "
				"to change all of the channel settings for the channel; "
				"%s will also automatically give the founder "
				"channel operator privileges when they enter the channel."
			),
			source.service->nick.c_str(),
			source.service->nick.c_str());

		BotInfo *bi;
		Anope::string cmd;
		if (Command::FindCommandFromService("chanserv/access", bi, cmd))
		{
			source.Reply(" ");
			source.Reply(_(
					"See the \002%s\002 command (\002%s\032ACCESS\002) for "
					"information on giving a subset of these privileges to "
					"other channel users."
				),
				cmd.c_str(),
				bi->GetQueryCommand("generic/help").c_str());
		}

		source.Reply(" ");
		source.Reply(_(
			"NOTICE: In order to register a channel, you must have "
			"first registered your nickname."));

		return true;
	}
};


class CSRegister final
	: public Module
{
	CommandCSRegister commandcsregister;

public:
	CSRegister(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandcsregister(this)
	{
	}
};

MODULE_INIT(CSRegister)
