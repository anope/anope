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
			source.Reply(_("xOP system is not available."), "XOP");
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
			source.Reply(_("xOP lists system for %s is now \002\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_XOP);

			Log(LOG_COMMAND, u, this, ci) << "to disable XOP";
			source.Reply(_("xOP lists system for %s is now \002\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "XOP");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002%s \037channel\037 XOP {ON | OFF}\002\n"
				" \n"
				"Enables or disables the xOP lists system for a channel.\n"
				"When \002XOP\002 is set, you have to use the \002AOP\002/\002SOP\002/\002VOP\002\n"
				"commands in order to give channel privileges to\n"
				"users, else you have to use the \002ACCESS\002 command.\n"
				" \n"
				"\002Technical Note\002: when you switch from access list to xOP \n"
				"lists system, your level definitions and user levels will be\n"
				"changed, so you won't find the same values if you\n"
				"switch back to access system! \n"
				" \n"
				"You should also check that your users are in the good xOP \n"
				"list after the switch from access to xOP lists, because the \n"
				"guess is not always perfect... in fact, it is not recommended \n"
				"to use the xOP lists if you changed level definitions with \n"
				"the \002LEVELS\002 command.\n"
				" \n"
				"Switching from xOP lists system to access list system\n"
				"causes no problem though."), this->name.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET XOP", _("SET \037channel\037 XOP {ON | OFF}"));
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    XOP           Toggle the user privilege system"));
	}
};

class CommandCSSASetXOP : public CommandCSSetXOP
{
 public:
	CommandCSSASetXOP() : CommandCSSetXOP("chanserv/saset/xop")
	{
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET XOP", _("SASET \002channel\002 XOP {ON | OFF}"));
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
