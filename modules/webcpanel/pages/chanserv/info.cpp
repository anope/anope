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
#include "utils.h"

WebCPanel::ChanServ::Info::Info(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::ChanServ::Info::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{
	const Anope::string &chname = message.get_data["channel"];

	if (!chname.empty())
		replacements["ESCAPED_CHANNEL"] = HTTPUtils::URLEncode(chname);

	BuildChanList(na, replacements);

	TemplateFileServer page("chanserv/main.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

