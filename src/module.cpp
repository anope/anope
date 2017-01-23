/*
 * Anope IRC Services
 *
 * Copyright (C) 2008-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "services.h"
#include "modules.h"
#include "language.h"

#ifdef GETTEXT_FOUND
# include <libintl.h>
#endif

Module::Module(const Anope::string &modname, const Anope::string &, ModType modtype)
	: name(modname)
	, type(modtype)
	, logger(this)
{
	this->handle = NULL;
	this->permanent = false;
	this->created = Anope::CurTime;
	this->SetVersion(Anope::Version());

	if (type & VENDOR)
	{
		this->SetAuthor("Anope");
	}
	else
	{
		/* Not vendor implies third */
		type |= THIRD;
		this->SetAuthor("Unknown");
	}

	if (ModuleManager::FindModule(this->name))
		throw CoreException("Module already exists!");

	if (Anope::NoThird && type & THIRD)
		throw ModuleException("Third party modules may not be loaded");

	ModuleManager::Modules.push_back(this);

#if GETTEXT_FOUND
	for (unsigned int i = 0; i < Language::Languages.size(); ++i)
	{
		/* Remove .UTF-8 or any other suffix */
		Anope::string lang;
		sepstream(Language::Languages[i], '.').GetToken(lang);

		if (Anope::IsFile(Anope::LocaleDir + "/" + lang + "/LC_MESSAGES/" + modname + ".mo"))
		{
			if (!bindtextdomain(this->name.c_str(), Anope::LocaleDir.c_str()))
			{
				Anope::Logger.Log("Error calling bindtextdomain, {0}", Anope::LastError());
			}
			else
			{
				Anope::Logger.Log("Found language file {0} for {1}", lang, modname);
				Language::Domains.push_back(modname);
			}
			break;
		}
	}
#endif
}

Module::~Module()
{
	/* Clear any active timers this module has */
	TimerManager::DeleteTimersFor(this);

	std::list<Module *>::iterator it = std::find(ModuleManager::Modules.begin(), ModuleManager::Modules.end(), this);
	if (it != ModuleManager::Modules.end())
		ModuleManager::Modules.erase(it);

#if GETTEXT_FOUND
	std::vector<Anope::string>::iterator dit = std::find(Language::Domains.begin(), Language::Domains.end(), this->name);
	if (dit != Language::Domains.end())
		Language::Domains.erase(dit);
#endif
}

const Anope::string &Module::GetName() const
{
	return this->name;
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

ModuleVersion::ModuleVersion(const ModuleVersionC &ver)
{
	version_major = ver.version_major;
	version_minor = ver.version_minor;
	version_patch = ver.version_patch;
}

int ModuleVersion::GetMajor() const
{
	return this->version_major;
}

int ModuleVersion::GetMinor() const
{
	return this->version_minor;
}

int ModuleVersion::GetPatch() const
{
	return this->version_patch;
}

