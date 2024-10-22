/* ChanServ core functions
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandCSDrop final
	: public Command
{
private:
	PrimitiveExtensibleItem<Anope::string> dropcode;

public:
	CommandCSDrop(Module *creator)
		: Command(creator, "chanserv/drop", 1, 2)
		, dropcode(creator, "channel-dropcode")
	{
		this->SetDesc(_("Cancel the registration of a channel"));
		this->SetSyntax(_("\037channel\037 [\037code\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		if (Anope::ReadOnly && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(chan);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if ((ci->HasExt("SECUREFOUNDER") ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER")) && !source.HasCommand("chanserv/drop"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		auto *code = dropcode.Get(ci);
		if (params.size() < 2 || ((!code || !code->equals_ci(params[1])) && (!source.HasPriv("chanserv/drop/override") || params[1] != "OVERRIDE")))
		{
			if (!code)
			{
				code = ci->Extend<Anope::string>("channel-dropcode");
				*code = Anope::Random(15);
			}

			source.Reply(CONFIRM_DROP, ci->name.c_str(), source.service->GetQueryCommand().c_str(),
				ci->name.c_str(), code->c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnChanDrop, MOD_RESULT, (source, ci));
		if (MOD_RESULT == EVENT_STOP)
		{
			dropcode.Unset(ci);
			return;
		}

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
			source.Reply(_("Unregisters the specified channel.  Only \002Services Operators\002\n"
					"can drop a channel of which they are not the founder of."));
		else
			source.Reply(_("Unregisters the named channel.  Can only be used by\n"
					"the \002channel founder\002."));

		source.Reply(" ");
		if (source.HasPriv("chanserv/drop/override"))
			source.Reply(_("Additionally, Services Operators with the \037chanserv/drop/override\037 permission can\n"
				"replace \037code\037 with \002OVERRIDE\002 to drop without a confirmation code."));
		return true;
	}
};

class CSDrop final
	: public Module
{
	CommandCSDrop commandcsdrop;

public:
	CSDrop(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR), commandcsdrop(this)
	{

	}
};

MODULE_INIT(CSDrop)
