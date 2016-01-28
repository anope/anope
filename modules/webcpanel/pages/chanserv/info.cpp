/*
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"
#include "utils.h"

WebCPanel::ChanServ::Info::Info(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::ChanServ::Info::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	const Anope::string &chname = message.get_data["channel"];

	if (!chname.empty())
		replacements["ESCAPED_CHANNEL"] = HTTPUtils::URLEncode(chname);

	BuildChanList(na, replacements);

	TemplateFileServer page("chanserv/main.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

