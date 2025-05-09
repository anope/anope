/*
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

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
