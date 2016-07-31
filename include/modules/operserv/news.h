/*
 * Anope IRC Services
 *
 * Copyright (C) 2013-2016 Anope Team <team@anope.org>
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

#pragma once

enum NewsType
{
	NEWS_LOGON,
	NEWS_RANDOM,
	NEWS_OPER
};

class NewsItem : public Serialize::Object
{
 public:
	static constexpr const char *const NAME = "newsitem";
	 
	using Serialize::Object::Object;

	virtual NewsType GetNewsType() anope_abstract;
	virtual void SetNewsType(const NewsType &) anope_abstract;

	virtual Anope::string GetText() anope_abstract;
	virtual void SetText(const Anope::string &) anope_abstract;

	virtual Anope::string GetWho() anope_abstract;
	virtual void SetWho(const Anope::string &) anope_abstract;

	virtual time_t GetTime() anope_abstract;
	virtual void SetTime(const time_t &) anope_abstract;
};
