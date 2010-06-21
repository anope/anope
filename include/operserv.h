/* OperServ support
 *
 * (C) 2008-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#ifndef OPERSERV_H
#define OPERSERV_H

extern CoreExport std::vector<NewsItem *> News;
extern CoreExport std::vector<std::bitset<32> > DefCon;
extern CoreExport bool DefConModesSet;
extern CoreExport Flags<ChannelModeName> DefConModesOn;
extern CoreExport Flags<ChannelModeName> DefConModesOff;
extern CoreExport std::map<ChannelModeName, std::string> DefConModesOnParams;

class XLineManager;
extern CoreExport XLineManager *SGLine;
extern CoreExport XLineManager *SZLine;
extern CoreExport XLineManager *SQLine;
extern CoreExport XLineManager *SNLine;

extern CoreExport bool SetDefConParam(ChannelModeName, std::string &);
extern CoreExport bool GetDefConParam(ChannelModeName, std::string &);
extern CoreExport void UnsetDefConParam(ChannelModeName);
extern CoreExport bool CheckDefCon(DefconLevel Level);
extern CoreExport bool CheckDefCon(int level, DefconLevel Level);
extern CoreExport void AddDefCon(int level, DefconLevel Level);
extern CoreExport void DelDefCon(int level, DefconLevel Level);

extern CoreExport void operserv(User *u, const std::string &message);
extern CoreExport void os_init();

extern CoreExport void oper_global(char *nick, const char *fmt, ...);
extern CoreExport void server_global(Server *s, const std::string &message);

enum XLineType
{
	X_SNLINE,
	X_SQLINE,
	X_SZLINE
};

class CoreExport XLine
{
 public:
	ci::string Mask;
	ci::string By;
	time_t Created;
	time_t Expires;
	std::string Reason;

	XLine(const ci::string &mask, const std::string &reason = "");

	XLine(const ci::string &mask, const ci::string &by, const time_t expires, const std::string &reason);

	ci::string GetNick() const;
	ci::string GetUser() const;
	ci::string GetHost() const;
};

class CoreExport XLineManager
{
 private:
	/* List of XLine managers we check users against in XLineManager::CheckAll */
	static std::list<XLineManager *> XLineManagers;

 protected:
	/* List of XLines in this XLineManager */
	std::deque<XLine *> XLines;
 public:
	/** Constructor
	 */
	XLineManager();

	/** Destructor
	 */
	virtual ~XLineManager();

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
	static std::pair<XLineManager *, XLine *> CheckAll(User *u);

	/** Get the number of XLines in this XLineManager
	 * @return The number of XLines
	 */
	const size_t GetCount() const;

	/** Get the XLine list
	 * @return The list
	 */
	const std::deque<XLine *>& GetList() const;

	/** Add an entry to this XLineManager
	 * @param x The entry
	 */
	void AddXLine(XLine *x);

	/** Delete an entry from this XLineManager
	 * @param x The entry
	 * @return true if the entry was found and deleted, else false
	 */
	bool DelXLine(XLine *x);

	/** Gets an entry by index
	 * @param index The index
	 * @return The XLine, or NULL if the index is out of bounds
	 */
	XLine *GetEntry(unsigned index) const;

	/** Clear the XLine list
	 */
	void Clear();

	/** Add an entry to this XLine Manager
	 * @param bi The bot error replies should be sent from
	 * @param u The user adding the XLine
	 * @param mask The mask of the XLine
	 * @param expires When this should expire
	 * @param reaosn The reason
	 * @return A pointer to the XLine
	 */
	virtual XLine *Add(BotInfo *bi, User *u, const ci::string &mask, time_t expires, const std::string &reason);

	/** Delete an XLine, eg, remove it from the IRCd.
	 * @param x The xline
	 */
	virtual void Del(XLine *x);

	/** Checks if a mask can/should be added to the XLineManager
	 * @param mask The mask
	 * @param expires When the mask would expire
	 * @return A pair of int and XLine*.
	 * 1 - Mask already exists
	 * 2 - Mask already exists, but the expiry time was changed
	 * 3 - Mask is already covered by another mask
	 * In each case the XLine it matches/is covered by is returned in XLine*
	 */
	std::pair<int, XLine *> CanAdd(const ci::string &mask, time_t expires);

	/** Checks if this list has an entry
	 * @param mask The mask
	 * @return The XLine the user matches, or NULL
	 */
	XLine *HasEntry(const ci::string &mask) const;

	/** Check a user against all of the xlines in this XLineManager
	 * @param u The user
	 * @return The xline the user marches, if any. Also calls OnMatch()
	 */
	virtual XLine *Check(User *u);

	/** Called when a user matches a xline in this XLineManager
	 * @param u The user
	 * @param x The XLine they match
	 */
	virtual void OnMatch(User *u, XLine *x);

	/** Called when an XLine expires
	 * @param x The xline
	 */
	virtual void OnExpire(XLine *x);
};

/* This is for AKILLS */
class SGLineManager : public XLineManager
{
 public:
	XLine *Add(BotInfo *bi, User *u, const ci::string &mask, time_t expires, const std::string &reason);

	void Del(XLine *x);

	void OnMatch(User *u, XLine *x);

	void OnExpire(XLine *x);
};

class SNLineManager : public XLineManager
{
 public:
	XLine *Add(BotInfo *bi, User *u, const ci::string &mask, time_t expires, const std::string &reason);

	void Del(XLine *x);

	void OnMatch(User *u, XLine *x);

	void OnExpire(XLine *x);
};

class SQLineManager : public XLineManager
{
 public:
	XLine *Add(BotInfo *bi, User *u, const ci::string &mask, time_t expires, const std::string &reason);

	void Del(XLine *x);

	void OnMatch(User *u, XLine *x);

	void OnExpire(XLine *x);

	static bool Check(Channel *c);
};

class SZLineManager : public XLineManager
{
 public:
	XLine *Add(BotInfo *bi, User *u, const ci::string &mask, time_t expires, const std::string &reason);

	void Del(XLine *x);

	void OnMatch(User *u, XLine *x);

	void OnExpire(XLine *x);
};

#endif // OPERSERV_H
