/*
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

WebCPanel::NickServ::Confirm::Confirm(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::NickServ::Confirm::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{

	std::vector<Anope::string> params;
	params.push_back(message.post_data["code"]);

	WebPanel::RunCommand(client, na->nc->display, na->nc, "NickServ", "nickserv/confirm", params, replacements);

	TemplateFileServer page("nickserv/confirm.html");

	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}
