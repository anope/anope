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

enum ForbidType
{
	FT_NICK = 1,
	FT_CHAN,
	FT_EMAIL,
	FT_REGISTER,
	FT_SIZE
};

class ForbidData : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	static constexpr const char *NAME = "forbid";

	virtual Anope::string GetMask() anope_abstract;
	virtual void SetMask(const Anope::string &) anope_abstract;

	virtual Anope::string GetCreator() anope_abstract;
	virtual void SetCreator(const Anope::string &) anope_abstract;

	virtual Anope::string GetReason() anope_abstract;
	virtual void SetReason(const Anope::string &) anope_abstract;

	virtual time_t GetCreated() anope_abstract;
	virtual void SetCreated(const time_t &) anope_abstract;

	virtual time_t GetExpires() anope_abstract;
	virtual void SetExpires(const time_t &) anope_abstract;

	virtual ForbidType GetType() anope_abstract;
	virtual void SetType(ForbidType) anope_abstract;
};

class ForbidService : public Service
{
 public:
	static constexpr const char *NAME = "forbid";
	
	ForbidService(Module *m) : Service(m, NAME) { }

	virtual ForbidData *FindForbid(const Anope::string &mask, ForbidType type) anope_abstract;

	virtual std::vector<ForbidData *> GetForbids() anope_abstract;
};


