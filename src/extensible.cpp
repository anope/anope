/*
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 *
 */

#include "extensible.h"

Extensible::Extensible() : extension_items(NULL)
{
}

Extensible::~Extensible()
{
	if (extension_items)
	{
		for (extensible_map::iterator it = extension_items->begin(), it_end = extension_items->end(); it != it_end; ++it)
			delete it->second;
		delete extension_items;
	}
}

void Extensible::Extend(const Anope::string &key, ExtensibleItem *p)
{
	this->Shrink(key);
	if (!extension_items)
		extension_items = new extensible_map();
	(*this->extension_items)[key] = p;
}

void Extensible::ExtendMetadata(const Anope::string &key, const Anope::string &value)
{
	this->Extend(key, new ExtensibleMetadata(value));
}

bool Extensible::Shrink(const Anope::string &key)
{
	if (!extension_items)
		return false;

	extensible_map::iterator it = this->extension_items->find(key);
	if (it != this->extension_items->end())
	{
		delete it->second;
		/* map::size_type map::erase( const key_type& key );
		 * returns the number of elements removed, std::map
		 * is single-associative so this should only be 0 or 1
		 */
		return this->extension_items->erase(key) > 0;
	}

	return false;
}

bool Extensible::HasExt(const Anope::string &key) const
{
	return this->extension_items != NULL && this->extension_items->count(key) > 0;
}

void Extensible::GetExtList(std::deque<Anope::string> &list) const
{
	if (extension_items)
		for (extensible_map::const_iterator it = extension_items->begin(), it_end = extension_items->end(); it != it_end; ++it)
			list.push_back(it->first);
}

void Extensible::ExtensibleSerialize(Serialize::Data &data) const
{
	if (extension_items)
		for (extensible_map::const_iterator it = extension_items->begin(), it_end = extension_items->end(); it != it_end; ++it)
			if (it->second && it->second->Serialize())
				data["extensible:" + it->first] << *it->second->Serialize();
}

void Extensible::ExtensibleUnserialize(Serialize::Data &data)
{
	/* Shrink existing extensible items */
	if (extension_items)
		for (extensible_map::iterator it = extension_items->begin(), it_end = extension_items->end(); it != it_end; ++it)
			this->Shrink(it->first);
		
	std::set<Anope::string> keys = data.KeySet();
	for (std::set<Anope::string>::iterator it = keys.begin(), it_end = keys.end(); it != it_end; ++it)
		if (it->find("extensible:") == 0)
		{
			if (!extension_items)
				extension_items = new extensible_map();

			Anope::string str;
			data[*it] >> str;

			this->ExtendMetadata(it->substr(11), str);
		}
}

