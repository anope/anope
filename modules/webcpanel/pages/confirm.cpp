/*
 * (C) 2003-2026 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../webcpanel.h"

bool WebCPanel::Confirm::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply)
{
	TemplateFileServer::Replacements replacements;
	replacements["TITLE"] = page_title;

	if (!WebPanel::IsAllowedOrigin(message))
	{
		reply.error = HTTP_BAD_REQUEST;
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
