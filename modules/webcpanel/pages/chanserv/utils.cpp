/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

namespace
{
	bool ChannelSort(ChanServ::Channel *ci1, ChanServ::Channel *ci2)
	{
		return ci::less()(ci1->GetName(), ci2->GetName());
	}
}

namespace WebCPanel
{

namespace ChanServ
{

void BuildChanList(::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{
	std::vector<::ChanServ::Channel *> chans = na->GetAccount()->GetRefs<::ChanServ::Channel *>(::ChanServ::channel);
	std::sort(chans.begin(), chans.end(), ChannelSort);

	for (::ChanServ::Channel *ci : chans)
	{
		if (na->GetAccount() != ci->GetFounder() && ci->AccessFor(na->GetAccount()).empty())
			continue;

		replacements["CHANNEL_NAMES"] = ci->GetName();
		replacements["ESCAPED_CHANNEL_NAMES"] = HTTPUtils::URLEncode(ci->GetName());
	}
}

}

}

