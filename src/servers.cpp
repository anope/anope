/* Routines to maintain a list of connected servers
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "xline.h"
#include "servers.h"
#include "bots.h"
#include "regchannel.h"
#include "protocol.h"
#include "config.h"
#include "channels.h"

/* Anope */
Server *Me = NULL;

Anope::map<Server *> Servers::ByName;
Anope::map<Server *> Servers::ByID;

std::set<Anope::string> Servers::Capab;

Server::Server(Server *up, const Anope::string &sname, unsigned shops, const Anope::string &desc, const Anope::string &ssid, bool jupe) : name(sname), hops(shops), description(desc), sid(ssid), uplink(up)
{
	syncing = true;
	juped = jupe;
	quitting = false;

	Servers::ByName[sname] = this;
	if (!ssid.empty())
		Servers::ByID[ssid] = this;

	Log(this, "connect") << "has connected to the network (uplinked to " << (this->uplink ? this->uplink->GetName() : "no uplink") << ")";

	/* Add this server to our uplinks leaf list */
	if (this->uplink)
	{
		this->uplink->AddLink(this);

		/* Check to be sure this isn't a juped server */
		if (Me == this->uplink && !juped)
		{
			/* Now do mode related stuff as we know what modes exist .. */
			for (auto &[_, bi] : *BotListByNick)
			{
				spacesepstream modesep(bi->botmodes.empty() ? IRCD->DefaultPseudoclientModes : "+" + bi->botmodes);

				Anope::string modechars;
				modesep.GetToken(modechars);

				std::vector<Anope::string> modeparams;
				modesep.GetTokens(modeparams);

				bi->SetModesInternal(bi, modechars, modeparams);
				for (const auto &botchannel : bi->botchannels)
				{
					size_t h = botchannel.find('#');
					if (h == Anope::string::npos)
						continue;
					Anope::string chname = botchannel.substr(h);
					Channel *c = Channel::Find(chname);
					if (c && c->FindUser(bi))
					{
						Anope::string want_modes = botchannel.substr(0, h);
						for (char want_mode : want_modes)
						{
							ChannelMode *cm = ModeManager::FindChannelModeByChar(want_mode);
							if (cm == NULL)
								cm = ModeManager::FindChannelModeByChar(ModeManager::GetStatusChar(want_mode));
							if (cm && cm->type == MODE_STATUS)
							{
								MessageSource ms = bi;
								c->SetModeInternal(ms, cm, bi->nick);
							}
						}
					}
				}
			}

			IRCD->SendBOB();

			for (auto *link : Me->GetLinks())
			{
				if (link->juped)
					IRCD->SendServer(link);
			}

			/* We make the bots go online */
			for (const auto &[_, u] : UserListByNick)
			{
				BotInfo *bi = BotInfo::Find(u->GetUID());
				if (bi)
				{
					XLine x(bi->nick, "Reserved for services");
					IRCD->SendSQLine(NULL, &x);
				}

				IRCD->SendClientIntroduction(u);
				if (bi)
					bi->introduced = true;
			}

			for (const auto &[_, c] : ChannelList)
			{
				if (c->users.empty())
					IRCD->SendChannel(c);
				else
				{
					for (const auto &[_, uc] : c->users)
						IRCD->SendJoin(uc->user, c, &uc->status);
				}

				for (const auto &[mode, value] :  c->GetModes())
				{
					ChannelMode *cm = ModeManager::FindChannelModeByName(mode);
					if (!cm || cm->type != MODE_LIST)
						continue;
					ModeManager::StackerAdd(c->WhoSends(), c, cm, true, value);
				}

				if (!c->topic.empty() && !c->topic_setter.empty())
					IRCD->SendTopic(c->WhoSends(), c);

				c->syncing = true;
			}
		}
	}

	FOREACH_MOD(OnNewServer, (this));
}

Server::~Server()
{
	Log(this, "quit") << "quit from " << (this->uplink ? this->uplink->GetName() : "no uplink") << " for " << this->quit_reason;

	for (const auto &[_, u] : UserListByNick)
	{
		if (u->server == this)
		{
			u->Quit(this->quit_reason);
			u->server = NULL;
		}
	}

	Log(LOG_DEBUG) << "Finished removing all users for " << this->GetName();

	if (this->uplink)
		this->uplink->DelLink(this);

	for (unsigned i = this->links.size(); i > 0; --i)
		this->links[i - 1]->Delete(this->quit_reason);

	Servers::ByName.erase(this->name);
	if (!this->sid.empty())
		Servers::ByID.erase(this->sid);
}

