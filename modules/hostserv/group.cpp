/* HostServ core functions
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
#include "modules/nickserv/group.h"

class CommandHSGroup : public Command
{
	bool setting;

 public:
	void Sync(NickServ::Nick *na)
	{
		if (setting)
			return;

		if (!na || !na->HasVhost())
			return;

		setting = true;
		for (NickServ::Nick *nick : na->GetAccount()->GetRefs<NickServ::Nick *>())
		{
			nick->SetVhost(na->GetVhostIdent(), na->GetVhostHost(), na->GetVhostCreator());
			EventManager::Get()->Dispatch(&Event::SetVhost::OnSetVhost, nick);
		}
		setting = false;
	}

	CommandHSGroup(Module *creator) : Command(creator, "hostserv/group", 0, 0), setting(false)
	{
		this->SetDesc(_("Syncs the vhost for all nicks in a group"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		NickServ::Nick *na = NickServ::FindNick(source.GetNick());
		if (!na || na->GetAccount() != source.GetAccount())
		{
			source.Reply(_("Access denied."));
			return;
		}

		if (!na->HasVhost())
		{
			source.Reply(_("There is no vhost assigned to this nickname."));
			return;
		}

		this->Sync(na);
		if (!na->GetVhostIdent().empty())
			source.Reply(_("All vhosts in the group \002{0}\002 have been set to \002{1}\002@\002{2}\002."), source.nc->GetDisplay(), na->GetVhostIdent(), na->GetVhostHost());
		else
			source.Reply(_("All vhosts in the group \002{0}\002 have been set to \002{1}\002."), source.nc->GetDisplay(), na->GetVhostHost());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sets the vhost of all nicks in your group to the vhost of your current nick."));
		return true;
	}
};

class HSGroup : public Module
	, public EventHook<Event::SetVhost>
	, public EventHook<Event::NickGroup>
{
	CommandHSGroup commandhsgroup;
	bool syncongroup;
	bool synconset;

 public:
	HSGroup(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::SetVhost>(this)
		, EventHook<Event::NickGroup>(this)
		, commandhsgroup(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}

	void OnSetVhost(NickServ::Nick *na) override
	{
		if (!synconset)
			return;

		commandhsgroup.Sync(na);
	}

	void OnNickGroup(User *u, NickServ::Nick *na) override
	{
		if (!syncongroup)
			return;

		commandhsgroup.Sync(na);
	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *block = conf->GetModule(this);
		syncongroup = block->Get<bool>("syncongroup");
		synconset = block->Get<bool>("synconset");
	}
};

MODULE_INIT(HSGroup)
