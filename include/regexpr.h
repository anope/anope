/*
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

#include "services.h"
#include "anope.h"
#include "service.h"

class CoreExport RegexException final
	: public CoreException
{
public:
	RegexException(const Anope::string &reason = "") : CoreException(reason) { }

	virtual ~RegexException() noexcept = default;
};

class CoreExport Regex
{
	Anope::string expression;
protected:
	Regex(const Anope::string &expr) : expression(expr) { }
public:
	virtual ~Regex() = default;
	const Anope::string &GetExpression() { return expression; }
	virtual bool Matches(const Anope::string &str) = 0;
};

class CoreExport RegexProvider
	: public Service
{
public:
	RegexProvider(Module *o, const Anope::string &n) : Service(o, "Regex", n) { }
	virtual Regex *Compile(const Anope::string &) = 0;
};
