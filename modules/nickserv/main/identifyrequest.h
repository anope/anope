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

#include "modules/nickserv.h"

class IdentifyRequestImpl : public NickServ::IdentifyRequest
{
 public:
	IdentifyRequestImpl(NickServ::IdentifyRequestListener *, Module *o, const Anope::string &acc, const Anope::string &pass);
	virtual ~IdentifyRequestImpl();

	void Hold(Module *m) override;
	void Release(Module *m) override;
	void Success(Module *m) override;
	void Dispatch() override;
	void Unload(Module *);
};
