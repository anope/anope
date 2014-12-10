/* BotServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

class KickerData : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	virtual ChanServ::Channel *GetChannel() anope_abstract;
	virtual void SetChannel(ChanServ::Channel *) anope_abstract;

	virtual bool GetAmsgs() anope_abstract;
	virtual void SetAmsgs(const bool &) anope_abstract;

	virtual bool GetBadwords() anope_abstract;
	virtual void SetBadwords(const bool &) anope_abstract;

	virtual bool GetBolds() anope_abstract;
	virtual void SetBolds(const bool &) anope_abstract;

	virtual bool GetCaps() anope_abstract;
	virtual void SetCaps(const bool &) anope_abstract;

	virtual bool GetColors() anope_abstract;
	virtual void SetColors(const bool &) anope_abstract;

	virtual bool GetFlood() anope_abstract;
	virtual void SetFlood(const bool &) anope_abstract;

	virtual bool GetItalics() anope_abstract;
	virtual void SetItalics(const bool &) anope_abstract;

	virtual bool GetRepeat() anope_abstract;
	virtual void SetRepeat(const bool &) anope_abstract;

	virtual bool GetReverses() anope_abstract;
	virtual void SetReverses(const bool &) anope_abstract;

	virtual bool GetUnderlines() anope_abstract;
	virtual void SetUnderlines(const bool &) anope_abstract;

	virtual int16_t GetTTBBolds() anope_abstract;
	virtual void SetTTBBolds(const int16_t &) anope_abstract;

	virtual int16_t GetTTBColors() anope_abstract;
	virtual void SetTTBColors(const int16_t &) anope_abstract;

	virtual int16_t GetTTBReverses() anope_abstract;
	virtual void SetTTBReverses(const int16_t &) anope_abstract;

	virtual int16_t GetTTBUnderlines() anope_abstract;
	virtual void SetTTBUnderlines(const int16_t &) anope_abstract;

	virtual int16_t GetTTBBadwords() anope_abstract;
	virtual void SetTTBBadwords(const int16_t &) anope_abstract;

	virtual int16_t GetTTBCaps() anope_abstract;
	virtual void SetTTBCaps(const int16_t &) anope_abstract;
	
	virtual int16_t GetTTBFlood() anope_abstract;
	virtual void SetTTBFlood(const int16_t &) anope_abstract;

	virtual int16_t GetTTBRepeat() anope_abstract;
	virtual void SetTTBRepeat(const int16_t &) anope_abstract;

	virtual int16_t GetTTBItalics() anope_abstract;
	virtual void SetTTBItalics(const int16_t &) anope_abstract;

	virtual int16_t GetTTBAmsgs() anope_abstract;
	virtual void SetTTBAmsgs(const int16_t &) anope_abstract;

	virtual int16_t GetCapsMin() anope_abstract;
	virtual void SetCapsMin(const int16_t &) anope_abstract;

	virtual int16_t GetCapsPercent() anope_abstract;
	virtual void SetCapsPercent(const int16_t &) anope_abstract;

	virtual int16_t GetFloodLines() anope_abstract;
	virtual void SetFloodLines(const int16_t &) anope_abstract;

	virtual int16_t GetFloodSecs() anope_abstract;
	virtual void SetFloodSecs(const int16_t &) anope_abstract;

	virtual int16_t GetRepeatTimes() anope_abstract;
	virtual void SetRepeatTimes(const int16_t &) anope_abstract;

	virtual bool GetDontKickOps() anope_abstract;
	virtual void SetDontKickOps(const bool &) anope_abstract;

	virtual bool GetDontKickVoices() anope_abstract;
	virtual void SetDontKickVoices(const bool &) anope_abstract;
};

static Serialize::TypeReference<KickerData> kickerdata("KickerData");

inline KickerData *GetKickerData(ChanServ::Channel *ci)
{
	KickerData *kd = ci->GetRef<KickerData *>(kickerdata);
	if (!kd && kickerdata)
	{
		kd = kickerdata.Create();
		kd->SetChannel(ci);
	}
	return kd;
}

namespace Event
{
	struct CoreExport BotBan : Events
	{
		/** Called when a bot places a ban
		 * @param u User being banned
		 * @param ci Channel the ban is placed on
		 * @param mask The mask being banned
		 */
		virtual void OnBotBan(User *u, ChanServ::Channel *ci, const Anope::string &mask) anope_abstract;
	};
}

template<> struct EventName<Event::BotBan> { static constexpr const char *const name = "OnBotBan"; };

