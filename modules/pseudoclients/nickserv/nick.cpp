/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#include "module.h"
#include "nick.h"

NickImpl::NickImpl(const Anope::string &nickname, NickServ::Account* nickcore)
{
	if (nickname.empty())
		throw CoreException("Empty nick passed to NickServ::Nick constructor");
	else if (!nickcore)
		throw CoreException("Empty nickcore passed to NickServ::Nick constructor");

	this->time_registered = this->last_seen = Anope::CurTime;
	this->nick = nickname;
	this->nc = nickcore;
	nickcore->aliases->push_back(this);

	NickServ::nickalias_map &map = NickServ::service->GetNickList();
	NickServ::Nick* &na = map[this->nick];
	if (na)
		Log(LOG_DEBUG) << "Duplicate nick " << nickname << " in nickalias table";
	na = this;

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

NickImpl::~NickImpl()
{
	Event::OnDelNick(&Event::DelNick::OnDelNick, this);

	/* Accept nicks that have no core, because of database load functions */
	if (this->nc)
	{
		/* Next: see if our core is still useful. */
		std::vector<NickServ::Nick *>::iterator it = std::find(this->nc->aliases->begin(), this->nc->aliases->end(), this);
		if (it != this->nc->aliases->end())
			this->nc->aliases->erase(it);
		if (this->nc->aliases->empty())
		{
			delete this->nc;
			this->nc = NULL;
		}
		else
		{
			/* Display updating stuff */
			if (this->nick.equals_ci(this->nc->display))
				this->nc->SetDisplay(this->nc->aliases->front());
		}
	}

	/* Remove us from the aliases list */
	NickServ::nickalias_map &map = NickServ::service->GetNickList();
	map.erase(this->nick);
}

void NickImpl::SetVhost(const Anope::string &ident, const Anope::string &host, const Anope::string &creator, time_t created)
{
	this->vhost_ident = ident;
	this->vhost_host = host;
	this->vhost_creator = creator;
	this->vhost_created = created;
}

void NickImpl::RemoveVhost()
{
	this->vhost_ident.clear();
	this->vhost_host.clear();
	this->vhost_creator.clear();
	this->vhost_created = 0;
}

bool NickImpl::HasVhost() const
{
	return !this->vhost_host.empty();
}

const Anope::string &NickImpl::GetVhostIdent() const
{
	return this->vhost_ident;
}

const Anope::string &NickImpl::GetVhostHost() const
{
	return this->vhost_host;
}

const Anope::string &NickImpl::GetVhostCreator() const
{
	return this->vhost_creator;
}

time_t NickImpl::GetVhostCreated() const
{
	return this->vhost_created;
}

void NickImpl::Serialize(Serialize::Data &data) const
{
	data["nick"] << this->nick;
	data["last_quit"] << this->last_quit;
	data["last_realname"] << this->last_realname;
	data["last_usermask"] << this->last_usermask;
	data["last_realhost"] << this->last_realhost;
	data.SetType("time_registered", Serialize::Data::DT_INT); data["time_registered"] << this->time_registered;
	data.SetType("last_seen", Serialize::Data::DT_INT); data["last_seen"] << this->last_seen;
	data["nc"] << this->nc->display;

	if (this->HasVhost())
	{
		data["vhost_ident"] << this->GetVhostIdent();
		data["vhost_host"] << this->GetVhostHost();
		data["vhost_creator"] << this->GetVhostCreator();
		data["vhost_time"] << this->GetVhostCreated();
	}

	Extensible::ExtensibleSerialize(this, this, data);
}

Serializable* NickImpl::Unserialize(Serializable *obj, Serialize::Data &data)
{
	Anope::string snc, snick;

	data["nc"] >> snc;
	data["nick"] >> snick;

	NickServ::Account *core = NickServ::FindAccount(snc);
	if (core == NULL)
		return NULL;

	NickServ::Nick *na;
	if (obj)
		na = anope_dynamic_static_cast<NickServ::Nick *>(obj);
	else
		na = new NickImpl(snick, core);

	if (na->nc != core)
	{
		std::vector<NickServ::Nick *>::iterator it = std::find(na->nc->aliases->begin(), na->nc->aliases->end(), na);
		if (it != na->nc->aliases->end())
			na->nc->aliases->erase(it);

		if (na->nc->aliases->empty())
			delete na->nc;
		else if (na->nick.equals_ci(na->nc->display))
			na->nc->SetDisplay(na->nc->aliases->front());

		na->nc = core;
		core->aliases->push_back(na);
	}

	data["last_quit"] >> na->last_quit;
	data["last_realname"] >> na->last_realname;
	data["last_usermask"] >> na->last_usermask;
	data["last_realhost"] >> na->last_realhost;
	data["time_registered"] >> na->time_registered;
	data["last_seen"] >> na->last_seen;

	Anope::string vhost_ident, vhost_host, vhost_creator;
	time_t vhost_time;

	data["vhost_ident"] >> vhost_ident;
	data["vhost_host"] >> vhost_host;
	data["vhost_creator"] >> vhost_creator;
	data["vhost_time"] >> vhost_time;

	na->SetVhost(vhost_ident, vhost_host, vhost_creator, vhost_time);

	Extensible::ExtensibleUnserialize(na, na, data);

	/* compat */
	bool b;
	b = false;
	data["extensible:NO_EXPIRE"] >> b;
	if (b)
		na->Extend<bool>("NS_NO_EXPIRE");
	/* end compat */

	return na;
}

