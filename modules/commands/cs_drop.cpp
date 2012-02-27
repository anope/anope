/* ChanServ core functions
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

class CommandCSDrop : public Command
{
 public:
	CommandCSDrop(Module *creator) : Command(creator, "chanserv/drop", 1, 1)
	{
		this->SetDesc(_("Cancel the registration of a channel"));
		this->SetSyntax(_("\037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &chan = params[0];

		User *u = source.u;

		if (readonly)
		{
			source.Reply(_("Sorry, channel de-registration is temporarily disabled.")); // XXX: READ_ONLY_MODE?
			return;
		}

		ChannelInfo *ci = cs_findchan(chan);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (ci->HasFlag(CI_SUSPENDED) && !u->HasCommand("chanserv/drop"))
		{
			source.Reply(CHAN_X_SUSPENDED, chan.c_str());
			return;
		}

		if ((ci->HasFlag(CI_SECUREFOUNDER) ? !IsFounder(u, ci) : !ci->AccessFor(u).HasPriv("FOUNDER")) && !u->HasCommand("chanserv/drop"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		bool override = (ci->HasFlag(CI_SECUREFOUNDER) ? !IsFounder(u, ci) : !ci->AccessFor(u).HasPriv("FOUNDER"));
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "(founder was: " << (ci->GetFounder() ? ci->GetFounder()->display : "none") << ")";

		FOREACH_MOD(I_OnChanDrop, OnChanDrop(ci));

		Channel *c = ci->c;
		delete ci;

		source.Reply(_("Channel \002%s\002 has been dropped."), chan.c_str());

		if (c)
			c->CheckModes();

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		User *u = source.u;
		this->SendSyntax(source);
		source.Reply(" ");
		if (u->IsServicesOper())
			source.Reply(_("Unregisters the named channel.  Only \002Services Operators\002\n"
					"can drop a channel for which they have not identified."));
		else
			source.Reply(_("Unregisters the named channel.  Can only be used by\n"
					"\002channel founder\002."));

		return true;
	}
};

class CSDrop : public Module
{
	CommandCSDrop commandcsdrop;

 public:
	CSDrop(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), commandcsdrop(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSDrop)
