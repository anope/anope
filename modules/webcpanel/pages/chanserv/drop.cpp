/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2017 Anope Team <team@anope.org>
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
#include "utils.h"

WebCPanel::ChanServ::Drop::Drop(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage (cat, u)
{
}


bool WebCPanel::ChanServ::Drop::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{

	if (message.post_data.count("channel") > 0 && message.post_data.count("confChan") > 0)
	{
		if (message.post_data["channel"] == message.post_data["confChan"])
		{
			std::vector<Anope::string> params;
			const Anope::string &channel = HTTPUtils::URLDecode(message.post_data["channel"]);
			params.push_back(channel);
			params.push_back(channel);

			WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "ChanServ", "chanserv/drop", params, replacements);
		}
		else
			replacements["MESSAGES"] = "Invalid Confirmation";
	}

	for (::ChanServ::Channel *ci : na->GetAccount()->GetRefs<::ChanServ::Channel *>())
		if ((ci->IsSecureFounder() ? ci->AccessFor(na->GetAccount()).founder : ci->AccessFor(na->GetAccount()).HasPriv("FOUNDER")) || (na->GetAccount()->GetOper() && na->GetAccount()->GetOper()->HasCommand("chanserv/drop")))
		{
			replacements["CHANNEL_NAMES"] = ci->GetName();
			replacements["ESCAPED_CHANNEL_NAMES"] = HTTPUtils::URLEncode(ci->GetName());
		}

	if (message.get_data.count("channel") > 0)
	{
		const Anope::string &chname = message.get_data["channel"];

		replacements["CHANNEL_DROP"] = chname;
		replacements["ESCAPED_CHANNEL"] = HTTPUtils::URLEncode(chname);
	}

	TemplateFileServer page("chanserv/drop.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;

}
