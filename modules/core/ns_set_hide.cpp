/* NickServ core functions
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

class CommandNSSetHide : public Command
{
 public:
	CommandNSSetHide(const Anope::string &spermission = "") : Command("HIDE", 2, 3, spermission)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		NickAlias *na = findnick(params[0]);
		if (!na)
			throw CoreException("NULL na in CommandNSSetHide");
		NickCore *nc = na->nc;

		LanguageString onmsg, offmsg;
		NickCoreFlag flag;

		Anope::string param = params[1];
		Anope::string arg = params.size() > 2 ? params[2] : "";

		if (param.equals_ci("EMAIL"))
		{
			flag = NI_HIDE_EMAIL;
			onmsg = NICK_SASET_HIDE_EMAIL_ON;
			offmsg = NICK_SASET_HIDE_EMAIL_OFF;
		}
		else if (param.equals_ci("USERMASK"))
		{
			flag = NI_HIDE_MASK;
			onmsg = NICK_SASET_HIDE_MASK_ON;
			offmsg = NICK_SASET_HIDE_MASK_OFF;
		}
		else if (param.equals_ci("STATUS"))
		{
			flag = NI_HIDE_STATUS;
			onmsg = NICK_SASET_HIDE_STATUS_ON;
			offmsg = NICK_SASET_HIDE_STATUS_OFF;
		}
		else if (param.equals_ci("QUIT"))
		{
			flag = NI_HIDE_QUIT;
			onmsg = NICK_SASET_HIDE_QUIT_ON;
			offmsg = NICK_SASET_HIDE_QUIT_OFF;
		}
		else
		{
			this->OnSyntaxError(source, "HIDE");
			return MOD_CONT;
		}

		if (arg.equals_ci("ON"))
		{
			nc->SetFlag(flag);
			source.Reply(onmsg, nc->display.c_str(), Config->s_NickServ.c_str());
		}
		else if (arg.equals_ci("OFF"))
		{
			nc->UnsetFlag(flag);
			source.Reply(offmsg, nc->display.c_str(), Config->s_NickServ.c_str());
		}
		else
			this->OnSyntaxError(source, "HIDE");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(NICK_HELP_SET_HIDE);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET HIDE", NICK_SET_HIDE_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_SET_HIDE);
	}
};

class CommandNSSASetHide : public CommandNSSetHide
{
 public:
	CommandNSSASetHide() : CommandNSSetHide("nickserv/saset/command")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(NICK_HELP_SASET_HIDE);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET HIDE", NICK_SASET_HIDE_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_SET_HIDE);
	}
};

class NSSetHide : public Module
{
	CommandNSSetHide commandnssethide;
	CommandNSSASetHide commandnssasethide;

 public:
	NSSetHide(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(&commandnssethide);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(&commandnssasethide);
	}

	~NSSetHide()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand(&commandnssethide);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand(&commandnssasethide);
	}
};

MODULE_INIT(NSSetHide)
