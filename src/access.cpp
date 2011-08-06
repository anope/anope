/*
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "access.h"

AccessProvider::AccessProvider(Module *o, const Anope::string &n) : Service(o, n)
{
}

AccessProvider::~AccessProvider()
{
}

ChanAccess::ChanAccess(AccessProvider *p) : provider(p)
{
}

ChanAccess::~ChanAccess()
{
}

bool ChanAccess::operator>(ChanAccess &other)
{
	for (size_t i = CA_SIZE; i > 0; --i)
		if (this->HasPriv(static_cast<ChannelAccess>(i - 1)) && !other.HasPriv(static_cast<ChannelAccess>(i - 1)))
			return true;
	return false;
}

bool ChanAccess::operator<(ChanAccess &other)
{
	for (size_t i = CA_SIZE; i > 0; --i)
		if (!this->HasPriv(static_cast<ChannelAccess>(i - 1)) && other.HasPriv(static_cast<ChannelAccess>(i - 1)))
			return true;
	return false;
}

bool ChanAccess::operator>=(ChanAccess &other)
{
	for (size_t i = CA_SIZE; i > 0; --i)
	{
		bool this_p = this->HasPriv(static_cast<ChannelAccess>(i - 1)),
			other_p = other.HasPriv(static_cast<ChannelAccess>(i - 1));

		if ((this_p && !other_p) || (this_p && other_p))
			return true;
	}

	return false;
}

bool ChanAccess::operator<=(ChanAccess &other)
{
	for (size_t i = CA_SIZE; i > 0; --i)
	{
		bool this_p = this->HasPriv(static_cast<ChannelAccess>(i - 1)),
			other_p = other.HasPriv(static_cast<ChannelAccess>(i - 1));

		if ((!this_p && other_p) || (this_p && other_p))
			return true;
	}

	return false;
}

AccessGroup::AccessGroup() : std::vector<ChanAccess *>()
{
	this->SuperAdmin = false;
}

bool AccessGroup::HasPriv(ChannelAccess priv) const
{
	if (this->SuperAdmin)
		return true;
	for (unsigned i = this->size(); i > 0; --i)
	{
		ChanAccess *access = this->at(i - 1);
		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnCheckPriv, OnCheckPriv(access, priv));
		if (MOD_RESULT == EVENT_ALLOW || access->HasPriv(priv))
			return true;
	}
	return false;
}

ChanAccess *AccessGroup::Highest() const
{
	for (size_t i = CA_SIZE; i > 0; --i)
		for (unsigned j = this->size(); j > 0; --j)
			if (this->at(j - 1)->HasPriv(static_cast<ChannelAccess>(i - 1)))
				return this->at(j - 1);
	return NULL;
}

bool AccessGroup::operator>(const AccessGroup &other) const
{
	for (size_t i = CA_SIZE; i > 0; --i)
		if (this->HasPriv(static_cast<ChannelAccess>(i - 1)) && !other.HasPriv(static_cast<ChannelAccess>(i - 1)))
			return true;
	return true;
}

bool AccessGroup::operator<(const AccessGroup &other) const
{
	for (size_t i = CA_SIZE; i > 0; --i)
		if (!this->HasPriv(static_cast<ChannelAccess>(i - 1)) && other.HasPriv(static_cast<ChannelAccess>(i - 1)))
			return true;
	return false;
}

bool AccessGroup::operator>=(const AccessGroup &other) const
{
	for (size_t i = CA_SIZE; i > 0; --i)
	{
		bool this_p = this->HasPriv(static_cast<ChannelAccess>(i - 1)),
			other_p = other.HasPriv(static_cast<ChannelAccess>(i - 1));

		if ((this_p && !other_p) || (this_p && other_p))
			return true;
	}

	return false;
}

bool AccessGroup::operator<=(const AccessGroup &other) const
{
	for (size_t i = CA_SIZE; i > 0; --i)
	{
		bool this_p = this->HasPriv(static_cast<ChannelAccess>(i - 1)),
			other_p = other.HasPriv(static_cast<ChannelAccess>(i - 1));

		if ((!this_p && other_p) || (this_p && other_p))
			return true;
	}

	return false;
}

