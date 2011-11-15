#ifndef SERIALIZE_H
#define SERIALIZE_H

namespace Serialize
{
 	enum DataType
	{
		DT_TEXT,
		DT_INT
	};

	class stringstream : public std::stringstream
	{
	 private:
		DataType type;
		bool key;
		unsigned _max;

	 public:
		stringstream() : std::stringstream(), type(DT_TEXT), key(false), _max(0) { }
		stringstream(const stringstream &ss) : std::stringstream(ss.str()), type(DT_TEXT), key(false), _max(0) { }
		Anope::string astr() const { return this->str(); }
		template<typename T> std::istream &operator>>(T &val)
		{
			std::istringstream is(this->str());
			is >> val;
			return *this;
		}
		std::istream &operator>>(Anope::string &val)
		{
			val = this->str();
			return *this;
		}
		stringstream &setType(DataType t)
		{
			this->type = t;
			return *this;
		}
		DataType getType() const
		{
			return this->type;
		}
		stringstream &setKey()
		{
			this->key = true;
			return *this;
		}
		bool getKey() const
		{
			return this->key;
		}
		stringstream &setMax(unsigned m)
		{
			this->_max = m;
			return *this;
		}
		unsigned getMax() const
		{
			return this->_max;
		}
	};
}

extern void RegisterTypes();

class CoreExport Serializable
{
 private:
	static std::list<Serializable *> *serizliable_items;

	Anope::string serizliable_name;
	std::list<Serializable *>::iterator s_iter;

	Serializable()
	{
		throw CoreException();
	}

 protected:
	Serializable(const Anope::string &n) : serizliable_name(n)
	{
		if (serizliable_items == NULL)
			serizliable_items = new std::list<Serializable *>();
		serizliable_items->push_front(this);
		this->s_iter = serizliable_items->begin();
	}

	Serializable(const Serializable &)
	{
		serizliable_items->push_front(this);
		this->s_iter = serizliable_items->begin();
	}

	virtual ~Serializable()
	{
		serizliable_items->erase(this->s_iter);
	}

	Serializable &operator=(const Serializable &)
	{
		return *this;
	}

 public:
	typedef std::map<Anope::string, Serialize::stringstream> serialized_data;

	const Anope::string &GetSerializeName()
	{
		return this->serizliable_name;
	}

	virtual serialized_data serialize() = 0;

	static const std::list<Serializable *> &GetItems()
	{
		return *serizliable_items;
	}
};

class CoreExport SerializeType
{
	typedef void (*unserialize_func)(Serializable::serialized_data &);

	static std::vector<Anope::string> type_order;
	static Anope::map<SerializeType *> types;

	Anope::string name;
	unserialize_func unserialize;

 public:
	SerializeType(const Anope::string &n, unserialize_func f) : name(n), unserialize(f)
	{
		type_order.push_back(this->name);
		types[this->name] = this;
	}

	~SerializeType()
	{
		std::vector<Anope::string>::iterator it = std::find(type_order.begin(), type_order.end(), this->name);
		if (it != type_order.end())
			type_order.erase(it);
		types.erase(this->name);
	}

	const Anope::string &GetName()
	{
		return this->name;
	}

	void Create(Serializable::serialized_data &data)
	{
		this->unserialize(data);
	}

	static SerializeType *Find(const Anope::string &name)
	{
		Anope::map<SerializeType *>::iterator it = types.find(name);
		if (it != types.end())
			return it->second;
		return NULL;
	}

	static const std::vector<Anope::string> &GetTypeOrder()
	{
		return type_order;
	}
};

#endif // SERIALIZE_H
