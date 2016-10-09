/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2016 Anope Team <team@anope.org>
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

WebCPanel::HostServ::Request::Request(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage (cat, u)
{
}

bool WebCPanel::HostServ::Request::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{
	if (message.post_data.count("req") > 0)
	{
		std::vector<Anope::string> params;
		params.push_back(HTTPUtils::URLDecode(message.post_data["req"]));

		WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "HostServ", "hostserv/request", params, replacements, "CMDR");
	}

	::HostServ::VHost *vhost = ::HostServ::FindVHost(na->GetAccount());
	if (vhost != nullptr)
	{
		replacements["VHOST"] = vhost->Mask();
	}
	if (ServiceReference<Command>("hostserv/request"))
		replacements["CAN_REQUEST"] = "YES";
	TemplateFileServer page("hostserv/request.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}
