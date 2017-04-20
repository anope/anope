/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2017 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "../../webcpanel.h"
#include "utils.h"

WebCPanel::ChanServ::Access::Access(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::ChanServ::Access::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{
	TemplateFileServer Page("chanserv/access.html");
	const Anope::string &chname = message.get_data["channel"];

	BuildChanList(na, replacements);

	if (chname.empty())
	{
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	::ChanServ::Channel *ci = ::ChanServ::Find(chname);

	if (!ci)
	{
		replacements["MESSAGES"] = "Channel not registered.";
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	::ChanServ::AccessGroup u_access = ci->AccessFor(na->GetAccount());
	bool has_priv = na->GetAccount()->GetOper() && na->GetAccount()->GetOper()->HasPriv("chanserv/access/modify");

	if (!u_access.HasPriv("ACCESS_LIST") && !has_priv)
	{
		replacements["MESSAGES"] = "Access denied.";
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	replacements["ACCESS_LIST"] = "YES";

	if (u_access.HasPriv("ACCESS_CHANGE") || has_priv)
	{
		if (message.get_data["del"].empty() == false && message.get_data["mask"].empty() == false)
		{
			std::vector<Anope::string> params;
			params.push_back(ci->GetName());
			params.push_back("DEL");
			params.push_back(message.get_data["mask"]);

			WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "ChanServ", "chanserv/access", params, replacements);
		}
		else if (message.post_data["mask"].empty() == false && message.post_data["access"].empty() == false && message.post_data["provider"].empty() == false)
		{
			const Anope::string &provider = message.post_data["provider"];

			if (provider == "chanserv/access")
			{
				std::vector<Anope::string> params;
				params.push_back(ci->GetName());
				params.push_back("ADD");
				params.push_back(message.post_data["mask"]);
				params.push_back(message.post_data["access"]);

				WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "ChanServ", "chanserv/access", params, replacements);
			}
			else if (provider == "chanserv/xop")
			{
				std::vector<Anope::string> params;
				params.push_back(ci->GetName());
				params.push_back("ADD");
				params.push_back(message.post_data["mask"]);

				WebPanel::RunCommandWithName(na->GetAccount(), "ChanServ", "chanserv/xop", message.post_data["access"], params, replacements);
			}
			else if (provider == "chanserv/flags")
			{
				std::vector<Anope::string> params;
				params.push_back(ci->GetName());
				params.push_back("MODIFY");
				params.push_back(message.post_data["mask"]);
				params.push_back(message.post_data["access"]);

				WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "ChanServ", "chanserv/flags", params, replacements);
			}
		}
	}

	/* command might have invalidated u_access */
	u_access = ci->AccessFor(na->GetAccount());

	replacements["ESCAPED_CHANNEL"] = HTTPUtils::URLEncode(chname);
	replacements["ACCESS_CHANGE"] = u_access.HasPriv("ACCESS_CHANGE") ? "YES" : "NO";

	for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
	{
		::ChanServ::ChanAccess *access = ci->GetAccess(i);

		replacements["MASKS"] = HTTPUtils::Escape(access->Mask());
		replacements["ACCESSES"] = HTTPUtils::Escape(access->AccessSerialize());
		replacements["CREATORS"] = HTTPUtils::Escape(access->GetCreator());
	}

	if (ServiceManager::Get()->FindService("Command", "chanserv/access"))
		replacements["PROVIDERS"] = "chanserv/access";
	if (ServiceManager::Get()->FindService("Command", "chanserv/xop"))
		replacements["PROVIDERS"] = "chanserv/xop";
	if (ServiceManager::Get()->FindService("Command", "chanserv/flags"))
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

