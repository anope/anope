/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */


#include "module.h"
#include "modules/nickserv/ajoin.h"

class AutoJoinImpl : public AutoJoin
{
	friend class AutoJoinType;

	Serialize::Storage<NickServ::Account *> account;
	Serialize::Storage<Anope::string> channel, key;

 public:
	using AutoJoin::AutoJoin;

	NickServ::Account *GetAccount() override;
	void SetAccount(NickServ::Account *acc) override;

	Anope::string GetChannel() override;
	void SetChannel(const Anope::string &c) override;

	Anope::string GetKey() override;
	void SetKey(const Anope::string &k) override;
};

class AutoJoinType : public Serialize::Type<AutoJoinImpl>
{
 public:
	Serialize::ObjectField<AutoJoinImpl, NickServ::Account *> owner;
	Serialize::Field<AutoJoinImpl, Anope::string> channel, key;

	AutoJoinType(Module *me) : Serialize::Type<AutoJoinImpl>(me)
		, owner(this, "account", &AutoJoinImpl::account, true)
		, channel(this, "channel", &AutoJoinImpl::channel)
		, key(this, "key", &AutoJoinImpl::key)
	{
	}
};

NickServ::Account *AutoJoinImpl::GetAccount()
{
	return Get(&AutoJoinType::owner);
}

void AutoJoinImpl::SetAccount(NickServ::Account *acc)
{
	Set(&AutoJoinType::owner, acc);
}

Anope::string AutoJoinImpl::GetChannel()
{
	return Get(&AutoJoinType::channel);
}

void AutoJoinImpl::SetChannel(const Anope::string &c)
{
	Set(&AutoJoinType::channel, c);
}

Anope::string AutoJoinImpl::GetKey()
{
	return Get(&AutoJoinType::key);
}

void AutoJoinImpl::SetKey(const Anope::string &k)
{
	Set(&AutoJoinType::key, k);
}

class CommandNSAJoin : public Command
{
	void DoList(CommandSource &source, NickServ::Account *nc)
	{
		std::vector<AutoJoin *> channels = nc->GetRefs<AutoJoin *>();

		if (channels.empty())
		{
			source.Reply(_("The auto join list of \002{0}\002 is empty."), nc->GetDisplay());
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Channel")).AddColumn(_("Key"));
		for (unsigned i = 0; i < channels.size(); ++i)
		{
			AutoJoin *aj = channels[i];
			ListFormatter::ListEntry entry;
			entry["Number"] = stringify(i + 1);
			entry["Channel"] = aj->GetChannel();
			entry["Key"] = aj->GetKey();
			list.AddEntry(entry);
		}

		source.Reply(_("Auto join list of \002{0}\002:"), nc->GetDisplay());

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);
	}

	void DoAdd(CommandSource &source, NickServ::Account *nc, const Anope::string &chans, const Anope::string &keys)
	{
		std::vector<AutoJoin *> channels = nc->GetRefs<AutoJoin *>();

		Anope::string addedchans;
		Anope::string alreadyadded;
		Anope::string invalidkey;
		commasepstream ksep(keys, true);
		commasepstream csep(chans);
		for (Anope::string chan, key; csep.GetToken(chan);)
		{
			ksep.GetToken(key);

			unsigned i = 0;
			for (; i < channels.size(); ++i)
				if (channels[i]->GetChannel().equals_ci(chan))
					break;

			if (channels.size() >= Config->GetModule(this->GetOwner())->Get<unsigned>("ajoinmax"))
			{
				source.Reply(_("Sorry, the maximum of \002{0}\002 auto join entries has been reached."), Config->GetModule(this->GetOwner())->Get<unsigned>("ajoinmax"));
				return;
			}

			if (i != channels.size())
				alreadyadded += chan + ", ";
			else if (IRCD->IsChannelValid(chan) == false)
	 			source.Reply(_("\002{0}\002 isn't a valid channel."), chan);
			else
			{
				Channel *c = Channel::Find(chan);
				Anope::string k;
				if (c && c->GetParam("KEY", k) && key != k)
				{
					invalidkey += chan + ", ";
					continue;
				}

				AutoJoin *entry = Serialize::New<AutoJoin *>();
				entry->SetAccount(nc);
				entry->SetChannel(chan);
				entry->SetKey(key);

				addedchans += chan + ", ";
			}
		}

		if (!alreadyadded.empty())
		{
			alreadyadded = alreadyadded.substr(0, alreadyadded.length() - 2);
			source.Reply(_("\002{0}\002 is already on the auto join list of \002{1}\002."), alreadyadded, nc->GetDisplay());
		}

		if (!invalidkey.empty())
		{
			invalidkey = invalidkey.substr(0, invalidkey.length() - 2);
			source.Reply(_("\002{0}\002 had an invalid key specified, and was ignored."), invalidkey);
		}

		if (addedchans.empty())
			return;

		addedchans = addedchans.substr(0, addedchans.length() - 2);
		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to ADD channel " << addedchans << " to " << nc->GetDisplay();
		source.Reply(_("\002{0}\002 added to the auto join list of \002{1}\002."), addedchans, nc->GetDisplay());
	}

