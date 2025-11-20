// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
// Copyright (C) 2017-2018 Matt Schatz <genius3000@g3k.solutions>
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"

#define HOSTOFFER_TYPE "HostOffer"

class HostOfferList;

namespace
{
	HostOfferList* host_offers = nullptr;
	Anope::string network_name;
	time_t take_delay = 0;

	Anope::string GetVHostMask(const Anope::string &ident, const Anope::string &host)
	{
		Anope::string buffer;
		if (!ident.empty())
			buffer.append(ident).append("@");
		buffer.append(host);
		return buffer;
	}

	Anope::string Template(const Anope::string &ih, const Anope::string &nick)
	{
		if (ih.empty() || ih.find_first_of("{}") == Anope::string::npos)
			return ih;

		NickAlias *na = nullptr;
		if (!nick.empty())
			na = NickAlias::Find(nick);

		auto regdate = [](NickAlias *na)
		{
			auto tm = localtime(&(na->registered));
			char buf[128];
			strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
			return Anope::string(buf);
		};

		// NOTE: when updating this also update dummy_template.
		return Anope::Template(ih, {
			{ "account",  na ? na->nc->display : ""                 },
			{ "nick",     na ? na->nick : ""                        },
			{ "network",  network_name                              },
			{ "regdate",  na ? regdate(na) : ""                     },
			{ "regepoch", na ? Anope::ToString(na->registered) : "" },

		});
	}
}

class HostOffer final
	: public Serializable
{
public:
	Anope::string ident;
	Anope::string host;
	Anope::string creator;
	Anope::string reason;
	time_t created = 0;
	time_t expires = 0;

	HostOffer()
		: Serializable(HOSTOFFER_TYPE)
	{
	}

	HostOffer(const Anope::string &i, const Anope::string &h,  const Anope::string &cr, const Anope::string &r, time_t cd, time_t e)
		: Serializable(HOSTOFFER_TYPE)
		, ident(i)
		, host(h)
		, creator(cr)
		, reason(r)
		, created(cd)
		, expires(e)
	{
	}

	Anope::string GetMask() const
	{
		return GetVHostMask(ident, host);
	}

	~HostOffer();
};

class HostOfferList final
{
protected:
	Serialize::Checker<std::vector<HostOffer *>> offers;

public:
	HostOfferList()
		: offers(HOSTOFFER_TYPE)
	{
	}

	~HostOfferList()
	{
		Clear();
	}

	void Add(HostOffer *ho)
	{
		offers->push_back(ho);
	}

	void Del(HostOffer *ho)
	{
		auto it = std::find(offers->begin(), offers->end(), ho);
		if (it != offers->end())
			offers->erase(it);
	}

	void Clear()
	{
		while (!offers->empty())
			delete offers->back();
	}

	void Expire(const HostOffer *ho)
	{
		Log(Config->GetClient("HostServ"), "expire/offer") << "Expiring vhost offer " << ho->GetMask();
		delete ho;
	}

	size_t GetCount() const
	{
		return offers->size();
	}

	HostOffer *Get(const Anope::string &match)
	{
		for (auto i = offers->size(); i > 0; --i)
		{
			auto* ho = offers->at(i - 1);
			if (ho->expires && ho->expires <= Anope::CurTime)
				Expire(ho);
			else if (match.equals_ci(ho->GetMask()))
				return ho;
		}

		return nullptr;
	}

	HostOffer *Get(size_t number)
	{
		if (number >= offers->size())
			return nullptr;

		auto *ho = offers->at(number);
		if (ho->expires && ho->expires <= Anope::CurTime)
		{
			Expire(ho);
			return nullptr;
		}

		return ho;
	}

	std::vector<HostOffer *> GetAll()
	{
		std::vector<HostOffer *> list;

		for (auto i = offers->size(); i > 0; --i)
		{
			auto *ho = offers->at(i - 1);
			if (ho->expires && ho->expires <= Anope::CurTime)
				Expire(ho);
			else
				list.push_back(ho);
		}

		std::reverse(list.begin(), list.end());
		return list;
	}
};

HostOffer::~HostOffer()
{
	host_offers->Del(this);
}

