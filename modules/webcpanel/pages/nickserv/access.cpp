/*
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

WebCPanel::NickServ::Access::Access(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::NickServ::Access::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	if (message.post_data.count("access") > 0)
	{
		std::vector<Anope::string> params;
		params.push_back("ADD");
		params.push_back(message.post_data["access"]);

		WebPanel::RunCommand(na->nc->display, na->nc, "NickServ", "nickserv/access", params, replacements);
	}
	else if (message.get_data.count("del") > 0 && message.get_data.count("mask") > 0)
	{
		std::vector<Anope::string> params;
		params.push_back("DEL");
		params.push_back(message.get_data["mask"]);

		WebPanel::RunCommand(na->nc->display, na->nc, "NickServ", "nickserv/access", params, replacements);
	}

	for (unsigned i = 0; i < na->nc->access.size(); ++i)
		replacements["ACCESS"] = na->nc->access[i];

	TemplateFileServer page("nickserv/access.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

