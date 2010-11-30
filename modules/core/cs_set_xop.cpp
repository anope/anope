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

#define CHECKLEV(lev) (ci->levels[(lev)] != ACCESS_INVALID && access->level >= ci->levels[(lev)])

class CommandCSSetXOP : public Command
{
 public:
	CommandCSSetXOP(const Anope::string &cpermission = "") : Command("XOP", 2, 2, cpermission)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (!FindModule("cs_xop"))
		{
			source.Reply(CHAN_XOP_NOT_AVAILABLE, "XOP");
			return MOD_CONT;
		}

		if (!ci)
			throw CoreException("NULL ci in CommandCSSetXOP");

		if (params[1].equals_ci("ON"))
		{
			if (!ci->HasFlag(CI_XOP))
			{
				ChanAccess *access;

				for (unsigned i = ci->GetAccessCount(); i > 0; --i)
				{
					access = ci->GetAccess(i - 1);

					/* This will probably cause wrong levels to be set, but hey,
					 * it's better than losing it altogether.
					 */
					if (access->level == ACCESS_QOP)
						access->level = ACCESS_QOP;
					else if (CHECKLEV(CA_AKICK) || CHECKLEV(CA_SET))
						access->level = ACCESS_SOP;
					else if (CHECKLEV(CA_AUTOOP) || CHECKLEV(CA_OPDEOP) || CHECKLEV(CA_OPDEOPME))
						access->level = ACCESS_AOP;
					else if (ModeManager::FindChannelModeByName(CMODE_HALFOP) && (CHECKLEV(CA_AUTOHALFOP) || CHECKLEV(CA_HALFOP) || CHECKLEV(CA_HALFOPME)))
						access->level = ACCESS_HOP;
					else if (CHECKLEV(CA_AUTOVOICE) || CHECKLEV(CA_VOICE) || CHECKLEV(CA_VOICEME))
						access->level = ACCESS_VOP;
					else
						ci->EraseAccess(i - 1);
				}

				reset_levels(ci);
				ci->SetFlag(CI_XOP);
			}

			Log(LOG_COMMAND, u, this, ci) << "to enable XOP";
			source.Reply(CHAN_SET_XOP_ON, ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_XOP);

			Log(LOG_COMMAND, u, this, ci) << "to disable XOP";
			source.Reply(CHAN_SET_XOP_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "XOP");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_XOP, "SET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET XOP", CHAN_SET_XOP_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_SET_XOP);
	}
};

class CommandCSSASetXOP : public CommandCSSetXOP
{
 public:
	CommandCSSASetXOP() : CommandCSSetXOP("chanserv/saset/xop")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_XOP, "SASET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET XOP", CHAN_SASET_XOP_SYNTAX);
	}
};

class CSSetXOP : public Module
{
	CommandCSSetXOP commandcssetxop;
	CommandCSSASetXOP commandcssasetxop;

 public:
	CSSetXOP(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssetxop);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetxop);
	}

	~CSSetXOP()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetxop);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetxop);
	}
};

MODULE_INIT(CSSetXOP)
