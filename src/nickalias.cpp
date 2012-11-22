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
 */

#include "services.h"
#include "account.h"
#include "modules.h"
#include "opertype.h"
#include "protocol.h"
#include "users.h"
#include "servers.h"
#include "config.h"

Serialize::Checker<nickalias_map> NickAliasList("NickAlias");

NickAlias::NickAlias(const Anope::string &nickname, NickCore* nickcore) : Serializable("NickAlias")
{
	if (nickname.empty())
		throw CoreException("Empty nick passed to NickAlias constructor");
	else if (!nickcore)
		throw CoreException("Empty nickcore passed to NickAlias constructor");

	this->time_registered = this->last_seen = Anope::CurTime;
	this->nick = nickname;
	this->nc = nickcore;
	nickcore->aliases.push_back(this);

	size_t old = NickAliasList->size();
	(*NickAliasList)[this->nick] = this;
	if (old == NickAliasList->size())
		Log(LOG_DEBUG) << "Duplicate nick " << nickname << " in nickalias table";

	if (this->nc->o == NULL)
	{
		Oper *o = Oper::Find(this->nick);
		if (o == NULL)
			o = Oper::Find(this->nc->display);
		nickcore->o = o;
		if (this->nc->o != NULL)
			Log() << "Tied oper " << this->nc->display << " to type " << this->nc->o->ot->GetName();
	}
}

NickAlias::~NickAlias()
{
	FOREACH_MOD(I_OnDelNick, OnDelNick(this));

	/* Accept nicks that have no core, because of database load functions */
	if (this->nc)
	{
		/* Next: see if our core is still useful. */
		std::list<Serialize::Reference<NickAlias> >::iterator it = std::find(this->nc->aliases.begin(), this->nc->aliases.end(), this);
		if (it != this->nc->aliases.end())
			this->nc->aliases.erase(it);
		if (this->nc->aliases.empty())
		{
			this->nc->Destroy();
			this->nc = NULL;
		}
		else
		{
			/* Display updating stuff */
			if (this->nick.equals_ci(this->nc->display))
				this->nc->SetDisplay(this->nc->aliases.front());
		}
	}

	/* Remove us from the aliases list */
	NickAliasList->erase(this->nick);
}

void NickAlias::Release()
{
	if (this->HasFlag(NS_HELD))
	{
		if (IRCD->CanSVSHold)
			IRCD->SendSVSHoldDel(this->nick);
		else
		{
			User *u = User::Find(this->nick);
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
	static std::map<Anope::string, NickServHeld *> NickServHelds;

	Reference<NickAlias> na;
	Anope::string nick;
 public:
	NickServHeld(NickAlias *n, long l) : Timer(l), na(n), nick(na->nick)
	{
		std::map<Anope::string, NickServHeld *>::iterator nit = NickServHelds.find(na->nick);
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
std::map<Anope::string, NickServHeld *> NickServHeld::NickServHelds;

/** Timers for releasing nicks to be available for use
 */
class CoreExport NickServRelease : public User, public Timer
{
	static std::map<Anope::string, NickServRelease *> NickServReleases;
	Anope::string nick;

 public:
	/** Constructor
	 * @param na The nick
	 * @param delay The delay before the nick is released
	 */
	NickServRelease(NickAlias *na, time_t delay) : User(na->nick, Config->NSEnforcerUser, Config->NSEnforcerHost, "", "", Me, "Services Enforcer", Anope::CurTime, "", Servers::TS6_UID_Retrieve()), Timer(delay), nick(na->nick)
	{
		/* Erase the current release timer and use the new one */
		std::map<Anope::string, NickServRelease *>::iterator nit = NickServReleases.find(this->nick);
		if (nit != NickServReleases.end())
		{
			IRCD->SendQuit(nit->second, "");
			delete nit->second;
		}

		NickServReleases.insert(std::make_pair(this->nick, this));

		IRCD->SendClientIntroduction(this);
	}

	virtual ~NickServRelease()
	{
		NickServReleases.erase(this->nick);
	}

	/** Called when the delay is up
	 * @param t The current time
	 */
	void Tick(time_t t)
	{
		IRCD->SendQuit(this, "");
	}
};
std::map<Anope::string, NickServRelease *> NickServRelease::NickServReleases;

void NickAlias::OnCancel(User *)
{
	if (this->HasFlag(NS_COLLIDED))
	{
		this->SetFlag(NS_HELD);
		this->UnsetFlag(NS_COLLIDED);

		new NickServHeld(this, Config->NSReleaseTimeout);

		if (IRCD->CanSVSHold)
			IRCD->SendSVSHold(this->nick);
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

NickAlias *NickAlias::Find(const Anope::string &nick)
{
	nickalias_map::const_iterator it = NickAliasList->find(nick);
	if (it != NickAliasList->end())
	{
		it->second->QueueUpdate();
		return it->second;
	}

	return NULL;
}

Serialize::Data NickAlias::Serialize() const
{
	Serialize::Data data;	

	data["nick"].SetMax(Config->NickLen) << this->nick;
	data["last_quit"] << this->last_quit;
	data["last_realname"] << this->last_realname;
	data["last_usermask"] << this->last_usermask;
	data["last_realhost"] << this->last_realhost;
	data["time_registered"].SetType(Serialize::DT_INT) << this->time_registered;
	data["last_seen"].SetType(Serialize::DT_INT) << this->last_seen;
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

Serializable* NickAlias::Unserialize(Serializable *obj, Serialize::Data &data)
{
	NickCore *core = NickCore::Find(data["nc"].astr());
	if (core == NULL)
		return NULL;

	NickAlias *na;
	if (obj)
		na = anope_dynamic_static_cast<NickAlias *>(obj);
	else
		na = new NickAlias(data["nick"].astr(), core);

	if (na->nc != core)
	{
		std::list<Serialize::Reference<NickAlias> >::iterator it = std::find(na->nc->aliases.begin(), na->nc->aliases.end(), na);
		if (it != na->nc->aliases.end())
			na->nc->aliases.erase(it);

		if (na->nc->aliases.empty())
			delete na->nc;
		else if (na->nick.equals_ci(na->nc->display))
			na->nc->SetDisplay(na->nc->aliases.front());

		na->nc = core;
		core->aliases.push_back(na);
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
	return na;
}

