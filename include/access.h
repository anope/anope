#ifndef ACCESS_H
#define ACCESS_H

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

class ChanAccess;

class CoreExport AccessProvider : public Service<AccessProvider>
{
 public:
	AccessProvider(Module *o, const Anope::string &n);
	virtual ~AccessProvider();
	virtual ChanAccess *Create() = 0;
};

class CoreExport ChanAccess : public Serializable
{
 public:
	AccessProvider *provider;
	ChannelInfo *ci;
	Anope::string mask;
	Anope::string creator;
	time_t last_seen;
	time_t created;

	ChanAccess(AccessProvider *p);
	virtual ~ChanAccess();

	Anope::string serialize_name() const;
	serialized_data serialize();
	static void unserialize(serialized_data &);

	virtual bool Matches(User *u, NickCore *nc);
	virtual bool HasPriv(const Anope::string &name) const = 0;
	virtual Anope::string Serialize() = 0;
	virtual void Unserialize(const Anope::string &data) = 0;

	bool operator>(const ChanAccess &other) const;
	bool operator<(const ChanAccess &other) const;
	bool operator>=(const ChanAccess &other) const;
	bool operator<=(const ChanAccess &other) const;
};

class CoreExport AccessGroup : public std::vector<ChanAccess *>
{
 public:
 	ChannelInfo *ci;
	NickCore *nc;
	bool SuperAdmin, Founder;
	AccessGroup();
	bool HasPriv(const Anope::string &priv) const;
	ChanAccess *Highest() const;
	bool operator>(const AccessGroup &other) const;
	bool operator<(const AccessGroup &other) const;
	bool operator>=(const AccessGroup &other) const;
	bool operator<=(const AccessGroup &other) const;
};

#endif