void Server::Delete(const Anope::string &reason)
{
	this->quit_reason = reason;
	this->quitting = true;
	FOREACH_MOD(OnServerQuit, (this));
	delete this;
}

const Anope::string &Server::GetName() const
{
	return this->name;
}

unsigned Server::GetHops() const
{
	return this->hops;
}

void Server::SetDescription(const Anope::string &desc)
{
	this->description = desc;
}

const Anope::string &Server::GetDescription() const
{
	return this->description;
}

void Server::SetSID(const Anope::string &nsid)
{
	if (!this->sid.empty())
		throw CoreException("Server already has an id?");
	this->sid = nsid;
	Servers::ByID[nsid] = this;
}

const Anope::string &Server::GetSID() const
{
	if (!this->sid.empty() && IRCD->RequiresID)
		return this->sid;
	else
		return this->name;
}

const Anope::string &Server::GetQuitReason() const
{
	return this->quit_reason;
}

const std::vector<Server *> &Server::GetLinks() const
{
	return this->links;
}

Server *Server::GetUplink()
{
	return this->uplink;
}

void Server::AddLink(Server *s)
{
	this->links.push_back(s);

	Log(this, "connect") << "introduced " << s->GetName();
}

void Server::DelLink(Server *s)
{
	if (this->links.empty())
		throw CoreException("Server::DelLink called on " + this->GetName() + " for " + s->GetName() + " but we have no links?");

	for (unsigned i = 0, j = this->links.size(); i < j; ++i)
	{
		if (this->links[i] == s)
		{
			this->links.erase(this->links.begin() + i);
			break;
		}
	}

	Log(this, "quit") << "quit " << s->GetName();
}

void Server::Sync(bool sync_links)
{
	if (this->IsSynced())
		return;

	syncing = false;

	Log(this, "sync") << "is done syncing";

	FOREACH_MOD(OnServerSync, (this));

	if (sync_links && !this->links.empty())
	{
		for (auto *link : this->links)
			link->Sync(true);
	}

	bool me = this->GetUplink() && this->GetUplink() == Me;

	if (me)
	{
		FOREACH_MOD(OnPreUplinkSync, (this));
	}

	for (channel_map::const_iterator it = ChannelList.begin(), it_end = ChannelList.end(); it != it_end;)
	{
		Channel *c = it->second;
		++it;

		if (c->syncing)
			c->Sync();
	}

	if (me)
	{
		IRCD->SendEOB();
		Me->Sync(false);

		FOREACH_MOD(OnUplinkSync, (this));

		if (!Anope::NoFork)
		{
			Log(LOG_TERMINAL) << "Successfully linked, launching into background...";
			Anope::Fork();
		}
	}
}

bool Server::IsSynced() const
{
	return !syncing;
}

void Server::Unsync()
{
	syncing = true;
}

bool Server::IsULined() const
{
	if (this == Me)
		return true;

	for (const auto &uline : Config->Ulines)
	{
		if (uline.equals_ci(this->GetName()))
			return true;
	}
	return false;
}

bool Server::IsJuped() const
{
	return juped;
}

bool Server::IsQuitting() const
{
	return quitting;
}

void Server::Notice(BotInfo *source, const Anope::string &message)
{
	if (Config->UsePrivmsg && Config->DefPrivmsg)
		IRCD->SendGlobalPrivmsg(source, this, message);
	else
		IRCD->SendGlobalNotice(source, this, message);
}

Server *Server::Find(const Anope::string &name, bool name_only)
{
	Anope::map<Server *>::iterator it;

	if (!name_only)
	{
		it = Servers::ByID.find(name);
		if (it != Servers::ByID.end())
			return it->second;
	}

	it = Servers::ByName.find(name);
	if (it != Servers::ByName.end())
		return it->second;

	return NULL;
}

Server *Servers::GetUplink()
{
	for (auto *link : Me->GetLinks())
	{
		if (!link->IsJuped())
			return link;
	}
	return NULL;
}
