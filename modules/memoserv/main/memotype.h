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

#include "memo.h"

class MemoType : public Serialize::Type<MemoImpl>
{
 public:
	Serialize::ObjectField<MemoImpl, MemoServ::MemoInfo *> mi;
	Serialize::Field<MemoImpl, time_t> time;
	Serialize::Field<MemoImpl, Anope::string> sender;
	Serialize::Field<MemoImpl, Anope::string> text;
	Serialize::Field<MemoImpl, bool> unread, receipt;

 	MemoType(Module *);
};
