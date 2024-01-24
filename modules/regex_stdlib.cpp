/*
 *
 * (C) 2012-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

#include <regex>

class StdLibRegex final
	: public Regex
{
private:
	std::regex regex;

public:
	StdLibRegex(const Anope::string &expr, std::regex::flag_type type)
		: Regex(expr)
	{
		try
		{
			regex.assign(expr.str(), type | std::regex::optimize);
		}
		catch (const std::regex_error& error)
		{
			throw RegexException("Error in regex " + expr + ": " + error.what());
		}
	}

	bool Matches(const Anope::string &str)
	{
		return std::regex_search(str.str(), regex);
	}
};

class StdLibRegexProvider final
	: public RegexProvider
{
public:
	std::regex_constants::syntax_option_type type;

	StdLibRegexProvider(Module *creator)
		: RegexProvider(creator, "regex/stdlib")
	{
	}

	Regex *Compile(const Anope::string &expression) override
	{
		return new StdLibRegex(expression, type);
	}
};

class ModuleRegexStdLib final
	: public Module
{
private:
	StdLibRegexProvider stdlib_regex_provider;

public:
	ModuleRegexStdLib(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, EXTRA | VENDOR)
		, stdlib_regex_provider(this)
	{
		this->SetPermanent(true);
	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *block = conf->GetModule(this);

		const Anope::string syntax = block->Get<const Anope::string>("syntax", "ecmascript");
		if (syntax == "awk")
			stdlib_regex_provider.type = std::regex::awk;
		else if (syntax == "basic")
			stdlib_regex_provider.type = std::regex::basic;
		else if (syntax == "ecmascript")
			stdlib_regex_provider.type = std::regex::ECMAScript;
		else if (syntax == "egrep")
			stdlib_regex_provider.type = std::regex::egrep;
		else if (syntax == "extended")
			stdlib_regex_provider.type = std::regex::extended;
		else if (syntax == "grep")
			stdlib_regex_provider.type = std::regex::grep;
		else
			throw ConfigException(this->name + ": syntax must be set to awk, basic, ecmascript, egrep, extended, or grep.");
	}

	~ModuleRegexStdLib()
	{
		for (auto *xlm : XLineManager::XLineManagers)
		{
			for (auto *x : xlm->GetList())
			{
				if (x->regex && dynamic_cast<StdLibRegex *>(x->regex))
				{
					delete x->regex;
					x->regex = NULL;
				}
			}
		}
	}
};

MODULE_INIT(ModuleRegexStdLib)
