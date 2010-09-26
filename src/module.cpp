/* Modular support
 *
 * (C) 2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "modules.h"

#ifdef GETTEXT_FOUND
# include <libintl.h>
#endif

Module::Module(const Anope::string &mname, const Anope::string &creator)
{
	this->name = mname; /* Our name */
	this->type = THIRD;
	this->handle = NULL;

	this->permanent = false;

	if (FindModule(this->name))
		throw CoreException("Module already exists!");

	this->created = Anope::CurTime;

	this->SetVersion(Anope::Version());

	Modules.push_back(this);

#if GETTEXT_FOUND
	if (!bindtextdomain(this->name.c_str(), (services_dir + "/languages/").c_str()))
		Log() << "Error calling bindtextdomain, " << Anope::LastError();
#endif
}

Module::~Module()
{
	/* Detach all event hooks for this module */
	ModuleManager::DetachAll(this);
	/* Clear any active callbacks this module has */
	ModuleManager::ClearCallBacks(this);

	std::list<Module *>::iterator it = std::find(Modules.begin(), Modules.end(), this);
	if (it != Modules.end())
		Modules.erase(it);
}

void Module::SetType(MODType ntype)
{
	this->type = ntype;
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

Version::Version(unsigned vMajor, unsigned vMinor, unsigned vBuild) : Major(vMajor), Minor(vMinor), Build(vBuild)
{
}

Version::~Version()
{
}

unsigned Version::GetMajor() const
{
	return this->Major;
}

unsigned Version::GetMinor() const
{
	return this->Minor;
}

unsigned Version::GetBuild() const
{
	return this->Build;
}

