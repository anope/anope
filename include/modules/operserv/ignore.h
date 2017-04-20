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

class Ignore : public Serialize::Object
{
 public:
	static constexpr const char *NAME = "ignore";

	using Serialize::Object::Object;

	virtual Anope::string GetMask() anope_abstract;
	virtual void SetMask(const Anope::string &) anope_abstract;

	virtual Anope::string GetCreator() anope_abstract;
	virtual void SetCreator(const Anope::string &) anope_abstract;

	virtual Anope::string GetReason() anope_abstract;
	virtual void SetReason(const Anope::string &) anope_abstract;

	virtual time_t GetTime() anope_abstract;
	virtual void SetTime(const time_t &) anope_abstract;
};

class IgnoreService : public Service
{
 public:
	static constexpr const char *NAME = "ignore";
	
	IgnoreService(Module *c) : Service(c, NAME) { }

	virtual Ignore *Find(const Anope::string &mask) anope_abstract;
};

