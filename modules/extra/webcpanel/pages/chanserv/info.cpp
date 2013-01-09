/*
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

WebCPanel::ChanServ::Info::Info(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::ChanServ::Info::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	// XXX this is slightly inefficient
	for (registered_channel_map::const_iterator it = RegisteredChannelList->begin(), it_end = RegisteredChannelList->end(); it != it_end; ++it)
	{
		ChannelInfo *ci = it->second;

		if (ci->AccessFor(na->nc).HasPriv("SET") || ci->AccessFor(na->nc).HasPriv("ACCESS_LIST"))
		{
			replacements["CHANNEL_NAMES"] = ci->name;
			replacements["ESCAPED_CHANNEL_NAMES"] = HTTPUtils::URLEncode(ci->name);
		}
	}

	TemplateFileServer page("chanserv/main.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

