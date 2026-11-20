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

WebCPanel::ChanServ::Drop::Drop(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage (cat, u)
{
}


bool WebCPanel::ChanServ::Drop::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	const Anope::string channel = WebPanel::GetRequestData(message.post_data, "channel");
	const Anope::string confchan = WebPanel::GetRequestData(message.post_data, "confChan");
	if (!channel.empty() && !confchan.empty())
	{
		if (channel == confchan)
		{
			std::vector<Anope::string> params;
			params.push_back(channel);
			params.push_back(channel);

			WebPanel::RunCommand(client, na->nc->display, na->nc, "ChanServ", "chanserv/drop", params, replacements);
		}
		else
			replacements["MESSAGES"] = "Invalid Confirmation";
	}

	std::deque<ChannelInfo *> queue;
	na->nc->GetChannelReferences(queue);
	for (auto *ci : queue)
	{
		if ((ci->HasExt("SECUREFOUNDER") ? ci->AccessFor(na->nc).founder : ci->AccessFor(na->nc).HasPriv("FOUNDER")) || (na->nc->IsServicesOper() && na->nc->o->ot->HasCommand("chanserv/drop")))
		{
			replacements["CHANNEL_NAMES"] = ci->name;
			replacements["ESCAPED_CHANNEL_NAMES"] = HTTP::URLEncode(ci->name);
		}
	}

	const Anope::string chname = WebPanel::GetRequestData(message.get_data, "channel");
	if (!chname.empty())
	{
		replacements["CHANNEL_DROP"] = chname;
		replacements["ESCAPED_CHANNEL"] = HTTP::URLEncode(chname);
	}

	TemplateFileServer page("chanserv/drop.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;

}
