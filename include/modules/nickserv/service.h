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

#pragma once


#define NICKSERV_SERVICE "NickServ::Service"

namespace NickServ
{
	class Service;

	ServiceReference<Service> service(NICKSERV_SERVICE, NICKSERV_SERVICE);
}

class NickServ::Service
	: public ::Service
{
public:
	Service(Module *m)
		: ::Service(m, NICKSERV_SERVICE, NICKSERV_SERVICE)
	{
	}

	virtual void Validate(User *u) = 0;
	virtual void Collide(User *u, NickAlias *na) = 0;
	virtual void Release(NickAlias *na) = 0;
	virtual bool IsGuestNick(const Anope::string &nick) const = 0;
};
