/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

static bool ChannelSort(ChanServ::Channel *ci1, ChanServ::Channel *ci2)
{
	return ci::less()(ci1->GetName(), ci2->GetName());
}

WebCPanel::NickServ::Alist::Alist(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::NickServ::Alist::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{
	std::vector<::ChanServ::Channel *> chans = na->GetAccount()->GetRefs<::ChanServ::Channel *>(::ChanServ::channel);
	std::sort(chans.begin(), chans.end(), ChannelSort);

	int chan_count = 0;

	for (::ChanServ::Channel *ci : chans)
	{
		if (ci->GetFounder() == na->GetAccount())
		{
			++chan_count;

			replacements["NUMBERS"] = stringify(chan_count);
			replacements["CHANNELS"] = (ci->HasFieldS("CS_NO_EXPIRE") ? "!" : "") + ci->GetName();
			replacements["ACCESSES"] = "Founder";
			continue;
		}

		::ChanServ::AccessGroup access = ci->AccessFor(na->GetAccount());
		if (access.empty())
			continue;

		++chan_count;

		replacements["NUMBERS"] = stringify(chan_count);
		replacements["CHANNELS"] = (ci->HasFieldS("CS_NO_EXPIRE") ? "!" : "") + ci->GetName();
		Anope::string access_str;
		for (unsigned i = 0; i < access.size(); ++i)
			access_str += ", " + access[i]->AccessSerialize();
		replacements["ACCESSES"] = access_str.substr(2);
	}

	TemplateFileServer page("nickserv/alist.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

