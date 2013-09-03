/*
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

void BuildChanlist(const Anope::string &page_name, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	std::deque<ChannelInfo *> queue;
	na->nc->GetChannelReferences(queue);
	for (unsigned i = 0; i < queue.size(); ++i)
	{
		ChannelInfo *ci = queue[i];
		replacements["CHANNEL_NAMES"] = ci->name;
		replacements["ESCAPED_CHANNEL_NAMES"] = HTTPUtils::URLEncode(ci->name);
	}
	replacements["PAGE"] = page_name;
	return;
}

bool ServePage(const Anope::string &page, HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, TemplateFileServer::Replacements &replacements)
{
	TemplateFileServer Page(page);
	Page.Serve(server, page_name, client, message, reply, replacements);
	return 1;
}