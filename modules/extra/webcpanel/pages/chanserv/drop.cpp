/*
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

WebCPanel::ChanServ::Drop::Drop(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage (cat, u)
{
}


bool WebCPanel::ChanServ::Drop::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{

	if (message.post_data.count("channel") > 0 && message.post_data.count("confChan") > 0)
	{
		if (message.post_data["channel"] == message.post_data["confChan"]) {
			std::vector<Anope::string> params;
			params.push_back(HTTPUtils::URLDecode(message.post_data["channel"]));

			WebPanel::RunCommand(na->nc->display, na->nc, Config->ChanServ, "chanserv/drop", params, replacements);
		}
		else replacements["MESSAGES"] = "Invalid Confirmation";
	}
	// XXX this is slightly inefficient
	for (registered_channel_map::const_iterator it = RegisteredChannelList->begin(), it_end = RegisteredChannelList->end(); it != it_end; ++it)
	{
		ChannelInfo *ci = it->second;
		if ((ci->HasFlag(CI_SECUREFOUNDER) ? ci->AccessFor(na->nc).founder : ci->AccessFor(na->nc).HasPriv("FOUNDER")) || (na->nc->IsServicesOper() && na->nc->o->ot->HasCommand("chanserv/drop")))
		{
			replacements["CHANNEL_NAMES"] = ci->name;
			replacements["ESCAPED_CHANNEL_NAMES"] = HTTPUtils::URLEncode(ci->name);
		}
	}

	if (message.get_data.count("channel") > 0) {
		replacements["CHANNEL_DROP"] = message.get_data["channel"];
	}
	TemplateFileServer page("chanserv/drop.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}
