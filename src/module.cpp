/* Modular support
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */


#include "services.h"
#include "modules.h"
#include "extern.h"
#include "dns.h"

#ifdef GETTEXT_FOUND
# include <libintl.h>
#endif

Module::Module(const Anope::string &modname, const Anope::string &, ModType modtype) : name(modname), type(modtype)
{
	this->handle = NULL;
	this->permanent = false;
	this->created = Anope::CurTime;
	this->SetVersion(Anope::Version());

	if (ModuleManager::FindModule(this->name))
		throw CoreException("Module already exists!");
	
	if (nothird && modtype == THIRD)
		throw ModuleException("Third party modules may not be loaded");

	Modules.push_back(this);

#if GETTEXT_FOUND
	for (unsigned i = 0; i < languages.size(); ++i)
		if (IsFile("languages/" + languages[i] + "/LC_MESSAGES/" + modname + ".mo"))
		{
			if (!bindtextdomain(this->name.c_str(), (services_dir + "/languages/").c_str()))
				Log() << "Error calling bindtextdomain, " << Anope::LastError();
			else
				domains.push_back(modname);
			break;
		}
#endif
}

Module::~Module()
{
	if (DNSEngine)
		DNSEngine->Cleanup(this);
	/* Detach all event hooks for this module */
	ModuleManager::DetachAll(this);
	/* Clear any active callbacks this module has */
	ModuleManager::ClearCallBacks(this);

	std::list<Module *>::iterator it = std::find(Modules.begin(), Modules.end(), this);
	if (it != Modules.end())
		Modules.erase(it);

#if GETTEXT_FOUND
	std::vector<Anope::string>::iterator dit = std::find(domains.begin(), domains.end(), this->name);
	if (dit != domains.end())
		domains.erase(dit);
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

ModuleVersion::ModuleVersion(int vMajor, int vMinor, int vBuild) : Major(vMajor), Minor(vMinor), Build(vBuild)
{
}

ModuleVersion::~ModuleVersion()
{
}

int ModuleVersion::GetMajor() const
{
	return this->Major;
}

int ModuleVersion::GetMinor() const
{
	return this->Minor;
}

int ModuleVersion::GetBuild() const
{
	return this->Build;
}

