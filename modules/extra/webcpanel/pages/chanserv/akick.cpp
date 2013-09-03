/*
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"
#include "utils.h"

WebCPanel::ChanServ::Akick::Akick(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::ChanServ::Akick::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	const Anope::string &chname = message.get_data["channel"];

	BuildChanlist(page_name, na, replacements);
	if (chname.empty())
	{
		replacements["STOP"];
		return ServePage("chanserv/akick.html", server, page_name, client, message, reply, replacements);
	}

	ChannelInfo *ci = ChannelInfo::Find(chname);

	if (!ci)
	{
		replacements["STOP"];
		replacements["MESSAGES"] = "Channel not registered";
		return ServePage("chanserv/akick.html", server, page_name, client, message, reply, replacements);
	}

	AccessGroup u_access = ci->AccessFor(na->nc);
	bool has_priv = na->nc->IsServicesOper() && na->nc->o->ot->HasPriv("chanserv/access/modify");

	if (!u_access.HasPriv("akick") && !has_priv)
	{
		replacements["STOP"];
		replacements["MESSAGES"] = "Permission denied.";
		return ServePage("chanserv/akick.html", server, page_name, client, message, reply, replacements);
	}

	if (message.get_data["del"].empty() == false && message.get_data["mask"].empty() == false)
	{
		std::vector<Anope::string> params;
		params.push_back(ci->name);
		params.push_back("DEL");
		params.push_back(message.get_data["mask"]);

		WebPanel::RunCommand(na->nc->display, na->nc, "ChanServ", "chanserv/akick", params, replacements);
	}
	else if (message.post_data["mask"].empty() == false)
	{
		std::vector<Anope::string> params;
		params.push_back(ci->name);
		params.push_back("ADD");
		params.push_back(message.post_data["mask"]);
		if (message.post_data["reason"].empty() == false)
			params.push_back(message.post_data["reason"]);

		WebPanel::RunCommand(na->nc->display, na->nc, "ChanServ", "chanserv/akick", params, replacements);
	}

	replacements["ESCAPED_CHANNEL"] = HTTPUtils::URLEncode(chname);

	for (unsigned i = 0; i < ci->GetAkickCount(); ++i)
	{
		AutoKick *akick = ci->GetAkick(i);

		if (akick->nc)
			replacements["MASKS"] = HTTPUtils::Escape(akick->nc->display);
		else
			replacements["MASKS"] = HTTPUtils::Escape(akick->mask);
		replacements["CREATORS"] = HTTPUtils::Escape(akick->creator);
		replacements["REASONS"] = HTTPUtils::Escape(akick->reason);
	}

	return ServePage("chanserv/akick.html", server, page_name, client, message, reply, replacements);
}

std::set<Anope::string> WebCPanel::ChanServ::Akick::GetData()
{
	std::set<Anope::string> v;
	v.insert("channel");
	return v;
}

