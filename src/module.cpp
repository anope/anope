/* Modular support
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#include "services.h"
#include "modules.h"
#include "dns.h"
#include "language.h"

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
	
	if (Anope::NoThird && modtype == THIRD)
		throw ModuleException("Third party modules may not be loaded");

	ModuleManager::Modules.push_back(this);

#if GETTEXT_FOUND
	for (unsigned i = 0; i < Language::Languages.size(); ++i)
		if (Anope::IsFile(Anope::LocaleDir + "/" + Language::Languages[i] + "/LC_MESSAGES/" + modname + ".mo"))
		{
			if (!bindtextdomain(this->name.c_str(), Anope::LocaleDir.c_str()))
				Log() << "Error calling bindtextdomain, " << Anope::LastError();
			else
				Language::Domains.push_back(modname);
			break;
		}
#endif
}

Module::~Module()
{
	if (DNS::Engine)
		DNS::Engine->Cleanup(this);
	/* Detach all event hooks for this module */
	ModuleManager::DetachAll(this);
	/* Clear any active callbacks this module has */
	ModuleManager::ClearCallBacks(this);
	IdentifyRequest::ModuleUnload(this);

	std::list<Module *>::iterator it = std::find(ModuleManager::Modules.begin(), ModuleManager::Modules.end(), this);
	if (it != ModuleManager::Modules.end())
		ModuleManager::Modules.erase(it);

#if GETTEXT_FOUND
	std::vector<Anope::string>::iterator dit = std::find(Language::Domains.begin(), Language::Domains.end(), this->name);
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

IRCDProto *Module::GetIRCDProto()
{
	return NULL;
}

ModuleVersion::ModuleVersion(int maj, int min, int pa) : version_major(maj), version_minor(min), version_patch(pa)
{
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

CallBack::CallBack(Module *mod, long time_from_now, time_t now, bool repeating) : Timer(time_from_now, now, repeating),  m(mod)
{
}

CallBack::~CallBack()
{
	std::list<CallBack *>::iterator it = std::find(m->callbacks.begin(), m->callbacks.end(), this);
	if (it != m->callbacks.end())
		m->callbacks.erase(it);
}

