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
	CommandCSSetXOP(const ci::string &cname, const ci::string &cpermission = "") : Command(cname, 2, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		if (!FindModule("cs_xop"))
		{
			notice_lang(Config.s_ChanServ, u, CHAN_XOP_NOT_AVAILABLE, "XOP");
			return MOD_CONT;
		}

		ChannelInfo *ci = cs_findchan(params[0]);
		assert(ci);

		if (params[1] == "ON")
		{
			if (!ci->HasFlag(CI_XOP))
			{
				ChanAccess *access;

				for (unsigned i = ci->GetAccessCount() - 1; 0 <= i; --i)
				{
					access = ci->GetAccess(i);

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
						ci->EraseAccess(i);
				}

				reset_levels(ci);
				ci->SetFlag(CI_XOP);
			}

			Alog() << Config.s_ChanServ << ": " << u->GetMask() << " enabled XOP for " << ci->name;
			notice_lang(Config.s_ChanServ, u, CHAN_SET_XOP_ON, ci->name.c_str());
		}
		else if (params[1] == "OFF")
		{
			ci->UnsetFlag(CI_XOP);

			Alog() << Config.s_ChanServ << ": " << u->GetMask() << " disabled XOP for " << ci->name;
			notice_lang(Config.s_ChanServ, u, CHAN_SET_XOP_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "XOP");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_XOP, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SET XOP", CHAN_SET_XOP_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_XOP);
	}
};

class CommandCSSASetXOP : public CommandCSSetXOP
{
 public:
	CommandCSSASetXOP(const ci::string &cname) : CommandCSSetXOP(cname, "chanserv/saset/xop")
	{
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_XOP, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SASET XOP", CHAN_SASET_XOP_SYNTAX);
	}
};

class CSSetXOP : public Module
{
 public:
	CSSetXOP(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(new CommandCSSetXOP("XOP"));
	}

	~CSSetXOP()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand("XOP");
	}
};

MODULE_INIT(CSSetXOP)
