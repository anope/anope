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

WebCPanel::ChanServ::Modes::Modes(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::ChanServ::Modes::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{
	const Anope::string &chname = message.get_data["channel"];
	const Anope::string &mode = message.get_data["m"];
	TemplateFileServer Page("chanserv/modes.html");

	BuildChanList(na, replacements);

	if (chname.empty())
	{
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	replacements["ESCAPED_CHANNEL"] = HTTPUtils::URLEncode(chname);
	::ChanServ::Channel *ci = ::ChanServ::Find(chname);

	if (!ci)
	{
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	Channel *c = Channel::Find(chname);

	if (!c)
	{
		replacements["MESSAGES"] = Anope::printf(_("Channel \002%s\002 doesn't exist."), chname.c_str());
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	::ChanServ::AccessGroup u_access = ci->AccessFor(na->GetAccount());
	bool has_priv = na->GetAccount()->GetOper() && na->GetAccount()->GetOper()->HasPriv("chanserv/administration");

	if (!u_access.HasPriv("MODE") && !has_priv)
	{
		replacements["MESSAGES"] = "Access denied.";
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	replacements["MODE"] = "YES";

	/* build a list with the names of all listmodes */
	for (unsigned i = 0; i < ModeManager::GetChannelModes().size(); ++i)
	{
		ChannelMode *cm = ModeManager::GetChannelModes()[i];

		if (cm->type == MODE_LIST && cm->mchar)
			replacements["LISTMODES"] = cm->mchar;
	}

	if (mode.empty())
	{
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}
	replacements["ESCAPED_MODE"] = HTTPUtils::URLEncode(mode);

	ChannelMode *cm = ModeManager::FindChannelModeByChar(mode[0]);
	if (cm)
	{
		if (message.get_data["del"].empty() == false && message.get_data["mask"].empty() == false)
		{
			std::vector<Anope::string> params;
			params.push_back(ci->GetName());
			params.push_back("SET");
			params.push_back("-" + Anope::string(cm->mchar));
			params.push_back(message.get_data["mask"]);
			WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "ChanServ", "chanserv/mode", params, replacements);
		}
		else if (message.post_data["mask"].empty() == false)
		{
			std::vector<Anope::string> params;
			params.push_back(ci->GetName());
			params.push_back("SET");
			params.push_back("+" + Anope::string(cm->mchar));
			params.push_back(message.post_data["mask"]);
			WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "ChanServ", "chanserv/mode", params, replacements);
		}

		std::vector<Anope::string> v = c->GetModeList(cm->name);
		for (unsigned int i = 0; i < v.size(); ++i)
			replacements["MASKS"] = HTTPUtils::Escape(v[i]);
	}

	Page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

std::set<Anope::string> WebCPanel::ChanServ::Modes::GetData()
{
	std::set<Anope::string> v;
	v.insert("channel");
	return v;
}

