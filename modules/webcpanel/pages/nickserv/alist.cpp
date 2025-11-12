// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
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

WebCPanel::NickServ::Alist::Alist(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::NickServ::Alist::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	std::deque<ChannelInfo *> queue;
	na->nc->GetChannelReferences(queue);
	std::sort(queue.begin(), queue.end(), [](auto *lhs, auto *rhs) {
		return ci::less()(lhs->name, rhs->name);
	});

	int chan_count = 0;

	for (auto *ci : queue)
	{
		if (ci->GetFounder() == na->nc)
		{
			++chan_count;

			replacements["NUMBERS"] = Anope::ToString(chan_count);
			replacements["CHANNELS"] = (ci->HasExt("CS_NO_EXPIRE") ? "!" : "") + ci->name;
			replacements["ACCESSES"] = "Founder";
			continue;
		}

		AccessGroup access = ci->AccessFor(na->nc);
		if (access.empty())
			continue;

		++chan_count;

		replacements["NUMBERS"] = Anope::ToString(chan_count);
		replacements["CHANNELS"] = (ci->HasExt("CS_NO_EXPIRE") ? "!" : "") + ci->name;

		const ChanAccess *highest = access.Highest();
		replacements["ACCESSES"] = highest ? highest->AccessSerialize() : "";
	}

	TemplateFileServer page("nickserv/alist.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}
