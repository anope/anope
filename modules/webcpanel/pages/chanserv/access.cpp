/*
 * (C) 2003-2026 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"
#include "utils.h"

WebCPanel::ChanServ::Access::Access(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::ChanServ::Access::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	TemplateFileServer Page("chanserv/access.html");

	BuildChanList(na, replacements);

	const Anope::string chname = WebPanel::GetRequestData(message.get_data, "channel");
	if (chname.empty())
	{
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	ChannelInfo *ci = ChannelInfo::Find(chname);

	if (!ci)
	{
		replacements["MESSAGES"] = "Channel not registered.";
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	AccessGroup u_access = ci->AccessFor(na->nc);
	bool has_priv = na->nc->IsServicesOper() && na->nc->o->ot->HasPriv("chanserv/access/modify");

	if (!u_access.HasPriv("ACCESS_LIST") && !has_priv)
	{
		replacements["MESSAGES"] = "Access denied.";
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	replacements["ACCESS_LIST"] = "YES";

	const Anope::string paccess = WebPanel::GetRequestData(message.post_data, "access");
	const Anope::string pmask = WebPanel::GetRequestData(message.post_data, "mask");

	if (u_access.HasPriv("ACCESS_CHANGE") || has_priv)
	{
		const Anope::string mask = WebPanel::GetRequestData(message.get_data, "mask");
		if (message.get_data.count("del") && !mask.empty())
		{
			std::vector<Anope::string> params;
			params.push_back(ci->name);
			params.push_back("DEL");
			params.push_back(mask);

			WebPanel::RunCommand(client, na->nc->display, na->nc, "ChanServ", "chanserv/access", params, replacements);
		}
		else if (!pmask.empty() && !paccess.empty() && message.post_data.count("provider"))
		{
			const Anope::string &provider = message.post_data["provider"];

			if (provider == "chanserv/access")
			{
				std::vector<Anope::string> params;
				params.push_back(ci->name);
				params.push_back("ADD");
				params.push_back(pmask);
				params.push_back(paccess);

				WebPanel::RunCommand(client, na->nc->display, na->nc, "ChanServ", "chanserv/access", params, replacements);
			}
			else if (provider == "chanserv/xop")
			{
				std::vector<Anope::string> params;
				params.push_back(ci->name);
				params.push_back("ADD");
				params.push_back(pmask);

				WebPanel::RunCommandWithName(client, na->nc, "ChanServ", "chanserv/xop", paccess, params, replacements);
			}
			else if (provider == "chanserv/flags")
			{
				std::vector<Anope::string> params;
				params.push_back(ci->name);
				params.push_back("MODIFY");
				params.push_back(pmask);
				params.push_back(paccess);

				WebPanel::RunCommand(client, na->nc->display, na->nc, "ChanServ", "chanserv/flags", params, replacements);
			}
		}
	}

	/* command might have invalidated u_access */
	u_access = ci->AccessFor(na->nc);

	replacements["ESCAPED_CHANNEL"] = HTTPUtils::URLEncode(chname);
	replacements["ACCESS_CHANGE"] = u_access.HasPriv("ACCESS_CHANGE") ? "YES" : "NO";

	for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
	{
		ChanAccess *access = ci->GetAccess(i);

		replacements["MASKS"] = access->Mask();
		replacements["ACCESSES"] = access->AccessSerialize();
		replacements["CREATORS"] = access->creator;
	}

	if (Service::FindService("Command", "chanserv/access"))
		replacements["PROVIDERS"] = "chanserv/access";
	if (Service::FindService("Command", "chanserv/xop"))
		replacements["PROVIDERS"] = "chanserv/xop";
	if (Service::FindService("Command", "chanserv/flags"))
		replacements["PROVIDERS"] = "chanserv/flags";

	Page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

std::set<Anope::string> WebCPanel::ChanServ::Access::GetData()
{
	std::set<Anope::string> v;
	v.insert("channel");
	return v;
}
