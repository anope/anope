/*
 *
 * (C) 2014-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/nickserv/sasl.h"

class Anonymous final
	: public SASL::Mechanism
{
public:
	Anonymous(Module *o)
		: SASL::Mechanism(o, "ANONYMOUS")
	{
	}

	bool ProcessMessage(SASL::Session *sess, const SASL::Message &m) override
	{
		if (m.type == "S")
		{
			SASL::service->SendMessage(sess, "C", "+");
		}
		else if (m.type == "C")
		{
			auto decoded = Anope::B64Decode(m.data[0]);

			auto user = sess->GetUserInfo();
			if (!decoded.empty())
				user += " [" + decoded + "]";

			Log(this->owner, "sasl", Config->GetClient("NickServ")) << user << " unidentified using SASL ANONYMOUS";
			SASL::service->Succeed(sess, nullptr);
		}
		return true;
	}
};

class ModuleSASLAnonymous final
	: public Module
{
private:
	Anonymous anonymous;

public:
	ModuleSASLAnonymous(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, anonymous(this)
	{
		if (!SASL::protocol_interface)
			throw ModuleException("Your IRCd does not support SASL");
	}
};

MODULE_INIT(ModuleSASLAnonymous)
