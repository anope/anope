// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

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
