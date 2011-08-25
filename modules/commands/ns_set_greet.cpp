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

class CommandNSSetGreet : public Command
{
 public:
	CommandNSSetGreet(Module *creator, const Anope::string &sname = "nickserv/set/greet", size_t min = 0) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Associate a greet message with your nickname"));
		this->SetSyntax(_("\037message\037"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		NickAlias *na = findnick(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		if (!param.empty())
		{
			nc->greet = param;
			source.Reply(_("Greet message for \002%s\002 changed to \002%s\002."), nc->display.c_str(), nc->greet.c_str());
		}
		else
		{
			nc->greet.clear();
			source.Reply(_("Greet message for \002%s\002 unset."), nc->display.c_str());
		}

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		this->Run(source, source.u->Account()->display, params.size() > 0 ? params[0] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Makes the given message the greet of your nickname, that\n"
				"will be displayed when joining a channel that has GREET\n"
				"option enabled, provided that you have the necessary \n"
				"access on it."));
		return true;
	}
};

class CommandNSSASetGreet : public CommandNSSetGreet
{
 public:
	CommandNSSASetGreet(Module *creator) : CommandNSSetGreet(creator, "nickserv/saset/greet", 1)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 \037message\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		this->Run(source, params[0], params.size() > 1 ? params[1] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Makes the given message the greet of the nickname, that\n"
				"will be displayed when joining a channel that has GREET\n"
				"option enabled, provided that the user has the necessary \n"
				"access on it."));
		return true;
	}
};

class NSSetGreet : public Module
{
	CommandNSSetGreet commandnssetgreet;
	CommandNSSASetGreet commandnssasetgreet;

 public:
	NSSetGreet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssetgreet(this), commandnssasetgreet(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(NSSetGreet)
