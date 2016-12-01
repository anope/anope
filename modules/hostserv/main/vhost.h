/*
 * Anope IRC Services
 *
 * Copyright (C) 2016 Anope Team <team@anope.org>
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

#include "modules/hostserv.h"

class VHostImpl : public HostServ::VHost
{
	friend class VHostType;

	Serialize::Storage<Serialize::Object *> owner;
	Serialize::Storage<Anope::string> vident;
	Serialize::Storage<Anope::string> vhost;
	Serialize::Storage<Anope::string> creator;
	Serialize::Storage<time_t> created;
	Serialize::Storage<bool> default_;

 public:
	using HostServ::VHost::VHost;

	Serialize::Object *GetOwner() override;
	void SetOwner(Serialize::Object *) override;

	Anope::string GetIdent() override;
	void SetIdent(const Anope::string &) override;

	Anope::string GetHost() override;
	void SetHost(const Anope::string &) override;

	Anope::string GetCreator() override;
	void SetCreator(const Anope::string &) override;

	time_t GetCreated() override;
	void SetCreated(time_t) override;

	bool IsDefault() override;
	void SetDefault(bool) override;
};

