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

Serialize::Object *VHostImpl::GetOwner()
{
	return Get(&VHostType::owner);
}

void VHostImpl::SetOwner(Serialize::Object *owner)
{
	Set(&VHostType::owner, owner);
}

Anope::string VHostImpl::GetIdent()
{
	return Get(&VHostType::vident);
}

void VHostImpl::SetIdent(const Anope::string &vident)
{
	Set(&VHostType::vident, vident);
}

Anope::string VHostImpl::GetHost()
{
	return Get(&VHostType::vhost);
}

void VHostImpl::SetHost(const Anope::string &vhost)
{
	Set(&VHostType::vhost, vhost);
}

Anope::string VHostImpl::GetCreator()
{
	return Get(&VHostType::creator);
}

void VHostImpl::SetCreator(const Anope::string &creator)
{
	Set(&VHostType::creator, creator);
}

time_t VHostImpl::GetCreated()
{
	return Get(&VHostType::created);
}

void VHostImpl::SetCreated(time_t created)
{
	Set(&VHostType::created, created);
}

bool VHostImpl::IsDefault()
{
	return Get(&VHostType::default_);
}

void VHostImpl::SetDefault(bool default_)
{
	Set(&VHostType::default_, default_);
}
