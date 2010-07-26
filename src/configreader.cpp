/*       +------------------------------------+
 *       | Inspire Internet Relay Chat Daemon |
 *       +------------------------------------+
 *
 *  InspIRCd: (C) 2002-2010 InspIRCd Development Team
 * See: http://www.inspircd.org/wiki/index.php/Credits
 *
 * This program is free but copyrighted software; see
 *            the file COPYING for details.
 *
 * ---------------------------------------------------
 */

#include "services.h"

ConfigReader::ConfigReader() : data(&Config.config_data), errorlog(new std::ostringstream(std::stringstream::in | std::stringstream::out)), privatehash(false), error(CONF_NO_ERROR)
{
}

ConfigReader::~ConfigReader()
{
	if (this->errorlog)
		delete this->errorlog;
	if (this->privatehash)
		delete this->data;
}

ConfigReader::ConfigReader(const Anope::string &filename) : data(new ConfigDataHash), errorlog(new std::ostringstream(std::stringstream::in | std::stringstream::out)), privatehash(true), error(CONF_NO_ERROR)
{
	Config.ClearStack();
}

Anope::string ConfigReader::ReadValue(const Anope::string &tag, const Anope::string &name, const Anope::string &default_value, int index, bool allow_linefeeds)
{
	/* Don't need to strlcpy() tag and name anymore, ReadConf() takes const char* */
	Anope::string result;

	if (!Config.ConfValue(*this->data, tag, name, default_value, index, result, allow_linefeeds))
		this->error = CONF_VALUE_NOT_FOUND;

	return result;
}

Anope::string ConfigReader::ReadValue(const Anope::string &tag, const Anope::string &name, int index, bool allow_linefeeds)
{
	return ReadValue(tag, name, "", index, allow_linefeeds);
}

bool ConfigReader::ReadFlag(const Anope::string &tag, const Anope::string &name, const Anope::string &default_value, int index)
{
	return Config.ConfValueBool(*this->data, tag, name, default_value, index);
}

bool ConfigReader::ReadFlag(const Anope::string &tag, const Anope::string &name, int index)
{
	return ReadFlag(tag, name, "", index);
}

int ConfigReader::ReadInteger(const Anope::string &tag, const Anope::string &name, const Anope::string &default_value, int index, bool need_positive)
{
	int result;

	if (!Config.ConfValueInteger(*this->data, tag, name, default_value, index, result))
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

void ConfigReader::DumpErrors(bool bail)
{
	Config.ReportConfigError(this->errorlog->str(), bail);
}

int ConfigReader::Enumerate(const Anope::string &tag)
{
	return Config.ConfValueEnum(*this->data, tag);
}

int ConfigReader::EnumerateValues(const Anope::string &tag, int index)
{
	return Config.ConfVarEnum(*this->data, tag, index);
}

bool ConfigReader::Verify()
{
	return this->readerror;
}
