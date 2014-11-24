/*
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

class CSMiscData : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	virtual ChanServ::Channel *GetChannel() anope_abstract;
	virtual void SetChannel(ChanServ::Channel *) anope_abstract;

	virtual Anope::string GetName() anope_abstract;
	virtual void SetName(const Anope::string &) anope_abstract;

	virtual Anope::string GetData() anope_abstract;
	virtual void SetData(const Anope::string &) anope_abstract;
};

static Serialize::TypeReference<CSMiscData> csmiscdata("CSMiscData");

