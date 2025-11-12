// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
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

struct LogSetting
{
	Anope::string chan;
	/* Our service name of the command */
	Anope::string service_name;
	/* The name of the client the command is on */
	Anope::string command_service;
	/* Name of the command to the user, can have spaces */
	Anope::string command_name;
	Anope::string method, extra;
	Anope::string creator;
	time_t created;

	virtual ~LogSetting() = default;
protected:
	LogSetting() = default;
};

struct LogSettings
	: Serialize::Checker<std::vector<LogSetting *> >
{
	typedef std::vector<LogSetting *>::iterator iterator;

protected:
	LogSettings() : Serialize::Checker<std::vector<LogSetting *> >("LogSetting")
	{
	}

public:
	virtual ~LogSettings() = default;
	virtual LogSetting *Create() = 0;
};