class HostOfferType final
	: public Serialize::Type
{
public:
	HostOfferType()
		: Serialize::Type(HOSTOFFER_TYPE)
	{
	}

	void Serialize(Serializable *obj, Serialize::Data &data) const override
	{
		const auto *ho = static_cast<const HostOffer *>(obj);
		data["ident"] << ho->ident;
		data["host"] << ho->host;
		data["creator"] << ho->creator;
		data["reason"] << ho->reason;
		data["created"] << ho->created;
		data["expires"] << ho->expires;
	}

	Serializable* Unserialize(Serializable *obj, Serialize::Data &data) const override
	{
		HostOffer *ho;
		if (obj)
			ho = anope_dynamic_static_cast<HostOffer *>(obj);
		else
			ho = new HostOffer();

		data["ident"] >> ho->ident;
		data["host"] >> ho->host;
		data["reason"] >> ho->reason;
		data["creator"] >> ho->creator;
		data["created"] >> ho->created;
		data["expires"] >> ho->expires;

		if (!obj)
			host_offers->Add(ho);

		return ho;
	}
};

class OfferDelCallback final
	: public NumberList
{
private:
	CommandSource &source;
	Command *cmd;
	size_t deleted = 0;
	Anope::string lastdeleted;

public:
	OfferDelCallback(const Anope::string &numlist, CommandSource &_source, Command *c)
		: NumberList(numlist, true)
		, source(_source)
		, cmd(c)
	{
	}

	~OfferDelCallback()
	{
		switch (deleted)
		{
			case 0:
				source.Reply(_("No matching entries on the host offer list."));
				break;

			case 1:
				source.Reply(_("Deleted %s from the host offer list."), lastdeleted.c_str());
				break;

			default:
				source.Reply(deleted, N_("Deleted %zu entry from the host offer list.", "Deleted %zu entries from the host offer list."), deleted);
				break;
		}
	}

	void HandleNumber(unsigned number) override
	{
		if (!number)
			return;

		const auto *ho = host_offers->Get(number - 1);
		if (!ho)
			return;

		deleted++;
		lastdeleted = ho->GetMask();

		Log(LOG_ADMIN, source, cmd) << "to remove " << lastdeleted << " from the host offer list";
		delete ho;
	}
};

class OfferListCallback final
	: public NumberList
{
private:
	CommandSource &source;
	ListFormatter &list;

public:
	OfferListCallback(const Anope::string &numlist, CommandSource &_source, ListFormatter &_list)
		: NumberList(numlist, false)
		, source(_source)
		, list(_list)
	{
	}

	void HandleNumber(unsigned number) override
	{
		if (!number)
			return;

		const auto *ho = host_offers->Get(number - 1);
		if (!ho)
			return;

		ListFormatter::ListEntry entry;
		entry["Number"] = Anope::ToString(number);
		entry["VHost"] = ho->GetMask();
		entry["Reason"] = ho->reason;
		entry["Creator"] = ho->creator;
		entry["Created"] = Anope::strftime(ho->created, source.GetAccount(), true);
		entry["Expires"] = Anope::Expires(ho->expires, source.GetAccount());
		list.AddEntry(entry);
	}
};

