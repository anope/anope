/* NickServ core functions
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

class CommandNSSetMessage : public Command
{
 public:
	CommandNSSetMessage(const Anope::string &spermission = "") : Command("MSG", 2, 2, spermission)
	{
		this->SetDesc(_("Change the communication method of Services"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		NickAlias *na = findnick(params[0]);
		if (!na)
			throw CoreException("NULL na in CommandNSSetMessage");
		NickCore *nc = na->nc;

		if (!Config->UsePrivmsg)
		{
			source.Reply(_("Option \002%s\02 cannot be set on this network."), "MSG");
			return MOD_CONT;
		}

		const Anope::string &param = params.size() > 1 ? params[1] : "";

		if (param.equals_ci("ON"))
		{
			nc->SetFlag(NI_MSG);
			source.Reply(_("Services will now reply to \002%s\002 with \002messages\002."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetFlag(NI_MSG);
			source.Reply(_("Services will now reply to \002%s\002 with \002notices\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "MSG");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SET MSG {ON | OFF}\002\n"
				" \n"
				"Allows you to choose the way Services are communicating with \n"
				"you. With \002MSG\002 set, Services will use messages, else they'll \n"
				"use notices."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET MSG", _("SET MSG {ON | OFF}"));
	}
};

class CommandNSSASetMessage : public CommandNSSetMessage
{
 public:
	CommandNSSASetMessage() : CommandNSSetMessage("nickserv/saset/message")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SASET \037nickname\037 MSG {ON | OFF}\002\n"
				" \n"
				"Allows you to choose the way Services are communicating with \n"
				"the given user. With \002MSG\002 set, Services will use messages,\n"
				"else they'll use notices."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET MSG", _("SASAET \037nickname\037 PRIVATE {ON | OFF}"));
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
			c->AddSubcommand(this, &commandnssetmessage);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandnssasetmessage);
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
