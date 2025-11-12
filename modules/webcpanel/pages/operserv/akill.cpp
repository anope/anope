// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "../../webcpanel.h"

WebCPanel::OperServ::Akill::Akill(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::OperServ::Akill::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
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
			params.emplace_back("ADD");
			cmdstr << "+" << HTTP::URLDecode(message.post_data["expiry"]);
			cmdstr << " " << HTTP::URLDecode(message.post_data["mask"]);
			cmdstr << " " << HTTP::URLDecode(message.post_data["reason"]);
			params.emplace_back(cmdstr.str());
			WebPanel::RunCommand(client, na->nc->display, na->nc, "OperServ", "operserv/akill", params, replacements);
		}

		if (message.get_data["del"] == "1" && message.get_data.count("number") > 0)
		{
			std::vector<Anope::string> params;
			params.emplace_back("DEL");
			params.push_back(HTTP::URLDecode(message.get_data["number"]));
			WebPanel::RunCommand(client, na->nc->display, na->nc, "OperServ", "operserv/akill", params, replacements);
		}

		for (unsigned i = 0, end = akills->GetCount(); i < end; ++i)
		{
			const XLine *x = akills->GetEntry(i);
			replacements["NUMBER"] = Anope::ToString(i + 1);
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
