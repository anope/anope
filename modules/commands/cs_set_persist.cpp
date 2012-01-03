/* ChanServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandCSSetPersist : public Command
{
 public:
	CommandCSSetPersist(Module *creator, const Anope::string &cname = "chanserv/set/persist") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Set the channel as permanent"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (source.permission.empty() && !ci->AccessFor(u).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_PERM);

		if (params[1].equals_ci("ON"))
		{
			if (!ci->HasFlag(CI_PERSIST))
			{
				ci->SetFlag(CI_PERSIST);
				if (ci->c)
					ci->c->SetFlag(CH_PERSIST);

				/* Channel doesn't exist, create it */
				if (!ci->c)
				{
					Channel *c = new Channel(ci->name);
					if (ci->bi)
						ci->bi->Join(c);
				}

				/* No botserv bot, no channel mode */
				/* Give them ChanServ
				 * Yes, this works fine with no Config->s_BotServ
				 */
				if (!ci->bi && !cm)
				{
					BotInfo *bi = findbot(Config->ChanServ);
					if (!bi)
					{
						source.Reply(_("ChanServ is required to enable persist on this network."));
						return;
					}
					bi->Assign(NULL, ci);
					if (!ci->c->FindUser(bi))
						bi->Join(ci->c);
				}

				/* Set the perm mode */
				if (cm)
				{
					if (ci->c && !ci->c->HasMode(CMODE_PERM))
						ci->c->SetMode(NULL, cm);
					/* Add it to the channels mlock */
					ci->SetMLock(cm, true);
				}
			}

			source.Reply(_("Channel \002%s\002 is now persistent."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			if (ci->HasFlag(CI_PERSIST))
			{
				ci->UnsetFlag(CI_PERSIST);
				if (ci->c)
					ci->c->UnsetFlag(CH_PERSIST);

				/* Unset perm mode */
				if (cm)
				{
					if (ci->c && ci->c->HasMode(CMODE_PERM))
						ci->c->RemoveMode(NULL, cm);
					/* Remove from mlock */
					ci->RemoveMLock(cm);
				}

				/* No channel mode, no BotServ, but using ChanServ as the botserv bot
				 * which was assigned when persist was set on
				 */
				if (!cm && Config->BotServ.empty() && ci->bi)
				{
					BotInfo *bi = findbot(Config->ChanServ);
					if (!bi)
					{
						source.Reply(_("ChanServ is required to enable persist on this network."));
						return;
					}
					/* Unassign bot */
					bi->UnAssign(NULL, ci);
				}
			}

			source.Reply(_("Channel \002%s\002 is no longer persistent."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "PERSIST");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the persistent channel setting.\n"
				"When persistent is set, the service bot will remain\n"
				"in the channel when it has emptied of users.\n"
				" \n"
				"If your IRCd does not a permanent (persistent) channel\n"
				"mode you must have a service bot in your channel to\n"
				"set persist on, and it can not be unassigned while persist\n"
				"is on.\n"
				" \n"
				"If this network does not have BotServ enabled and does\n"
				"not have a permanent channel mode, ChanServ will\n"
				"join your channel when you set persist on (and leave when\n"
				"it has been set off).\n"
				" \n"
				"If your IRCd has a permanent (persistent) channel mode\n"
				"and is is set or unset (for any reason, including MLOCK),\n"
				"persist is automatically set and unset for the channel aswell.\n"
				"Additionally, services will set or unset this mode when you\n"
				"set persist on or off."));
		return true;
	}
};

class CommandCSSASetPersist : public CommandCSSetPersist
{
 public:
	CommandCSSASetPersist(Module *creator) : CommandCSSetPersist(creator, "chanserv/saset/persist")
	{
	}
};

class CSSetPersist : public Module
{
	CommandCSSetPersist commandcssetpeace;
	CommandCSSASetPersist commandcssasetpeace;

 public:
	CSSetPersist(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetpeace(this), commandcssasetpeace(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSSetPersist)
