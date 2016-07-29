/* ChanServ core functions
 *
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 * 
 */

class LogSetting : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	static constexpr const char *const NAME = "logsetting";

	virtual ChanServ::Channel *GetChannel() anope_abstract;
	virtual void SetChannel(ChanServ::Channel *) anope_abstract;

	virtual Anope::string GetServiceName() anope_abstract;
	virtual void SetServiceName(const Anope::string &) anope_abstract;

	virtual Anope::string GetCommandService() anope_abstract;
	virtual void SetCommandService(const Anope::string &) anope_abstract;

	virtual Anope::string GetCommandName() anope_abstract;
	virtual void SetCommandName(const Anope::string &) anope_abstract;

	virtual Anope::string GetMethod() anope_abstract;
	virtual void SetMethod(const Anope::string &) anope_abstract;

	virtual Anope::string GetExtra() anope_abstract;
	virtual void SetExtra(const Anope::string &) anope_abstract;

	virtual Anope::string GetCreator() anope_abstract;
	virtual void SetCreator(const Anope::string &) anope_abstract;

	virtual time_t GetCreated() anope_abstract;
	virtual void SetCreated(const time_t &) anope_abstract;
};
