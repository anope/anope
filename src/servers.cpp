/* Routines to maintain a list of connected servers
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "oper.h"

/* Anope */
Server *Me = NULL;

CapabInfo Capab_Info[] = {
	{"NOQUIT", CAPAB_NOQUIT},
	{"TSMODE", CAPAB_TSMODE},
	{"UNCONNECT", CAPAB_UNCONNECT},
	{"QS", CAPAB_QS},
	{"", CAPAB_END}
};

const Anope::string CapabFlags[] = {
	"NOQUIT", "TSMODE", "UNCONNECT", "QS", ""
};
Flags<CapabType, CAPAB_END> Capab(CapabFlags);

/** Constructor
 * @param uplink The uplink this server is from, is only NULL when creating Me
 * @param name The server name
 * @param hops Hops from services server
 * @param description Server rdescription
 * @param sid Server sid/numeric
 * @param flag An optional server flag
 */
Server::Server(Server *uplink, const Anope::string &name, unsigned hops, const Anope::string &description, const Anope::string &sid, ServerFlag flag) : Flags<ServerFlag>(ServerFlagStrings), Name(name), Hops(hops), Description(description), SID(sid), UplinkServer(uplink)
{
	this->SetFlag(SERVER_SYNCING);
	this->SetFlag(flag);

	Log(this, "connect") << "uplinked to " << (this->UplinkServer ? this->UplinkServer->GetName() : "no uplink") << " connected to the network";

	/* Add this server to our uplinks leaf list */
	if (this->UplinkServer)
	{
		this->UplinkServer->AddLink(this);

		/* Check to be sure this isn't a juped server */
		if (Me == this->UplinkServer && !this->HasFlag(SERVER_JUPED))
		{
			/* Now do mode related stuff as we know what modes exist .. */
			for (botinfo_map::iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
			{
				BotInfo *bi = it->second;
				Anope::string modes = !bi->botmodes.empty() ? ("+" + bi->botmodes) : ircd->pseudoclient_mode;
				bi->SetModesInternal(modes.c_str());
				for (unsigned i = 0; i < bi->botchannels.size(); ++i)
				{
					size_t h = bi->botchannels[i].find('#');
					if (h == Anope::string::npos)
						continue;
					Anope::string chname = bi->botchannels[i].substr(h);
					Channel *c = findchan(chname);
					if (c && c->FindUser(bi))
					{
						Anope::string want_modes = bi->botchannels[i].substr(0, h);
						for (unsigned j = 0; j < want_modes.length(); ++j)
						{
							ChannelMode *cm = ModeManager::FindChannelModeByChar(want_modes[j]);
							if (cm == NULL)
								cm = ModeManager::FindChannelModeByChar(ModeManager::GetStatusChar(want_modes[j]));
							if (cm && cm->Type == MODE_STATUS)
								c->SetModeInternal(cm, bi->nick);
						}
					}
				}
			}

			ircdproto->SendBOB();
	
			for (unsigned i = 0; i < Me->GetLinks().size(); ++i)
			{
				Server *s = Me->GetLinks()[i];

				if (s->HasFlag(SERVER_JUPED))
					ircdproto->SendServer(s);
			}

			/* We make the bots go online */
			for (Anope::insensitive_map<User *>::iterator it = UserListByNick.begin(), it_end = UserListByNick.end(); it != it_end; ++it)
			{
				User *u = it->second;

				ircdproto->SendClientIntroduction(u);

				BotInfo *bi = findbot(u->nick);
				if (bi)
				{
					bi->introduced = true;
					XLine x(bi->nick, "Reserved for services");
					ircdproto->SendSQLine(NULL, &x);
				}
			}

			for (channel_map::const_iterator it = ChannelList.begin(), it_end = ChannelList.end(); it != it_end; ++it)
			{
				Channel *c = it->second;
				if (c->users.empty())
					ircdproto->SendChannel(c);
				else
					for (CUserList::const_iterator cit = c->users.begin(), cit_end = c->users.end(); cit != cit_end; ++cit)
						ircdproto->SendJoin((*cit)->user, c, (*cit)->Status);
			}
		}
	}
}

/** Destructor
 */
Server::~Server()
{
	Log(this, "quit") << "quit from " << (this->UplinkServer ? this->UplinkServer->GetName() : "no uplink") << " for " << this->QReason;

	if (Capab.HasFlag(CAPAB_NOQUIT) || Capab.HasFlag(CAPAB_QS))
	{
		for (Anope::insensitive_map<User *>::const_iterator it = UserListByNick.begin(); it != UserListByNick.end();)
		{
			User *u = it->second;
			++it;

			if (u->server == this)
			{
				NickAlias *na = findnick(u->nick);
				if (na && !na->nc->HasFlag(NI_SUSPENDED) && (u->IsRecognized() || u->IsIdentified()))
				{
					na->last_seen = Anope::CurTime;
					na->last_quit = this->QReason;
				}

				delete u;
			}
		}

		Log(LOG_DEBUG) << "Finished removing all users for " << this->GetName();
	}

	if (this->UplinkServer)
		this->UplinkServer->DelLink(this);

	for (unsigned i = this->Links.size(); i > 0; --i)
		this->Links[i - 1]->Delete(this->QReason);
}

/** Delete this server with a reason
 * @param reason The reason
 */
void Server::Delete(const Anope::string &reason)
{
	this->QReason = reason;
	delete this;
}

/** Get the name for this server
 * @return The name
 */
const Anope::string &Server::GetName() const
{
	return this->Name;
}

/** Get the number of hops this server is from services
 * @return Number of hops
 */
unsigned Server::GetHops() const
{
	return this->Hops;
}

/** Set the server description
 * @param desc The new description
 */
void Server::SetDescription(const Anope::string &desc)
{
	this->Description = desc;
}

/** Get the server description
 * @return The server description
 */
const Anope::string &Server::GetDescription() const
{
	return this->Description;
}

/** Change this servers SID
 * @param sid The new SID
 */
void Server::SetSID(const Anope::string &sid)
{
	this->SID = sid;
}

/** Get the server numeric/SID
 * @return The numeric/SID
 */
const Anope::string &Server::GetSID() const
{
	return this->SID;
}

/** Get the list of links this server has, or NULL if it has none
 * @return A list of servers
 */
const std::vector<Server *> &Server::GetLinks() const
{
	return this->Links;
}

/** Get the uplink server for this server, if this is our uplink will be Me
 * @return The servers uplink
 */
Server *Server::GetUplink()
{
	return this->UplinkServer;
}

/** Adds a link to this server
 * @param s The linking server
 */
void Server::AddLink(Server *s)
{
	this->Links.push_back(s);

	Log(this, "connect") << "introduced " << s->GetName();
}

/** Delinks a server from this server
 * @param s The server
 */
void Server::DelLink(Server *s)
{
	if (this->Links.empty())
		throw CoreException("Server::DelLink called on " + this->GetName() + " for " + s->GetName() + " but we have no links?");

	for (unsigned i = 0, j = this->Links.size(); i < j; ++i)
	{
		if (this->Links[i] == s)
		{
			this->Links.erase(this->Links.begin() + i);
			break;
		}
	}

	Log(this, "quit") << "quit " << s->GetName();
}

/** Finish syncing this server and optionally all links to it
 * @param SyncLinks True to sync the links for this server too (if any)
 */
void Server::Sync(bool SyncLinks)
{
	if (this->IsSynced())
		return;

	this->UnsetFlag(SERVER_SYNCING);

	Log(this, "sync") << "is done syncing";

	FOREACH_MOD(I_OnServerSync, OnServerSync(this));

	if (SyncLinks && !this->Links.empty())
	{
		for (unsigned i = 0, j = this->Links.size(); i < j; ++i)
			this->Links[i]->Sync(true);
	}

	if (this->GetUplink() && this->GetUplink() == Me)
	{
		for (registered_channel_map::iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
		{
			ChannelInfo *ci = it->second;
			if (ci->HasFlag(CI_PERSIST))
			{
				bool created = false;
				if (!ci->c)
				{
					ci->c = new Channel(ci->name, ci->time_registered);
					created = true;
				}
				if (ModeManager::FindChannelModeByName(CMODE_PERM) != NULL)
				{
					ci->c->SetMode(NULL, CMODE_PERM);
					if (created)
						ircdproto->SendChannel(ci->c);
				}
				else
				{
					if (!ci->bi)
						ci->WhoSends()->Assign(NULL, ci);
					if (ci->c->FindUser(ci->bi) == NULL)
						ci->bi->Join(ci->c);
				}
			}
		}

		FOREACH_MOD(I_OnPreUplinkSync, OnPreUplinkSync(this));

		ircdproto->SendEOB();
		Me->Sync(false);

		FOREACH_MOD(I_OnUplinkSync, OnUplinkSync(this));

		for (channel_map::const_iterator it = ChannelList.begin(), it_end = ChannelList.end(); it != it_end; ++it)
		{
			Channel *c = it->second;
			check_modes(c);
			if (c->ci)
				c->ci->RestoreTopic();
		}

		if (!nofork && AtTerm())
		{
			Log(LOG_TERMINAL) << "Successfully linked, launching into background...";
			Fork();
		}
	}
}

/** Check if this server is synced
 * @return true or false
 */
bool Server::IsSynced() const
{
	return !this->HasFlag(SERVER_SYNCING);
}

/** Check if this server is ULined
 * @return true or false
 */
bool Server::IsULined() const
{
	if (this == Me)
		return true;

	for (std::list<Anope::string>::const_iterator it = Config->Ulines.begin(), it_end = Config->Ulines.end(); it != it_end; ++it)
		if (it->equals_ci(this->GetName()))
			return true;
	return false;
}

/** Find a server
 * @param name The name or SID/numeric
 * @param s The server list to search for this server on, defaults to our Uplink
 * @return The server
 */
Server *Server::Find(const Anope::string &name, Server *s)
{
	Log(LOG_DEBUG_2) << "Server::Find called for " << name;

	if (!s)
		s = Me;
	if (s->GetName().equals_ci(name) || s->GetSID().equals_cs(name))
		return s;

	if (!s->GetLinks().empty())
	{
		for (unsigned i = 0, j = s->GetLinks().size(); i < j; ++i)
		{
			Server *serv = s->GetLinks()[i];

			if (serv->GetName().equals_ci(name) || serv->GetSID().equals_cs(name))
				return serv;
			Log(LOG_DEBUG_2) << "Server::Find checking " << serv->GetName() << " server tree for " << name;
			Server *server = Server::Find(name, serv);
			if (server)
				return server;
		}
	}

	return NULL;
}

/*************************************************************************/

/**
 * Handle adding the server to the Server struct
 * @param source Name of the uplink if any
 * @param servername Name of the server being linked
 * @param hops Number of hops to reach this server
 * @param descript Description of the server
 * @param numeric Server Numberic/SUID
 * @return void
 */
void do_server(const Anope::string &source, const Anope::string &servername, unsigned int hops, const Anope::string &descript, const Anope::string &numeric)
{
	if (source.empty())
		Log(LOG_DEBUG) << "Server " << servername << " introduced";
	else
		Log(LOG_DEBUG) << "Server introduced (" << servername << ")" << " from " << source;

	Server *s = NULL;

	if (!source.empty())
	{
		s = Server::Find(source);

		if (!s)
			throw CoreException("Recieved a server from a nonexistant uplink?");
	}
	else
		s = Me;

	/* Create a server structure. */
	Server *newserver = new Server(s, servername, hops, descript, numeric);

	/* Let modules know about the connection */
	FOREACH_MOD(I_OnNewServer, OnNewServer(newserver));
}

static inline char nextID(char &c)
{
	if (c == 'Z')
		c = '0';
	else if (c != '9')
		++c;
	else
		c = 'A';
	return c;
}

/** Recieve the next UID in our list
 * @return The UID
 */
const Anope::string ts6_uid_retrieve()
{
	if (!ircd || !ircd->ts6)
		return "";

	static Anope::string current_uid = "AAAAAA";

	while (finduser(Config->Numeric + current_uid) != NULL)
	{
		int current_len = current_uid.length() - 1;
		while (current_len >= 0 && nextID(current_uid[current_len--]) == 'A');
	}

	return Config->Numeric + current_uid;
}

/** Return the next SID in our list
 * @return The SID
 */
const Anope::string ts6_sid_retrieve()
{
	if (!ircd || !ircd->ts6)
		return "";

	static Anope::string current_sid;
	if (current_sid.empty())
	{
		current_sid = Config->Numeric;
		if (current_sid.empty())
			current_sid = "00A";
	}

	while (Server::Find(current_sid) != NULL)
	{
		int current_len = current_sid.length() - 1;
		while (current_len >= 0 && nextID(current_sid[current_len--]) == 'A');
	}

	return current_sid;
}

