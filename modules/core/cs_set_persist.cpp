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

class CommandCSSetPersist : public Command
{
 public:
	CommandCSSetPersist(const Anope::string &cpermission = "") : Command("PERSIST", 2, 2, cpermission)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetPersist");

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
					ChanServ->Assign(NULL, ci);
					if (!ci->c->FindUser(ChanServ))
						ChanServ->Join(ci->c);
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

			source.Reply(CHAN_SET_PERSIST_ON, ci->name.c_str());
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
				if (!cm && Config->s_BotServ.empty() && ci->bi)
					/* Unassign bot */
					ChanServ->UnAssign(NULL, ci);
			}

			source.Reply(CHAN_SET_PERSIST_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "PERSIST");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_PERSIST, "SET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET PERSIST", CHAN_SET_PERSIST_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_SET_PERSIST);
	}
};

class CommandCSSASetPersist : public CommandCSSetPersist
{
 public:
	CommandCSSASetPersist() : CommandCSSetPersist("chanserv/saset/persist")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_PERSIST, "SASET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET PERSIST", CHAN_SASET_PERSIST_SYNTAX);
	}
};

class CSSetPersist : public Module
{
	CommandCSSetPersist commandcssetpeace;
	CommandCSSASetPersist commandcssasetpeace;

 public:
	CSSetPersist(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssetpeace);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetpeace);
	}

	~CSSetPersist()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetpeace);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetpeace);
	}
};

MODULE_INIT(CSSetPersist)
