/* NickServ core functions
 *
 * (C) 2003-2012 Anope Team
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
	CommandNSSetHide(Module *creator, const Anope::string &sname = "nickserv/set/hide", size_t min = 2) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Hide certain pieces of nickname information"));
		this->SetSyntax(_("{EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param, const Anope::string &arg)
	{
		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		Anope::string onmsg, offmsg;
		NickCoreFlag flag;

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
			return;
		}

		if (arg.equals_ci("ON"))
		{
			nc->SetFlag(flag);
			source.Reply(onmsg.c_str(), nc->display.c_str(), Config->NickServ.c_str());
		}
		else if (arg.equals_ci("OFF"))
		{
			nc->UnsetFlag(flag);
			source.Reply(offmsg.c_str(), nc->display.c_str(), Config->NickServ.c_str());
		}
		else
			this->OnSyntaxError(source, "HIDE");

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to prevent certain pieces of information from\n"
				"being displayed when someone does a %s \002INFO\002 on your\n"
				"nick.  You can hide your E-mail address (\002EMAIL\002), last seen\n"
				"user@host mask (\002USERMASK\002), your services access status\n"
				"(\002STATUS\002) and  last quit message (\002QUIT\002).\n"
				"The second parameter specifies whether the information should\n"
				"be displayed (\002OFF\002) or hidden (\002ON\002)."), Config->NickServ.c_str());
		return true;
	}
};

class CommandNSSASetHide : public CommandNSSetHide
{
 public:
	CommandNSSASetHide(Module *creator) : CommandNSSetHide(creator, "nickserv/saset/hide", 3)
	{
		this->SetSyntax("\037nickname\037 {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->ClearSyntax();
		this->Run(source, params[0], params[1], params[2]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to prevent certain pieces of information from\n"
				"being displayed when someone does a %s \002INFO\002 on the\n"
				"nick.  You can hide the E-mail address (\002EMAIL\002), last seen\n"
				"user@host mask (\002USERMASK\002), the services access status\n"
				"(\002STATUS\002) and  last quit message (\002QUIT\002).\n"
				"The second parameter specifies whether the information should\n"
				"be displayed (\002OFF\002) or hidden (\002ON\002)."), Config->NickServ.c_str());
		return true;
	}
};

class NSSetHide : public Module
{
	CommandNSSetHide commandnssethide;
	CommandNSSASetHide commandnssasethide;

 public:
	NSSetHide(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssethide(this), commandnssasethide(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(NSSetHide)
