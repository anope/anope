/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2016 Anope Team <team@anope.org>
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

namespace Encryption
{
	typedef std::pair<const unsigned char *, size_t> Hash;
	typedef std::pair<const uint32_t *, size_t> IV;

	class Context
	{
	 public:
	 	virtual ~Context() { }
		virtual void Update(const unsigned char *data, size_t len) anope_abstract;
		virtual void Finalize() anope_abstract;
		virtual Hash GetFinalizedHash() anope_abstract;
	};

	class Provider : public Service
	{
	 public:
		static constexpr const char *NAME = "hash";
		
		Provider(Module *creator, const Anope::string &sname) : Service(creator, NAME, sname) { }
		virtual ~Provider() { }

		virtual Context *CreateContext(IV * = NULL) anope_abstract;
		virtual IV GetDefaultIV() anope_abstract;
	};
}

