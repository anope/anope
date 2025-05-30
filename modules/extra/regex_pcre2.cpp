/*
 *
 * (C) 2012-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

/* RequiredLibraries: pcre2-8 */
/* RequiredWindowsLibraries: pcre2-8 */

#include "module.h"

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

class PCRERegex final
	: public Regex
{
	pcre2_code *regex;

public:
	PCRERegex(const Anope::string &expr) : Regex(expr)
	{
		int errcode;
		PCRE2_SIZE erroffset;
		this->regex = pcre2_compile(reinterpret_cast<PCRE2_SPTR8>(expr.c_str()), expr.length(), PCRE2_CASELESS, &errcode, &erroffset, NULL);

		if (!this->regex)
		{
			PCRE2_UCHAR error[128];
			pcre2_get_error_message(errcode, error, sizeof error);
			throw RegexException("Error in regex " + expr + " at offset " + Anope::ToString(erroffset) + ": " + reinterpret_cast<const char*>(error));
		}
	}

	~PCRERegex()
	{
		pcre2_code_free(this->regex);
	}

	bool Matches(const Anope::string &str)
	{
		pcre2_match_data *unused = pcre2_match_data_create_from_pattern(this->regex, NULL);
		int result = pcre2_match(regex, reinterpret_cast<PCRE2_SPTR8>(str.c_str()), str.length(), 0, 0, unused, NULL);
		pcre2_match_data_free(unused);
		return result >= 0;
	}
};

class PCRERegexProvider final
	: public RegexProvider
{
public:
	PCRERegexProvider(Module *creator) : RegexProvider(creator, "regex/pcre") { }

	Regex *Compile(const Anope::string &expression) override
	{
		return new PCRERegex(expression);
	}
};

class ModuleRegexPCRE final
	: public Module
{
	PCRERegexProvider pcre_regex_provider;

public:
	ModuleRegexPCRE(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR),
		pcre_regex_provider(this)
	{
		this->SetPermanent(true);

		std::vector<char> pcre_version(16);
		if (pcre2_config(PCRE2_CONFIG_VERSION, pcre_version.data()) >= 0)
			Log(this) << "Module was compiled against PCRE2 version " << PCRE2_MAJOR << "." << PCRE2_MINOR << " and is running against version " << pcre_version.data();
	}

	~ModuleRegexPCRE()
	{
		for (auto *xlm : XLineManager::XLineManagers)
		{
			for (auto *x : xlm->GetList())
			{
				if (x->regex && dynamic_cast<PCRERegex *>(x->regex))
				{
					delete x->regex;
					x->regex = NULL;
				}
			}
		}
	}
};

MODULE_INIT(ModuleRegexPCRE)
