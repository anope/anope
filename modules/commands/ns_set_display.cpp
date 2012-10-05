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

class CommandNSSetDisplay : public Command
{
 public:
	CommandNSSetDisplay(Module *creator, const Anope::string &sname = "nickserv/set/display", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Set the display of your group in Services"));
		this->SetSyntax(_("\037new-display\037"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		const NickAlias *user_na = findnick(user), *na = findnick(param);

		if (user_na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		else if (!na || *na->nc != *user_na->nc)
		{
			source.Reply(_("The new display MUST be a nickname of the nickname group %s"), user_na->nc->display.c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, user_na->nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		change_core_display(user_na->nc, na->nick);
		source.Reply(NICK_SET_DISPLAY_CHANGED, user_na->nc->display.c_str());
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the display used to refer to your nickname group in \n"
				"Services. The new display MUST be a nick of your group."));
		return true;
	}
};

class CommandNSSASetDisplay : public CommandNSSetDisplay
{
 public:
	CommandNSSASetDisplay(Module *creator) : CommandNSSetDisplay(creator, "nickserv/saset/display", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 \037new-display\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the display used to refer to the nickname group in \n"
				"Services. The new display MUST be a nick of your group."));
		return true;
	}
};

class NSSetDisplay : public Module
{
	CommandNSSetDisplay commandnssetdisplay;
	CommandNSSASetDisplay commandnssasetdisplay;

 public:
	NSSetDisplay(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssetdisplay(this), commandnssasetdisplay(this)
	{
		this->SetAuthor("Anope");

		if (Config->NoNicknameOwnership)
			throw ModuleException(modname + " can not be used with options:nonicknameownership enabled");
	}
};

MODULE_INIT(NSSetDisplay)
