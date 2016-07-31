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

#include "../webcpanel.h"

bool WebCPanel::Confirm::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply)
{
	TemplateFileServer::Replacements replacements;
	const Anope::string &user = message.post_data["username"], &pass = message.post_data["password"], &email = message.post_data["email"];

	replacements["TITLE"] = page_title;

	if (!user.empty() && !pass.empty())
	{
		std::vector<Anope::string> params;
		params.push_back(pass);
		if (!email.empty())
			params.push_back(email);

		WebPanel::RunCommand(user, NULL, "NickServ", "nickserv/register", params, replacements);
	}

	TemplateFileServer page("confirm.html");

	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

