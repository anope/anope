/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#ifndef EXTENSIBLE_H
#define EXTENSIBLE_H

#include "anope.h"

/* All items added to Extensible must inherit from this.
 */
class CoreExport ExtensibleItem
{
 public:
	virtual ~ExtensibleItem() { }

	/* Called when this ExtensibleItem is being deleted. This should
	 * clean up things (eg, delete this;) if necessary.
	 */
	virtual void OnDelete() { delete this; }
};

/** Common class used to Extensible::Extend non-pointers from, as it doesn't delete
 * itself when removed. Eg, obj->Extend(key, new ExtensibleItemClass<Anope::string>(value));
 */
template<typename T> struct CoreExport ExtensibleItemClass : T, ExtensibleItem
{
	ExtensibleItemClass(const T& t) : T(t) { }
};

/* Used to attach arbitrary objects to this object using unique keys */
class CoreExport Extensible
{
 private:
 	typedef std::map<Anope::string, ExtensibleItem *> extensible_map;
	extensible_map extension_items;

 public:
	/** Default constructor, does nothing
	 */
	Extensible() { }

	/** Destructor, deletes all of the extensible items in this object
	 * then clears the map
	 */
	virtual ~Extensible()
	{
		for (extensible_map::iterator it = extension_items.begin(), it_end = extension_items.end(); it != it_end; ++it)
			if (it->second)
				it->second->OnDelete();
		extension_items.clear();
	}

	/** Extend an Extensible class.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 * @param p This parameter is a pointer to an ExtensibleItem or ExtensibleItemBase derived class
	 *
	 * You must provide a key to store the data as via the parameter 'key'.
	 * The data will be inserted into the map. If the data already exists, you may not insert it
	 * twice, Extensible::Extend will return false in this case.
	 *
	 * @return Returns true on success, false if otherwise
	 */
	void Extend(const Anope::string &key, ExtensibleItem *p)
	{
		this->Shrink(key);
		this->extension_items[key] = p;
	}

	/** Shrink an Extensible class.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 *
	 * You must provide a key name. The given key name will be removed from the classes data. If
	 * you provide a nonexistent key (case is important) then the function will return false.
	 * @return Returns true on success.
	 */
	bool Shrink(const Anope::string &key)
	{
		extensible_map::iterator it = this->extension_items.find(key);
		if (it != this->extension_items.end())
		{
			if (it->second != NULL)
				it->second->OnDelete();
			/* map::size_type map::erase( const key_type& key );
			 * returns the number of elements removed, std::map
			 * is single-associative so this should only be 0 or 1
			 */
			return this->extension_items.erase(key) > 0;
		}

		return false;
	}

	/** Get an extension item.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 * @return The item found
	 */
	template<typename T> T GetExt(const Anope::string &key) const
	{
		extensible_map::const_iterator it = this->extension_items.find(key);
		if (it != this->extension_items.end())
			return anope_dynamic_reinterpret_cast<T>(it->second);

		return NULL;
	}

	/** Check if an extension item exists.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 * @return True if the item was found.
	 */
	bool HasExt(const Anope::string &key) const
	{
		return this->extension_items.count(key) > 0;
	}

	/** Get a list of all extension items names.
	 * @param list A deque of strings to receive the list
	 * @return This function writes a list of all extension items stored
	 *	 in this object by name into the given deque and returns void.
	 */
	void GetExtList(std::deque<Anope::string> &list) const
	{
		for (extensible_map::const_iterator it = extension_items.begin(), it_end = extension_items.end(); it != it_end; ++it)
			list.push_back(it->first);
	}
};

#endif // EXTENSIBLE_H
