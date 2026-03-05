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

#include "services.h"
#include "modules.h"
#include "language.h"
#include "account.h"
#include "textproc.h"

#if HAVE_LOCALIZATION
# include <libintl.h>
#endif

Module::Module(const Anope::string &modname, const Anope::string &, ModType modtype) : name(modname), type(modtype)
{
	this->handle = NULL;
	this->permanent = false;
	this->created = Anope::CurTime;
	this->SetVersion(Anope::Version());

	if (type & VENDOR)
		this->SetAuthor("Anope");
	else
	{
		/* Not vendor implies third */
		type |= THIRD;
		this->SetAuthor("Unknown");
	}

	if (ModuleManager::FindModule(this->name))
		throw CoreException("Module already exists!");

	if (Anope::NoDB && type & DATABASE)
		throw ModuleException("Database modules may not be loaded");

	if (Anope::NoThird && type & THIRD)
		throw ModuleException("Third party modules may not be loaded");

	ModuleManager::Modules.push_back(this);

#if HAVE_LOCALIZATION
	for (const auto &language : Language::Languages)
	{
		/* Remove .UTF-8 or any other suffix */
		Anope::string lang;
		sepstream(language, '.').GetToken(lang);

		if (Anope::IsFile(Anope::ExpandLocale(lang + "/LC_MESSAGES/" + modname + ".mo")))
		{
			if (!bindtextdomain(this->name.c_str(), Anope::LocaleDir.c_str()))
				Log() << "Error calling bindtextdomain, " << Anope::LastError();
			else
			{
				Log() << "Found language file " << lang << " for " << modname;
				Language::Domains.push_back(modname);
			}
			break;
		}
	}
#endif
}

Module::~Module()
{
	UnsetExtensibles();

	/* Detach all event hooks for this module */
	ModuleManager::DetachAll(this);
	IdentifyRequest::ModuleUnload(this);
	/* Clear any active timers this module has */
	TimerManager::DeleteTimersFor(this);

	auto it = std::find(ModuleManager::Modules.begin(), ModuleManager::Modules.end(), this);
	if (it != ModuleManager::Modules.end())
		ModuleManager::Modules.erase(it);

#if HAVE_LOCALIZATION
	auto dit = std::find(Language::Domains.begin(), Language::Domains.end(), this->name);
	if (dit != Language::Domains.end())
		Language::Domains.erase(dit);
#endif
}

void Module::SetPermanent(bool state)
{
	this->permanent = state;
}

bool Module::GetPermanent() const
{
	return this->permanent;
}

void Module::SetVersion(const Anope::string &nversion)
{
	this->version = nversion;
}

void Module::SetAuthor(const Anope::string &nauthor)
{
	this->author = nauthor;
}

void Module::Prioritize()
{
}

ModuleVersion::ModuleVersion(const ModuleVersionC &ver)
	: version_major(ver.version_major)
	, version_minor(ver.version_minor)
	, version_patch(ver.version_patch)
{
}

unsigned ModuleVersion::GetMajor() const
{
	return this->version_major;
}

unsigned ModuleVersion::GetMinor() const
{
	return this->version_minor;
}

unsigned ModuleVersion::GetPatch() const
{
	return this->version_patch;
}
