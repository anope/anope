// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "../webcpanel.h"

bool WebCPanel::ConfirmEmail::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply)
{
	TemplateFileServer::Replacements replacements;
    const Anope::string &confirm_type = message.get_data["type"], &nick = message.get_data["nick"], &code = message.get_data["code"];

    auto fail = [&]() {
        replacements["INVALID_LOGIN"] = "Invalid confirmation link. Please contact an IRC operator for further assistance.";
        TemplateFileServer page("login.html");

        Log(LOG_NORMAL, "confirm_email") << "Invalid confirmation link with type: " << message.get_data["type"]
            << " nick: " << message.get_data["nick"]
            << " code: " << message.get_data["code"];

        page.Serve(server, page_name, client, message, reply, replacements);
        return true;
    };

    NickAlias *na = NickAlias::Find(nick);
    if (!na || code.empty())
    {
        return fail();
    }
    NickCore *nc = na->nc;
    std::vector<Anope::string> params;
    params.push_back(code);

    if (confirm_type == "REGISTER")
    {
        if (nc->HasExt("UNCONFIRMED"))
        {
            WebPanel::RunCommand(client, na->nc->display, na->nc, "NickServ", "nickserv/confirm/register", params, replacements);
            if (nc->HasExt("UNCONFIRMED"))
                return fail();
        }
        else
            return fail();
    }
    else if (confirm_type == "EMAIL")
    {
        auto old_email = nc->email;
        WebPanel::RunCommand(client, na->nc->display, na->nc, "NickServ", "nickserv/confirm/email", params, replacements);
        if (nc->email == old_email)
            return fail();
    }
    else // TODO other types
        return fail();

    // User may or may not be logged in, but if they're logged in as a different nick then log them out to avoid confusion
    NickAlias *logged_in_as = ServiceReference<Panel>("Panel", "webcpanel")->GetNickFromSession(client, message);
    if (logged_in_as && logged_in_as != na)
    {
        logged_in_as->Shrink<Anope::string>("webcpanel_id");
        logged_in_as->Shrink<Anope::string>("webcpanel_ip");
    }

    TemplateFileServer page("confirm_email.html");
    page.Serve(server, page_name, client, message, reply, replacements);
    return true;
}
