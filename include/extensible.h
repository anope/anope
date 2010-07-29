/*
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#ifndef EXTENSIBLE_H
#define EXTENSIBLE_H

#include "hashcomp.h"

/** Dummy base class we use to cast everything to/from
 */
class ExtensibleItemBase
{
 public:
	ExtensibleItemBase() { }
	virtual ~ExtensibleItemBase() { }
};

/** Class used to represent an extensible item that doesn't hold a pointer
 */
template<typename T> class ExtensibleItemRegular : public ExtensibleItemBase
{
 protected:
	T Item;

 public:
	ExtensibleItemRegular(T item) : Item(item) { }
	virtual ~ExtensibleItemRegular() { }
	T GetItem() const { return Item; }
};

/** Class used to represent an extensible item that holds a pointer
 */
template<typename T> class ExtensibleItemPointer : public ExtensibleItemBase
{
 protected:
	T *Item;

 public:
	ExtensibleItemPointer(T *item) : Item(item) { }
	virtual ~ExtensibleItemPointer() { delete Item; }
	T *GetItem() const { return Item; }
};

/** Class used to represent an extensible item that holds a pointer to an arrray
 */
template<typename T> class ExtensibleItemPointerArray : public ExtensibleItemBase
{
 protected:
	T *Item;

 public:
	ExtensibleItemPointerArray(T *item) : Item(item) { }
	virtual ~ExtensibleItemPointerArray() { delete [] Item; }
	T *GetItem() const { return Item; }
};

class CoreExport Extensible
{
 private:
	typedef std::map<Anope::string, ExtensibleItemBase *> extensible_map;
	extensible_map Extension_Items;

 public:
	/** Default constructor, does nothing
	 */
	Extensible() { }

	/** Default destructor, deletes all of the extensible items in this object
	 * then clears the map
	 */
	virtual ~Extensible()
	{
		for (extensible_map::iterator it = Extension_Items.begin(), it_end = Extension_Items.end(); it != it_end; ++it)
			delete it->second;
		Extension_Items.clear();
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
	bool Extend(const Anope::string &key, ExtensibleItemBase *p)
	{
		bool Ret = this->Extension_Items.insert(std::make_pair(key, p)).second;
		if (!Ret)
			delete p;
		return Ret;
	}

	/** Extend an Extensible class.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 *
	 * You must provide a key to store the data as via the parameter 'key', this single-parameter
	 * version takes no 'data' parameter, this is used purely for boolean values.
	 * The key will be inserted into the map with a NULL 'data' pointer. If the key already exists
	 * then you may not insert it twice, Extensible::Extend will return false in this case.
	 *
	 * @return Returns true on success, false if otherwise
	 */
	bool Extend(const Anope::string &key)
	{
		/* This will only add an item if it doesnt already exist,
		 * the return value is a std::pair of an iterator to the
		 * element, and a bool saying if it was actually inserted.
		 */
		return this->Extend(key, new ExtensibleItemRegular<char *>(NULL));
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
		extensible_map::iterator it = this->Extension_Items.find(key);
		if (it != this->Extension_Items.end())
		{
			delete it->second;
			/* map::size_type map::erase( const key_type& key );
			 * returns the number of elements removed, std::map
			 * is single-associative so this should only be 0 or 1
			 */
			return this->Extension_Items.erase(key);
		}

		return false;
	}

	/** Get an extension item that is not a pointer.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 * @param p If you provide a non-existent key, this value will be 0. Otherwise a copy to the item you requested will be placed in this templated parameter.
	 * @return Returns true if the item was found and false if it was nor, regardless of wether 'p' is NULL. This allows you to store NULL values in Extensible.
	 */
	template<typename T> bool GetExtRegular(const Anope::string &key, T &p)
	{
		extensible_map::iterator it = this->Extension_Items.find(key);

		if (it != this->Extension_Items.end())
		{
			p = debug_cast<ExtensibleItemRegular<T> *>(it->second)->GetItem();
			return true;
		}

		return false;
	}

	/** Get an extension item that is a pointer.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 * * @param p If you provide a non-existent key, this value will be NULL. Otherwise a pointer to the item you requested will be placed in this templated parameter.
	 * @return Returns true if the item was found and false if it was nor, regardless of wether 'p' is NULL. This allows you to store NULL values in Extensible.
	 */
	template<typename T> bool GetExtPointer(const Anope::string &key, T *&p)
	{
		extensible_map::iterator it = this->Extension_Items.find(key);

		if (it != this->Extension_Items.end())
		{
			p = debug_cast<ExtensibleItemPointer<T> *>(it->second)->GetItem();
			return true;
		}

		p = NULL;
		return false;
	}

	/** Get an extension item that is a pointer to an array
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 * @param p If you provide a non-existent key, this value will be NULL. Otherwise a pointer to the item you requested will be placed in this templated parameter.
	 * @return Returns true if the item was found and false if it was nor, regardless of wether 'p' is NULL. This allows you to store NULL values in Extensible.
	 */
	template<typename T> bool GetExtArray(const Anope::string &key, T *&p)
	{
		extensible_map::iterator it = this->Extension_Items.find(key);

		if (it != this->Extension_Items.end())
		{
			p = debug_cast<ExtensibleItemPointerArray<T> *>(it->second)->GetItem();
			return true;
		}

		p = NULL;
		return false;
	}

	/** Get an extension item.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 * @return Returns true if the item was found and false if it was not.
	 *
	 * This single-parameter version only checks if the key exists, it does nothing with
	 * the 'data' field and is probably only useful in conjunction with the single-parameter
	 * version of Extend().
	 */
	bool GetExt(const Anope::string &key)
	{
		return this->Extension_Items.find(key) != this->Extension_Items.end();
	}

	/** Get a list of all extension items names.
	 * @param list A deque of strings to receive the list
	 * @return This function writes a list of all extension items stored
	 *	 in this object by name into the given deque and returns void.
	 */
	void GetExtList(std::deque<Anope::string> &list)
	{
		for (extensible_map::iterator it = Extension_Items.begin(), it_end = Extension_Items.end(); it != it_end; ++it)
			list.push_back(it->first);
	}
};

#endif // EXTENSIBLE_H
