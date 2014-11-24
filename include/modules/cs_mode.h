/* ChanServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

class ModeLock : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	virtual ChanServ::Channel *GetChannel() anope_abstract;
	virtual void SetChannel(ChanServ::Channel *) anope_abstract;

	virtual bool GetSet() anope_abstract;
	virtual void SetSet(const bool &) anope_abstract;

	virtual Anope::string GetName() anope_abstract;
	virtual void SetName(const Anope::string &) anope_abstract;

	virtual Anope::string GetParam() anope_abstract;
	virtual void SetParam(const Anope::string &) anope_abstract;

	virtual Anope::string GetSetter() anope_abstract;
	virtual void SetSetter(const Anope::string &) anope_abstract;

	virtual time_t GetCreated() anope_abstract;
	virtual void SetCreated(const time_t &) anope_abstract;
};

static Serialize::TypeReference<ModeLock> modelock("ModeLock");

class ModeLocks : public Service
{
 public:
	ModeLocks(Module *me) : Service(me, "ModeLocks", "mlocks") { }

	typedef std::vector<ModeLock *> ModeList;

	/** Check if a mode is mlocked
	 * @param mode The mode
	 * @param An optional param
	 * @param status True to check mlock on, false for mlock off
	 * @return true on success, false on fail
	 */
	virtual bool HasMLock(ChanServ::Channel *, ChannelMode *mode, const Anope::string &param, bool status) const anope_abstract;

	/** Set a mlock
	 * @param mode The mode
	 * @param status True for mlock on, false for mlock off
	 * @param param An optional param arg for + mlocked modes
	 * @param setter Who is setting the mlock
	 * @param created When the mlock was created
	 * @return true on success, false on failure (module blocking)
	 */
	virtual bool SetMLock(ChanServ::Channel *, ChannelMode *mode, bool status, const Anope::string &param = "", Anope::string setter = "", time_t created = Anope::CurTime) anope_abstract;

	/** Remove a mlock
	 * @param mode The mode
	 * @param status True for mlock on, false for mlock off
	 * @param param The param of the mode, required if it is a list or status mode
	 * @return true on success, false on failure
	 */
	virtual bool RemoveMLock(ChanServ::Channel *, ChannelMode *mode, bool status, const Anope::string &param = "") anope_abstract;

	/** Clear all mlocks on the channel
	 */
	virtual void ClearMLock(ChanServ::Channel *) anope_abstract;

	/** Get all of the mlocks for this channel
	 * @return The mlocks
	 */
	virtual ModeList GetMLock(ChanServ::Channel *) const anope_abstract;

	/** Get a list of mode locks on a channel
	 * @param name The mode name to get a list of
	 * @return a list of mlocks for the given mode
	 */
	virtual std::list<ModeLock *> GetModeLockList(ChanServ::Channel *, const Anope::string &name) anope_abstract;

	/** Get details for a specific mlock
	 * @param mname The mode name
 	 * @param An optional param to match with
	 * @return The MLock, if any
	 */
	virtual ModeLock *GetMLock(ChanServ::Channel *, const Anope::string &mname, const Anope::string &param = "") anope_abstract;

	/** Get the current mode locks as a string
	 * @param complete True to show mlock parameters aswell
	 * @return A string of mode locks, eg: +nrt
	 */
	virtual Anope::string GetMLockAsString(ChanServ::Channel *, bool complete) const anope_abstract;
};

static ServiceReference<ModeLocks> mlocks("ModeLocks", "mlocks");

namespace Event
{
	struct CoreExport MLockEvents : Events
	{
		/** Called when a mode is about to be mlocked
		 * @param ci The channel the mode is being locked on
		 * @param lock The mode lock
		 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to deny the mlock.
		 */
		virtual EventReturn OnMLock(ChanServ::Channel *ci, ModeLock *lock) anope_abstract;

		/** Called when a mode is about to be unlocked
		 * @param ci The channel the mode is being unlocked from
		 * @param lock The mode lock
		 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to deny the mlock.
		 */
		virtual EventReturn OnUnMLock(ChanServ::Channel *ci, ModeLock *lock) anope_abstract;
	};
}

