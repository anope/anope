/*
 *
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifndef ACCESS_H
#define ACCESS_H

#include "services.h"
#include "anope.h"
#include "serialize.h"
#include "service.h"

enum
{
	ACCESS_INVALID = -10000,
	ACCESS_FOUNDER = 10001
};

/* A privilege, probably configured using a privilege{} block. Most
 * commands require specific privileges to be executed. The AccessProvider
 * backing each ChanAccess determines whether that ChanAccess has a given
 * privilege.
 */
struct CoreExport Privilege
{
	Anope::string name;
	Anope::string desc;
	/* Rank relative to other privileges */
	int rank;

	Privilege(const Anope::string &name, const Anope::string &desc, int rank);
	bool operator==(const Privilege &other) const;
};

class CoreExport PrivilegeManager
{
	static std::vector<Privilege> Privileges;
 public:
	static void AddPrivilege(Privilege p);
	static void RemovePrivilege(Privilege &p);
	static Privilege *FindPrivilege(const Anope::string &name);
	static std::vector<Privilege> &GetPrivileges();
	static void ClearPrivileges();
};

/* A provider of access. Only used for creating ChanAccesses, as
 * they contain pure virtual functions.
 */
class CoreExport AccessProvider : public Service
{
 public:
	AccessProvider(Module *owner, const Anope::string &name);
	virtual ~AccessProvider();

	/** Creates a new ChanAccess entry using this provider.
	 * @return The new entry
	 */
	virtual ChanAccess *Create() = 0;

 private:
	static std::list<AccessProvider *> Providers;
 public:
	static const std::list<AccessProvider *>& GetProviders();
};

/* Represents one entry of an access list on a channel. */
class CoreExport ChanAccess : public Serializable
{
	Anope::string mask;
	/* account this access entry is for, if any */
	Serialize::Reference<NickCore> nc;

 public:
	typedef std::multimap<const ChanAccess *, const ChanAccess *> Set;
 	/* shows the 'path' taken to determine if an access entry matches a user
	 * .first are access entries checked
	 * .second are access entries which match
	 */
	typedef std::pair<Set, Set> Path;

 	/* The provider that created this access entry */
	AccessProvider *provider;
	/* Channel this access entry is on */
	Serialize::Reference<ChannelInfo> ci;
	Anope::string creator;
	time_t last_seen;
	time_t created;

	ChanAccess(AccessProvider *p);
	virtual ~ChanAccess();

	void SetMask(const Anope::string &mask, ChannelInfo *ci);
	const Anope::string &Mask() const;
	NickCore *GetAccount() const;

	void Serialize(Serialize::Data &data) const anope_override;
	static Serializable* Unserialize(Serializable *obj, Serialize::Data &);

	/** Check if this access entry matches the given user or account
	 * @param u The user
	 * @param nc The account
	 * @param p The path to the access object which matches will be put here
	 */
	virtual bool Matches(const User *u, const NickCore *nc, Path &p) const;

	/** Check if this access entry has the given privilege.
	 * @param name The privilege name
	 */
	virtual bool HasPriv(const Anope::string &name) const = 0;

	/** Serialize the access given by this access entry into a human
	 * readable form. chanserv/access will return a number, chanserv/xop
	 * will be AOP, SOP, etc.
	 */
	virtual Anope::string AccessSerialize() const = 0;

	/** Unserialize this access entry from the given data. This data
	 * will be fetched from AccessSerialize.
	 */
	virtual void AccessUnserialize(const Anope::string &data) = 0;

	/* Comparison operators to other Access entries */
	virtual bool operator>(const ChanAccess &other) const;
	virtual bool operator<(const ChanAccess &other) const;
	bool operator>=(const ChanAccess &other) const;
	bool operator<=(const ChanAccess &other) const;
};

/* A group of access entries. This is used commonly, for example with ChannelInfo::AccessFor,
 * to show what access a user has on a channel because users can match multiple access entries.
 */
class CoreExport AccessGroup : public std::vector<ChanAccess *>
{
 public:
	/* Channel these access entries are on */
 	const ChannelInfo *ci;
	/* Path from these entries to other entries that they depend on */
	ChanAccess::Path path;
	/* Account these entries affect, if any */
	const NickCore *nc;
	/* super_admin always gets all privs. founder is a special case where ci->founder == nc */
	bool super_admin, founder;

	AccessGroup();

	/** Check if this access group has a certain privilege. Eg, it
	 * will check every ChanAccess entry of this group for any that
	 * has the given privilege.
	 * @param priv The privilege
	 * @return true if any entry has the given privilege
	 */
	bool HasPriv(const Anope::string &priv) const;

	/** Get the "highest" access entry from this group of entries.
	 * The highest entry is determined by the entry that has the privilege
	 * with the highest rank (see Privilege::rank).
	 * @return The "highest" entry
	 */
	const ChanAccess *Highest() const;

	/* Comparison operators to other AccessGroups */
	bool operator>(const AccessGroup &other) const;
	bool operator<(const AccessGroup &other) const;
	bool operator>=(const AccessGroup &other) const;
	bool operator<=(const AccessGroup &other) const;
};

#endif
