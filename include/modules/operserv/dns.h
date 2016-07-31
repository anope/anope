/*
 * Anope IRC Services
 *
 * Copyright (C) 2014-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

class DNSZone : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	static constexpr const char *const NAME = "dnszone";

	virtual Anope::string GetName() anope_abstract;
	virtual void SetName(const Anope::string &) anope_abstract;
};

class DNSServer : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	static constexpr const char *const NAME = "dnsserver";

	virtual DNSZone *GetZone() anope_abstract;
	virtual void SetZone(DNSZone *) anope_abstract;

	virtual Anope::string GetName() anope_abstract;
	virtual void SetName(const Anope::string &) anope_abstract;

	virtual unsigned int GetLimit() anope_abstract;
	virtual void SetLimit(const unsigned int &) anope_abstract;

	virtual bool GetPooled() anope_abstract;
	virtual void SetPool(const bool &) anope_abstract;
};

class DNSZoneMembership : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	static constexpr const char *const NAME = "dnszonemembership";

	virtual DNSServer *GetServer() anope_abstract;
	virtual void SetServer(DNSServer *) anope_abstract;

	virtual DNSZone *GetZone() anope_abstract;
	virtual void SetZone(DNSZone *) anope_abstract;
};

class DNSIP : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	static constexpr const char *const NAME = "dnsip";

	virtual DNSServer *GetServer() anope_abstract;
	virtual void SetServer(DNSServer *) anope_abstract;

	virtual Anope::string GetIP() anope_abstract;
	virtual void SetIP(const Anope::string &) anope_abstract;
};


