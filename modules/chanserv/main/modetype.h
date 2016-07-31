
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

#include "mode.h"

class CSModeType : public Serialize::Type<ModeImpl>
{
 public:
	Serialize::ObjectField<ModeImpl, ChanServ::Channel *> channel;
	Serialize::Field<ModeImpl, Anope::string> mode, param;

	CSModeType(Module *creator) : Serialize::Type<ModeImpl>(creator)
		, channel(this, "channel", &ModeImpl::channel, true)
		, mode(this, "mode", &ModeImpl::mode)
		, param(this, "param", &ModeImpl::param)
	{
	}
};
