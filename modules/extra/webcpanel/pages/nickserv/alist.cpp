/*
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

WebCPanel::NickServ::Alist::Alist(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

void WebCPanel::NickServ::Alist::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	int chan_count = 0;

	for (registered_channel_map::const_iterator it = RegisteredChannelList->begin(), it_end = RegisteredChannelList->end(); it != it_end; ++it)
	{
		ChannelInfo *ci = it->second;

		if (ci->GetFounder() && ci->GetFounder() == na->nc)
		{
			++chan_count;

			replacements["NUMBERS"] = stringify(chan_count);
			replacements["CHANNELS"] = (ci->HasFlag(CI_NO_EXPIRE) ? "!" : "") + ci->name;
			replacements["ACCESSES"] = "Founder";
			continue;
		}

		AccessGroup access = ci->AccessFor(na->nc);
		if (access.empty())
			continue;
				
		++chan_count;

		replacements["NUMBERS"] = stringify(chan_count);
		replacements["CHANNELS"] = (ci->HasFlag(CI_NO_EXPIRE) ? "!" : "") + ci->name;
		Anope::string access_str;
		for (unsigned i = 0; i < access.size(); ++i)
			access_str += ", " + access[i]->Serialize();
		replacements["ACCESSES"] = access_str.substr(2);
	}

	TemplateFileServer page("nickserv/alist.html");
	page.Serve(server, page_name, client, message, reply, replacements);
}

