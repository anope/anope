/*
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#ifndef SERVERS_H
#define SERVERS_H

#include "services.h"
#include "anope.h"
#include "extensible.h"

/* Anope. We are at the top of the server tree, our uplink is
 * almost always me->GetLinks()[0]. We never have an uplink. */
extern CoreExport Server *Me;

namespace Servers
{
	/* Retrieves the next free TS6 UID or SID */
	extern CoreExport const Anope::string TS6_UID_Retrieve();
	extern CoreExport const Anope::string TS6_SID_Retrieve();

	/* Gets our uplink. Note we don't actually have an "uplink", this is just
	 * the only server whose uplink *is* Me that is not a juped server.
	 * @return Our uplink, or NULL if not uplinked to anything
	 */
	extern CoreExport Server* GetUplink();

	/* CAPAB/PROTOCTL given by the uplink */
	extern CoreExport std::set<Anope::string> Capab;
}

/** Flags set on servers
 */
enum ServerFlag
{
	SERVER_NONE,
	/* Server is syncing */
	SERVER_SYNCING,
	/* This server was juped */
	SERVER_JUPED
};

/** Class representing a server
 */
class CoreExport Server : public Flags<ServerFlag>, public Extensible
{
 private:
	/* Server name */
	Anope::string name;
	/* Hops between services and server */
	unsigned int hops;
	/* Server description */
	Anope::string description;
	/* Server ID */
	Anope::string sid;
	/* Links for this server */
	std::vector<Server *> links;
	/* Uplink for this server */
	Server *uplink;

	/* Reason this server was quit */
	Anope::string quit_reason;

 public:
	/** Constructor
	 * @param uplink The uplink this server is from, is only NULL when creating Me
	 * @param name The server name
	 * @param hops Hops from services server
	 * @param description Server rdescription
	 * @param sid Server sid/numeric
	 * @param flag An optional server flag
	 */
	Server(Server *uplink, const Anope::string &name, unsigned hops, const Anope::string &description, const Anope::string &sid = "", ServerFlag flag = SERVER_NONE);

 private:
	/** Destructor
	 */
	~Server();

 public:
 	/* Number of users on the server */
 	unsigned users;

	/** Delete this server with a reason
	 * @param reason The reason
	 */
	void Delete(const Anope::string &reason);

	/** Get the name for this server
	 * @return The name
	 */
	const Anope::string &GetName() const;

	/** Get the number of hops this server is from services
	 * @return Number of hops
	 */
	unsigned GetHops() const;

	/** Set the server description
	 * @param desc The new description
	 */
	void SetDescription(const Anope::string &desc);

	/** Get the server description
	 * @return The server description
	 */
	const Anope::string &GetDescription() const;

	/** Change this servers SID
	 * @param sid The new SID
	 */
	void SetSID(const Anope::string &sid);

	/** Get the server numeric/SID, else the server name
	 * @return The numeric/SID
	 */
	const Anope::string &GetSID() const;

	/** Get the list of links this server has, or NULL if it has none
	 * @return A list of servers
	 */
	const std::vector<Server *> &GetLinks() const;

	/** Get the uplink server for this server, if this is our uplink will be Me
	 * @return The servers uplink
	 */
	Server *GetUplink();

	/** Adds a link to this server
	 * @param s The linking server
	 */
	void AddLink(Server *s);

	/** Delinks a server from this server
	 * @param s The server
	 */
	void DelLink(Server *s);

	/** Finish syncing this server and optionally all links to it
	 * @param SyncLinks True to sync the links for this server too (if any)
	 */
	void Sync(bool sync_links);

	/** Check if this server is synced
	 * @return true or false
	 */
	bool IsSynced() const;

	/** Check if this server is ULined
	 * @return true or false
	 */
	bool IsULined() const;

	/** Send a message to alll users on this server
	 * @param source The source of the message
	 * @param message The message
	 */
	void Notice(const BotInfo *source, const Anope::string &message);

	/** Find a server
	 * @param name The name or SID/numeric
	 * @param s The server list to search for this server on, defaults to our Uplink
	 * @return The server
	 */
	static Server *Find(const Anope::string &name, Server *s = NULL);
};

#endif // SERVERS_H
