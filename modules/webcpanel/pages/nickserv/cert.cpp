/*
 * (C) 2003-2026 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"
#include "modules/ns_cert.h"

WebCPanel::NickServ::Cert::Cert(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::NickServ::Cert::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	const Anope::string certfp = WebPanel::GetRequestData(message.post_data, "certfp");
	const Anope::string mask = WebPanel::GetRequestData(message.get_data, "mask");
	if (!certfp.empty())
	{
		std::vector<Anope::string> params;
		params.push_back("ADD");
		params.push_back(certfp);

		WebPanel::RunCommand(client, na->nc->display, na->nc, "NickServ", "nickserv/cert", params, replacements);
	}
	else if (message.get_data.count("del") && !mask.empty())
	{
		std::vector<Anope::string> params;
		params.push_back("DEL");
		params.push_back(mask);

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
