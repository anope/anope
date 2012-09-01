/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
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

struct CoreExport Privilege
{
	Anope::string name;
	Anope::string desc;
	int rank;

	Privilege(const Anope::string &n, const Anope::string &d, int r);
	bool operator==(const Privilege &other) const;
};

class CoreExport PrivilegeManager
{
	static std::vector<Privilege> privs;
 public:
	static void AddPrivilege(Privilege p);
	static void RemovePrivilege(Privilege &p);
	static Privilege *FindPrivilege(const Anope::string &name);
	static std::vector<Privilege> &GetPrivileges();
	static void ClearPrivileges();
};


class CoreExport AccessProvider : public Service
{
 public:
	AccessProvider(Module *o, const Anope::string &n);
	virtual ~AccessProvider();
	virtual ChanAccess *Create() = 0;

 private:
	static std::list<AccessProvider *> providers;
 public:
	static const std::list<AccessProvider *>& GetProviders();
};

class CoreExport ChanAccess : public Serializable
{
 public:
	AccessProvider *provider;
	serialize_obj<ChannelInfo> ci;
	Anope::string mask;
	Anope::string creator;
	time_t last_seen;
	time_t created;

	ChanAccess(AccessProvider *p);
	virtual ~ChanAccess();

	const Anope::string serialize_name() const anope_override;
	Serialize::Data serialize() const anope_override;
	static Serializable* unserialize(Serializable *obj, Serialize::Data &);

	virtual bool Matches(const User *u, const NickCore *nc) const;
	virtual bool HasPriv(const Anope::string &name) const = 0;
	virtual Anope::string Serialize() const = 0;
	virtual void Unserialize(const Anope::string &data) = 0;

	bool operator>(const ChanAccess &other) const;
	bool operator<(const ChanAccess &other) const;
	bool operator>=(const ChanAccess &other) const;
	bool operator<=(const ChanAccess &other) const;
};

class CoreExport AccessGroup : public std::vector<ChanAccess *>
{
 public:
 	const ChannelInfo *ci;
	const NickCore *nc;
	bool SuperAdmin, Founder;
	AccessGroup();
	bool HasPriv(const Anope::string &priv) const;
	const ChanAccess *Highest() const;
	bool operator>(const AccessGroup &other) const;
	bool operator<(const AccessGroup &other) const;
	bool operator>=(const AccessGroup &other) const;
	bool operator<=(const AccessGroup &other) const;
};

#endif

