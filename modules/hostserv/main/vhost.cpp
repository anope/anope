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

#include "vhosttype.h"
#include "modules/nickserv.h"

NickServ::Account *VHostImpl::GetAccount()
{
	return Get(&VHostType::account);
}

void VHostImpl::SetAccount(NickServ::Account *acc)
{
	Set(&VHostType::account, acc);
}

Anope::string VHostImpl::GetIdent()
{
	return Get(&VHostType::vident);
}

void VHostImpl::SetIdent(const Anope::string &vi)
{
	Set(&VHostType::vident, vi);
}

Anope::string VHostImpl::GetHost()
{
	return Get(&VHostType::vhost);
}

void VHostImpl::SetHost(const Anope::string &vh)
{
	Set(&VHostType::vhost, vh);
}

Anope::string VHostImpl::GetCreator()
{
	return Get(&VHostType::creator);
}

void VHostImpl::SetCreator(const Anope::string &c)
{
	Set(&VHostType::creator, c);
}

time_t VHostImpl::GetCreated()
{
	return Get(&VHostType::created);
}

void VHostImpl::SetCreated(time_t cr)
{
	Set(&VHostType::created, cr);
}

bool VHostImpl::IsDefault()
{
	return Get(&VHostType::default_);
}

void VHostImpl::SetDefault(bool d)
{
	Set(&VHostType::default_, d);
}
