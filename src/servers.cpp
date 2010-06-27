/* Routines to maintain a list of connected servers
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"

char *TS6SID = NULL;

/* Anope */
Server *Me = NULL;

CapabInfo Capab_Info[] = {
	{"NOQUIT", CAPAB_NOQUIT},
	{"TSMODE", CAPAB_TSMODE},
	{"UNCONNECT", CAPAB_UNCONNECT},
	{"NICKIP", CAPAB_NICKIP},
	{"SSJOIN", CAPAB_NSJOIN},
	{"ZIP", CAPAB_ZIP},
	{"BURST", CAPAB_BURST},
	{"TS5", CAPAB_TS5},
	{"TS3", CAPAB_TS3},
	{"DKEY", CAPAB_DKEY},
	{"PT4", CAPAB_PT4},
	{"SCS", CAPAB_SCS},
	{"QS", CAPAB_QS},
	{"UID", CAPAB_UID},
	{"KNOCK", CAPAB_KNOCK},
	{"CLIENT", CAPAB_CLIENT},
	{"IPV6", CAPAB_IPV6},
	{"SSJ5", CAPAB_SSJ5},
	{"SN2", CAPAB_SN2},
	{"TOK1", CAPAB_TOKEN},
	{"TOKEN", CAPAB_TOKEN},
	{"VHOST", CAPAB_VHOST},
	{"SSJ3", CAPAB_SSJ3},
	{"SJB64", CAPAB_SJB64},
	{"CHANMODES", CAPAB_CHANMODE},
	{"NICKCHARS", CAPAB_NICKCHARS},
	{"", CAPAB_END}
};

Flags<CapabType, CAPAB_END> Capab;

/** Constructor
 * @param uplink The uplink this server is from, is only NULL when creating Me
 * @param name The server name
 * @param hops Hops from services server
 * @param description Server rdescription
 * @param sid Server sid/numeric
 */
Server::Server(Server *uplink, const std::string &name, unsigned int hops, const std::string &description, const std::string &sid) : Name(name), Hops(hops), Description(description), SID(sid), UplinkServer(uplink)
{
	Links = NULL;

	SetFlag(SERVER_SYNCING);

	Alog(LOG_DEBUG) << "Creating " << GetName() << " (" << GetSID() << ") uplinked to " << (UplinkServer ? UplinkServer->GetName() : "no uplink");

	/* Add this server to our uplinks leaf list */
	if (UplinkServer)
		UplinkServer->AddLink(this);

	if (Me && UplinkServer && Me == UplinkServer)
	{
		/* Bring in our pseudo-clients */
		introduce_user("");

		/* And some IRCds needs Global joined in the logchan */
		if (LogChan && ircd->join2msg)
			/* XXX might desync */
			ircdproto->SendJoin(Global, Config.LogChannel, time(NULL));
	}
}

/** Destructor
 */
Server::~Server()
{
	Alog(LOG_DEBUG) << "Deleting server " << GetName() << " (" << GetSID() << ") uplinked to " << GetName();

	if (Capab.HasFlag(CAPAB_NOQUIT) || Capab.HasFlag(CAPAB_QS))
	{
		time_t t = time(NULL);

		for (user_map::const_iterator it = UserListByNick.begin(), it_end = UserListByNick.end(); it != it_end; )
		{
			User *u = it->second;
			++it;

			if (u->server == this)
			{
				NickAlias *na = findnick(u->nick);
				if (na && !na->HasFlag(NS_FORBIDDEN) && (!na->nc->HasFlag(NI_SUSPENDED)) && (u->IsRecognized() || u->IsIdentified()))
				{
					na->last_seen = t;
					if (na->last_quit)
						delete [] na->last_quit;
					na->last_quit = (!QReason.empty() ? sstrdup(QReason.c_str()) : NULL);
				}

				if (Config.LimitSessions && !u->server->IsULined())
					del_session(u->host);

				delete u;
			}
		}

		Alog(LOG_DEBUG) << "Finished removing all users for " << GetName();
	}

	if (Links)
	{
		for (std::list<Server *>::iterator it = Links->begin(), it_end = Links->end(); it != it_end; ++it)
			delete *it;
	}

	delete Links;
}

/** Delete this server with a reason
 * @param reason The reason
 */
void Server::Delete(const std::string &reason)
{
	QReason = reason;
	delete this;
}

/** Get the name for this server
 * @return The name
 */
const std::string &Server::GetName() const
{
	return Name;
}

/** Get the number of hops this server is from services
 * @return Number of hops
 */
unsigned int Server::GetHops() const
{
	return Hops;
}

/** Set the server description
 * @param desc The new description
 */
void Server::SetDescription(const std::string &desc)
{
	Description = desc;
}

/** Get the server description
 * @return The server description
 */
const std::string &Server::GetDescription() const
{
	return Description;
}

/** Get the server numeric/SID
 * @return The numeric/SID
 */
const std::string &Server::GetSID() const
{
	return SID;
}

