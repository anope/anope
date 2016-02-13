/*
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

static bool ChannelSort(ChannelInfo *ci1, ChannelInfo *ci2)
{
	return ci::less()(ci1->name, ci2->name);
}

WebCPanel::NickServ::Alist::Alist(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::NickServ::Alist::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	std::deque<ChannelInfo *> queue;
	na->nc->GetChannelReferences(queue);
	std::sort(queue.begin(), queue.end(), ChannelSort);

	int chan_count = 0;

	for (unsigned q = 0; q < queue.size(); ++q)
	{
		ChannelInfo *ci = queue[q];

		if (ci->GetFounder() == na->nc)
		{
			++chan_count;

			replacements["NUMBERS"] = stringify(chan_count);
			replacements["CHANNELS"] = (ci->HasExt("CS_NO_EXPIRE") ? "!" : "") + ci->name;
			replacements["ACCESSES"] = "Founder";
			continue;
		}

		AccessGroup access = ci->AccessFor(na->nc);
		if (access.empty())
			continue;
				
		++chan_count;

		replacements["NUMBERS"] = stringify(chan_count);
		replacements["CHANNELS"] = (ci->HasExt("CS_NO_EXPIRE") ? "!" : "") + ci->name;

		const ChanAccess *highest = access.Highest();
		replacements["ACCESSES"] = highest ? highest->AccessSerialize() : "";
	}

	TemplateFileServer page("nickserv/alist.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

