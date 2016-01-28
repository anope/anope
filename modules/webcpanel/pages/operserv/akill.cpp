/*
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

WebCPanel::OperServ::Akill::Akill(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::OperServ::Akill::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{

	static ServiceReference<XLineManager> akills("XLineManager","xlinemanager/sgline");

	if (!na->nc->o || !na->nc->o->ot->HasCommand("operserv/akill"))
	{
		replacements["NOACCESS"];
	}
	else
	{
		if (akills->GetCount() == 0)
			replacements["AKILLS"] = "No Akills to display.";

		if (message.post_data.count("mask") > 0 && message.post_data.count("expiry") > 0 && message.post_data.count("reason") > 0)
		{
			std::vector<Anope::string> params;
			std::stringstream cmdstr;
			params.push_back("ADD");
			cmdstr << "+" << HTTPUtils::URLDecode(message.post_data["expiry"]);
			cmdstr << " " << HTTPUtils::URLDecode(message.post_data["mask"]);
			cmdstr << " " << HTTPUtils::URLDecode(message.post_data["reason"]);
			params.push_back(cmdstr.str());
			WebPanel::RunCommand(na->nc->display, na->nc, "OperServ", "operserv/akill", params, replacements);
		}

		if (message.get_data["del"] == "1" && message.get_data.count("number") > 0)
		{
			std::vector<Anope::string> params;
			params.push_back("DEL");
			params.push_back(HTTPUtils::URLDecode(message.get_data["number"]));
			WebPanel::RunCommand(na->nc->display, na->nc, "OperServ", "operserv/akill", params, replacements);
		}

		for (unsigned i = 0, end = akills->GetCount(); i < end; ++i)
		{
			const XLine *x = akills->GetEntry(i);
			replacements["NUMBER"] = stringify(i + 1);
			replacements["HOST"] = x->mask;
			replacements["SETTER"] = x->by;
			replacements["TIME"] = Anope::strftime(x->created, NULL, true);
			replacements["EXPIRE"] = Anope::Expires(x->expires, na->nc);
			replacements["REASON"] = x->reason;
		}
	}

	TemplateFileServer page("operserv/akill.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

