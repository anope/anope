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

#include "../webcpanel.h"

bool WebCPanel::Confirm::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply)
{
	TemplateFileServer::Replacements replacements;
	replacements["TITLE"] = page_title;

	if (!WebPanel::IsAllowedOrigin(message))
	{
		reply.error = HTTP::BAD_REQUEST;
		reply.Write("Invalid request origin.");
		return true;
	}

	const Anope::string user = WebPanel::GetRequestData(message.post_data, "username");
	const Anope::string pass = WebPanel::GetRequestData(message.post_data, "password");
	if (!user.empty() && !pass.empty())
	{
		std::vector<Anope::string> params;
		params.push_back(pass);
		const Anope::string email = WebPanel::GetRequestData(message.post_data, "email");
		if (!email.empty())
			params.push_back(email);

		WebPanel::RunCommand(client, user, NULL, "NickServ", "nickserv/register", params, replacements);
	}

	TemplateFileServer page("confirm.html");

	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}
