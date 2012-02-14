/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */


#include "services.h"
#include "account.h"
#include "modules.h"
#include "opertype.h"
#include "protocol.h"
#include "users.h"
#include "servers.h"
#include "config.h"

class NickServHeld;

typedef std::map<Anope::string, NickServHeld *> nickservheld_map;
static nickservheld_map NickServHelds;

/** Default constructor
 * @param nick The nick
 * @param nickcore The nickcofe for this nick
 */
NickAlias::NickAlias(const Anope::string &nickname, NickCore *nickcore) : Flags<NickNameFlag, NS_END>(NickNameFlagStrings)
{
	if (nickname.empty())
		throw CoreException("Empty nick passed to NickAlias constructor");
	else if (!nickcore)
		throw CoreException("Empty nickcore passed to NickAlias constructor");

	this->time_registered = this->last_seen = Anope::CurTime;
	this->nick = nickname;
	this->nc = nickcore;
	this->nc->aliases.push_back(this);

	NickAliasList[this->nick] = this;

	if (this->nc->o == NULL)
	{
		Oper *o = Oper::Find(this->nick);
		if (o == NULL)
			o = Oper::Find(this->nc->display);
		this->nc->o = o;
		if (this->nc->o != NULL)
			Log() << "Tied oper " << this->nc->display << " to type " << this->nc->o->ot->GetName();
	}
}

/** Default destructor
 */
NickAlias::~NickAlias()
{
	FOREACH_MOD(I_OnDelNick, OnDelNick(this));

	/* Accept nicks that have no core, because of database load functions */
	if (this->nc)
	{
		/* Next: see if our core is still useful. */
		std::list<NickAlias *>::iterator it = std::find(this->nc->aliases.begin(), this->nc->aliases.end(), this);
		if (it != this->nc->aliases.end())
			this->nc->aliases.erase(it);
		if (this->nc->aliases.empty())
		{
			delete this->nc;
			this->nc = NULL;
		}
		else
		{
			/* Display updating stuff */
			if (this->nick.equals_ci(this->nc->display))
				change_core_display(this->nc);
		}
	}

	/* Remove us from the aliases list */
	NickAliasList.erase(this->nick);
}

/** Release a nick from being held. This can be called from the core (ns_release)
 * or from a timer used when forcing clients off of nicks. Note that if this is called
 * from a timer, ircd->svshold is NEVER true
 */
void NickAlias::Release()
{
	if (this->HasFlag(NS_HELD))
	{
		if (ircd->svshold)
			ircdproto->SendSVSHoldDel(this->nick);
		else
		{
			User *u = finduser(this->nick);
			if (u && u->server == Me)
			{
				delete u;
			}
		}

		this->UnsetFlag(NS_HELD);
	}
}

/** Timers for removing HELD status from nicks.
 */
class NickServHeld : public Timer
{
	dynamic_reference<NickAlias> na;
	Anope::string nick;
 public:
	NickServHeld(NickAlias *n, long l) : Timer(l), na(n), nick(na->nick)
	{
		nickservheld_map::iterator nit = NickServHelds.find(na->nick);
		if (nit != NickServHelds.end())
			delete nit->second;

		NickServHelds[na->nick] = this;
	}

	~NickServHeld()
	{
		NickServHelds.erase(this->nick);
	}

	void Tick(time_t)
	{
		if (na)
			na->UnsetFlag(NS_HELD);
	}
};

/** Timers for releasing nicks to be available for use
 */
class CoreExport NickServRelease : public User, public Timer
{
	static std::map<Anope::string, NickServRelease *> NickServReleases;
	Anope::string nick;

 public:
	/** Default constructor
	 * @param na The nick
	 * @param delay The delay before the nick is released
	 */
	NickServRelease(NickAlias *na, time_t delay) : User(na->nick, Config->NSEnforcerUser, Config->NSEnforcerHost, ts6_uid_retrieve()), Timer(delay), nick(na->nick)
	{
		this->realname = "Services Enforcer";
		this->server = Me;

		/* Erase the current release timer and use the new one */
		std::map<Anope::string, NickServRelease *>::iterator nit = NickServReleases.find(this->nick);
		if (nit != NickServReleases.end())
		{
			ircdproto->SendQuit(nit->second, "");
			delete nit->second;
		}

		NickServReleases.insert(std::make_pair(this->nick, this));

		ircdproto->SendClientIntroduction(this);
	}

