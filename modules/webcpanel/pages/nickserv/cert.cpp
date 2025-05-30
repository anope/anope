/*
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"
#include "modules/nickserv/cert.h"

WebCPanel::NickServ::Cert::Cert(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::NickServ::Cert::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	if (message.post_data.count("certfp") > 0)
	{
		std::vector<Anope::string> params;
		params.emplace_back("ADD");
		params.push_back(message.post_data["certfp"]);

		WebPanel::RunCommand(client, na->nc->display, na->nc, "NickServ", "nickserv/cert", params, replacements);
	}
	else if (message.get_data.count("del") > 0 && message.get_data.count("mask") > 0)
	{
		std::vector<Anope::string> params;
		params.emplace_back("DEL");
		params.push_back(message.get_data["mask"]);

		WebPanel::RunCommand(client, na->nc->display, na->nc, "NickServ", "nickserv/cert", params, replacements);
	}

	NSCertList *cl = na->nc->GetExt<NSCertList>("certificates");
	if (cl)
		for (unsigned i = 0; i < cl->GetCertCount(); ++i)
			replacements["CERTS"] = cl->GetCert(i);

	TemplateFileServer page("nickserv/cert.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}
