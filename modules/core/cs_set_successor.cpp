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

class CommandCSSetSuccessor : public Command
{
 public:
	CommandCSSetSuccessor(Module *creator, const Anope::string &cname = "chanserv/set/successor", const Anope::string &cpermission = "") : Command(creator, cname, 1, 2, cpermission)
	{
		this->SetDesc(_("Set the successor for a channel"));
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

		if (this->permission.empty() && ci->HasFlag(CI_SECUREFOUNDER) ? !IsFounder(u, ci) : !ci->HasPriv(u, CA_FOUNDER))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		NickCore *nc;

		if (params.size() > 1)
		{
			NickAlias *na = findnick(params[1]);

			if (!na)
			{
				source.Reply(NICK_X_NOT_REGISTERED, params[1].c_str());
				return;
			}
			if (na->nc == ci->GetFounder())
			{
				source.Reply(_("%s cannot be the successor on channel %s they are the founder."), na->nick.c_str(), ci->name.c_str());
				return;
			}
			nc = na->nc;
		}
		else
			nc = NULL;

		Log(!this->permission.empty() ? LOG_ADMIN : LOG_COMMAND, u, this, ci) << "to change the successor from " << (ci->successor ? ci->successor->display : "none") << " to " << (nc ? nc->display : "none");

		ci->successor = nc;

		if (nc)
			source.Reply(_("Successor for %s changed to \002%s\002."), ci->name.c_str(), nc->display.c_str());
		else
			source.Reply(_("Successor for \002%s\002 unset."), ci->name.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the successor of a channel. If the founder's\n"
				"nickname expires or is dropped while the channel is still\n"
				"registered, the successor will become the new founder of the\n"
				"channel.  However, if the successor already has too many\n"
				"channels registered (%d), the channel will be dropped\n"
				"instead, just as if no successor had been set.  The new\n"
				"nickname must be a registered one."), Config->CSMaxReg);
		return true;
	}
};

class CommandCSSASetSuccessor : public CommandCSSetSuccessor
{
 public:
	CommandCSSASetSuccessor(Module *creator) : CommandCSSetSuccessor(creator, "chanserv/saset/successor", "chanserv/saset/successor")
	{
	}
};

class CSSetSuccessor : public Module
{
	CommandCSSetSuccessor commandcssetsuccessor;
	CommandCSSASetSuccessor commandcssasetsuccessor;

 public:
	CSSetSuccessor(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetsuccessor(this), commandcssasetsuccessor(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandcssetsuccessor);
		ModuleManager::RegisterService(&commandcssasetsuccessor);
	}
};

MODULE_INIT(CSSetSuccessor)