class CommandHSOffer final
	: public Command
{
private:
	void DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const size_t expiry_idx = params.size() >= 2 && params[1][0] == '+' ? 1 : 0;
		const size_t vhost_idx = expiry_idx ? 2 : 1;
		const size_t reason_idx = expiry_idx ? 3 : 2;

		if (params.size() <= vhost_idx)
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		time_t expiry = 0;
		if (expiry_idx != 0)
		{
			expiry = Anope::DoTime(params[expiry_idx].substr(1));
			if (expiry < 0)
			{
				source.Reply(BAD_EXPIRY_TIME);
				return;
			}
			else if (expiry)
				expiry += Anope::CurTime;
		}

		const auto &vhost = params[vhost_idx];

		Anope::string ident, host;
		const auto at = vhost.find('@');
		if (at == Anope::string::npos)
			host = vhost;
		else
		{
			ident = vhost.substr(0, at);
			host = vhost.substr(at + 1);
		}

		if (host.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		auto dummy_template = [](const Anope::string &v)
		{
			return Anope::Template(v, {
				{ "account",  "a"          },
				{ "nick",     "a"          },
				{ "network",  network_name },
				{ "regdate",  "1111-11-11" },
				{ "regepoch", "1111111111" },
			});
		};

		if (!ident.empty())
		{
			if (!IRCD->CanSetVIdent)
			{
				source.Reply(HOST_NO_VIDENT);
				return;
			}

			const auto sub_ident = dummy_template(ident);
			if (sub_ident.length() > IRCD->MaxUser)
			{
				source.Reply(HOST_SET_VIDENT_TOO_LONG, IRCD->MaxUser);
				return;
			}
			if (!IRCD->IsIdentValid(sub_ident))
			{
				source.Reply(HOST_SET_VIDENT_ERROR);
				return;
			}
		}

		const auto sub_host = dummy_template(host);
		if (sub_host.length() > IRCD->MaxHost)
		{
			source.Reply(HOST_SET_VHOST_TOO_LONG, IRCD->MaxHost);
			return;
		}
		if (!IRCD->IsHostValid(sub_host))
		{
			source.Reply(HOST_SET_VHOST_ERROR);
			return;
		}

		const auto full_vhost = GetVHostMask(ident, host);
		if (host_offers->Get(full_vhost))
		{
			source.Reply(_("Host offer \002%s\002 already exists."), full_vhost.c_str());
			return;
		}

		Anope::string reason;
		for (auto idx = reason_idx; idx < params.size(); ++idx)
			reason.append(reason.empty() ? "" : " ").append(params[idx]);

		auto *ho = new HostOffer(ident, host, source.GetNick(), reason, Anope::CurTime, expiry);
		host_offers->Add(ho);

		Log(LOG_ADMIN, source, this) << "to add a host offer of " << full_vhost << " (reason: " << reason << ")";
		source.Reply(_("\002%s\002 added to the host offer list."), full_vhost.c_str());
	}

	void DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const auto &match = params.size() > 1 ? params[1] : "";
		if (match.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return;
		}

		if (host_offers->GetCount() == 0)
		{
			source.Reply(_("Host offer list is empty."));
			return;
		}

		if (isdigit(match[0]) && match.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			OfferDelCallback nl_list(match, source, this);
			nl_list.Process();
		}
		else
		{
			const auto *ho = host_offers->Get(match);
			if (!ho)
			{
				source.Reply(_("\002%s\002 not found on the host offer list."), match.c_str());
				return;
			}

			const auto vhost = ho->GetMask();
			Log(LOG_ADMIN, source, this) << "to remove " << vhost << " from the list";
			source.Reply(_("\002%s\002 deleted from the host offer list."), vhost.c_str());

			delete ho;
		}
	}

	void DoList(CommandSource &source, const std::vector<Anope::string> &params, bool view)
	{
		if (host_offers->GetCount() == 0)
		{
			source.Reply(_("Host offer list is empty."));
			return;
		}

		const auto &match = params.size() > 1 ? params[1] : "";

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("VHost")).AddColumn(_("Reason"));
		list.SetFlexible([](ListFormatter::ListEntry &row)
		{
			return row["Reason"].empty()
				? _("{number}: \002{vhost}\002")
				: _("{number}: \002{vhost}\002 ({reason})");
		});
		if (view)
		{
			list.AddColumn(_("Creator")).AddColumn(_("Created")).AddColumn(_("Expires"));
			list.SetFlexible([](ListFormatter::ListEntry &row)
			{
				return row["Reason"].empty()
					? _("{number}: \002{vhost}\002 -- created by {creator} on {created}; {expires}")
					: _("{number}: \002{vhost}\002 -- created by {creator} on {created}; {expires} ({reason})");
			});
		}

		if (!match.empty() && isdigit(match[0]) && match.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			OfferListCallback nl_list(match, source, list);
			nl_list.Process();
		}
		else
		{
			const auto &list_offers = host_offers->GetAll();
			for (size_t i = 0; i < list_offers.size(); ++i)
			{
				const auto *ho = list_offers.at(i);
				const auto vhost = ho->GetMask();
				if (match.empty() || match.equals_ci(vhost) || Anope::Match(vhost, match))
				{
					ListFormatter::ListEntry entry;
					entry["Number"] = Anope::ToString(i + 1);
					entry["VHost"] = vhost;
					entry["Reason"] = ho->reason;
					entry["Creator"] = ho->creator;
					entry["Created"] = Anope::strftime(ho->created, source.GetAccount(), true);
					entry["Expires"] = Anope::Expires(ho->expires, source.GetAccount());
					list.AddEntry(entry);
				}
			}
		}

		if (list.IsEmpty())
			source.Reply(_("No matching entries on the host offer list."));
		else
		{
			source.Reply(_("Current host offer list:"));
			list.SendTo(source);
			source.Reply(_("End of host offer list."));
		}
	}

	void DoClear(CommandSource &source)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		if (host_offers->GetCount() == 0)
		{
			source.Reply(_("Host offer list is empty."));
			return;
		}

		host_offers->Clear();

		Log(LOG_ADMIN, source, this) << "to clear the list";
		source.Reply(_("Host offer list has been cleared."));
	}

