/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"
#include "modules/nickserv/access.h"

WebCPanel::NickServ::Access::Access(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::NickServ::Access::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{
	if (message.post_data.count("access") > 0)
	{
		std::vector<Anope::string> params;
		params.push_back("ADD");
		params.push_back(message.post_data["access"]);

		WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "NickServ", "nickserv/access", params, replacements);
	}
	else if (message.get_data.count("del") > 0 && message.get_data.count("mask") > 0)
	{
		std::vector<Anope::string> params;
		params.push_back("DEL");
		params.push_back(message.get_data["mask"]);

		WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "NickServ", "nickserv/access", params, replacements);
	}

	for (NickAccess *a : na->GetAccount()->GetRefs<NickAccess *>())
		replacements["ACCESS"] = a->GetMask();

	TemplateFileServer page("nickserv/access.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

