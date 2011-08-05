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

class CommandCSSetOpNotice : public Command
{
 public:
	CommandCSSetOpNotice(Module *creator, const Anope::string &cname = "chanserv/set/notice") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Send a notice when OP/DEOP commands are used"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
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

		if (source.permission.empty() && !ci->HasPriv(u, CA_SET))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_OPNOTICE);
			source.Reply(_("Op-notice option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_OPNOTICE);
			source.Reply(_("Op-notice option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "OPNOTICE");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the \002op-notice\002 option for a channel.\n"
				"When \002op-notice\002 is set, %s will send a notice to the\n"
				"channel whenever the \002OP\002 or \002DEOP\002 commands are used for a user\n"
				"in the channel."), this->name.c_str(), source.owner->nick.c_str());
		return true;
	}
};

class CommandCSSASetOpNotice : public CommandCSSetOpNotice
{
 public:
	CommandCSSASetOpNotice(Module *creator) : CommandCSSetOpNotice(creator, "chanserv/saset/opnotice")
	{
	}
};

class CSSetOpNotice : public Module
{
	CommandCSSetOpNotice commandcssetopnotice;
	CommandCSSASetOpNotice commandcssasetopnotice;

 public:
	CSSetOpNotice(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetopnotice(this), commandcssasetopnotice(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandcssetopnotice);
		ModuleManager::RegisterService(&commandcssasetopnotice);
	}
};

MODULE_INIT(CSSetOpNotice)
