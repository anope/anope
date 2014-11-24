/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"
#include "utils.h"

WebCPanel::ChanServ::Drop::Drop(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage (cat, u)
{
}


bool WebCPanel::ChanServ::Drop::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{

	if (message.post_data.count("channel") > 0 && message.post_data.count("confChan") > 0)
	{
		if (message.post_data["channel"] == message.post_data["confChan"])
		{
			std::vector<Anope::string> params;
			const Anope::string &channel = HTTPUtils::URLDecode(message.post_data["channel"]);
			params.push_back(channel);
			params.push_back(channel);

			WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "ChanServ", "chanserv/drop", params, replacements);
		}
		else
			replacements["MESSAGES"] = "Invalid Confirmation";
	}

	for (::ChanServ::Channel *ci : na->GetAccount()->GetRefs<::ChanServ::Channel *>(::ChanServ::channel))
		if ((ci->HasFieldS("SECUREFOUNDER") ? ci->AccessFor(na->GetAccount()).founder : ci->AccessFor(na->GetAccount()).HasPriv("FOUNDER")) || (na->GetAccount()->IsServicesOper() && na->GetAccount()->o->GetType()->HasCommand("chanserv/drop")))
		{
			replacements["CHANNEL_NAMES"] = ci->GetName();
			replacements["ESCAPED_CHANNEL_NAMES"] = HTTPUtils::URLEncode(ci->GetName());
		}

	if (message.get_data.count("channel") > 0)
	{
		const Anope::string &chname = message.get_data["channel"];

		replacements["CHANNEL_DROP"] = chname;
		replacements["ESCAPED_CHANNEL"] = HTTPUtils::URLEncode(chname);
	}

	TemplateFileServer page("chanserv/drop.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;

}
