/* ChanServ core functions
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

class CommandCSSetFounder : public Command
{
 public:
	CommandCSSetFounder(Module *creator, const Anope::string &cname = "chanserv/set/founder", const Anope::string &cpermission = "") : Command(creator, cname, 2, 2, cpermission)
	{
		this->SetDesc(_("Set the founder of a channel"));
		this->SetSyntax(_("\037channel\037 \037nick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!this->permission.empty() && !ci->HasPriv(u, CA_SET))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (this->permission.empty() && (ci->HasFlag(CI_SECUREFOUNDER) ? !IsFounder(u, ci) : !ci->HasPriv(u, CA_FOUNDER)))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		NickAlias *na = findnick(params[1]);

		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, params[1].c_str());
			return;
		}

		NickCore *nc = na->nc;
		if (Config->CSMaxReg && nc->channelcount >= Config->CSMaxReg && !u->HasPriv("chanserv/no-register-limit"))
		{
			source.Reply(_("\002%s\002 has too many channels registered."), na->nick.c_str());
			return;
		}

		Log(!this->permission.empty() ? LOG_ADMIN : LOG_COMMAND, u, this, ci) << "to change the founder to " << nc->display;

		ci->SetFounder(nc);

		source.Reply(_("Founder of %s changed to \002%s\002."), ci->name.c_str(), na->nick.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the founder of a channel. The new nickname must\n"
				"be a registered one."), this->name.c_str());
		return true;
	}
};

class CommandCSSASetFounder : public CommandCSSetFounder
{
 public:
	CommandCSSASetFounder(Module *creator) : CommandCSSetFounder(creator, "chanserv/saset/founder", "chanserv/saset/founder")
	{
	}
};

class CSSetFounder : public Module
{
	CommandCSSetFounder commandcssetfounder;
	CommandCSSASetFounder commandcssasetfounder;

 public:
	CSSetFounder(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetfounder(this), commandcssasetfounder(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandcssetfounder);
		ModuleManager::RegisterService(&commandcssasetfounder);
	}
};

MODULE_INIT(CSSetFounder)
