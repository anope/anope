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

WebCPanel::HostServ::Request::Request(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage (cat, u)
{
}

bool WebCPanel::HostServ::Request::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	if (message.post_data.count("req") > 0)
	{
		std::vector<Anope::string> params;
		params.push_back(HTTP::URLDecode(message.post_data["req"]));

		WebPanel::RunCommand(client, na->nc->display, na->nc, "HostServ", "hostserv/request", params, replacements, "CMDR");
	}

	if (na->HasVHost())
		replacements["VHOST"] = na->GetVHostMask();
	if (ServiceReference<Command>("Command", "hostserv/request"))
		replacements["CAN_REQUEST"] = "YES";
	TemplateFileServer page("hostserv/request.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}
