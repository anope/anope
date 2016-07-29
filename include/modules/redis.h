/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

namespace Redis
{
	struct Reply
	{
		enum Type
		{
			NOT_PARSED,
			NOT_OK,
			OK,
			INT,
			BULK,
			MULTI_BULK
		}
		type;

		Reply() { Clear(); }
		~Reply() { Clear(); }

		void Clear()
		{
			type = NOT_PARSED;
			i = 0;
			bulk.clear();
			multi_bulk_size = 0;
			for (unsigned j = 0; j < multi_bulk.size(); ++j)
				delete multi_bulk[j];
			multi_bulk.clear();
		}

		int64_t i;
		Anope::string bulk;
		int multi_bulk_size;
		std::deque<Reply *> multi_bulk;
	};

	class Interface
	{
	 public:
		Module *owner;

		Interface(Module *m) : owner(m) { }
		virtual ~Interface() = default;

		virtual void OnResult(const Reply &r) anope_abstract;
		virtual void OnError(const Anope::string &error) { Log(owner) << error; }
	};

	class FInterface : public Interface
	{
	 public:
		using Func = std::function<void(const Reply &)>;
		Func function;

		FInterface(Module *m, Func f) : Interface(m), function(f) { }
	
		void OnResult(const Reply &r) override { function(r); }
	};

	class Provider : public Service
	{
	 public:
		static constexpr const char *NAME = "redis";
		
		Provider(Module *c, const Anope::string &n) : Service(c, NAME, n) { }

		virtual void SendCommand(Interface *i, const std::vector<Anope::string> &cmds) anope_abstract;
		virtual void SendCommand(Interface *i, const Anope::string &str) anope_abstract;

		virtual bool BlockAndProcess() anope_abstract;

		virtual void Subscribe(Interface *i, const Anope::string &) anope_abstract;
		virtual void Unsubscribe(const Anope::string &pattern) anope_abstract;

		virtual void StartTransaction() anope_abstract;
		virtual void CommitTransaction() anope_abstract;
	};
}

