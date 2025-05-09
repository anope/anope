/*
 *
 * (C) 2011-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

struct MyOper final
	: Oper
	, Serializable
{
	MyOper(const Anope::string &n, OperType *o) : Oper(n, o), Serializable("Oper") { }
};
