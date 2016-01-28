/*
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
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

