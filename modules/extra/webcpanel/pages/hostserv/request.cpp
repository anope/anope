/*
 * (C) 2003-2012 Anope Team
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
	if (message.post_data.count("req") > 0)
	{
		std::vector<Anope::string> params;
		std::stringstream cmdstr;

		cmdstr << HTTPUtils::URLDecode(message.post_data["req"]);
		params.push_back(cmdstr.str());
		WebPanel::RunCommand(na->nc->display, na->nc, Config->HostServ, "hostserv/request", params, replacements);
	}

	if (na->HasVhost())
	{
		if (na->GetVhostIdent().empty() == false)
			replacements["VHOST"] = na->GetVhostIdent() + "@" + na->GetVhostHost();
		else
			replacements["VHOST"] = na->GetVhostHost();
	}
	TemplateFileServer page("hostserv/request.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}
