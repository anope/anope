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

WebCPanel::NickServ::Confirm::Confirm(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::NickServ::Confirm::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	const auto code = message.post_data["code"].trim();
	if (code.empty())
		replacements["MESSAGES"] = "You must specify a confirmation code";
	else
	{
		std::vector<Anope::string> params;
		params.push_back(code);

		WebPanel::RunCommand(client, na->nc->display, na->nc, "NickServ", "nickserv/confirm/register", params, replacements);
	}

	TemplateFileServer page("nickserv/confirm.html");

	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}
