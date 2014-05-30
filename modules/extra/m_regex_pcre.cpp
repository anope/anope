/* RequiredLibraries: pcre */
/* RequiredWindowsLibraries: libpcre */

#include "module.h"
#include <pcre.h>

class PCRERegex : public Regex
{
	pcre *regex;

 public:
	PCRERegex(const Anope::string &expr) : Regex(expr)
	{
		const char *error;
		int erroffset;
		this->regex = pcre_compile(expr.c_str(), PCRE_CASELESS, &error, &erroffset, NULL);
		if (!this->regex)
			throw RegexException("Error in regex " + expr + " at offset " + stringify(erroffset) + ": " + error); 
	}

	~PCRERegex()
	{
		pcre_free(this->regex);
	}

	bool Matches(const Anope::string &str)
	{
		return pcre_exec(this->regex, NULL, str.c_str(), str.length(), 0, 0, NULL, 0) > -1;
	}
};

class PCRERegexProvider : public RegexProvider
{
 public:
	PCRERegexProvider(Module *creator) : RegexProvider(creator, "regex/pcre") { }

	Regex *Compile(const Anope::string &expression) anope_override
	{
		return new PCRERegex(expression);
	}
};

class ModuleRegexPCRE : public Module
{
	PCRERegexProvider pcre_regex_provider;

 public:
	ModuleRegexPCRE(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR),
		pcre_regex_provider(this)
	{
		this->SetPermanent(true);
	}
};

MODULE_INIT(ModuleRegexPCRE)
