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

WebCPanel::NickServ::Info::Info(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::NickServ::Info::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{
	if (message.post_data.empty() == false)
	{
		if (message.post_data.count("email") > 0)
		{
			if (message.post_data["email"] != na->GetAccount()->GetEmail())
			{
				if (!message.post_data["email"].empty() && !Mail::Validate(message.post_data["email"]))
					replacements["ERRORS"] = "Invalid email";
				else
				{
					na->GetAccount()->SetEmail(message.post_data["email"]);
					replacements["MESSAGES"] = "Email updated";
				}
			}
		}
		if (message.post_data.count("greet") > 0)
		{
			const Anope::string &post_greet = HTTPUtils::URLDecode(message.post_data["greet"].replace_all_cs("+", " "));

			na->GetAccount()->SetGreet(post_greet);

			replacements["MESSAGES"] = "Greet updated";
		}
		if (na->GetAccount()->IsAutoOp() != message.post_data.count("autoop"))
		{
			na->GetAccount()->SetAutoOp(!na->GetAccount()->IsAutoOp());
			replacements["MESSAGES"] = "Autoop updated";
		}
		if (na->GetAccount()->IsPrivate() != message.post_data.count("private"))
		{
			na->GetAccount()->SetPrivate(!na->GetAccount()->IsPrivate());
			replacements["MESSAGES"] = "Private updated";
		}
		if (na->GetAccount()->IsSecure() != message.post_data.count("secure"))
		{
			na->GetAccount()->SetSecure(!na->GetAccount()->IsSecure());
			replacements["MESSAGES"] = "Secure updated";
		}
		if (message.post_data["kill"] == "on" && !na->GetAccount()->IsKillProtect())
		{
			na->GetAccount()->SetKillProtect(true);
			na->GetAccount()->SetKillQuick(false);
			replacements["MESSAGES"] = "Kill updated";
		}
		else if (message.post_data["kill"] == "quick" && !na->GetAccount()->IsKillQuick())
		{
			na->GetAccount()->SetKillProtect(false);
			na->GetAccount()->SetKillQuick(true);
			replacements["MESSAGES"] = "Kill updated";
		}
		else if (message.post_data["kill"] == "off" && (na->GetAccount()->IsKillProtect() || na->GetAccount()->IsKillQuick()))
		{
			na->GetAccount()->SetKillProtect(false);
			na->GetAccount()->SetKillQuick(false);
			replacements["MESSAGES"] = "Kill updated";
		}
	}

	replacements["DISPLAY"] = HTTPUtils::Escape(na->GetAccount()->GetDisplay());
	if (na->GetAccount()->GetEmail().empty() == false)
		replacements["EMAIL"] = HTTPUtils::Escape(na->GetAccount()->GetEmail());
	replacements["TIME_REGISTERED"] = Anope::strftime(na->GetTimeRegistered(), na->GetAccount());

	::HostServ::VHost *vhost = ::HostServ::FindVHost(na->GetAccount());
	if (vhost != nullptr)
	{
		replacements["VHOST"] = vhost->Mask();
	}

	Anope::string greet = na->GetAccount()->GetGreet();
	if (greet.empty() == false)
		replacements["GREET"] = HTTPUtils::Escape(greet);
	if (na->GetAccount()->IsAutoOp())
		replacements["AUTOOP"];
	if (na->GetAccount()->IsPrivate())
		replacements["PRIVATE"];
	if (na->GetAccount()->IsSecure())
		replacements["SECURE"];
	if (na->GetAccount()->IsKillProtect())
		replacements["KILL_ON"];
	if (na->GetAccount()->IsKillQuick())
		replacements["KILL_QUICK"];
	if (!na->GetAccount()->IsKillProtect() && !na->GetAccount()->IsKillQuick())
		replacements["KILL_OFF"];

	TemplateFileServer page("nickserv/info.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

