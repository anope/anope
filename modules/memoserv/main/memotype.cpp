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

#include "module.h"
#include "memotype.h"

MemoType::MemoType(Module *me) : Serialize::Type<MemoImpl>(me)
	, mi(this, "mi", &MemoImpl::memoinfo, true)
	, time(this, "time", &MemoImpl::time)
	, sender(this, "sender", &MemoImpl::sender)
	, text(this, "text", &MemoImpl::text)
	, unread(this, "unread", &MemoImpl::unread)
	, receipt(this, "receipt", &MemoImpl::receipt)
{

}

