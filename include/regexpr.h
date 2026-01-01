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
