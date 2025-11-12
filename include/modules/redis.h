// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

namespace Redis
{
	struct Reply final
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
			for (const auto *reply : multi_bulk)
				delete reply;
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

		virtual void OnResult(const Reply &r) = 0;
		virtual void OnError(const Anope::string &error) { Log(owner) << error; }
	};

	class Provider
		: public Service
	{
	public:
		Provider(Module *c, const Anope::string &n) : Service(c, "Redis::Provider", n) { }

		virtual bool IsSocketDead() = 0;

		virtual void SendCommand(Interface *i, const std::vector<Anope::string> &cmds) = 0;
		virtual void SendCommand(Interface *i, const Anope::string &str) = 0;

		virtual bool BlockAndProcess() = 0;

		virtual void Subscribe(Interface *i, const Anope::string &pattern) = 0;
		virtual void Unsubscribe(const Anope::string &pattern) = 0;

		virtual void StartTransaction() = 0;
		virtual void CommitTransaction() = 0;
	};
}
