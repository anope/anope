#ifndef ACCESS_H
#define ACCESS_H

enum ChannelAccess
{
	CA_ACCESS_LIST,
	CA_NOKICK,
	CA_FANTASIA,
	CA_GREET,
	CA_AUTOVOICE,
	CA_VOICEME,
	CA_VOICE,
	CA_INFO,
	CA_SAY,
	CA_AUTOHALFOP,
	CA_HALFOPME,
	CA_HALFOP,
	CA_KICK,
	CA_SIGNKICK,
	CA_BAN,
	CA_TOPIC,
	CA_MODE,
	CA_GETKEY,
	CA_INVITE,
	CA_UNBAN,
	CA_AUTOOP,
	CA_OPDEOPME,
	CA_OPDEOP,
	CA_AUTOPROTECT,
	CA_AKICK,
	CA_BADWORDS,
	CA_ASSIGN,
	CA_MEMO,
	CA_ACCESS_CHANGE,
	CA_PROTECTME,
	CA_PROTECT,
	CA_SET,
	CA_AUTOOWNER,
	CA_OWNERME,
	CA_OWNER,
	CA_FOUNDER,
	CA_SIZE
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
	virtual bool HasPriv(ChannelAccess priv) = 0;
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
	bool HasPriv(ChannelAccess priv) const;
	ChanAccess *Highest() const;
	bool operator>(const AccessGroup &other) const;
	bool operator<(const AccessGroup &other) const;
	bool operator>=(const AccessGroup &other) const;
	bool operator<=(const AccessGroup &other) const;
};

#endif

