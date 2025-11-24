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

#define CHANSERV_LOG_SETTING_EXT "logsettings"
#define CHANSERV_LOG_SETTING_TYPE "LogSetting"

namespace ChanServ
{
	class LogSetting;
	class LogSettings;
}

class ChanServ::LogSetting
{
protected:
	LogSetting() = default;

public:
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
};

class ChanServ::LogSettings
	: public Serialize::Checker<std::vector<ChanServ::LogSetting *>>
{
protected:
	LogSettings()
		: Serialize::Checker<std::vector<ChanServ::LogSetting *>>(CHANSERV_LOG_SETTING_TYPE)
	{
	}

public:
	virtual ~LogSettings() = default;
	virtual ChanServ::LogSetting *Create() = 0;
};
