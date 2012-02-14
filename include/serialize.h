/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <sstream>

#include "anope.h"

namespace Serialize
{
 	enum DataType
	{
		DT_TEXT,
		DT_INT
	};
}

class stringstream : public std::stringstream
{
 private:
	Serialize::DataType type;
	bool key;
	unsigned _max;

 public:
	stringstream();
	stringstream(const stringstream &ss);
	Anope::string astr() const;
	template<typename T> std::istream &operator>>(T &val)
	{
		std::istringstream is(this->str());
		is >> val;
		return *this;
	}
	std::istream &operator>>(Anope::string &val);
	stringstream &setType(Serialize::DataType t);
	Serialize::DataType getType() const;
	stringstream &setKey();
	bool getKey() const;
	stringstream &setMax(unsigned m);
	unsigned getMax() const;
};


extern void RegisterTypes();

class CoreExport Serializable
{
 private:
	static std::list<Serializable *> *serizliable_items;

	std::list<Serializable *>::iterator s_iter;

 protected:
	Serializable();
	Serializable(const Serializable &);

	virtual ~Serializable();

	Serializable &operator=(const Serializable &);

 public:
	typedef std::map<Anope::string, stringstream> serialized_data;

	virtual Anope::string serialize_name() const = 0;
	virtual serialized_data serialize() = 0;

	static const std::list<Serializable *> &GetItems();
};

class CoreExport SerializeType
{
	typedef void (*unserialize_func)(Serializable::serialized_data &);

	static std::vector<Anope::string> type_order;
	static Anope::map<SerializeType *> types;

	Anope::string name;
	unserialize_func unserialize;

 public:
	SerializeType(const Anope::string &n, unserialize_func f);
	~SerializeType();

	const Anope::string &GetName();

	void Create(Serializable::serialized_data &data);

	static SerializeType *Find(const Anope::string &name);

	static const std::vector<Anope::string> &GetTypeOrder();
};

#endif // SERIALIZE_H
