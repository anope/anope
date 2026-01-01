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

bool WebCPanel::Register::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply)
{
	TemplateFileServer::Replacements replacements;

	replacements["TITLE"] = page_title;

	if (Config->GetModule("nickserv").Get<bool>("forceemail", "yes"))
		replacements["FORCE_EMAIL"] = "yes";

	TemplateFileServer page("register.html");

	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}
