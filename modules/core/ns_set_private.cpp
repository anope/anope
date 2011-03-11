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

class CommandNSSetPrivate : public Command
{
 public:
	CommandNSSetPrivate(const Anope::string &spermission = "") : Command("PRIVATE", 2, 2, spermission)
	{
		this->SetDesc(Anope::printf(_("Prevent the nickname from appearing in a \002%R%s LIST\002"), NickServ->nick.c_str()));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		NickAlias *na = findnick(params[0]);
		if (!na)
			throw CoreException("NULL na in CommandNSSetPrivate");
		NickCore *nc = na->nc;

		Anope::string param = params[1];

		if (param.equals_ci("ON"))
		{
			nc->SetFlag(NI_PRIVATE);
			source.Reply(_("Private option is now \002\002 for \002%s\002."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetFlag(NI_PRIVATE);
			source.Reply(_("Private option is now \002\002 for \002%s\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "PRIVATE");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SET PRIVATE {ON | OFF}\002\n"
				" \n"
				"Turns %s's privacy option on or off for your nick.\n"
				"With \002PRIVATE\002 set, your nickname will not appear in\n"
				"nickname lists generated with %s's \002LIST\002 command.\n"
				"(However, anyone who knows your nickname can still get\n"
				"information on it using the \002INFO\002 command.)"),
				NickServ->nick.c_str(), NickServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET PRIVATE", _("SET PRIVATE {ON | OFF}"));
	}
};

class CommandNSSASetPrivate : public CommandNSSetPrivate
{
 public:
	CommandNSSASetPrivate() : CommandNSSetPrivate("nickserv/saset/private")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SASET \037nickname\037 PRIVATE {ON | OFF}\002\n"
				" \n"
				"Turns %s's privacy option on or off for the nick.\n"
				"With \002PRIVATE\002 set, the nickname will not appear in\n"
				"nickname lists generated with %s's \002LIST\002 command.\n"
				"(However, anyone who knows the nickname can still get\n"
				"information on it using the \002INFO\002 command.)"),
				NickServ->nick.c_str(), NickServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET PRIVATE", _("SASET \037nickname\037 PRIVATE {ON | OFF}"));
	}
};

class NSSetPrivate : public Module
{
	CommandNSSetPrivate commandnssetprivate;
	CommandNSSASetPrivate commandnssasetprivate;

 public:
	NSSetPrivate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandnssetprivate);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandnssasetprivate);
	}

	~NSSetPrivate()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand(&commandnssetprivate);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand(&commandnssasetprivate);
	}
};

MODULE_INIT(NSSetPrivate)
