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
			Anope::string *greet = na->GetAccount()->GetExt<Anope::string>("greet");
			const Anope::string &post_greet = HTTPUtils::URLDecode(message.post_data["greet"].replace_all_cs("+", " "));

			if (post_greet.empty())
				na->GetAccount()->Shrink<Anope::string>("greet");
			else if (!greet || post_greet != *greet)
				na->GetAccount()->Extend<Anope::string>("greet", post_greet);

			replacements["MESSAGES"] = "Greet updated";
		}
		if (na->GetAccount()->HasFieldS("AUTOOP") != message.post_data.count("autoop"))
		{
			if (!na->GetAccount()->HasFieldS("AUTOOP"))
				na->GetAccount()->SetS<bool>("AUTOOP", true);
			else
				na->GetAccount()->UnsetS<bool>("AUTOOP");
			replacements["MESSAGES"] = "Autoop updated";
		}
		if (na->GetAccount()->HasFieldS("NS_PRIVATE") != message.post_data.count("private"))
		{
			if (!na->GetAccount()->HasFieldS("NS_PRIVATE"))
				na->GetAccount()->SetS<bool>("NS_PRIVATE", true);
			else
				na->GetAccount()->UnsetS<bool>("NS_PRIVATE");
			replacements["MESSAGES"] = "Private updated";
		}
		if (na->GetAccount()->HasFieldS("NS_SECURE") != message.post_data.count("secure"))
		{
			if (!na->GetAccount()->HasFieldS("NS_SECURE"))
				na->GetAccount()->SetS<bool>("NS_SECURE", true);
			else
				na->GetAccount()->UnsetS<bool>("NS_SECURE");
			replacements["MESSAGES"] = "Secure updated";
		}
		if (message.post_data["kill"] == "on" && !na->GetAccount()->HasFieldS("KILLPROTECT"))
		{
			na->GetAccount()->SetS<bool>("KILLPROTECT", true);
			na->GetAccount()->UnsetS<bool>("KILL_QUICK");
			replacements["MESSAGES"] = "Kill updated";
		}
		else if (message.post_data["kill"] == "quick" && !na->GetAccount()->HasFieldS("KILL_QUICK"))
		{
			na->GetAccount()->UnsetS<bool>("KILLPROTECT");
			na->GetAccount()->SetS<bool>("KILL_QUICK", true);
			replacements["MESSAGES"] = "Kill updated";
		}
		else if (message.post_data["kill"] == "off" && (na->GetAccount()->HasFieldS("KILLPROTECT") || na->GetAccount()->HasFieldS("KILL_QUICK")))
		{
			na->GetAccount()->UnsetS<bool>("KILLPROTECT");
			na->GetAccount()->UnsetS<bool>("KILL_QUICK");
			replacements["MESSAGES"] = "Kill updated";
		}
	}

	replacements["DISPLAY"] = HTTPUtils::Escape(na->GetAccount()->GetDisplay());
	if (na->GetAccount()->GetEmail().empty() == false)
		replacements["EMAIL"] = HTTPUtils::Escape(na->GetAccount()->GetEmail());
	replacements["TIME_REGISTERED"] = Anope::strftime(na->GetTimeRegistered(), na->GetAccount());
	if (na->HasVhost())
	{
		if (na->GetVhostIdent().empty() == false)
			replacements["VHOST"] = na->GetVhostIdent() + "@" + na->GetVhostHost();
		else
			replacements["VHOST"] = na->GetVhostHost();
	}
	Anope::string *greet = na->GetAccount()->GetExt<Anope::string>("greet");
	if (greet)
		replacements["GREET"] = HTTPUtils::Escape(*greet);
	if (na->GetAccount()->HasFieldS("AUTOOP"))
		replacements["AUTOOP"];
	if (na->GetAccount()->HasFieldS("NS_PRIVATE"))
		replacements["PRIVATE"];
	if (na->GetAccount()->HasFieldS("NS_SECURE"))
		replacements["SECURE"];
	if (na->GetAccount()->HasFieldS("KILLPROTECT"))
		replacements["KILL_ON"];
	if (na->GetAccount()->HasFieldS("KILL_QUICK"))
		replacements["KILL_QUICK"];
	if (!na->GetAccount()->HasFieldS("KILLPROTECT") && !na->GetAccount()->HasFieldS("KILL_QUICK"))
		replacements["KILL_OFF"];

	TemplateFileServer page("nickserv/info.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