	void DoDel(CommandSource &source, NickServ::Account *nc, const Anope::string &chans)
	{
		std::vector<AutoJoin *> channels = nc->GetRefs<AutoJoin *>();
		Anope::string delchans;
		Anope::string notfoundchans;
		commasepstream sep(chans);

		for (Anope::string chan; sep.GetToken(chan);)
		{
			unsigned i = 0;
			for (; i < channels.size(); ++i)
				if (channels[i]->GetChannel().equals_ci(chan))
					break;

			if (i == channels.size())
				notfoundchans += chan + ", ";
			else
			{
				delete channels[i];
				delchans += chan + ", ";
			}
		}

		if (!notfoundchans.empty())
		{
			notfoundchans = notfoundchans.substr(0, notfoundchans.length() - 2);
			source.Reply(_("\002{0}\002 was not found on the auto join list of \002{1}\002."), notfoundchans, nc->GetDisplay());
		}

		if (delchans.empty())
			return;

		delchans = delchans.substr(0, delchans.length() - 2);
		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to DELETE channel " << delchans << " from " << nc->GetDisplay();
		source.Reply(_("\002{0}\002 was removed from the auto join list of \002{1}\002."), delchans, nc->GetDisplay());
	}

 public:
	CommandNSAJoin(Module *creator) : Command(creator, "nickserv/ajoin", 1, 4)
	{
		this->SetDesc(_("Manage your auto join list"));
		this->SetSyntax(_("ADD [\037nickname\037] \037channel\037 [\037key\037]"));
		this->SetSyntax(_("DEL [\037nickname\037] \037channel\037"));
		this->SetSyntax(_("LIST [\037nickname\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[0];
		Anope::string nick, param, param2;

		if (cmd.equals_ci("LIST"))
			nick = params.size() > 1 ? params[1] : "";
		else
			nick = (params.size() > 2 && IRCD->IsChannelValid(params[2])) ? params[1] : "";

		NickServ::Account *nc;
		if (!nick.empty() && !source.HasCommand("nickserv/ajoin"))
		{
			NickServ::Nick *na = NickServ::FindNick(nick);
			if (na == NULL)
			{
				source.Reply(_("\002{0}\002 isn't registered."), nick);
				return;
			}

			nc = na->GetAccount();
			param = params.size() > 2 ? params[2] : "";
			param2 = params.size() > 3 ? params[3] : "";
		}
		else
		{
			nc = source.nc;
			param = params.size() > 1 ? params[1] : "";
			param2 = params.size() > 2 ? params[2] : "";
		}

		if (cmd.equals_ci("LIST"))
			return this->DoList(source, nc);
		else if (nc->HasFieldS("NS_SUSPENDED"))
			source.Reply(_("\002{0}\002 isn't registered."), nc->GetDisplay());
		else if (param.empty())
			this->OnSyntaxError(source, "");
		else if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode."));
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, nc, param, param2);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, nc, param);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("This command manages your auto join list."
		               " When you identify you will automatically join the channels on your auto join list."
		               " Services Operators may provide \037nickname\037 to modify other users' auto join lists."));
		return true;
	}
};

class NSAJoin : public Module
	, public EventHook<Event::UserLogin>
{
	CommandNSAJoin commandnsajoin;
	AutoJoinType ajtype;

 public:
	NSAJoin(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::UserLogin>(this)
		, commandnsajoin(this)
		, ajtype(this)
	{

		if (!IRCD || !IRCD->CanSVSJoin)
			throw ModuleException("Your IRCd does not support SVSJOIN");

	}

	void OnUserLogin(User *u) override
	{
		ServiceBot *NickServ = Config->GetClient("NickServ");
		if (!NickServ)
			return;

		std::vector<AutoJoin *> channels = u->Account()->GetRefs<AutoJoin *>();
		if (channels.empty())
			return;

		/* Set +r now, so we can ajoin users into +R channels */
		ModeManager::ProcessModes();

		for (AutoJoin *entry : channels)
		{
			Channel *c = Channel::Find(entry->GetChannel());
			ChanServ::Channel *ci;

			if (c)
				ci = c->ci;
			else
				ci = ChanServ::Find(entry->GetChannel());

			bool need_invite = false;
			Anope::string key = entry->GetKey();
			ChanServ::AccessGroup u_access;

			if (ci != NULL)
			{
				if (ci->HasFieldS("CS_SUSPENDED"))
					continue;
				u_access = ci->AccessFor(u);
			}
			if (c != NULL)
			{
				if (c->FindUser(u) != NULL)
					continue;
				else if (c->HasMode("OPERONLY") && !u->HasMode("OPER"))
					continue;
				else if (c->HasMode("ADMINONLY") && !u->HasMode("ADMIN"))
					continue;
				else if (c->HasMode("SSL") && !(u->HasMode("SSL") || u->HasExtOK("ssl")))
					continue;
				else if (c->MatchesList(u, "BAN") == true && c->MatchesList(u, "EXCEPT") == false)
					need_invite = true;
				else if (c->HasMode("INVITE") && c->MatchesList(u, "INVITEOVERRIDE") == false)
					need_invite = true;

				if (c->HasMode("KEY"))
				{
					Anope::string k;
					if (c->GetParam("KEY", k))
					{
						if (u_access.HasPriv("GETKEY"))
							key = k;
						else if (key != k)
							need_invite = true;
					}
				}
				if (c->HasMode("LIMIT"))
				{
					Anope::string l;
					if (c->GetParam("LIMIT", l))
					{
						try
						{
							unsigned limit = convertTo<unsigned>(l);
							if (c->users.size() >= limit)
								need_invite = true;
						}
						catch (const ConvertException &) { }
					}
				}
			}

			if (need_invite && c != NULL)
			{
				if (!u_access.HasPriv("INVITE"))
					continue;
				IRCD->SendInvite(NickServ, c, u);
			}

			IRCD->SendSVSJoin(NickServ, u, entry->GetChannel(), key);
		}
	}
};

MODULE_INIT(NSAJoin)
