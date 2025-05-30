/* OperServ ignore interface
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

struct IgnoreData
{
	Anope::string mask;
	Anope::string creator;
	Anope::string reason;
	time_t time = 0; /* When do we stop ignoring them? */

	virtual ~IgnoreData() = default;
protected:
	IgnoreData() = default;
};

class IgnoreService
	: public Service
{
protected:
	IgnoreService(Module *c) : Service(c, "IgnoreService", "ignore") { }

public:
	virtual void AddIgnore(IgnoreData *) = 0;

	virtual void DelIgnore(IgnoreData *) = 0;

	virtual void ClearIgnores() = 0;

	virtual IgnoreData *Create() = 0;

	virtual IgnoreData *Find(const Anope::string &mask) = 0;

	virtual std::vector<IgnoreData *> &GetIgnores() = 0;
};

static ServiceReference<IgnoreService> ignore_service("IgnoreService", "ignore");
