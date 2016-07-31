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

#include "memotype.h"

MemoImpl::~MemoImpl()
{
}

MemoServ::MemoInfo *MemoImpl::GetMemoInfo()
{
	return Get(&MemoType::mi);
}

void MemoImpl::SetMemoInfo(MemoServ::MemoInfo *mi)
{
	Set(&MemoType::mi, mi);
}

time_t MemoImpl::GetTime()
{
	return Get(&MemoType::time);
}

void MemoImpl::SetTime(const time_t &t)
{
	Set(&MemoType::time, t);
}

Anope::string MemoImpl::GetSender()
{
	return Get(&MemoType::sender);
}

void MemoImpl::SetSender(const Anope::string &s)
{
	Set(&MemoType::sender, s);
}

Anope::string MemoImpl::GetText()
{
	return Get(&MemoType::text);
}

void MemoImpl::SetText(const Anope::string &t)
{
	Set(&MemoType::text, t);
}

bool MemoImpl::GetUnread()
{
	return Get(&MemoType::unread);
}

void MemoImpl::SetUnread(const bool &b)
{
	Set(&MemoType::unread, b);
}

bool MemoImpl::GetReceipt()
{
	return Get(&MemoType::receipt);
}

void MemoImpl::SetReceipt(const bool &b)
{
	Set(&MemoType::receipt, b);
}

