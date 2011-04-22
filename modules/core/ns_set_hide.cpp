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
#include "nickserv.h"

class CommandNSSetHide : public Command
{
 public:
	CommandNSSetHide(const Anope::string &spermission = "") : Command("HIDE", 2, 3, spermission)
	{
		this->SetDesc(_("Hide certain pieces of nickname information"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		NickAlias *na = findnick(params[0]);
		if (!na)
			throw CoreException("NULL na in CommandNSSetHide");
		NickCore *nc = na->nc;

		Anope::string onmsg, offmsg;
		NickCoreFlag flag;

		Anope::string param = params[1];
		Anope::string arg = params.size() > 2 ? params[2] : "";

		if (param.equals_ci("EMAIL"))
		{
			flag = NI_HIDE_EMAIL;
			onmsg = _("The E-mail address of \002%s\002 will now be hidden from %s INFO displays.");
			offmsg = _("The E-mail address of \002%s\002 will now be shown in %s INFO displays.");
		}
		else if (param.equals_ci("USERMASK"))
		{
			flag = NI_HIDE_MASK;
			onmsg = _("The last seen user@host mask of \002%s\002 will now be hidden from %s INFO displays.");
			offmsg = _("The last seen user@host mask of \002%s\002 will now be shown in %s INFO displays.");
		}
		else if (param.equals_ci("STATUS"))
		{
			flag = NI_HIDE_STATUS;
			onmsg = _("The services access status of \002%s\002 will now be hidden from %s INFO displays.");
			offmsg = _("The services access status of \002%s\002 will now be shown in %s INFO displays.");
		}
		else if (param.equals_ci("QUIT"))
		{
			flag = NI_HIDE_QUIT;
			onmsg = _("The last quit message of \002%s\002 will now be hidden from %s INFO displays.");
			offmsg = _("The last quit message of \002%s\002 will now be shown in %s INFO displays.");
		}
		else
		{
			this->OnSyntaxError(source, "HIDE");
			return MOD_CONT;
		}

		if (arg.equals_ci("ON"))
		{
			nc->SetFlag(flag);
			source.Reply(onmsg.c_str(), nc->display.c_str(), Config->s_NickServ.c_str());
		}
		else if (arg.equals_ci("OFF"))
		{
			nc->UnsetFlag(flag);
			source.Reply(offmsg.c_str(), nc->display.c_str(), Config->s_NickServ.c_str());
		}
		else
			this->OnSyntaxError(source, "HIDE");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SET HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}\002\n"
				" \n"
				"Allows you to prevent certain pieces of information from\n"
				"being displayed when someone does a %s \002INFO\002 on your\n"
				"nick.  You can hide your E-mail address (\002EMAIL\002), last seen\n"
				"user@host mask (\002USERMASK\002), your services access status\n"
				"(\002STATUS\002) and  last quit message (\002QUIT\002).\n"
				"The second parameter specifies whether the information should\n"
				"be displayed (\002OFF\002) or hidden (\002ON\002)."), Config->s_NickServ.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET HIDE", _("SET HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}"));
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
		source.Reply(_("Syntax: \002SASET \037nickname\037 HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}\002\n"
				" \n"
				"Allows you to prevent certain pieces of information from\n"
				"being displayed when someone does a %s \002INFO\002 on the\n"
				"nick.  You can hide the E-mail address (\002EMAIL\002), last seen\n"
				"user@host mask (\002USERMASK\002), the services access status\n"
				"(\002STATUS\002) and  last quit message (\002QUIT\002).\n"
				"The second parameter specifies whether the information should\n"
				"be displayed (\002OFF\002) or hidden (\002ON\002)."), Config->s_NickServ.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET HIDE", _("SASET NICK_SASET_HIDE_SYNTAX37nicknameNICK_SASET_HIDE_SYNTAX37 HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}"));
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

		if (!nickserv)
			throw ModuleException("NickServ is not loaded!");

		Command *c = FindCommand(nickserv->Bot(), "SET");
		if (c)
			c->AddSubcommand(this, &commandnssethide);

		c = FindCommand(nickserv->Bot(), "SASET");
		if (c)
			c->AddSubcommand(this, &commandnssasethide);
	}

	~NSSetHide()
	{
		Command *c = FindCommand(nickserv->Bot(), "SET");
		if (c)
			c->DelSubcommand(&commandnssethide);

		c = FindCommand(nickserv->Bot(), "SASET");
		if (c)
			c->DelSubcommand(&commandnssasethide);
	}
};

MODULE_INIT(NSSetHide)
