/*
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"
#include "utils.h"

WebCPanel::ChanServ::Modes::Modes(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::ChanServ::Modes::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	const Anope::string &chname = message.get_data["channel"];
	const Anope::string &mode = message.get_data["m"];

	BuildChanlist(page_name, na, replacements);

	if (chname.empty())
	{
		replacements["STOP"];
		return ServePage("chanserv/modes.html", server, page_name, client, message, reply, replacements);
	}

	ChannelInfo *ci = ChannelInfo::Find(chname);

	if (!ci)
	{
		replacements["STOP"];
		return ServePage("chanserv/modes.html", server, page_name, client, message, reply, replacements);
	}

	Channel *c = Channel::Find(chname);

	if (!c)
	{
		replacements["MESSAGES"] = "Channel is empty / does not exist on the network.";
		replacements["STOP"];
		return ServePage("chanserv/modes.html", server, page_name, client, message, reply, replacements);
	}

	AccessGroup u_access = ci->AccessFor(na->nc);
	bool has_priv = na->nc->IsServicesOper() && na->nc->o->ot->HasPriv("chanserv/administration");

	if (!u_access.HasPriv("MODE") && !has_priv)
	{
		replacements["MESSAGES"] = "Access denied.";
		replacements["STOP"];
		return ServePage("chanserv/modes.html", server, page_name, client, message, reply, replacements);
	}

	/* build a list with the names of all listmodes */
	for (std::vector<ChannelMode *>::const_iterator it = ModeManager::GetChannelModes().begin(); it != ModeManager::GetChannelModes().end(); ++it)
	{
		/* "NAMEBASE" is a special mode from InspIRCds m_namedmodes, we dont want this here*/
		if ((*it) && (*it)->type == MODE_LIST && (*it)->name != "NAMEBASE")
			replacements["LISTMODES"] = (*it)->name;
	}

	ChannelMode *cm = ModeManager::FindChannelModeByName(mode);
	if (cm)
	{
		if (message.get_data["del"].empty() == false && message.get_data["mask"].empty() == false)
		{
			std::vector<Anope::string> params;
			params.push_back(ci->name);
			params.push_back("SET");
			params.push_back("-" + Anope::string(cm->mchar));
			params.push_back(message.get_data["mask"]);
			WebPanel::RunCommand(na->nc->display, na->nc, "ChanServ", "chanserv/mode", params, replacements);
		}
		else if (message.post_data["mask"].empty() == false)
		{
			std::vector<Anope::string> params;
			params.push_back(ci->name);
			params.push_back("SET");
			params.push_back("+" + Anope::string(cm->mchar));
			params.push_back(message.post_data["mask"]);
			WebPanel::RunCommand(na->nc->display, na->nc, "ChanServ", "chanserv/mode", params, replacements);
		}

		for (Channel::ModeList::const_iterator it = c->GetModes().begin(); it != c->GetModes().end(); ++it)
		{
			if (it->first == mode)
				replacements["MASKS"] = HTTPUtils::Escape(it->second);
		}
	}

	replacements["ESCAPED_CHANNEL"] = HTTPUtils::URLEncode(chname);
	replacements["ESCAPED_MODE"] = HTTPUtils::URLEncode(mode);

	return ServePage("chanserv/modes.html", server, page_name, client, message, reply, replacements);
}

std::set<Anope::string> WebCPanel::ChanServ::Modes::GetData()
{
	std::set<Anope::string> v;
	v.insert("channel");
	return v;
}

