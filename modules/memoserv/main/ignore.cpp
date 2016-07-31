/*
 * Anope IRC Services
 *
 * Copyright (C) 2015-2016 Anope Team <team@anope.org>
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
#include "ignoretype.h"

IgnoreImpl::~IgnoreImpl()
{
}

MemoServ::MemoInfo *IgnoreImpl::GetMemoInfo()
{
	return Get(&IgnoreType::mi);
}

void IgnoreImpl::SetMemoInfo(MemoServ::MemoInfo *m)
{
	Set(&IgnoreType::mi, m);
}

Anope::string IgnoreImpl::GetMask()
{
	return Get(&IgnoreType::mask);
}

void IgnoreImpl::SetMask(const Anope::string &mask)
{
	Set(&IgnoreType::mask, mask);
}

