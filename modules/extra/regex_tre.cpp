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
///   find_package("tre" REQUIRED)
///   target_link_libraries(${SO} PRIVATE "tre::tre")
/// else()
///   pkg_check_modules("TRE" IMPORTED_TARGET REQUIRED "tre")
///   target_link_libraries(${SO} PRIVATE PkgConfig::TRE)
///   target_compile_options(${SO} PRIVATE "-Wno-error=date-time") # Workaround for TRE bug 117
/// endif()
/// END CMAKE

#include "module.h"
#include <tre/regex.h>

class TRERegex final
	: public Regex
{
	regex_t regbuf;

public:
	TRERegex(const Anope::string &expr) : Regex(expr)
	{
		int err = regcomp(&this->regbuf, expr.c_str(), REG_EXTENDED | REG_NOSUB);
		if (err)
		{
			char buf[BUFSIZE];
			regerror(err, &this->regbuf, buf, sizeof(buf));
			regfree(&this->regbuf);
			throw RegexException("Error in regex " + expr + ": " + buf);
		}
	}

	~TRERegex()
	{
		regfree(&this->regbuf);
	}

	bool Matches(const Anope::string &str)
	{
		return regexec(&this->regbuf, str.c_str(), 0, NULL, 0) == 0;
	}
};

class TRERegexProvider final
	: public RegexProvider
{
public:
	TRERegexProvider(Module *creator) : RegexProvider(creator, "regex/tre") { }

	Regex *Compile(const Anope::string &expression) override
	{
		return new TRERegex(expression);
	}
};

class ModuleRegexTRE final
	: public Module
{
	TRERegexProvider tre_regex_provider;

public:
	ModuleRegexTRE(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR),
		tre_regex_provider(this)
	{
		this->SetPermanent(true);

		Log(this) << "Module was compiled against TRE version " << TRE_VERSION << " and is running against version " << tre_version();
	}

	~ModuleRegexTRE()
	{
		for (auto *xlm : XLineManager::XLineManagers)
		{
			for (auto *x : xlm->GetList())
			{
				if (x->regex && dynamic_cast<TRERegex *>(x->regex))
				{
					delete x->regex;
					x->regex = NULL;
				}
			}
		}
	}
};

MODULE_INIT(ModuleRegexTRE)
