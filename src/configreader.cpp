/* Configuration file handling.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/* Taken from:
 *       +------------------------------------+
 *       | Inspire Internet Relay Chat Daemon |
 *       +------------------------------------+
 *
 *  InspIRCd: (C) 2002-2011 InspIRCd Development Team
 * See: http://www.inspircd.org/wiki/index.php/Credits
 *
 * This program is free but copyrighted software; see
 *            the file COPYING for details.
 *
 * ---------------------------------------------------
 */

#include "services.h"

ConfigReader::ConfigReader() : error(CONF_NO_ERROR)
{
}

ConfigReader::ConfigReader(const Anope::string &filename) : error(CONF_NO_ERROR)
{
}

ConfigReader::~ConfigReader()
{
}

Anope::string ConfigReader::ReadValue(const Anope::string &tag, const Anope::string &name, const Anope::string &default_value, int index, bool allow_linefeeds)
{
	/* Don't need to strlcpy() tag and name anymore, ReadConf() takes const char* */
	Anope::string result;

	if (!Config->ConfValue(Config->config_data, tag, name, default_value, index, result, allow_linefeeds))
		this->error = CONF_VALUE_NOT_FOUND;

	return result;
}

Anope::string ConfigReader::ReadValue(const Anope::string &tag, const Anope::string &name, int index, bool allow_linefeeds)
{
	return ReadValue(tag, name, "", index, allow_linefeeds);
}

bool ConfigReader::ReadFlag(const Anope::string &tag, const Anope::string &name, const Anope::string &default_value, int index)
{
	return Config->ConfValueBool(Config->config_data, tag, name, default_value, index);
}

bool ConfigReader::ReadFlag(const Anope::string &tag, const Anope::string &name, int index)
{
	return ReadFlag(tag, name, "", index);
}

int ConfigReader::ReadInteger(const Anope::string &tag, const Anope::string &name, const Anope::string &default_value, int index, bool need_positive)
{
	int result;

	if (!Config->ConfValueInteger(Config->config_data, tag, name, default_value, index, result))
	{
		this->error = CONF_VALUE_NOT_FOUND;
		return 0;
	}

	if (need_positive && result < 0)
	{
		this->error = CONF_INT_NEGATIVE;
		return 0;
	}

	return result;
}

int ConfigReader::ReadInteger(const Anope::string &tag, const Anope::string &name, int index, bool need_positive)
{
	return ReadInteger(tag, name, "", index, need_positive);
}

long ConfigReader::GetError()
{
	long olderr = this->error;
	this->error = 0;
	return olderr;
}

int ConfigReader::Enumerate(const Anope::string &tag) const
{
	return Config->ConfValueEnum(Config->config_data, tag);
}

int ConfigReader::EnumerateValues(const Anope::string &tag, int index)
{
	return Config->ConfVarEnum(Config->config_data, tag, index);
}

bool ConfigReader::Verify()
{
	return this->readerror;
}
