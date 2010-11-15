/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
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
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Anope::string desc = params[1];
		Channel *c = findchan(chan);
		ChannelInfo *ci;
		ChannelMode *cm;

		if (readonly)
		{
			u->SendMessage(ChanServ, CHAN_REGISTER_DISABLED);
			return MOD_CONT;
		}

		if (chan[0] == '&')
			u->SendMessage(ChanServ, CHAN_REGISTER_NOT_LOCAL);
		else if (chan[0] != '#')
			u->SendMessage(ChanServ, CHAN_SYMBOL_REQUIRED);
		else if (!ircdproto->IsChannelValid(chan))
			u->SendMessage(ChanServ, CHAN_X_INVALID, chan.c_str());
		else if ((ci = cs_findchan(chan)))
			u->SendMessage(ChanServ, CHAN_ALREADY_REGISTERED, chan.c_str());
		else if (c && !c->HasUserStatus(u, CMODE_OP))
			u->SendMessage(ChanServ, CHAN_MUST_BE_CHANOP);
		else if (Config->CSMaxReg && u->Account()->channelcount >= Config->CSMaxReg && !u->Account()->HasPriv("chanserv/no-register-limit"))
			u->SendMessage(ChanServ, u->Account()->channelcount > Config->CSMaxReg ? CHAN_EXCEEDED_CHANNEL_LIMIT : CHAN_REACHED_CHANNEL_LIMIT, Config->CSMaxReg);
		else
		{
			ci = new ChannelInfo(chan);
			ci->founder = u->Account();
			ci->desc = desc;

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
			u->SendMessage(ChanServ, CHAN_REGISTERED, chan.c_str(), u->nick.c_str());

			/* Implement new mode lock */
			if (c)
			{
				check_modes(c);

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

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_REGISTER, Config->s_ChanServ.c_str());
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "REGISTER", CHAN_REGISTER_SYNTAX);
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