/** Get the list of links this server has, or NULL if it has none
 * @return A list of servers
 */
const std::list<Server*> *Server::GetLinks() const
{
	return Links;
}

/** Get the uplink server for this server, if this is our uplink will be Me
 * @return The servers uplink
 */
Server *Server::GetUplink() const
{
	return UplinkServer;
}

/** Adds a link to this server
 * @param s The linking server
 */
void Server::AddLink(Server *s)
{
	/* This is only used for Me, initially we start services with an uplink of NULL, then
	 * connect to the server which introduces itself and has us as the uplink, which calls this
	 */
	if (!UplinkServer)
		UplinkServer = s;
	else
	{
		if (!Links)
			Links = new std::list<Server *>();

		Links->push_back(s);
	}

	Alog() << "Server " << s->GetName() << " introduced from " << GetName();
}

/** Delinks a server from this server
 * @param s The server
 */
void Server::DelLink(Server *s)
{
	if (!Links)
		throw CoreException("Server::DelLink called on " + GetName() + " for " + s->GetName() + " but we have no links?");

	for (std::list<Server *>::iterator it = Links->begin(), it_end = Links->end(); it != it_end; ++it)
	{
		if (*it == s)
		{
			Links->erase(it);
			break;
		}
	}

	if (Links->empty())
	{
		delete Links;
		Links = NULL;
	}

	Alog() << "Server " << s->GetName() << " quit from " << GetName();
}

/** Finish syncing this server and optionally all links to it
 * @param SyncLinks True to sync the links for this server too (if any)
 */
void Server::Sync(bool SyncLinks)
{
	if (IsSynced())
		return;

	UnsetFlag(SERVER_SYNCING);

	if (SyncLinks && Links)
	{
		for (std::list<Server *>::iterator it = Links->begin(), it_end = Links->end(); it != it_end; ++it)
			(*it)->Sync(true);
	}

	if (this == Me->GetUplink())
	{
		FOREACH_MOD(I_OnPreUplinkSync, OnPreUplinkSync(this));
		ircdproto->SendEOB();
		Me->UnsetFlag(SERVER_SYNCING);
	}

	Alog() << "Server " << GetName() << " is done syncing";

	FOREACH_MOD(I_OnServerSync, OnServerSync(this));

	if (this == Me->GetUplink())
	{
		FOREACH_MOD(I_OnUplinkSync, OnUplinkSync(this));
		restore_unsynced_topics();
	}
}

/** Check if this server is synced
 * @return true or false
 */
bool Server::IsSynced() const
{
	return !HasFlag(SERVER_SYNCING);
}

/** Check if this server is ULined
 * @return true or false
 */
bool Server::IsULined() const
{
	for (int j = 0; j < Config.NumUlines; ++j)
		if (!stricmp(Config.Ulines[j], GetName().c_str()))
			return true;
	return false;
}

/** Find a server
 * @param name The name or SID/numeric
 * @param s The server list to search for this server on, defaults to our Uplink
 * @return The server
 */
