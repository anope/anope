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

namespace WebCPanel
{

namespace ChanServ
{

void BuildChanList(NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	std::deque<ChannelInfo *> queue;
	na->nc->GetChannelReferences(queue);
	std::sort(queue.begin(), queue.end(), [](auto *lhs, auto *rhs) {
		return ci::less()(lhs->name, rhs->name);
	});

	for (auto *ci : queue)
	{
		if (na->nc != ci->GetFounder() && ci->AccessFor(na->nc).empty())
			continue;

		replacements["CHANNEL_NAMES"] = ci->name;
		replacements["ESCAPED_CHANNEL_NAMES"] = HTTP::URLEncode(ci->name);
	}
}

}

}
