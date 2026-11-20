/*
 * (C) 2003-2026 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

WebCPanel::HostServ::Request::Request(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage (cat, u)
{
}

bool WebCPanel::HostServ::Request::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{

	const Anope::string req = WebPanel::GetRequestData(message.post_data, "req");
	if (!req.empty())
	{
		std::vector<Anope::string> params;
		params.push_back(req);

		WebPanel::RunCommand(client, na->nc->display, na->nc, "HostServ", "hostserv/request", params, replacements, "CMDR");
	}

	if (na->HasVhost())
	{
		if (na->GetVhostIdent().empty() == false)
			replacements["VHOST"] = na->GetVhostIdent() + "@" + na->GetVhostHost();
		else
			replacements["VHOST"] = na->GetVhostHost();
	}
	if (ServiceReference<Command>("Command", "hostserv/request"))
		replacements["CAN_REQUEST"] = "YES";
	TemplateFileServer page("hostserv/request.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}
