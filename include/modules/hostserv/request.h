// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#define HOSTSERV_HOST_REQUEST_EXT "hostrequest"

namespace HostServ
{
	class HostRequest;
}

class HostServ::HostRequest
{
protected:
	HostRequest() = default;

public:
	Anope::string nick;
	Anope::string ident;
	Anope::string host;
	time_t time = 0;
	Anope::string validation_token;
	time_t last_validation = 0;

	virtual ~HostRequest() = default;
};
