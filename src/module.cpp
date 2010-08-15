/* Modular support
 *
 * (C) 2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "modules.h"
#include "language.h"

Module::Module(const Anope::string &mname, const Anope::string &creator)
{
	this->name = mname; /* Our name */
	this->type = THIRD;
	this->handle = NULL;

	this->permanent = false;

	for (int i = 0; i < NUM_LANGS; ++i)
		this->lang[i].argc = 0;

	if (FindModule(this->name))
		throw CoreException("Module already exists!");

	this->created = time(NULL);

	this->SetVersion(Anope::Version());

	Modules.push_back(this);
}

Module::~Module()
{
	int i = 0;

	for (i = 0; i < NUM_LANGS; ++i)
		this->DeleteLanguage(i);

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

Service::Service(Module *o, const Anope::string &n) : owner(o), name(n)
{
	ModuleManager::RegisterService(this);
}

Service::~Service()
{
	ModuleManager::UnregisterService(this);
}

