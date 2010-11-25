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

class CommandNSSetMessage : public Command
{
 public:
	CommandNSSetMessage(const Anope::string &spermission = "") : Command("MSG", 2, 2, spermission)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickAlias *na = findnick(params[0]);
		if (!na)
			throw CoreException("NULL na in CommandNSSetMessage");
		NickCore *nc = na->nc;

		if (!Config->UsePrivmsg)
		{
			source.Reply(NICK_SET_OPTION_DISABLED, "MSG");
			return MOD_CONT;
		}

		const Anope::string &param = params.size() > 1 ? params[1] : "";

		if (param.equals_ci("ON"))
		{
			nc->SetFlag(NI_MSG);
			source.Reply(NICK_SASET_MSG_ON, nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetFlag(NI_MSG);
			source.Reply(NICK_SASET_MSG_OFF, nc->display.c_str());
		}
		else
			this->OnSyntaxError(u, "MSG");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(NickServ, NICK_HELP_SET_MSG);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		SyntaxError(NickServ, u, "SET MSG", NICK_SET_MSG_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(NickServ, NICK_HELP_CMD_SET_MSG);
	}
};

class CommandNSSASetMessage : public CommandNSSetMessage
{
 public:
	CommandNSSASetMessage() : CommandNSSetMessage("nickserv/saset/message")
	{
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(NickServ, NICK_HELP_SASET_MSG);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		SyntaxError(NickServ, u, "SASET MSG", NICK_SASET_MSG_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(NickServ, NICK_HELP_CMD_SET_MSG);
	}
};

class NSSetMessage : public Module
{
	CommandNSSetMessage commandnssetmessage;
	CommandNSSASetMessage commandnssasetmessage;

 public:
	NSSetMessage(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(&commandnssetmessage);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(&commandnssasetmessage);
	}

	~NSSetMessage()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand(&commandnssetmessage);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand(&commandnssasetmessage);
	}
};

MODULE_INIT(NSSetMessage)