public:
	CommandHSOffer(Module *creator)
		: Command(creator, "hostserv/offer", 1, 4)
	{
		this->SetDesc(_("Manipulate the host offer list"));
		this->SetSyntax(_("ADD [+\037expiry\037] \037vhost\037 [\037reason\037]"));
		this->SetSyntax(_("CLEAR"));
		this->SetSyntax(_("DEL {\037vhost\037 | \037entry-num\037 | \037list\037}"));
		this->SetSyntax(_("LIST [\037vhost-mask\037 | \037entry-num\037 | \037list\037]"));
		this->SetSyntax(_("VIEW [\037vhost-mask\037 | \037entry-num\037 | \037list\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const auto &subcmd = params[0];
		if (subcmd.equals_ci("ADD"))
			this->DoAdd(source, params);
		else if (subcmd.equals_ci("CLEAR"))
			this->DoClear(source);
		else if (subcmd.equals_ci("DEL"))
			this->DoDel(source, params);
		else if (subcmd.equals_ci("LIST"))
			this->DoList(source, params, false);
		else if (subcmd.equals_ci("VIEW"))
			this->DoList(source, params, true);
		else
			this->OnSyntaxError(source, subcmd);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Offer stock vhosts to unprivileged users."
				"\n"
				"The \002%s\032ADD\002 command adds an offered vhost. If "
				"\037expiry\037 is provided then the offered vhost will only be "
				"available for a limited time. The \037vhost\037 field may "
				"contain template variables. The supported template variables "
				"are:\n"
				"  \002{account}\002  - Your current account.\n"
				"  \002{network}\002  - The name of this IRC network.\n"
				"  \002{nick}\002     - Your current nickname.\n"
				"  \002{regdate}\002  - The YYYY-MM-DD date at which your nick was registered.\n"
				"  \002{regepoch}\002 - The UNIX time at which your nick was registered.\n"
				"\n"
				"The \002%s\032CLEAR\002 command removes all offered vhosts.\n"
				"\n"
				"The \002%s\032DEL\002 command removes an offered vhost.\n"
				"\n"
				"The \002%s\032LIST\002 command displays the offered vhosts, or "
				"optionally only those offered vhosts which match the given "
				"mask."
				"\n\n"
				"The \002%s\032VIEW\002 command is a more verbose version of "
				"the \002%s\032LIST\002 command."
			),
			source.command.c_str(),
			source.command.c_str(),
			source.command.c_str(),
			source.command.c_str(),
			source.command.c_str(),
			source.command.c_str());
		return true;
	}
};

