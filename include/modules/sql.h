/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

namespace SQL
{
	/** A SQL exception, can be thrown at various points
	 */
	class Exception : public ModuleException
	{
	 public:
		Exception(const Anope::string &reason) : ModuleException(reason) { }

		virtual ~Exception() throw() { }
	};

	/** A SQL query
	 */

	struct QueryData
	{
		Anope::string data;
		bool escape;
		bool null;
	};

	struct Query
	{
		Anope::string query;
		std::map<Anope::string, QueryData> parameters;

		Query() { }
		Query(const Anope::string &q) : query(q) { }

		Query& operator=(const Anope::string &q)
		{
			this->query = q;
			this->parameters.clear();
			return *this;
		}

		bool operator==(const Query &other) const
		{
			return this->query == other.query;
		}

		inline bool operator!=(const Query &other) const
		{
			return !(*this == other);
		}

		template<typename T> void SetValue(const Anope::string &key, const T& value, bool escape = true)
		{
			try
			{
				Anope::string string_value = stringify(value);
				this->parameters[key].data = string_value;
				this->parameters[key].escape = escape;
			}
			catch (const ConvertException &ex) { }
		}

		void SetNull(const Anope::string &key)
		{
			QueryData &qd = this->parameters[key];

			qd.data = "";
			qd.escape = false;
			qd.null = true;
		}

		Anope::string Unsafe() const
		{
			Anope::string q = query;
			for (auto it = parameters.begin(); it != parameters.end(); ++it)
				q = q.replace_all_cs("@" + it->first + "@", it->second.data);
			return q;
		}
	};

	/** A result from a SQL query
	 */
	class Result
	{
	 public:
		struct Value
		{
			bool null = false;
			Anope::string value;
		};

	 protected:
		std::vector<Anope::string> columns;
		// row, column
		std::vector<std::vector<Value>> values;

		Query query;
		Anope::string error;
	 public:
		unsigned int id;
 		Anope::string finished_query;

		Result() : id(0) { }
		Result(unsigned int i, const Query &q, const Anope::string &fq, const Anope::string &err = "") : query(q), error(err), id(i), finished_query(fq) { }

		inline operator bool() const { return this->error.empty(); }

		inline unsigned int GetID() const { return this->id; }
		inline const Query &GetQuery() const { return this->query; }
		inline const Anope::string &GetError() const { return this->error; }

		int Rows() const
		{
			return this->values.size();
		}

		const std::vector<Value> &Row(size_t index) const
		{
			try
			{
				return this->values.at(index);
			}
			catch (const std::out_of_range &)
			{
				throw Exception("Out of bounds access to SQLResult");
			}
		}

		const Value &GetValue(size_t index, const Anope::string &col) const
		{
			const std::vector<Value> &v = this->Row(index);

			auto it = std::find(this->columns.begin(), this->columns.end(), col);
			if (it == this->columns.end())
				throw Exception("Unknown column name in SQLResult: " + col);
			unsigned int col_idx = it - this->columns.begin();

			try
			{
				return v[col_idx];
			}
			catch (const std::out_of_range &)
			{
				throw Exception("Out of bounds access to SQLResult");
			}
		}

		const Anope::string &Get(size_t index, const Anope::string &col) const
		{
			const Value &value = GetValue(index, col);
			return value.value;
		}

		bool IsNull(size_t index, const Anope::string &col) const
		{
			const Value &value = GetValue(index, col);
			return value.null;
		}
	};

	/* An interface used by modules to retrieve the results
	 */
	class Interface
	{
	 public:
		Module *owner;

		Interface(Module *m) : owner(m) { }
		virtual ~Interface() { }

		virtual void OnResult(const Result &r) anope_abstract;
		virtual void OnError(const Result &r) anope_abstract;
	};

	/** Class providing the SQL service, modules call this to execute queries
	 */
	class Provider : public Service
	{
	 public:
		static constexpr const char *NAME = "sql";
		
		Provider(Module *c, const Anope::string &n) : Service(c, NAME, n) { }

		virtual void Run(Interface *i, const Query &query) anope_abstract;

		virtual Result RunQuery(const Query &query) anope_abstract;

		virtual std::vector<Query> InitSchema(const Anope::string &prefix) anope_abstract;
		virtual std::vector<Query> Replace(const Anope::string &table, const Query &, const std::set<Anope::string> &) anope_abstract;
		virtual std::vector<Query> CreateTable(const Anope::string &prefix, const Anope::string &table) anope_abstract;
		virtual std::vector<Query> AlterTable(const Anope::string &, const Anope::string &table, const Anope::string &field, bool object) anope_abstract;
		virtual std::vector<Query> CreateIndex(const Anope::string &table, const Anope::string &field) anope_abstract;

		virtual Query BeginTransaction() anope_abstract;
		virtual Query Commit() anope_abstract;

		virtual Serialize::ID GetID(const Anope::string &) anope_abstract;

		virtual Query GetTables(const Anope::string &prefix) anope_abstract;
	};

}

