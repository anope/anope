#include "services.h"
#include "modules.h"

NickRequest::NickRequest(const Anope::string &nickname)
{
	if (nickname.empty())
		throw CoreException("Empty nick passed to NickRequest constructor");

	requested = lastmail = 0;

	this->nick = nickname;

	NickRequestList[this->nick] = this;
}

NickRequest::~NickRequest()
{
	FOREACH_MOD(I_OnDelNickRequest, OnDelNickRequest(this));

	NickRequestList.erase(this->nick);
}

/** Default constructor
 * @param nick The nick
 * @param nickcore The nickcofe for this nick
 */
NickAlias::NickAlias(const Anope::string &nickname, NickCore *nickcore)
{
	if (nickname.empty())
		throw CoreException("Empty nick passed to NickAlias constructor");
	else if (!nickcore)
		throw CoreException("Empty nickcore passed to NickAlias constructor");

	time_registered = last_seen = 0;

	this->nick = nickname;
	this->nc = nickcore;
	nc->aliases.push_back(this);

	NickAliasList[this->nick] = this;

	for (std::list<std::pair<Anope::string, Anope::string> >::iterator it = Config.Opers.begin(), it_end = Config.Opers.end(); it != it_end; ++it)
	{
		if (nc->ot)
			break;
		if (!this->nick.equals_ci(it->first))
			continue;

		for (std::list<OperType *>::iterator tit = Config.MyOperTypes.begin(), tit_end = Config.MyOperTypes.end(); tit != tit_end; ++tit)
		{
			OperType *ot = *tit;

			if (ot->GetName().equals_ci(it->second))
			{
				Alog() << "Tied oper " << nc->display << " to type " << ot->GetName();
				nc->ot = ot;
				break;
			}
		}
	}
}

/** Default destructor
 */
NickAlias::~NickAlias()
{
	User *u = NULL;

	FOREACH_MOD(I_OnDelNick, OnDelNick(this));

	/* Second thing to do: look for an user using the alias
	 * being deleted, and make appropriate changes */
	if ((u = finduser(this->nick)) && u->Account())
	{
		ircdproto->SendAccountLogout(u, u->Account());
		u->RemoveMode(NickServ, UMODE_REGISTERED);
		ircdproto->SendUnregisteredNick(u);
		u->Logout();
	}

	/* Accept nicks that have no core, because of database load functions */
	if (this->nc)
	{
		/* Next: see if our core is still useful. */
		std::list<NickAlias *>::iterator it = std::find(this->nc->aliases.begin(), this->nc->aliases.end(), this);
		if (it != this->nc->aliases.end())
			nc->aliases.erase(it);
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
			ircdproto->SendQuit(this->nick, "");

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

		if (ircd->svshold)
			ircdproto->SendSVSHold(this->nick);
		else
		{
			Anope::string uid = ircd->ts6 ? ts6_uid_retrieve() : "";

			ircdproto->SendClientIntroduction(this->nick, Config.NSEnforcerUser, Config.NSEnforcerHost, "Services Enforcer", "+", uid);
			new NickServRelease(this->nick, uid, Config.NSReleaseTimeout);
		}
	}
}
