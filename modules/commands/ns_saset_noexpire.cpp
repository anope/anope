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

class CommandNSSASetNoexpire : public Command
{
 public:
	CommandNSSASetNoexpire(Module *creator) : Command(creator, "nickserv/saset/noexpire", 1, 2)
	{
		this->SetDesc(_("Prevent the nickname from expiring"));
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		NickAlias *na = findnick(params[0]);
		if (na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		Anope::string param = params.size() > 1 ? params[1] : "";

		if (param.equals_ci("ON"))
		{
			na->SetFlag(NS_NO_EXPIRE);
			source.Reply(_("Nick %s \002will not\002 expire."), na->nick.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			na->UnsetFlag(NS_NO_EXPIRE);
			source.Reply(_("Nick %s \002will\002 expire."), na->nick.c_str());
		}
		else
			this->OnSyntaxError(source, "NOEXPIRE");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets whether the given nickname will expire.  Setting this\n"
				"to \002ON\002 prevents the nickname from expiring."));
		return true;
	}
};

class NSSASetNoexpire : public Module
{
	CommandNSSASetNoexpire commandnssasetnoexpire;

 public:
	NSSASetNoexpire(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssasetnoexpire(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(NSSASetNoexpire)
