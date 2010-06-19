/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

class CommandCSSetSuccessor : public Command
{
 public:
	CommandCSSetSuccessor(const ci::string &cname, const std::string &cpermission = "") : Command(cname, 1, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		assert(ci);

		if (this->permission.empty() && ci->HasFlag(CI_SECUREFOUNDER) ? !IsFounder(u, ci) : !check_access(u, ci, CA_FOUNDER))
		{
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		NickCore *nc;

		if (params.size() > 1)
		{
			NickAlias *na = findnick(params[1]);

			if (!na)
			{
				notice_lang(Config.s_ChanServ, u, NICK_X_NOT_REGISTERED, params[1].c_str());
				return MOD_CONT;
			}
			if (na->HasFlag(NS_FORBIDDEN))
			{
				notice_lang(Config.s_ChanServ, u, NICK_X_FORBIDDEN, na->nick);
				return MOD_CONT;
			}
			if (na->nc == ci->founder)
			{
				notice_lang(Config.s_ChanServ, u, CHAN_SUCCESSOR_IS_FOUNDER, na->nick, ci->name.c_str());
				return MOD_CONT;
			}
			nc = na->nc;

		}
		else
			nc = NULL;

		Alog() << Config.s_ChanServ << ": Changing successor of " << ci->name << " from "
			<< (ci->successor ? ci->successor->display : "none")
			<< " to " << (nc ? nc->display : "none") << " by " << u->GetMask();

		ci->successor = nc;

		if (nc)
			notice_lang(Config.s_ChanServ, u, CHAN_SUCCESSOR_CHANGED, ci->name.c_str(), nc->display);
		else
			notice_lang(Config.s_ChanServ, u, CHAN_SUCCESSOR_UNSET, ci->name.c_str());

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_SUCCESSOR, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		// XXX
		syntax_error(Config.s_ChanServ, u, "SET", CHAN_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_SUCCESSOR);
	}
};

class CommandCSSASetSuccessor : public CommandCSSetSuccessor
{
 public:
	CommandCSSASetSuccessor(const ci::string &cname) : CommandCSSetSuccessor(cname, "chanserv/saset/successor")
	{
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_SUCCESSOR, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		// XXX
		syntax_error(Config.s_ChanServ, u, "SASEt", CHAN_SASET_SYNTAX);
	}
};

class CSSetSuccessor : public Module
{
 public:
	CSSetSuccessor(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(new CommandCSSetSuccessor("SUCCESSOR"));

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandCSSetSuccessor("SUCCESSOR"));
	}

	~CSSetSuccessor()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand("SUCCESSOR");

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand("SUCCESSOR");
	}
};

MODULE_INIT(CSSetSuccessor)
