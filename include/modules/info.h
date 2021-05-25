/*
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

class OperInfo
{
protected:
	OperInfo() = default;

	OperInfo(const Anope::string &t, const Anope::string &i, const Anope::string &a, time_t c)
		: target(t)
		, info(i)
		, adder(a)
		, created(c)
	{
	}

public:
	Anope::string target;
	Anope::string info;
	Anope::string adder;
	time_t created = 0;

	virtual ~OperInfo() = default;
};

class OperInfoList
	: public Serialize::Checker<std::vector<OperInfo *>>
{
public:
	OperInfoList()
		: Serialize::Checker<std::vector<OperInfo *>>("OperInfo")
	{
	}

	virtual ~OperInfoList()
	{
		for (auto *info : *(*this))
			delete info;
	}

	virtual OperInfo *Create() = 0;
};
