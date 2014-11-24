/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"
#include "utils.h"
#include "modules/cs_akick.h"

WebCPanel::ChanServ::Akick::Akick(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::ChanServ::Akick::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{
	const Anope::string &chname = message.get_data["channel"];
	TemplateFileServer Page("chanserv/akick.html");

	BuildChanList(na, replacements);

	if (chname.empty())
	{
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	::ChanServ::Channel *ci = ::ChanServ::Find(chname);

	if (!ci)
	{
		replacements["MESSAGES"] = "Channel not registered";
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	::ChanServ::AccessGroup u_access = ci->AccessFor(na->GetAccount());
	bool has_priv = na->GetAccount()->IsServicesOper() && na->GetAccount()->o->GetType()->HasPriv("chanserv/access/modify");

	if (!u_access.HasPriv("AKICK") && !has_priv)
	{
		replacements["MESSAGES"] = "Permission denied.";
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	replacements["AKICK"] = "YES";

	if (message.get_data["del"].empty() == false && message.get_data["mask"].empty() == false)
	{
		std::vector<Anope::string> params;
		params.push_back(ci->GetName());
		params.push_back("DEL");
		params.push_back(message.get_data["mask"]);

		WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "ChanServ", "chanserv/akick", params, replacements);
	}
	else if (message.post_data["mask"].empty() == false)
	{
		std::vector<Anope::string> params;
		params.push_back(ci->GetName());
		params.push_back("ADD");
		params.push_back(message.post_data["mask"]);
		if (message.post_data["reason"].empty() == false)
			params.push_back(message.post_data["reason"]);

		WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "ChanServ", "chanserv/akick", params, replacements);
	}

	replacements["ESCAPED_CHANNEL"] = HTTPUtils::URLEncode(chname);

	for (unsigned i = 0; i < ci->GetAkickCount(); ++i)
	{
		AutoKick *ak = ci->GetAkick(i);

		if (ak->GetAccount())
			replacements["MASKS"] = HTTPUtils::Escape(ak->GetAccount()->GetDisplay());
		else
			replacements["MASKS"] = HTTPUtils::Escape(ak->GetMask());
		replacements["CREATORS"] = HTTPUtils::Escape(ak->GetCreator());
		replacements["REASONS"] = HTTPUtils::Escape(ak->GetReason());
	}

	Page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

std::set<Anope::string> WebCPanel::ChanServ::Akick::GetData()
{
	std::set<Anope::string> v;
	v.insert("channel");
	return v;
}

