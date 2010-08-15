
/** A SQL exception, can be thrown at various points
 */
class SQLException : public ModuleException
{
 public:
	SQLException(const Anope::string &reason) : ModuleException(reason) { }

	virtual ~SQLException() throw() { }
};

/** A result from a SQL query
 */
class SQLResult
{
 protected:
	/* Rows, column, item */
	std::vector<std::map<Anope::string, Anope::string> > entries;
	Anope::string query;
	Anope::string error;
 public:
	SQLResult(const Anope::string &q, const Anope::string &err = "") : query(q), error(err) { }

	inline operator bool() const { return this->error.empty(); }

	inline const Anope::string &GetQuery() const { return this->query; }
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
class SQLProvider : public Service
{
 public:
	SQLProvider(Module *c, const Anope::string &n) : Service(c, n) { }

	virtual void Run(SQLInterface *i, const Anope::string &query) = 0;

	virtual SQLResult RunQuery(const Anope::string &query) = 0;

	virtual const Anope::string Escape(const Anope::string &buf) { return buf; }
};

