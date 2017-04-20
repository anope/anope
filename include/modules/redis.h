/*
 * Anope IRC Services
 *
 * Copyright (C) 2013-2017 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
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
		Module *owner;

	 public:
		Interface(Module *m) : owner(m) { }
		virtual ~Interface() = default;

		Module *GetOwner() const { return owner; }

		virtual void OnResult(const Reply &r) anope_abstract;
		virtual void OnError(const Anope::string &error) { owner->logger.Log(error); }
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

