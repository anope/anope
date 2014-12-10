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
			source.Reply(_("Sorry, channel de-registration is temporarily disabled."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (params.size() < 2 || !chan.equals_ci(params[1]))
		{
			source.Reply(_("You must enter the channel name twice as a confirmation that you wish to drop \002{0}\002."), ci->GetName());
			return;
		}

		if ((ci->HasFieldS("SECUREFOUNDER") ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER")) && !source.HasCommand("chanserv/drop"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "FOUNDER", ci->GetName());
			return;
		}

		EventReturn MOD_RESULT = this->onchandrop(&Event::ChanDrop::OnChanDrop, source, ci);
		if (MOD_RESULT == EVENT_STOP)
			return;

		bool override = (ci->HasFieldS("SECUREFOUNDER") ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER"));
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "(founder was: " << (ci->GetFounder() ? ci->GetFounder()->GetDisplay() : "none") << ")";

		Reference<Channel> c = ci->c;
		ci->Delete();

		source.Reply(_("Channel \002{0}\002 has been dropped."), chan);

		if (c)
			c->CheckModes();
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Unregisters \037channel\037. All channel settings will be deleted. As a precaution, you must give the \037channel\037 name twice.\n"
		               "\n"
		               "Use of this command requires the \002{0}\002 privilege on \037channel\037."),
		               "FOUNDER");

		if (source.HasCommand("chanserv/drop"))
			source.Reply(_("\n"
			               "As a Services Operator with the command \002{0}\002, you may drop any channel."),
			               "chanserv/drop");

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
		, onchandrop(this)
	{

	}
};

MODULE_INIT(CSDrop)
