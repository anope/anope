/* ChanServ core functions
 *
 * (C) 2003-2011 Anope Team
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		const Anope::string &chdesc = params.size() > 1 ? params[1] : "";

		User *u = source.u;
		Channel *c = findchan(params[0]);
		ChannelInfo *ci = cs_findchan(params[0]);

		if (readonly)
			source.Reply(_("Sorry, channel registration is temporarily disabled."));
		else if (u->Account()->HasFlag(NI_UNCONFIRMED))
			source.Reply(_("You must confirm your account before you can register a channel."));
		else if (chan[0] == '&')
			source.Reply(_("Local channels cannot be registered."));
		else if (chan[0] != '#')
			source.Reply(CHAN_SYMBOL_REQUIRED);
		else if (!ircdproto->IsChannelValid(chan))
			source.Reply(CHAN_X_INVALID, chan.c_str());
		else if (ci)
			source.Reply(_("Channel \002%s\002 is already registered!"), chan.c_str());
		else if (c && !c->HasUserStatus(u, CMODE_OP))
			source.Reply(_("You must be a channel operator to register the channel."));
		else if (Config->CSMaxReg && u->Account()->channelcount >= Config->CSMaxReg && !u->HasPriv("chanserv/no-register-limit"))
			source.Reply(u->Account()->channelcount > Config->CSMaxReg ? CHAN_EXCEEDED_CHANNEL_LIMIT : _(CHAN_REACHED_CHANNEL_LIMIT), Config->CSMaxReg);
		else
		{
			ci = new ChannelInfo(chan);
			ci->SetFounder(u->Account());
			if (!chdesc.empty())
				ci->desc = chdesc;

			ci->mode_locks = def_mode_locks;
			for (ChannelInfo::ModeList::iterator it = ci->mode_locks.begin(), it_end = ci->mode_locks.end(); it != it_end; ++it)
			{
				it->second.setter = u->nick;
				it->second.ci = ci;
			}

			if (c && !c->topic.empty())
			{
				ci->last_topic = c->topic;
				ci->last_topic_setter = c->topic_setter;
				ci->last_topic_time = c->topic_time;
			}
			else
				ci->last_topic_setter = source.owner->nick;

			Log(LOG_COMMAND, u, this, ci);
			source.Reply(_("Channel \002%s\002 registered under your nickname: %s"), chan.c_str(), u->nick.c_str());

			/* Implement new mode lock */
			if (c)
			{
				check_modes(c);

				ChannelMode *cm;
				if (u->FindChannel(c) != NULL)
				{
					/* On most ircds you do not receive the admin/owner mode till its registered */
					if ((cm = ModeManager::FindChannelModeByName(CMODE_OWNER)))
						c->SetMode(NULL, cm, u->nick);
					else if ((cm = ModeManager::FindChannelModeByName(CMODE_PROTECT)))
						c->RemoveMode(NULL, cm, u->nick);
				}

				/* Mark the channel as persistent */
				if (c->HasMode(CMODE_PERM))
					ci->SetFlag(CI_PERSIST);
				/* Persist may be in def cflags, set it here */
				else if (ci->HasFlag(CI_PERSIST) && (cm = ModeManager::FindChannelModeByName(CMODE_PERM)))
					c->SetMode(NULL, CMODE_PERM);
			}

			FOREACH_MOD(I_OnChanRegistered, OnChanRegistered(ci));
		}
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
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
			source.owner->nick.c_str(), source.owner->nick.c_str(), Config->UseStrictPrivMsgString.c_str(), source.owner->nick.c_str(), Config->UseStrictPrivMsgString.c_str(), source.owner->nick.c_str());
		return true;
	}
};


class CSRegister : public Module
{
	CommandCSRegister commandcsregister;

 public:
	CSRegister(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcsregister(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::Attach(I_OnDelChan, this);
	}

	void OnDelChan(ChannelInfo *ci)
	{
		if (ci->c && ci->c->HasMode(CMODE_REGISTERED))
			ci->c->RemoveMode(NULL, CMODE_REGISTERED, "", false);
		if (ci->c && ci->HasFlag(CI_PERSIST))
			ci->c->RemoveMode(NULL, CMODE_PERM, "", false);
	}
};

MODULE_INIT(CSRegister)
