/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2016 Anope Team <team@anope.org>
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

#include "../../webcpanel.h"

static bool ChannelSort(ChanServ::Channel *ci1, ChanServ::Channel *ci2)
{
	return ci::less()(ci1->GetName(), ci2->GetName());
}

WebCPanel::NickServ::Alist::Alist(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::NickServ::Alist::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{
	std::vector<::ChanServ::Channel *> chans = na->GetAccount()->GetRefs<::ChanServ::Channel *>();
	std::sort(chans.begin(), chans.end(), ChannelSort);

	int chan_count = 0;

	for (::ChanServ::Channel *ci : chans)
	{
		if (ci->GetFounder() == na->GetAccount())
		{
			++chan_count;

			replacements["NUMBERS"] = stringify(chan_count);
			replacements["CHANNELS"] = (ci->IsNoExpire() ? "!" : "") + ci->GetName();
			replacements["ACCESSES"] = "Founder";
			continue;
		}

		::ChanServ::AccessGroup access = ci->AccessFor(na->GetAccount());
		if (access.empty())
			continue;

		++chan_count;

		replacements["NUMBERS"] = stringify(chan_count);
		replacements["CHANNELS"] = (ci->IsNoExpire() ? "!" : "") + ci->GetName();
		Anope::string access_str;
		for (unsigned i = 0; i < access.size(); ++i)
			access_str += ", " + access[i]->AccessSerialize();
		replacements["ACCESSES"] = access_str.substr(2);
	}

	TemplateFileServer page("nickserv/alist.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

