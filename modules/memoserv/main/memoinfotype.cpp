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
#include "memoinfotype.h"

MemoInfoType::MemoInfoType(Module *me) : Serialize::Type<MemoInfoImpl>(me)
	, account(this, "account", &MemoInfoImpl::account, true)
	, channel(this, "channel", &MemoInfoImpl::channel, true)
	, memomax(this, "memomax", &MemoInfoImpl::memomax)
	, hardmax(this, "hardmax", &MemoInfoImpl::hardmax)
{

}