class OfferListListCallback final
	: public NumberList
{
private:
	CommandSource &source;
	ListFormatter &list;
	bool show_all;

public:
	OfferListListCallback(const Anope::string &numlist, CommandSource &_source, ListFormatter &_list, bool sa)
		: NumberList(numlist, false)
		, source(_source)
		, list(_list)
		, show_all(sa)
	{
	}

	void HandleNumber(unsigned number) override
	{
		if (!number)
			return;

		const auto *ho = host_offers->Get(number - 1);
		if (!ho)
			return;

		const auto &ident = Template(ho->ident, source.GetNick());
		const auto &host = Template(ho->host, source.GetNick());

		auto mask = GetVHostMask(ident, host);
		if ((!ident.empty() && !IRCD->IsIdentValid(ident)) || !IRCD->IsHostValid(host))
		{
			if (!show_all)
				return;
			mask = Anope::Format(Language::Translate(source.GetAccount(), _("%s [Invalid]")), mask.c_str());
		}

		ListFormatter::ListEntry entry;
		entry["Number"] = Anope::ToString(number);
		entry["Offered vhost"] = ho->GetMask();
		entry["Your vhost"] = mask;
		entry["Expires"] = Anope::Expires(ho->expires, source.GetAccount());
		entry["Reason"] = ho->reason;
		list.AddEntry(entry);
	}
};

class CommandHSOfferList final
	: public Command
{
private:
	void DoTake(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		auto *na = NickAlias::Find(source.GetNick());
		if (!na || na->nc != source.GetAccount())
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (source.GetAccount()->HasExt("UNCONFIRMED"))
		{
			source.Reply(_("You must confirm your account before you may take a vhost."));
			return;
		}

		if (take_delay > 0 && na->HasVHost() && na->GetVHostCreated() + take_delay > Anope::CurTime)
		{
			source.Reply(_("Please wait %s before taking a new vhost."), Anope::Duration(take_delay, source.GetAccount()).c_str());
			return;
		}

		const auto &match = params.size() > 1 ? params[1] : "";
		if (match.empty())
		{
			this->OnSyntaxError(source, "TAKE");
			return;
		}

		if (host_offers->GetCount() == 0)
		{
			source.Reply(_("Host offer list is empty."));
			return;
		}

		const HostOffer *ho = nullptr;
		if (match.find_first_not_of("1234567890") == Anope::string::npos)
		{
			const auto number = Anope::TryConvert<size_t>(match);
			if (number)
				ho = host_offers->Get(*number - 1);

			if (!ho)
			{
				source.Reply(_("%s is an invalid host offer entry number."), match.c_str());
				return;
			}
		}
		else
		{
			ho = host_offers->Get(match);
			if (!ho)
			{
				source.Reply(_("\002%s\002 not found on the host offer list."), match.c_str());
				return;
			}
		}

		const auto ident = Template(ho->ident, source.GetNick());
		if (ident.length() > IRCD->MaxUser)
		{
			source.Reply(HOST_SET_VIDENT_TOO_LONG, IRCD->MaxUser);
			return;
		}
		if (!ident.empty() && !IRCD->IsIdentValid(ident))
		{
			source.Reply(HOST_SET_VIDENT_ERROR);
			return;
		}

		const auto host = Template(ho->host, source.GetNick());
		if (host.length() > IRCD->MaxHost)
		{
			source.Reply(HOST_SET_VHOST_TOO_LONG, IRCD->MaxHost);
			return;
		}
		if (!IRCD->IsHostValid(host))
		{
			source.Reply(HOST_SET_VHOST_ERROR);
			return;
		}

		Log(LOG_COMMAND, source, this) << "to take offer " << ho->GetMask() << " and set their vhost to " << GetVHostMask(ident, host);
		na->SetVHost(ident, host, ho->creator);
		FOREACH_MOD(OnSetVHost, (na));
	}

	void DoList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const auto *na = NickAlias::Find(source.GetNick());
		const auto *sourcenc = source.GetAccount();
		if (!na || na->nc != sourcenc)
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (sourcenc->HasExt("UNCONFIRMED"))
		{
			source.Reply(_("You must confirm your account before you can view the host offer list."));
			return;
		}

		if (host_offers->GetCount() == 0)
		{
			source.Reply(_("Host offer list is empty."));
			return;
		}


		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Offered vhost")).AddColumn(_("Your vhost"))
			.AddColumn(_("Expires")).AddColumn(_("Reason"));
		list.SetFlexible([](ListFormatter::ListEntry &row)
		{
			return row["Reason"].empty()
				? _("{number}: \002{offered_vhost}\002 / \002{your_vhost}\002 -- {expires}")
				: _("{number}: \002{offered_vhost}\002 / \002{your_vhost}\002 -- {expires} ({reason})");
		});

		const auto &match = params.size() > 0 ? params[0] : "";
		const auto show_all = params.size() > 1 && params[1].equals_ci("ALL");
		if (!match.empty() && isdigit(match[0]) && match.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			OfferListListCallback nl_list(match, source, list, show_all);
			nl_list.Process();
		}
		else
		{
			const auto &list_offers = host_offers->GetAll();
			for (size_t i = 0; i < list_offers.size(); ++i)
			{
				const auto *ho = list_offers.at(i);
				const auto vhost = ho->GetMask();
				if (match.empty() || match.equals_ci(vhost) || Anope::Match(vhost, match, false, true))
				{
					const auto &ident = Template(ho->ident, source.GetNick());
					const auto &host = Template(ho->host, source.GetNick());

					auto mask = GetVHostMask(ident, host);
					if ((!ident.empty() && !IRCD->IsIdentValid(ident)) || !IRCD->IsHostValid(host))
					{
						if (!show_all)
							continue;
						mask = Anope::Format(Language::Translate(source.GetAccount(), _("%s [Invalid]")), mask.c_str());
					}

					ListFormatter::ListEntry entry;
					entry["Number"] = Anope::ToString(i + 1);
					entry["Offered vhost"] = vhost;
					entry["Your vhost"] = mask;
					entry["Expires"] = Anope::Expires(ho->expires, source.GetAccount());
					entry["Reason"] = ho->reason;
					list.AddEntry(entry);
				}
			}
		}

		if (list.IsEmpty())
			source.Reply(_("No matching entries on the host offer list."));
		else
		{
			source.Reply(_("Current host offer list:"));
			list.SendTo(source);
			source.Reply(_("End of host offer list."));
		}
	}

