/*
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

WebCPanel::ChanServ::Drop::Drop(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage (cat, u)
{
}


bool WebCPanel::ChanServ::Drop::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{

	if (message.post_data.count("channel") > 0 && message.post_data.count("confChan") > 0)
	{
		if (message.post_data["channel"] == message.post_data["confChan"])
		{
			std::vector<Anope::string> params;
			const Anope::string &channel = HTTP::URLDecode(message.post_data["channel"]);
			params.push_back(channel);
			params.push_back(channel);

			WebPanel::RunCommand(client, na->nc->display, na->nc, "ChanServ", "chanserv/drop", params, replacements);
		}
		else
			replacements["MESSAGES"] = "Invalid Confirmation";
	}

	std::deque<ChannelInfo *> queue;
	na->nc->GetChannelReferences(queue);
	for (auto *ci : queue)
	{
		if ((ci->HasExt("SECUREFOUNDER") ? ci->AccessFor(na->nc).founder : ci->AccessFor(na->nc).HasPriv("FOUNDER")) || (na->nc->IsServicesOper() && na->nc->o->ot->HasCommand("chanserv/drop")))
		{
			replacements["CHANNEL_NAMES"] = ci->name;
			replacements["ESCAPED_CHANNEL_NAMES"] = HTTP::URLEncode(ci->name);
		}
	}

	if (message.get_data.count("channel") > 0)
	{
		const Anope::string &chname = message.get_data["channel"];

		replacements["CHANNEL_DROP"] = chname;
		replacements["ESCAPED_CHANNEL"] = HTTP::URLEncode(chname);
	}

	TemplateFileServer page("chanserv/drop.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;

}
