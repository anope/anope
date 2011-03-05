
typedef int LDAPQuery;

class LDAPException : public ModuleException
{
 public:
	LDAPException(const Anope::string &reason) : ModuleException(reason) { }

	virtual ~LDAPException() throw() { }
};

struct LDAPAttributes : public std::map<Anope::string, std::vector<Anope::string> >
{
	size_t size(const Anope::string &attr) const
	{
		const std::vector<Anope::string>& array = this->getArray(attr);
		return array.size();
	}

	const std::vector<Anope::string> keys() const
	{
		std::vector<Anope::string> k;
		for (const_iterator it = this->begin(), it_end = this->end(); it != it_end; ++it)
			k.push_back(it->first);
		return k;
	}

	const Anope::string &get(const Anope::string &attr) const
	{
		const std::vector<Anope::string>& array = this->getArray(attr);
		if (array.empty())
			throw LDAPException("Empty attribute " + attr + " in LDAPResult::get");
		return array[0];
	}

	const std::vector<Anope::string>& getArray(const Anope::string &attr) const
	{
		const_iterator it = this->find(attr);
		if (it == this->end())
			throw LDAPException("Unknown attribute " + attr + " in LDAPResult::getArray");
		return it->second;
	}
};

struct LDAPResult
{
	std::vector<LDAPAttributes> messages;
	Anope::string error;

	enum QueryType
	{
		QUERY_BIND,
		QUERY_SEARCH
	};

	QueryType type;
	LDAPQuery id;

	size_t size() const
	{
		return this->messages.size();
	}

	const LDAPAttributes &get(size_t sz) const
	{
		if (sz >= this->messages.size())
			throw LDAPException("Index out of range");
		return this->messages[sz];
	}

	const Anope::string &getError() const
	{
		return this->error;
	}
};

class LDAPInterface
{
 public:
	Module *owner;

	LDAPInterface(Module *m) : owner(m) { }

	virtual void OnResult(const LDAPResult &r) { }

	virtual void OnError(const LDAPResult &err) { }
};


class LDAPProvider : public Service
{
 public:
	LDAPProvider(Module *c, const Anope::string &n) : Service(c, n) { }

	virtual LDAPQuery Bind(LDAPInterface *i, const Anope::string &who, const Anope::string &pass) = 0;

	virtual LDAPQuery Search(LDAPInterface *i, const Anope::string &base, const Anope::string &filter) = 0;
};

