/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

WebCPanel::OperServ::Akill::Akill(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u), akills("xlinemanager/sgline")
{
}

bool WebCPanel::OperServ::Akill::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{
	if (!na->GetAccount()->o || !na->GetAccount()->o->GetType()->HasCommand("operserv/akill"))
	{
		replacements["NOACCESS"];
	}
	else
	{
		std::vector<XLine *> xlines = akills->GetXLines();
		if (xlines.empty())
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
			WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "OperServ", "operserv/akill", params, replacements);
		}

		if (message.get_data["del"] == "1" && message.get_data.count("number") > 0)
		{
			std::vector<Anope::string> params;
			params.push_back("DEL");
			params.push_back(HTTPUtils::URLDecode(message.get_data["number"]));
			WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "OperServ", "operserv/akill", params, replacements);
		}

		unsigned int i = 0;
		for (XLine *x : xlines)
		{
			replacements["NUMBER"] = stringify(++i);
			replacements["HOST"] = x->GetMask();
			replacements["SETTER"] = x->GetBy();
			replacements["TIME"] = Anope::strftime(x->GetCreated(), NULL, true);
			replacements["EXPIRE"] = Anope::Expires(x->GetExpires(), na->GetAccount());
			replacements["REASON"] = x->GetReason();
		}
	}

	TemplateFileServer page("operserv/akill.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

