/*
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

namespace
{
	bool ChannelSort(ChannelInfo *ci1, ChannelInfo *ci2)
	{
		return ci::less()(ci1->name, ci2->name);
	}
}

namespace WebCPanel
{

namespace ChanServ
{

void BuildChanList(NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	std::deque<ChannelInfo *> queue;
	na->nc->GetChannelReferences(queue);
	std::sort(queue.begin(), queue.end(), ChannelSort);

	for (unsigned i = 0; i < queue.size(); ++i)
	{
		ChannelInfo *ci = queue[i];

		if (na->nc != ci->GetFounder() && ci->AccessFor(na->nc).empty())
			continue;

		replacements["CHANNEL_NAMES"] = ci->name;
		replacements["ESCAPED_CHANNEL_NAMES"] = HTTPUtils::URLEncode(ci->name);
	}
}

}

}

