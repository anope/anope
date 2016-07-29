/*
 *
 * (C) 2008-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#pragma once

#include "serialize.h"
#include "service.h"
#include "sockets.h"

/* An XLine, eg, anything added with operserv/akill, or any of the operserv/sxline commands */
class CoreExport XLine : public Serialize::Object
{
	void Recache();
	Anope::string nick, user, host, real;

	friend class XLineType;

	Anope::string type, mask, by, reason, id;
	time_t created = 0, expires = 0;

 public:
	static constexpr const char *const NAME = "xline";
	
	cidr *c = nullptr;
	std::regex *regex = nullptr;

	using Serialize::Object::Object;

	~XLine();

	void SetType(const Anope::string &);
	Anope::string GetType();

	void SetMask(const Anope::string &);
	Anope::string GetMask();

	void SetBy(const Anope::string &);
	Anope::string GetBy();

	void SetReason(const Anope::string &);
	Anope::string GetReason();

	void SetID(const Anope::string &);
	Anope::string GetID();

	void SetCreated(const time_t &);
	time_t GetCreated();

	void SetExpires(const time_t &);
	time_t GetExpires();

	const Anope::string &GetNick() const;
	const Anope::string &GetUser() const;
	const Anope::string &GetHost() const;
	const Anope::string &GetReal() const;

	Anope::string GetReasonWithID();

	bool HasNickOrReal() const;
	bool IsRegex();

	XLineManager *GetManager();
};

class XLineType : public Serialize::Type<XLine>
{
 public:
	Serialize::Field<XLine, Anope::string> type;
	struct Mask : Serialize::Field<XLine, Anope::string>
	{
		using Serialize::Field<XLine, Anope::string>::Field;

		void SetField(XLine *s, const Anope::string &value) override;
	} mask;
	Serialize::Field<XLine, Anope::string> by, reason, id;
	Serialize::Field<XLine, time_t> created, expires;

	XLineType(Module *m) : Serialize::Type<XLine>(m)
		, type(this, "type", &XLine::type)
		, mask(this, "mask", &XLine::mask)
		, by(this, "by", &XLine::by)
		, reason(this, "reason", &XLine::reason)
		, id(this, "id", &XLine::id)
		, created(this, "created", &XLine::created)
		, expires(this, "expires", &XLine::expires)
	{
	}
};

/* Managers XLines. There is one XLineManager per type of XLine. */
class CoreExport XLineManager : public Service
{
	char type;
 public:
	static constexpr const char *NAME = "xlinemanager";
	 
	/* List of XLine managers we check users against in XLineManager::CheckAll */
	static std::vector<XLineManager *> XLineManagers;

	/** Register a XLineManager, places it in XLineManagers for use in XLineManager::CheckAll
	 * It is important XLineManagers are registered in the proper order. Eg, if you had one akilling
	 * clients and one handing them free olines, you would want the akilling one first. This way if a client
	 * matches an entry on both of the XLineManagers, they would be akilled.
	 * @param xlm THe XLineManager
	 */
	static void RegisterXLineManager(XLineManager *xlm);

	/** Unregister a XLineManager
	 * @param xlm The XLineManager
	 */
	static void UnregisterXLineManager(XLineManager *xlm);

	/** Check a user against all known XLineManagers
	 * Wparam u The user
	 * @return A pair of the XLineManager the user was found in and the XLine they matched, both may be NULL for no match
	 */
	static void CheckAll(User *u);

	/** Generate a unique ID for this XLine
	 * @return A unique ID
	 */
	static Anope::string GenerateUID();

	/** Constructor
	 */
	XLineManager(Module *creator, const Anope::string &name, char t);

	/** Destructor
	 */
	virtual ~XLineManager();

	/** The type of xline provided by this service
	 * @return The type
	 */
	char Type();

	/** Get the XLine vector
	 * @return The vector
	 */
	std::vector<XLine *> GetXLines() const;

	inline unsigned int GetCount() const { return GetXLines().size(); }

	/** Add an entry to this XLineManager
	 * @param x The entry
	 */
	void AddXLine(XLine *x);

	/** Gets an entry by index
	 * @param index The index
	 * @return The XLine, or NULL if the index is out of bounds
	 */
	XLine* GetEntry(unsigned index);

	/** Clear the XLine vector
	 * Note: This does not remove the XLines from the IRCd
	 */
	void Clear();

	/** Checks if a mask can/should be added to the XLineManager
	 * @param source The source adding the mask.
	 * @param mask The mask
	 * @param expires When the mask would expire
	 * @param reason the reason
	 * @return true if the mask can be added
	 */
	bool CanAdd(CommandSource &source, const Anope::string &mask, time_t expires, const Anope::string &reason);

	/** Checks if this list has an entry
	 * @param mask The mask
	 * @return The XLine the user matches, or NULL
	 */
	XLine* HasEntry(const Anope::string &mask);

	/** Check a user against all of the xlines in this XLineManager
	 * @param u The user
	 * @return The xline the user marches, if any.
	 */
	XLine *CheckAllXLines(User *u);

	/** Check a user against an xline
	 * @param u The user
	 * @param x The xline
	 */
	virtual bool Check(User *u, XLine *x) anope_abstract;

	/** Called when a user matches a xline in this XLineManager
	 * @param u The user
	 * @param x The XLine they match
	 */
	virtual void OnMatch(User *u, XLine *x) anope_abstract;

	/** Called when an XLine expires
	 * @param x The xline
	 */
	virtual void OnExpire(XLine *x);

	/** Called to send an XLine to the IRCd
	 * @param u The user, if we know it
	 * @param x The xline
	 */
	virtual void Send(User *u, XLine *x) anope_abstract;

	/** Called to remove an XLine from the IRCd
	 * @param x The XLine
	 */
	virtual void SendDel(XLine *x) anope_abstract;
};

