
/** A SQL exception, can be thrown at various points
 */
class SQLException : public ModuleException
{
 public:
	SQLException(const Anope::string &reason) : ModuleException(reason) { }

	virtual ~SQLException() throw() { }
};

/** A SQL query
 */
struct SQLQuery
{
	Anope::string query;
	std::map<Anope::string, Anope::string> parameters;

	SQLQuery() { }
	SQLQuery(const Anope::string &q) : query(q) { }

	SQLQuery& operator=(const Anope::string &q)
	{
		this->query = q;
		this->parameters.clear();
		return *this;
	}
	
	bool operator==(const SQLQuery &other) const
	{
		return this->query == other.query;
	}

	inline bool operator!=(const SQLQuery &other) const
	{
		return !(*this == other);
	}

	template<typename T> void setValue(const Anope::string &key, const T& value)
	{
		try
		{
			Anope::string string_value = stringify(value);
			this->parameters[key] = string_value;
		}
		catch (const ConvertException &ex) { }
	}
};

/** A result from a SQL query
 */
class SQLResult
{
 protected:
	/* Rows, column, item */
	std::vector<std::map<Anope::string, Anope::string> > entries;
	SQLQuery query;
	Anope::string error;
 public:
 	Anope::string finished_query;

	SQLResult(const SQLQuery &q, const Anope::string &fq, const Anope::string &err = "") : query(q), error(err), finished_query(fq) { }

	inline operator bool() const { return this->error.empty(); }

	inline const SQLQuery &GetQuery() const { return this->query; }
	inline const Anope::string &GetError() const { return this->error; }

	int Rows() const { return this->entries.size(); }

	const std::map<Anope::string, Anope::string> &Row(size_t index) const
	{
		try
		{
			return this->entries.at(index);
		}
		catch (const std::out_of_range &)
		{
			throw SQLException("Out of bounds access to SQLResult");
		}
	}

	const Anope::string Get(size_t index, const Anope::string &col) const
	{
		const std::map<Anope::string, Anope::string> rows = this->Row(index);

		std::map<Anope::string, Anope::string>::const_iterator it = rows.find(col);
		if (it == rows.end())
			throw SQLException("Unknown column name in SQLResult: " + col);
	
		return it->second;
	}
};

/* An interface used by modules to retrieve the results
 */
class SQLInterface
{
 public:
	Module *owner;

	SQLInterface(Module *m) : owner(m) { }

	virtual void OnResult(const SQLResult &r) { }

	virtual void OnError(const SQLResult &r) { }
};

/** Class providing the SQL service, modules call this to execute queries
 */
class SQLProvider : public Service<SQLProvider>
{
 public:
	SQLProvider(Module *c, const Anope::string &n) : Service<SQLProvider>(c, n) { }

	virtual void Run(SQLInterface *i, const SQLQuery &query) = 0;

	virtual SQLResult RunQuery(const SQLQuery &query) = 0;
};

