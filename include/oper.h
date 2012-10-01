/* OperServ support
 *
 * (C) 2008-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#ifndef OPER_H
#define OPER_H

#include "serialize.h"
#include "service.h"

class CoreExport XLine : public Serializable
{
	void InitRegex();
 public:
	Anope::string Mask;
	Regex *regex;
	Anope::string By;
	time_t Created;
	time_t Expires;
	Anope::string Reason;
	XLineManager *manager;
	Anope::string UID;

	XLine(const Anope::string &mask, const Anope::string &reason = "", const Anope::string &uid = "");

	XLine(const Anope::string &mask, const Anope::string &by, const time_t expires, const Anope::string &reason, const Anope::string &uid = "");
	~XLine();

	Anope::string GetNick() const;
	Anope::string GetUser() const;
	Anope::string GetHost() const;
	Anope::string GetReal() const;

	Anope::string GetReason() const;

	bool HasNickOrReal() const;
	bool IsRegex() const;

	Serialize::Data serialize() const anope_override;
	static Serializable* unserialize(Serializable *obj, Serialize::Data &data);
};

class CoreExport XLineManager : public Service
{
	char type;
	/* List of XLines in this XLineManager */
	serialize_checker<std::vector<XLine *> > XLines;
	/* Akills can have the same IDs, sometimes */
	static serialize_checker<std::multimap<Anope::string, XLine *, ci::less> > XLinesByUID;
 public:
	/* List of XLine managers we check users against in XLineManager::CheckAll */
	static std::list<XLineManager *> XLineManagers;

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
	const char &Type();

	/** Get the number of XLines in this XLineManager
	 * @return The number of XLines
	 */
	size_t GetCount() const;

	/** Get the XLine vector
	 * @return The vector
	 */
	const std::vector<XLine *> &GetList() const;

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
	virtual bool Check(User *u, const XLine *x) = 0;

	/** Called when a user matches a xline in this XLineManager
	 * @param u The user
	 * @param x The XLine they match
	 */
	virtual void OnMatch(User *u, XLine *x) = 0;

	/** Called when an XLine expires
	 * @param x The xline
	 */
	virtual void OnExpire(const XLine *x);

	/** Called to send an XLine to the IRCd
	 * @param u The user, if we know it
	 * @param x The xline
	 */
	virtual void Send(User *u, XLine *x) = 0;

	/** Called to remove an XLine from the IRCd
	 * @param x The XLine
	 */
	virtual void SendDel(XLine *x) = 0;
};

#endif // OPER_H
