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
#include "modules.h"
#include "account.h"
#include "config.h"

serialize_checker<nickcore_map> NickCoreList("NickCore");

/** Default constructor
 * @param display The display nick
 */
NickCore::NickCore(const Anope::string &coredisplay) : Serializable("NickCore"), Flags<NickCoreFlag, NI_END>(NickCoreFlagStrings)
{
	if (coredisplay.empty())
		throw CoreException("Empty display passed to NickCore constructor");

	this->o = NULL;
	this->channelcount = 0;
	this->lastmail = 0;
	this->memos.memomax = Config->MSMaxMemos;
	this->language = Config->NSDefLanguage;

	this->display = coredisplay;

	/* Set default nick core flags */
	for (size_t t = NI_BEGIN + 1; t != NI_END; ++t)
		if (Config->NSDefFlags.HasFlag(static_cast<NickCoreFlag>(t)))
			this->SetFlag(static_cast<NickCoreFlag>(t));

	size_t old = NickCoreList->size();
	(*NickCoreList)[this->display] = this;
	if (old == NickCoreList->size())
		Log(LOG_DEBUG) << "Duplicate account " << coredisplay << " in nickcore table?";
}

/** Default destructor
 */
NickCore::~NickCore()
{
	FOREACH_MOD(I_OnDelCore, OnDelCore(this));

	/* Remove the core from the list */
	NickCoreList->erase(this->display);

	/* Clear access before deleting display name, we want to be able to use the display name in the clear access event */
	this->ClearAccess();

	if (!this->memos.memos->empty())
	{
		for (unsigned i = 0, end = this->memos.memos->size(); i < end; ++i)
			this->memos.GetMemo(i)->destroy();
		this->memos.memos->clear();
	}
}

Serialize::Data NickCore::serialize() const
{
	Serialize::Data data;	

	data["display"].setMax(Config->NickLen) << this->display;
	data["pass"] << this->pass;
	data["email"] << this->email;
	data["greet"] << this->greet;
	data["language"] << this->language;
	data["flags"] << this->ToString();
	for (unsigned i = 0; i < this->access.size(); ++i)
		data["access"] << this->access[i] << " ";
	for (unsigned i = 0; i < this->cert.size(); ++i)
		data["cert"] << this->cert[i] << " ";
	data["memomax"] << this->memos.memomax;
	for (unsigned i = 0; i < this->memos.ignores.size(); ++i)
		data["memoignores"] << this->memos.ignores[i] << " ";
	
	return data;
}

Serializable* NickCore::unserialize(Serializable *obj, Serialize::Data &data)
{
	NickCore *nc;

	if (obj)
		nc = anope_dynamic_static_cast<NickCore *>(obj);
	else
		nc = new NickCore(data["display"].astr());

	data["pass"] >> nc->pass;
	data["email"] >> nc->email;
	data["greet"] >> nc->greet;
	data["language"] >> nc->language;
	nc->FromString(data["flags"].astr());
	{
		Anope::string buf;
		data["access"] >> buf;
		spacesepstream sep(buf);
		nc->access.clear();
		while (sep.GetToken(buf))
			nc->access.push_back(buf);
	}
	{
		Anope::string buf;
		data["cert"] >> buf;
		spacesepstream sep(buf);
		nc->cert.clear();
		while (sep.GetToken(buf))
			nc->cert.push_back(buf);
	}
	data["memomax"] >> nc->memos.memomax;
	{
		Anope::string buf;
		data["memoignores"] >> buf;
		spacesepstream sep(buf);
		nc->memos.ignores.clear();
		while (sep.GetToken(buf))
			nc->memos.ignores.push_back(buf);
	}

	return nc;
}

bool NickCore::IsServicesOper() const
{
	return this->o != NULL;
}

void NickCore::AddAccess(const Anope::string &entry)
{
	this->access.push_back(entry);
	FOREACH_MOD(I_OnNickAddAccess, OnNickAddAccess(this, entry));
}

Anope::string NickCore::GetAccess(unsigned entry) const
{
	if (this->access.empty() || entry >= this->access.size())
		return "";
	return this->access[entry];
}

bool NickCore::FindAccess(const Anope::string &entry)
{
	for (unsigned i = 0, end = this->access.size(); i < end; ++i)
		if (this->access[i] == entry)
			return true;

	return false;
}

void NickCore::EraseAccess(const Anope::string &entry)
{
	for (unsigned i = 0, end = this->access.size(); i < end; ++i)
		if (this->access[i] == entry)
		{
			FOREACH_MOD(I_OnNickEraseAccess, OnNickEraseAccess(this, entry));
			this->access.erase(this->access.begin() + i);
			break;
		}
}

void NickCore::ClearAccess()
{
	FOREACH_MOD(I_OnNickClearAccess, OnNickClearAccess(this));
	this->access.clear();
}

void NickCore::AddCert(const Anope::string &entry)
{
	this->cert.push_back(entry);
	FOREACH_MOD(I_OnNickAddCert, OnNickAddCert(this, entry));
}

Anope::string NickCore::GetCert(unsigned entry) const
{
	if (this->cert.empty() || entry >= this->cert.size())
		return "";
	return this->cert[entry];
}

bool NickCore::FindCert(const Anope::string &entry) const
{
	for (unsigned i = 0, end = this->cert.size(); i < end; ++i)
		if (this->cert[i] == entry)
			return true;

	return false;
}

void NickCore::EraseCert(const Anope::string &entry)
{
	for (unsigned i = 0, end = this->cert.size(); i < end; ++i)
		if (this->cert[i] == entry)
		{
			FOREACH_MOD(I_OnNickEraseCert, OnNickEraseCert(this, entry));
			this->cert.erase(this->cert.begin() + i);
			break;
		}
}

void NickCore::ClearCert()
{
	FOREACH_MOD(I_OnNickClearCert, OnNickClearCert(this));
	this->cert.clear();
}
