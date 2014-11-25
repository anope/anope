/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"
#include "utils.h"

WebCPanel::ChanServ::Access::Access(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::ChanServ::Access::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	TemplateFileServer Page("chanserv/access.html");
	const Anope::string &chname = message.get_data["channel"];

	BuildChanList(na, replacements);

	if (chname.empty())
	{
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	ChannelInfo *ci = ChannelInfo::Find(chname);

	if (!ci)
	{
		replacements["MESSAGES"] = "Channel not registered.";
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	AccessGroup u_access = ci->AccessFor(na->nc);
	bool has_priv = na->nc->IsServicesOper() && na->nc->o->ot->HasPriv("chanserv/access/modify");

	if (!u_access.HasPriv("ACCESS_LIST") && !has_priv)
	{
		replacements["MESSAGES"] = "Access denied.";
		Page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	replacements["ACCESS_LIST"] = "YES";

	const ChanAccess *highest = u_access.Highest();
	
	if (u_access.HasPriv("ACCESS_CHANGE") || has_priv)
	{
		if (message.get_data["del"].empty() == false && message.get_data["mask"].empty() == false)
		{
			std::vector<Anope::string> params;
			params.push_back(ci->name);
			params.push_back("DEL");
			params.push_back(message.get_data["mask"]);

			WebPanel::RunCommand(na->nc->display, na->nc, "ChanServ", "chanserv/access", params, replacements);
		}
		else if (message.post_data["mask"].empty() == false && message.post_data["access"].empty() == false && message.post_data["provider"].empty() == false)
		{
			// Generic access add code here, works with any provider (so we can't call a command exactly)
			AccessProvider *a = NULL;
			for (std::list<AccessProvider *>::const_iterator it = AccessProvider::GetProviders().begin(); it != AccessProvider::GetProviders().end(); ++it)
				if ((*it)->name == message.post_data["provider"])
					a = *it;

			if (a)
			{
				bool denied = false;

				for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
				{
					ChanAccess *acc = ci->GetAccess(i);

					if (acc->Mask() == message.post_data["mask"])
					{
						if ((!highest || *acc >= *highest) && !u_access.founder && !has_priv)
						{
							replacements["MESSAGES"] = "Access denied";
							denied = true;
						}
						else
							delete acc;
						break;
					}
				}


				unsigned access_max = Config->GetModule("chanserv")->Get<unsigned>("accessmax", "1024");
				if (access_max && ci->GetAccessCount() >= access_max)
					replacements["MESSAGES"] = "Sorry, you can only have " + stringify(access_max) + " access entries on a channel.";
				else if (!denied)
				{
					ChanAccess *new_acc = a->Create();
					new_acc->SetMask(message.post_data["mask"], ci);
					new_acc->creator = na->nc->display;
					try
					{
						new_acc->AccessUnserialize(message.post_data["access"]);
					}
					catch (...)
					{
						replacements["MESSAGES"] = "Invalid access expression for the given type";
						delete new_acc;
						new_acc = NULL;
					}
					if (new_acc)
					{
						new_acc->last_seen = 0;
						new_acc->created = Anope::CurTime;

						if ((!highest || *highest <= *new_acc) && !u_access.founder && !has_priv)
							delete new_acc;
						else if (new_acc->AccessSerialize().empty())
						{
							replacements["MESSAGES"] = "Invalid access expression for the given type";
							delete new_acc;
						}
						else
						{
							ci->AddAccess(new_acc);
							replacements["MESSAGES"] = "Access for " + new_acc->Mask() + " set to " + new_acc->AccessSerialize();
						}
					}
				}
			}
		}
	}

	replacements["ESCAPED_CHANNEL"] = HTTPUtils::URLEncode(chname);
	replacements["ACCESS_CHANGE"] = u_access.HasPriv("ACCESS_CHANGE") ? "YES" : "NO";

	for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
	{
		ChanAccess *access = ci->GetAccess(i);

		replacements["MASKS"] = HTTPUtils::Escape(access->Mask());
		replacements["ACCESSES"] = HTTPUtils::Escape(access->AccessSerialize());
		replacements["CREATORS"] = HTTPUtils::Escape(access->creator);
	}

	for (std::list<AccessProvider *>::const_iterator it = AccessProvider::GetProviders().begin(); it != AccessProvider::GetProviders().end(); ++it)
	{
		const AccessProvider *a = *it;
		replacements["PROVIDERS"] = a->name;
	}

	Page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

std::set<Anope::string> WebCPanel::ChanServ::Access::GetData()
{
	std::set<Anope::string> v;
	v.insert("channel");
	return v;
}

