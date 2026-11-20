// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
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

#include "../../webcpanel.h"
#include "modules/chanserv/akick.h"

WebCPanel::ChanServ::Akick::Akick(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::ChanServ::Akick::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	TemplateFileServer Page("chanserv/akick.html");

	BuildChanList(na, replacements);

	const Anope::string chname = WebPanel::GetRequestData(message.get_data, "channel");
	if (chname.empty())
	{
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	ChannelInfo *ci = ChannelInfo::Find(chname);

	if (!ci)
	{
		replacements["MESSAGES"] = "Channel not registered";
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	AccessGroup u_access = ci->AccessFor(na->nc);
	bool has_priv = na->nc->IsServicesOper() && na->nc->o->ot->HasPriv("chanserv/access/modify");

	if (!u_access.HasPriv("AKICK") && !has_priv)
	{
		replacements["MESSAGES"] = "Permission denied.";
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	replacements["AKICK"] = "YES";

	const Anope::string mask = WebPanel::GetRequestData(message.get_data, "mask");
	const Anope::string pmask = WebPanel::GetRequestData(message.post_data, "mask");
	if (message.get_data.count("del") && !mask.empty())
	{
		std::vector<Anope::string> params;
		params.push_back(ci->name);
		params.push_back("DEL");
		params.push_back(mask);

		WebPanel::RunCommand(client, na->nc->display, na->nc, "ChanServ", "chanserv/akick", params, replacements);
	}
	else if (!pmask.empty())
	{
		std::vector<Anope::string> params;
		params.push_back(ci->name);
		params.push_back("ADD");
		params.push_back(pmask);

		const Anope::string reason = WebPanel::GetRequestData(message.post_data, "reason");
		if (!reason.empty())
			params.push_back(reason);

		WebPanel::RunCommand(client, na->nc->display, na->nc, "ChanServ", "chanserv/akick", params, replacements);
	}

	replacements["ESCAPED_CHANNEL"] = HTTP::URLEncode(chname);

	if (::ChanServ::akick_service)
	{
		for (unsigned i = 0; i < ::ChanServ::akick_service->GetAKickCount(ci); ++i)
		{
			const auto *akick = ::ChanServ::akick_service->GetAKick(ci, i);

			if (akick->nc)
				replacements["MASKS"] = akick->nc->display;
			else
				replacements["MASKS"] = akick->mask;
			replacements["CREATORS"] = akick->creator;
			replacements["REASONS"] = akick->reason;
		}
	}

	Page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

std::set<Anope::string> WebCPanel::ChanServ::Akick::GetData()
{
	std::set<Anope::string> v;
	v.insert("channel");
	return v;
}
