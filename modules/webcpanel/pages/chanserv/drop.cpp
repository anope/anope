/*
 * (C) 2003-2026 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"
#include "utils.h"

WebCPanel::ChanServ::Drop::Drop(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage (cat, u)
{
}


bool WebCPanel::ChanServ::Drop::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	const Anope::string channel = WebPanel::GetRequestData(message.post_data, "channel");
	const Anope::string confchan = WebPanel::GetRequestData(message.post_data, "confChan");
	if (!channel.empty() && !confchan.empty())
	{
		if (channel == confchan)
		{
			std::vector<Anope::string> params;
			params.push_back(channel);
			params.push_back(channel);

			WebPanel::RunCommand(client, na->nc->display, na->nc, "ChanServ", "chanserv/drop", params, replacements);
		}
		else
			replacements["MESSAGES"] = "Invalid Confirmation";
	}

	std::deque<ChannelInfo *> queue;
	na->nc->GetChannelReferences(queue);
	for (unsigned i = 0; i < queue.size(); ++i)
	{
		ChannelInfo *ci = queue[i];
		if ((ci->HasExt("SECUREFOUNDER") ? ci->AccessFor(na->nc).founder : ci->AccessFor(na->nc).HasPriv("FOUNDER")) || (na->nc->IsServicesOper() && na->nc->o->ot->HasCommand("chanserv/drop")))
		{
			replacements["CHANNEL_NAMES"] = ci->name;
			replacements["ESCAPED_CHANNEL_NAMES"] = HTTPUtils::URLEncode(ci->name);
		}
	}

	const Anope::string chname = WebPanel::GetRequestData(message.get_data, "channel");
	if (!chname.empty())
	{
		replacements["CHANNEL_DROP"] = chname;
		replacements["ESCAPED_CHANNEL"] = HTTPUtils::URLEncode(chname);
	}

	TemplateFileServer page("chanserv/drop.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;

}
