/*
 *
 * (C) 2012-2023 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include <sys/types.h>
#include <regex.h>

class POSIXRegex : public Regex
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

class POSIXRegexProvider : public RegexProvider
{
public:
	POSIXRegexProvider(Module *creator) : RegexProvider(creator, "regex/posix") { }

	Regex *Compile(const Anope::string &expression) override
	{
		return new POSIXRegex(expression);
	}
};

class ModuleRegexPOSIX : public Module
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
