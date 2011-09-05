#ifndef ACCESS_H
#define ACCESS_H

struct Privilege
{
	Anope::string name;
	Anope::string desc;

	Privilege(const Anope::string &n, const Anope::string &d);
	bool operator==(const Privilege &other);
};

class PrivilegeManager
{
	static std::vector<Privilege> privs;
 public:
	static void AddPrivilege(Privilege p, int pos = -1, int def = 0);
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

class CoreExport ChanAccess
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
	virtual bool Matches(User *u, NickCore *nc) = 0;
	virtual bool HasPriv(const Anope::string &name) = 0;	
	virtual Anope::string Serialize() = 0;
	virtual void Unserialize(const Anope::string &data) = 0;

	bool operator>(ChanAccess &other);
	bool operator<(ChanAccess &other);
	bool operator>=(ChanAccess &other);
	bool operator<=(ChanAccess &other);
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

