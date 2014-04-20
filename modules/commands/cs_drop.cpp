/* ChanServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/cs_drop.h"

class CommandCSDrop : public Command
{
	EventHandlers<Event::ChanDrop> &onchandrop;

 public:
	CommandCSDrop(Module *creator, EventHandlers<Event::ChanDrop> &event) : Command(creator, "chanserv/drop", 1, 2), onchandrop(event)
	{
		this->SetDesc(_("Cancel the registration of a channel"));
		this->SetSyntax(_("\037channel\037 \037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		if (Anope::ReadOnly && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Sorry, channel de-registration is temporarily disabled.")); // XXX: READ_ONLY_MODE?
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(chan);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (params.size() < 2 || !chan.equals_ci(params[1]))
		{
			source.Reply(_("You must enter the channel name twice as a confirmation that you wish to drop \002%s\002."), chan.c_str());
			return;
		}

		if ((ci->HasExt("SECUREFOUNDER") ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER")) && !source.HasCommand("chanserv/drop"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		EventReturn MOD_RESULT = this->onchandrop(&Event::ChanDrop::OnChanDrop, source, ci);
		if (MOD_RESULT == EVENT_STOP)
			return;

		bool override = (ci->HasExt("SECUREFOUNDER") ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER"));
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "(founder was: " << (ci->GetFounder() ? ci->GetFounder()->display : "none") << ")";

		Reference<Channel> c = ci->c;
		delete ci;

		source.Reply(_("Channel \002%s\002 has been dropped."), chan.c_str());

		if (c)
			c->CheckModes();
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		if (source.IsServicesOper())
			source.Reply(_("Unregisters the named channel.  Only \002Services Operators\002\n"
					"can drop a channel of which they are not the founder."));
		else
			source.Reply(_("Unregisters the named channel.  Can only be used by\n"
					"the \002channel founder\002."));

		return true;
	}
};

class CSDrop : public Module
{
	CommandCSDrop commandcsdrop;
	EventHandlers<Event::ChanDrop> onchandrop;

 public:
	CSDrop(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsdrop(this, onchandrop)
		, onchandrop(this, "OnChanDrop")
	{

	}
};

MODULE_INIT(CSDrop)