Server *Server::Find(const std::string &name, Server *s)
{
	if (!s)
		s = Me->GetUplink();
	if (s->GetName() == name || s->GetSID() == name)
		return s;

	if (s->GetLinks())
	{
		for (std::list<Server *>::const_iterator it = s->GetLinks()->begin(), it_end = s->GetLinks()->end(); it != it_end; ++it)
		{
			Server *serv = *it;

			if (serv->GetName() == name || serv->GetSID() == name)
				return serv;
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
void do_server(const std::string &source, const std::string &servername, unsigned int hops, const std::string &descript, const std::string &numeric)
{
	if (source.empty())
		Alog(LOG_DEBUG) << "Server " << servername << " introduced";
	else
		Alog(LOG_DEBUG) << "Server introduced (" << servername << ")" << " from " << source;

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

	/* Announce services being online. */
	if (Config.GlobalOnCycle && Config.GlobalOnCycleUP)
		notice_server(Config.s_GlobalNoticer, newserver, "%s", Config.GlobalOnCycleUP);

	/* Let modules know about the connection */
	FOREACH_MOD(I_OnNewServer, OnNewServer(newserver));
}

/*************************************************************************/

/**
 * Handle removing the server from the Server struct
 * @param source Name of the server leaving
 * @param ac Number of arguments in av
 * @param av Agruments as part of the SQUIT
 * @return void
 */
void do_squit(const char *source, int ac, const char **av)
{
	char buf[BUFSIZE];
	Server *s = Server::Find(av[0]);

	if (!s)
	{
		Alog() << "SQUIT for nonexistent server (" << av[0] << ")!!";
		return;
	}

	FOREACH_MOD(I_OnServerQuit, OnServerQuit(s));

	/* If this is a juped server, send a nice global to inform the online
	 * opers that we received it.
	 */
	if (s->HasFlag(SERVER_JUPED))
	{
		snprintf(buf, BUFSIZE, "Received SQUIT for juped server %s", s->GetName().c_str());
		ircdproto->SendGlobops(OperServ, buf);
	}

	snprintf(buf, sizeof(buf), "%s %s", s->GetName().c_str(), s->GetUplink()->GetName().c_str());

	if (s->GetUplink() == Me->GetUplink() && Capab.HasFlag(CAPAB_UNCONNECT))
	{
		Alog(LOG_DEBUG) << "Sending UNCONNECT SQUIT for " << s->GetName();
		/* need to fix */
		ircdproto->SendSquit(s->GetName().c_str(), buf);
	}

	s->Delete(buf);
}

/*************************************************************************/

/** Handle parsing the CAPAB/PROTOCTL messages
 * @param ac Number of args
 * @param av Args
 */
void CapabParse(int ac, const char **av)
{
	for (int i = 0; i < ac; ++i)
	{
		for (unsigned j = 0; !Capab_Info[j].Token.empty(); ++j)
		{
			if (av[i] == Capab_Info[j].Token)
			{
				Capab.SetFlag(Capab_Info[j].Flag);

				if (Capab_Info[j].Token == "NICKIP" && !ircd->nickip)
					ircd->nickip = 1;
				break;
			}
		}
	}

	/* Apply MLock now that we know what modes exist (capab is parsed after modes are added to Anope) */
	for (registered_channel_map::iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
	{
		ChannelInfo *ci = it->second;

		ci->LoadMLock();
	}
}

/*************************************************************************/

/* TS6 UID generator common code.
 *
 * Derived from atheme-services, uid.c (hg 2954:116d46894b4c).
 *    -nenolod
 */
static bool ts6_uid_initted = false;
static char ts6_new_uid[10];

static void ts6_uid_increment(unsigned int slot)
{
	if (slot != strlen(TS6SID))
	{
		if (ts6_new_uid[slot] == 'Z')
			ts6_new_uid[slot] = '0';
		else if (ts6_new_uid[slot] == '9')
		{
			ts6_new_uid[slot] = 'A';
			ts6_uid_increment(slot - 1);
		}
		else
			++ts6_new_uid[slot];
	}
	else
	{
		if (ts6_new_uid[slot] == 'Z')
			for (slot = 3; slot < 9; ++slot)
				ts6_new_uid[slot] = 'A';
		else
			++ts6_new_uid[slot];
	}
}

/** Recieve the next UID in our list
 * @return The UID
 */
const char *ts6_uid_retrieve()
{
	if (!ircd->ts6)
	{
		Alog(LOG_DEBUG) << "ts6_uid_retrieve(): TS6 not supported on this ircd";
		return "";
	}

	if (!ts6_uid_initted)
	{
		snprintf(ts6_new_uid, 10, "%sAAAAAA", TS6SID);
		ts6_uid_initted = true;
	}

	ts6_uid_increment(8);
	return ts6_new_uid;
}

/*******************************************************************/

/*
 * TS6 SID generator code, provided by DukePyrolator
 */

static bool ts6_sid_initted = false;
static char ts6_new_sid[4];

static void ts6_sid_increment(unsigned pos)
{
	/*
	 * An SID must be exactly 3 characters long, starts with a digit,
	 * and the other two characters are A-Z or digits
	 * The rules for generating an SID go like this...
	 * --> ABCDEFGHIJKLMNOPQRSTUVWXYZ --> 0123456789 --> WRAP
	 */
	if (!pos)
	{
		/* At pos 0, if we hit '9', we've run out of available SIDs,
		 * reset the SID to the smallest possible value and try again. */
		if (ts6_new_sid[pos] == '9')
		{
			ts6_new_sid[0] = '0';
			ts6_new_sid[1] = 'A';
			ts6_new_sid[2] = 'A';
		}
		else
			// But if we haven't, just keep incrementing merrily.
			++ts6_new_sid[0];
	}
	else
	{
		if (ts6_new_sid[pos] == 'Z')
			ts6_new_sid[pos] = '0';
		else if (ts6_new_sid[pos] == '9')
		{
			ts6_new_sid[pos] = 'A';
			ts6_sid_increment(pos - 1);
		}
		else
			++ts6_new_sid[pos];
	}
}

/** Return the next SID in our list
 * @return The SID
 */
const char *ts6_sid_retrieve()
{
	if (!ircd->ts6)
	{
		Alog(LOG_DEBUG) << "ts6_sid_retrieve(): TS6 not supported on this ircd";
		return "";
	}

	if (!ts6_sid_initted)
	{
		// Initialize ts6_new_sid with the services server SID
		snprintf(ts6_new_sid, 4, "%s", TS6SID);
		ts6_sid_initted = true;
	}

	while (1)
	{
		// Check if the new SID is used by a known server
		if (!Server::Find(ts6_new_sid))
			// return the new SID
			return ts6_new_sid;

		// Add one to the last SID
		ts6_sid_increment(2);
	}
	/* not reached */
	return "";
}