	/** Default destructor
	 */
	virtual ~NickServRelease()
	{
		NickServReleases.erase(this->nick);
	}

	/** Called when the delay is up
	 * @param t The current time
	 */
	void Tick(time_t t)
	{
		ircdproto->SendQuit(this, "");
	}
};
std::map<Anope::string, NickServRelease *> NickServRelease::NickServReleases;

/** Called when a user gets off this nick
 * See the comment in users.cpp for User::Collide()
 * @param u The user
 */
void NickAlias::OnCancel(User *)
{
	if (this->HasFlag(NS_COLLIDED))
	{
		this->SetFlag(NS_HELD);
		this->UnsetFlag(NS_COLLIDED);

		new NickServHeld(this, Config->NSReleaseTimeout);

		if (ircd->svshold)
			ircdproto->SendSVSHold(this->nick);
		else
			new NickServRelease(this, Config->NSReleaseTimeout);
	}
}

void NickAlias::SetVhost(const Anope::string &ident, const Anope::string &host, const Anope::string &creator, time_t created)
{
	this->vhost_ident = ident;
	this->vhost_host = host;
	this->vhost_creator = creator;
	this->vhost_created = created;
}

void NickAlias::RemoveVhost()
{
	this->vhost_ident.clear();
	this->vhost_host.clear();
	this->vhost_creator.clear();
	this->vhost_created = 0;
}

bool NickAlias::HasVhost() const
{
	return !this->vhost_host.empty();
}

const Anope::string &NickAlias::GetVhostIdent() const
{
	return this->vhost_ident;
}

const Anope::string &NickAlias::GetVhostHost() const
{
	return this->vhost_host;
}

const Anope::string &NickAlias::GetVhostCreator() const
{
	return this->vhost_creator;
}

time_t NickAlias::GetVhostCreated() const
{
	return this->vhost_created;
}

Anope::string NickAlias::serialize_name() const
{
	return "NickAlias";
}

Serializable::serialized_data NickAlias::serialize()
{
	serialized_data data;	

	data["nick"].setKey().setMax(Config->NickLen) << this->nick;
	data["last_quit"] << this->last_quit;
	data["last_realname"] << this->last_realname;
	data["last_usermask"] << this->last_usermask;
	data["last_realhost"] << this->last_realhost;
	data["time_registered"].setType(Serialize::DT_INT) << this->time_registered;
	data["last_seen"].setType(Serialize::DT_INT) << this->last_seen;
	data["nc"] << this->nc->display;
	data["flags"] << this->ToString();

	if (this->HasVhost())
	{
		data["vhost_ident"] << this->GetVhostIdent();
		data["vhost_host"] << this->GetVhostHost();
		data["vhost_creator"] << this->GetVhostCreator();
		data["vhost_time"] << this->GetVhostCreated();
	}

	return data;
}

void NickAlias::unserialize(serialized_data &data)
{
	NickCore *core = findcore(data["nc"].astr());
	if (core == NULL)
		return;

	NickAlias *na = findnick(data["nick"].astr());
	if (na == NULL)
		na = new NickAlias(data["nick"].astr(), core);
	else if (na->nc != core)
	{
		std::list<NickAlias *>::iterator it = std::find(na->nc->aliases.begin(), na->nc->aliases.end(), na);
		if (it != na->nc->aliases.end())
			na->nc->aliases.erase(it);

		if (na->nc->aliases.empty())
			delete na->nc;
		else if (na->nick.equals_ci(na->nc->display))
			change_core_display(na->nc);

		na->nc = core;
		na->nc->aliases.push_back(na);
	}

	data["last_quit"] >> na->last_quit;
	data["last_realname"] >> na->last_realname;
	data["last_usermask"] >> na->last_usermask;
	data["last_realhost"] >> na->last_realhost;
	data["time_registered"] >> na->time_registered;
	data["last_seen"] >> na->last_seen;
	na->FromString(data["flags"].astr());

	time_t vhost_time;
	data["vhost_time"] >> vhost_time;
	na->SetVhost(data["vhost_ident"].astr(), data["vhost_host"].astr(), data["vhost_creator"].astr(), vhost_time);
}

