/*
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#ifndef EXTENSIBLE_H
#define EXTENSIBLE_H

#include "anope.h"
#include "serialize.h"

/* All items added to Extensible must inherit from this.
 */
class CoreExport ExtensibleItem
{
 public:
	virtual ~ExtensibleItem() { }

	virtual const Anope::string *Serialize() { return NULL; }
};

/** Common class used to Extensible::Extend as it inherits from both ExtensibleItem
 * and whatever basic object you're trying to store.
 * Eg, obj->Extend(key, new ExtensibleItemClass<Anope::string>(value));
 */
template<typename T> struct CoreExport ExtensibleItemClass : T, ExtensibleItem
{
	ExtensibleItemClass(const T& t) : T(t) { }
};

/* Used to attach metadata to this object that is automatically saved
 * when the object is saved (assuming the object's Serialize method
 * correcly calls Extensible::ExtensibleSerialize).
 */
struct CoreExport ExtensibleMetadata : ExtensibleItemClass<Anope::string>
{
	ExtensibleMetadata(const Anope::string &t) : ExtensibleItemClass<Anope::string>(t) { }

	const Anope::string *Serialize() anope_override { return this; }
};

/* Used to attach arbitrary objects to this object using unique keys */
class CoreExport Extensible
{
 private:
 	typedef std::map<Anope::string, ExtensibleItem *> extensible_map;
	extensible_map *extension_items;

 public:
	/** Default constructor
	 */
	Extensible();

	/** Destructor, deletes all of the extensible items in this object
	 * then clears the map
	 */
	virtual ~Extensible();

	/** Extend an Extensible class.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 * @param p This parameter is a pointer to an ExtensibleItem or ExtensibleItemBase derived class
	 *
	 * You must provide a key to store the data as via the parameter 'key'.
	 * The data will be inserted into the map. If the data already exists, it will be overwritten.
	 */
	void Extend(const Anope::string &key, ExtensibleItem *p = NULL);

	void ExtendMetadata(const Anope::string &key, const Anope::string &value = "");

	/** Shrink an Extensible class.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 *
	 * You must provide a key name. The given key name will be removed from the classes data. If
	 * you provide a nonexistent key (case is important) then the function will return false.
	 * @return Returns true on success.
	 */
	bool Shrink(const Anope::string &key);

	/** Get an extension item.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 * @return The item found
	 */
	template<typename T> T GetExt(const Anope::string &key) const
	{
		if (this->extension_items)
		{
			extensible_map::const_iterator it = this->extension_items->find(key);
			if (it != this->extension_items->end())
				return anope_dynamic_static_cast<T>(it->second);
		}

		return NULL;
	}

	/** Check if an extension item exists.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 * @return True if the item was found.
	 */
	bool HasExt(const Anope::string &key) const;

	/** Get a list of all extension items names.
	 * @param list A deque of strings to receive the list
	 * @return This function writes a list of all extension items stored
	 *	 in this object by name into the given deque and returns void.
	 */
	void GetExtList(std::deque<Anope::string> &list) const;

	void ExtensibleSerialize(Serialize::Data &data) const;
	void ExtensibleUnserialize(Serialize::Data &data);
};

#endif // EXTENSIBLE_H
