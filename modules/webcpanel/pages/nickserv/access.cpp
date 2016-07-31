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
#include "modules/nickserv/access.h"

WebCPanel::NickServ::Access::Access(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::NickServ::Access::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{
	if (message.post_data.count("access") > 0)
	{
		std::vector<Anope::string> params;
		params.push_back("ADD");
		params.push_back(message.post_data["access"]);

		WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "NickServ", "nickserv/access", params, replacements);
	}
	else if (message.get_data.count("del") > 0 && message.get_data.count("mask") > 0)
	{
		std::vector<Anope::string> params;
		params.push_back("DEL");
		params.push_back(message.get_data["mask"]);

		WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "NickServ", "nickserv/access", params, replacements);
	}

	for (NickAccess *a : na->GetAccount()->GetRefs<NickAccess *>())
		replacements["ACCESS"] = a->GetMask();

	TemplateFileServer page("nickserv/access.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

