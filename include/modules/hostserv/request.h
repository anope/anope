/*
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

class HostRequest
{
protected:
	HostRequest() = default;

public:
	Anope::string nick;
	Anope::string ident;
	Anope::string host;
	time_t time = 0;

	virtual ~HostRequest() = default;
};
