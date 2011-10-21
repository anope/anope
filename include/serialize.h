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
		unsigned max;

	 public:
		stringstream() : std::stringstream(), type(DT_TEXT), key(false), max(0) { }
		stringstream(const stringstream &ss) : std::stringstream(ss.str()), type(DT_TEXT), key(false), max(0) { }
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
			this->max = m;
			return *this;
		}
		unsigned getMax() const
		{
			return this->max;
		}
	};
}

class SerializableBase;

extern std::vector<SerializableBase *> serialized_types;
extern std::list<SerializableBase *> *serialized_items;
extern void RegisterTypes();

class SerializableBase
{
 public:
 	typedef std::map<Anope::string, Serialize::stringstream> serialized_data;

	virtual Anope::string serialize_name() = 0;
	virtual serialized_data serialize() = 0;
	virtual void alloc(serialized_data &) = 0;
};

template<typename Type> class Serializable : public SerializableBase
{
 public:
	static class SerializableAllocator : public SerializableBase
	{
		Anope::string name;

	 public:
	 	SerializableAllocator()
		{
		}

		void Register(const Anope::string &n, int pos = -1)
		{
			this->name = n;
			serialized_types.insert(serialized_types.begin() + (pos < 0 ? serialized_types.size() : pos), this);
		}

		void Unregister()
		{
			std::vector<SerializableBase *>::iterator it = std::find(serialized_types.begin(), serialized_types.end(), this);
			if (it != serialized_types.end())
				serialized_types.erase(it);
		}

		Anope::string serialize_name()
		{
			if (this->name.empty())
				throw CoreException();
			return this->name;
		}

		serialized_data serialize()
		{
			throw CoreException();
		}

	 	void alloc(serialized_data &data)
		{
			Type::unserialize(data);
		}
	} Alloc;

 private:
	std::list<SerializableBase *>::iterator s_iter;

 protected:
	Serializable()
	{
		if (serialized_items == NULL)
			serialized_items = new std::list<SerializableBase *>();
		serialized_items->push_front(this);
		this->s_iter = serialized_items->begin();
	}

	Serializable(const Serializable &)
	{
		serialized_items->push_front(this);
		this->s_iter = serialized_items->begin();
	}

	~Serializable()
	{
		serialized_items->erase(this->s_iter);
	}

 public:
	Anope::string serialize_name()
	{
		return Alloc.serialize_name();
	}

	void alloc(serialized_data &)
	{
		throw CoreException();
	}
};

template<typename T> typename Serializable<T>::SerializableAllocator Serializable<T>::Alloc;

#endif // SERIALIZE_H
