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

/// BEGIN CMAKE
/// if(WIN32)
///   find_package("PCRE2" REQUIRED)
///   target_link_libraries(${SO} PRIVATE "PCRE2::POSIX")
/// endif()
/// END CMAKE

#include "module.h"

#ifdef _WIN32
# include "pcre2posix.h"
#else
# include <sys/types.h>
# include <regex.h>
#endif

class POSIXRegex final
	: public Regex
{
	regex_t regbuf;

public:
	POSIXRegex(const Anope::string &expr) : Regex(expr)
	{
		int err = regcomp(&this->regbuf, expr.c_str(), REG_EXTENDED | REG_NOSUB | REG_ICASE);
		if (err)
		{
			char buf[BUFSIZE];
			regerror(err, &this->regbuf, buf, sizeof(buf));
			regfree(&this->regbuf);
			throw RegexException("Error in regex " + expr + ": " + buf);
		}
	}

	~POSIXRegex()
	{
		regfree(&this->regbuf);
	}

	bool Matches(const Anope::string &str)
	{
		return regexec(&this->regbuf, str.c_str(), 0, NULL, 0) == 0;
	}
};

class POSIXRegexProvider final
	: public RegexProvider
{
public:
	POSIXRegexProvider(Module *creator) : RegexProvider(creator, "regex/posix") { }

	Regex *Compile(const Anope::string &expression) override
	{
		return new POSIXRegex(expression);
	}
};

class ModuleRegexPOSIX final
	: public Module
{
	POSIXRegexProvider posix_regex_provider;

public:
	ModuleRegexPOSIX(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR),
		posix_regex_provider(this)
	{
		this->SetPermanent(true);
	}

	~ModuleRegexPOSIX()
	{
		for (auto *xlm : XLineManager::XLineManagers)
		{
			for (auto *x : xlm->GetList())
			{
				if (x->regex && dynamic_cast<POSIXRegex *>(x->regex))
				{
					delete x->regex;
					x->regex = NULL;
				}
			}
		}
	}
};

MODULE_INIT(ModuleRegexPOSIX)
