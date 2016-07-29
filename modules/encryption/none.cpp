/* Module for plain text encryption.
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * This program is free but copyrighted software; see the file COPYING for
 * details.
 */

#include "module.h"

class ENone : public Module
	, public EventHook<Event::Encrypt>
	, public EventHook<Event::Decrypt>
	, public EventHook<Event::CheckAuthentication>
{
 public:
	ENone(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, ENCRYPTION | VENDOR)
		, EventHook<Event::Encrypt>(this)
		, EventHook<Event::Decrypt>(this)
		, EventHook<Event::CheckAuthentication>(this)
	{

	}

	EventReturn OnEncrypt(const Anope::string &src, Anope::string &dest) override
	{
		Anope::string buf = "plain:";
		Anope::string cpass;
		Anope::B64Encode(src, cpass);
		buf += cpass;
		Log(LOG_DEBUG_2) << "(enc_none) hashed password from [" << src << "] to [" << buf << "]";
		dest = buf;
		return EVENT_ALLOW;
	}

	EventReturn OnDecrypt(const Anope::string &hashm, const Anope::string &src, Anope::string &dest) override
	{
		if (!hashm.equals_cs("plain"))
			return EVENT_CONTINUE;
		size_t pos = src.find(':');
		Anope::string buf = src.substr(pos + 1);
		Anope::B64Decode(buf, dest);
		return EVENT_ALLOW;
	}

	void OnCheckAuthentication(User *, NickServ::IdentifyRequest *req) override
	{
		NickServ::Nick *na = NickServ::FindNick(req->GetAccount());
		if (na == NULL)
			return;
		NickServ::Account *nc = na->GetAccount();

		size_t pos = nc->GetPassword().find(':');
		if (pos == Anope::string::npos)
			return;
		Anope::string hash_method(nc->GetPassword().begin(), nc->GetPassword().begin() + pos);
		if (!hash_method.equals_cs("plain"))
			return;

		Anope::string buf;
		this->OnEncrypt(req->GetPassword(), buf);
		if (nc->GetPassword().equals_cs(buf))
		{
			/* if we are NOT the first module in the list,
			 * we want to re-encrypt the pass with the new encryption
			 */
			if (ModuleManager::FindFirstOf(ENCRYPTION) != this)
			{
				Anope::string p;
				Anope::Encrypt(req->GetPassword(), p);
				nc->SetPassword(p);
			}
			req->Success(this);
		}
	}
};

MODULE_INIT(ENone)
