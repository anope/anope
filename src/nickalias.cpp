#include "services.h"
#include "modules.h"

/** Default constructor
 * @param nick The nick
 * @param nickcore The nickcofe for this nick
 */
NickAlias::NickAlias(const Anope::string &nickname, NickCore *nickcore) : Flags<NickNameFlag, NS_END>(NickNameFlagStrings), Serializable("NickAlias")
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

	if (this->hostinfo.HasVhost())
	{
		data["vhost_ident"] << this->hostinfo.GetIdent();
		data["vhost_host"] << this->hostinfo.GetHost();
		data["vhost_creator"] << this->hostinfo.GetCreator();
		data["vhost_time"] << this->hostinfo.GetTime();
	}

	return data;
}

void NickAlias::unserialize(serialized_data &data)
{
	NickCore *core = findcore(data["nc"].astr());
	if (core == NULL)
		return;

	NickAlias *na = new NickAlias(data["nick"].astr(), core);
	data["last_quit"] >> na->last_quit;
	data["last_realname"] >> na->last_realname;
	data["last_usermask"] >> na->last_usermask;
	data["last_realhost"] >> na->last_realhost;
	data["time_registered"] >> na->time_registered;
	data["last_seen"] >> na->last_seen;
	na->FromString(data["flags"].astr());

	time_t vhost_time;
	data["vhost_time"] >> vhost_time;
	na->hostinfo.SetVhost(data["vhost_ident"].astr(), data["vhost_host"].astr(), data["vhost_creator"].astr(), vhost_time);
}

