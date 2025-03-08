/*
 *
 * (C) 2011-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

class NickServService
	: public Service
{
public:
	NickServService(Module *m) : Service(m, "NickServService", "NickServ")
	{
	}

	virtual void Validate(User *u) = 0;
	virtual void Collide(User *u, NickAlias *na) = 0;
	virtual void Release(NickAlias *na) = 0;
	virtual bool IsGuestNick(const Anope::string &nick) const = 0;
};
