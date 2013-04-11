/* ChanServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandCSRegister : public Command
{
 public:
	CommandCSRegister(Module *creator) : Command(creator, "chanserv/register", 1, 2)
	{
		this->SetDesc(_("Register a channel"));
		this->SetSyntax(_("\037channel\037 [\037description\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &chan = params[0];
		const Anope::string &chdesc = params.size() > 1 ? params[1] : "";

		User *u = source.GetUser();
		NickCore *nc = source.nc;
		Channel *c = Channel::Find(params[0]);
		ChannelInfo *ci = ChannelInfo::Find(params[0]);

		if (Anope::ReadOnly)
			source.Reply(_("Sorry, channel registration is temporarily disabled."));
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
		else if (c && !c->HasUserStatus(u, "OP"))
			source.Reply(_("You must be a channel operator to register the channel."));
		else if (Config->CSMaxReg && nc->channelcount >= Config->CSMaxReg && !source.HasPriv("chanserv/no-register-limit"))
			source.Reply(nc->channelcount > Config->CSMaxReg ? CHAN_EXCEEDED_CHANNEL_LIMIT : CHAN_REACHED_CHANNEL_LIMIT, Config->CSMaxReg);
		else
		{
			ci = new ChannelInfo(chan);
			ci->SetFounder(nc);
			ci->desc = chdesc;

			for (std::list<std::pair<Anope::string, Anope::string> >::const_iterator it = ModeManager::ModeLockOn.begin(), it_end = ModeManager::ModeLockOn.end(); it != it_end; ++it)
				ci->SetMLock(ModeManager::FindChannelModeByName(it->first), true, it->second, source.GetNick());
			for (std::list<Anope::string>::const_iterator it = ModeManager::ModeLockOff.begin(), it_end = ModeManager::ModeLockOff.end(); it != it_end; ++it)
				ci->SetMLock(ModeManager::FindChannelModeByName(*it), false, "", source.GetNick());

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

			/* Implement new mode lock */
			if (c)
			{
				c->CheckModes();

				ChannelMode *cm;
				if (u && u->FindChannel(c) != NULL)
				{
					/* On most ircds you do not receive the admin/owner mode till its registered */
					if ((cm = ModeManager::FindChannelModeByName("OWNER")))
						c->SetMode(NULL, cm, u->GetUID());
					else if ((cm = ModeManager::FindChannelModeByName("PROTECT")))
						c->RemoveMode(NULL, cm, u->GetUID());
				}

				/* Mark the channel as persistent */
				if (c->HasMode("PERM"))
					ci->Extend("PERSIST");
				/* Persist may be in def cflags, set it here */
				else if (ci->HasExt("PERSIST") && (cm = ModeManager::FindChannelModeByName("PERM")))
					c->SetMode(NULL, "PERM");
			}

			FOREACH_MOD(I_OnChanRegistered, OnChanRegistered(ci));
		}
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Registers a channel in the %s database.  In order\n"
			"to use this command, you must first be a channel operator\n"
			"on the channel you're trying to register.\n"
			"The description, which is optional, is a\n"
			"general description of the channel's purpose.\n"
			" \n"
			"When you register a channel, you are recorded as the\n"
			"\"founder\" of the channel. The channel founder is allowed\n"
			"to change all of the channel settings for the channel;\n"
			"%s will also automatically give the founder\n"
			"channel-operator privileges when s/he enters the channel.\n"
			"See the \002ACCESS\002 command (\002%s%s HELP ACCESS\002) for\n"
			"information on giving a subset of these privileges to\n"
			"other channel users.\n"
			" \n"
			"NOTICE: In order to register a channel, you must have\n"
			"first registered your nickname.  If you haven't,\n"
			"\002%s%s HELP\002 for information on how to do so."),
			source.service->nick.c_str(), source.service->nick.c_str(), Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str());
		return true;
	}
};


class CSRegister : public Module
{
	CommandCSRegister commandcsregister;

 public:
	CSRegister(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandcsregister(this)
	{
	}
};

MODULE_INIT(CSRegister)
