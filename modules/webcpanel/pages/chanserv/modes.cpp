/*
 * (C) 2003-2016 Anope Team
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
	TemplateFileServer Page("chanserv/modes.html");

	BuildChanList(na, replacements);

	if (chname.empty())
	{
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	replacements["ESCAPED_CHANNEL"] = HTTPUtils::URLEncode(chname);
	ChannelInfo *ci = ChannelInfo::Find(chname);

	if (!ci)
	{
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	Channel *c = Channel::Find(chname);

	if (!c)
	{
		replacements["MESSAGES"] = Anope::printf(CHAN_X_NOT_IN_USE, chname.c_str());
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	AccessGroup u_access = ci->AccessFor(na->nc);
	bool has_priv = na->nc->IsServicesOper() && na->nc->o->ot->HasPriv("chanserv/administration");

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