public:
	CommandHSOfferList(Module *creator)
		: Command(creator, "hostserv/offerlist", 0, 2)
	{
		this->SetDesc(_("List or take a vhost from the host offer list"));
		this->SetSyntax(_("[\037vhost-mask\037 | \037entry-num\037 | \037list\037] [ALL]"));
		this->SetSyntax(_("TAKE {\037vhost\037 | \037entry-num\037}"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() && params[0].equals_ci("TAKE"))
			this->DoTake(source, params);
		else
			this->DoList(source, params);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"List or take an offered vhost.\n"
				"\n"

				"With no parameters, a user@host or host mask, an entry number, or "
				"a list (e.g. 1-3,5) offered vhosts will be shown. If \002ALL\002 "
				"is specified vhosts that can not be taken will be labelled with "
				"\002[Invalid]\002 instead of being omitted."
				"\n"
				"The offered vhosts may contain template variables. The supported "
				"template variables are:\n"
				"  \002{account}\002  - Your current account.\n"
				"  \002{network}\002  - The name of this IRC network.\n"
				"  \002{nick}\002     - Your current nickname.\n"
				"  \002{regdate}\002  - The YYYY-MM-DD date at which your nick was registered.\n"
				"  \002{regepoch}\002 - The UNIX time at which your nick was registered.\n"
				"\n"
				"With \002%s\032TAKE\002 an offered vhost will be applied. You "
				"must specify the offered template variable or the entry number "
				"for the offered vhost you want to use. Once a vhost is taken "
				"you can not take another one for %s."
			),
			source.command.c_str(),
			Anope::Duration(take_delay, source.GetAccount()).c_str());
		return true;
	}
};

class HSOffer final
	: public Module
{
private:
	HostOfferList hostoffers;
	HostOfferType hostoffer_type;
	CommandHSOffer commandhsoffer;
	CommandHSOfferList commandhsofferlist;

public:
	HSOffer(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandhsoffer(this)
		, commandhsofferlist(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
		host_offers = &hostoffers;
	}

	void OnReload(Configuration::Conf &conf) override
	{
		network_name = Config->GetBlock("networkinfo").Get<const Anope::string>("networkname");
		take_delay = conf.GetModule(this).Get<time_t>("takedelay");
	}
};

MODULE_INIT(HSOffer)
