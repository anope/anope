/*
 *
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */


class DNSZone : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	virtual Anope::string GetName() anope_abstract;
	virtual void SetName(const Anope::string &) anope_abstract;
};

static Serialize::TypeReference<DNSZone> dnszone("DNSZone");

class DNSServer : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	virtual DNSZone *GetZone() anope_abstract;
	virtual void SetZone(DNSZone *) anope_abstract;

	virtual Anope::string GetName() anope_abstract;
	virtual void SetName(const Anope::string &) anope_abstract;

	virtual unsigned int GetLimit() anope_abstract;
	virtual void SetLimit(const unsigned int &) anope_abstract;

	virtual bool GetPooled() anope_abstract;
	virtual void SetPool(const bool &) anope_abstract;
};

static Serialize::TypeReference<DNSServer> dnsserver("DNSServer");

class DNSZoneMembership : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	virtual DNSServer *GetServer() anope_abstract;
	virtual void SetServer(DNSServer *) anope_abstract;

	virtual DNSZone *GetZone() anope_abstract;
	virtual void SetZone(DNSZone *) anope_abstract;
};

static Serialize::TypeReference<DNSZoneMembership> dnszonemembership("DNSZoneMembership");

class DNSIP : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	virtual DNSServer *GetServer() anope_abstract;
	virtual void SetServer(DNSServer *) anope_abstract;

	virtual Anope::string GetIP() anope_abstract;
	virtual void SetIP(const Anope::string &) anope_abstract;
};

static Serialize::TypeReference<DNSIP> dnsip("DNSIP");

