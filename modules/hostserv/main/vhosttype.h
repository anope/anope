/*
 * Anope IRC Services
 *
 * Copyright (C) 2015-2017 Anope Team <team@anope.org>
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

#include "vhost.h"

class VHostType : public Serialize::Type<VHostImpl>
{
 public:
	Serialize::ObjectField<VHostImpl, NickServ::Account *> account;
	Serialize::Field<VHostImpl, Anope::string> vident;
	Serialize::Field<VHostImpl, Anope::string> vhost;
	Serialize::Field<VHostImpl, Anope::string> creator;
	Serialize::Field<VHostImpl, time_t> created;
	Serialize::Field<VHostImpl, bool> default_;

	VHostType(Module *);
};
