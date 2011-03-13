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
	CommandCSRegister() : Command("REGISTER", 2, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTEREDCHANNEL);
		this->SetDesc(_("Register a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		const Anope::string &chdesc = params[1];

		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Channel *c = findchan(chan);

		if (readonly)
			source.Reply(_("Sorry, channel registration is temporarily disabled."));
		else if (u->Account()->HasFlag(NI_UNCONFIRMED))
			source.Reply(_("You must confirm your account before you can register a channel."));
		else if (chan[0] == '&')
			source.Reply(_("Local channels cannot be registered."));
		else if (chan[0] != '#')
			source.Reply(_(CHAN_SYMBOL_REQUIRED));
		else if (!ircdproto->IsChannelValid(chan))
			source.Reply(_(CHAN_X_INVALID), chan.c_str());
		else if (ci)
			source.Reply(_("Channel \002%s\002 is already registered!"), chan.c_str());
		else if (c && !c->HasUserStatus(u, CMODE_OP))
			source.Reply(_("You must be a channel operator to register the channel."));
		else if (Config->CSMaxReg && u->Account()->channelcount >= Config->CSMaxReg && !u->Account()->HasPriv("chanserv/no-register-limit"))
			source.Reply(u->Account()->channelcount > Config->CSMaxReg ? _(CHAN_EXCEEDED_CHANNEL_LIMIT) : _(CHAN_REACHED_CHANNEL_LIMIT), Config->CSMaxReg);
		else
		{
			ci = new ChannelInfo(chan);
			ci->founder = u->Account();
			ci->desc = chdesc;

			if (c && !c->topic.empty())
			{
				ci->last_topic = c->topic;
				ci->last_topic_setter = c->topic_setter;
				ci->last_topic_time = c->topic_time;
			}
			else
				ci->last_topic_setter = Config->s_ChanServ;

			ci->bi = NULL;
			++ci->founder->channelcount;
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

				/* Mark the channel as persistant */
				if (c->HasMode(CMODE_PERM))
					ci->SetFlag(CI_PERSIST);
				/* Persist may be in def cflags, set it here */
				else if (ci->HasFlag(CI_PERSIST) && (cm = ModeManager::FindChannelModeByName(CMODE_PERM)))
					c->SetMode(NULL, CMODE_PERM);
			}

			FOREACH_MOD(I_OnChanRegistered, OnChanRegistered(ci));
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002REGISTER \037channel\037 \037description\037\002\n"
			" \n"
			"Registers a channel in the %s database.  In order\n"
			"to use this command, you must first be a channel operator\n"
			"on the channel you're trying to register.\n"
			"The description, which \002must\002 be included, is a\n"
			"general description of the channel's purpose.\n"
			" \n"
			"When you register a channel, you are recorded as the\n"
			"\"founder\" of the channel. The channel founder is allowed\n"
			"to change all of the channel settings for the channel;\n"
			"%s will also automatically give the founder\n"
			"channel-operator privileges when s/he enters the channel.\n"
			"See the \002ACCESS\002 command (\002%R%s HELP ACCESS\002) for\n"
			"information on giving a subset of these privileges to\n"
			"other channel users.\n"
			" \n"
			"NOTICE: In order to register a channel, you must have\n"
			"first registered your nickname.  If you haven't,\n"
			"\002%R%s HELP\002 for information on how to do so."),
			ChanServ->nick.c_str(), ChanServ->nick.c_str(), ChanServ->nick.c_str(), ChanServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "REGISTER", _("REGISTER \037channel\037 \037description\037"));
	}
};

class CSRegister : public Module
{
	CommandCSRegister commandcsregister;

 public:
	CSRegister(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsregister);
	}
};

MODULE_INIT(CSRegister)
