/* Modular support
 *
 * (C) 2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "modules.h"
#include "language.h"
#include "version.h"
#include <algorithm> // std::find

std::map<Anope::string, Service *> ModuleManager::ServiceProviders;
std::vector<Module *> ModuleManager::EventHandlers[I_END];

void ModuleManager::LoadModuleList(std::list<Anope::string> &ModuleList)
{
	for (std::list<Anope::string>::iterator it = ModuleList.begin(), it_end = ModuleList.end(); it != it_end; ++it)
		ModuleManager::LoadModule(*it, NULL);
}

/**
 * Copy the module from the modules folder to the runtime folder.
 * This will prevent module updates while the modules is loaded from
 * triggering a segfault, as the actaul file in use will be in the
 * runtime folder.
 * @param name the name of the module to copy
 * @param output the destination to copy the module to
 * @return MOD_ERR_OK on success
 */
static int moduleCopyFile(const Anope::string &name, Anope::string &output)
{
	Anope::string input = services_dir + "/modules/" + name + ".so";
	FILE *source = fopen(input.c_str(), "rb");
	if (!source)
		return MOD_ERR_NOEXIST;

	char *tmp_output = strdup(output.c_str());
#ifndef _WIN32
	int srcfp = mkstemp(tmp_output);
	if (srcfp == -1)
#else
	if (!mktemp(tmp_output))
#endif
	{
		free(tmp_output);
		fclose(source);
		return MOD_ERR_FILE_IO;
	}
	output = tmp_output;
	free(tmp_output); // XXX

	Log(LOG_DEBUG) << "Runtime module location: " << output;

	FILE *target;
#ifndef _WIN32
	target = fdopen(srcfp, "w");
#else
	target = fopen(output.c_str(), "wb");
#endif
	if (!target)
	{
		fclose(source);
		return MOD_ERR_FILE_IO;
	}

	int ch;
	while ((ch = fgetc(source)) != EOF)
		fputc(ch, target);
	fclose(source);
	if (fclose(target))
		return MOD_ERR_FILE_IO;
	return MOD_ERR_OK;
}

static bool IsOneOfModuleTypeLoaded(MODType mt)
{
	int pmods = 0;

	for (std::list<Module *>::const_iterator it = Modules.begin(), it_end = Modules.end(); it != it_end; ++it)
		if ((*it)->type == mt)
			++pmods;

	/*
	 * 2, because module constructors now add modules to the hash.. so 1 (original module)
	 * and 2 (this module). -- w00t
	 */
	if (pmods == 2)
		return true;

	return false;
}

/* This code was found online at http://www.linuxjournal.com/article/3687#comment-26593
 *
 * This function will take a pointer from either dlsym or GetProcAddress and cast it in
 * a way that won't cause C++ warnings/errors to come up.
 */
template <class TYPE> TYPE function_cast(ano_module_t symbol)
{
	union
	{
		ano_module_t symbol;
		TYPE function;
	} cast;
	cast.symbol = symbol;
	return cast.function;
}

int ModuleManager::LoadModule(const Anope::string &modname, User *u)
{
	if (modname.empty())
		return MOD_ERR_PARAMS;

	if (FindModule(modname))
		return MOD_ERR_EXISTS;

	Log(LOG_DEBUG) << "trying to load [" << modname <<  "]";

	/* Generate the filename for the temporary copy of the module */
	Anope::string pbuf = services_dir + "/modules/runtime/" + modname + ".so.XXXXXX";

	/* Don't skip return value checking! -GD */
	int ret = moduleCopyFile(modname, pbuf);
	if (ret != MOD_ERR_OK)
	{
		/* XXX: This used to assign filename here, but I don't think that was correct..
		 * even if it was, it makes life very fucking difficult, so.
		 */
		return ret;
	}

	ano_modclearerr();

	ano_module_t handle = dlopen(pbuf.c_str(), RTLD_LAZY);
	const char *err = ano_moderr();
	if (!handle && err && *err)
	{
		Log() << err;
		return MOD_ERR_NOLOAD;
	}

	ano_modclearerr();
	Module *(*func)(const Anope::string &, const Anope::string &) = function_cast<Module *(*)(const Anope::string &, const Anope::string &)>(dlsym(handle, "AnopeInit"));
	err = ano_moderr();
	if (!func && err && *err)
	{
		Log() << "No init function found, not an Anope module";
		dlclose(handle);
		return MOD_ERR_NOLOAD;
	}

	if (!func)
		throw CoreException("Couldn't find constructor, yet moderror wasn't set?");

	/* Create module. */
	Anope::string nick;
	if (u)
		nick = u->nick;

	Module *m;

	try
	{
		m = func(modname, nick);
	}
	catch (const ModuleException &ex)
	{
		Log() << "Error while loading " << modname << ": " << ex.GetReason();
		return MOD_STOP;
	}

	m->filename = pbuf;
	m->handle = handle;

	Version v = m->GetVersion();
	if (v.GetMajor() < VERSION_MAJOR || (v.GetMajor() == VERSION_MAJOR && v.GetMinor() < VERSION_MINOR))
	{
		Log() << "Module " << modname << " is compiled against an older version of Anope " << v.GetMajor() << "." << v.GetMinor() << ", this is " << VERSION_MAJOR << "." << VERSION_MINOR;
		DeleteModule(m);
		return MOD_STOP;
	}
	else if (v.GetMajor() > VERSION_MAJOR || (v.GetMajor() == VERSION_MAJOR && v.GetMinor() > VERSION_MINOR))
	{
		Log() << "Module " << modname << " is compiled against a newer version of Anope " << v.GetMajor() << "." << v.GetMinor() << ", this is " << VERSION_MAJOR << "." << VERSION_MINOR;
		DeleteModule(m);
		return MOD_STOP;
	}
	else if (v.GetBuild() < VERSION_BUILD)
		Log() << "Module " << modname << " is compiled against an older revision of Anope " << v.GetBuild() << ", this is " << VERSION_BUILD;
	else if (v.GetBuild() > VERSION_BUILD)
		Log() << "Module " << modname << " is compiled against a newer revision of Anope " << v.GetBuild() << ", this is " << VERSION_BUILD;
	else if (v.GetBuild() == VERSION_BUILD)
		Log(LOG_DEBUG) << "Module " << modname << " compiled against current version of Anope " << v.GetBuild();

	if (m->type == PROTOCOL && IsOneOfModuleTypeLoaded(PROTOCOL))
	{
		DeleteModule(m);
		Log() << "You cannot load two protocol modules";
		return MOD_STOP;
	}

	if (u)
	{
		ircdproto->SendGlobops(OperServ, "%s loaded module %s", u->nick.c_str(), modname.c_str());
		notice_lang(Config->s_OperServ, u, OPER_MODULE_LOADED, modname.c_str());

		/* If a user is loading this module, then the core databases have already been loaded
		 * so trigger the event manually
		 */
		m->OnPostLoadDatabases();
	}

	FOREACH_MOD(I_OnModuleLoad, OnModuleLoad(u, m));

	return MOD_ERR_OK;
}

int ModuleManager::UnloadModule(Module *m, User *u)
{
	if (!m || !m->handle)
	{
		if (u)
			notice_lang(Config->s_OperServ, u, OPER_MODULE_REMOVE_FAIL, m->name.c_str());
		return MOD_ERR_PARAMS;
	}

	if (m->GetPermanent() || m->type == PROTOCOL || m->type == ENCRYPTION || m->type == DATABASE)
	{
		if (u)
			notice_lang(Config->s_OperServ, u, OPER_MODULE_NO_UNLOAD);
		return MOD_ERR_NOUNLOAD;
	}

	if (u)
	{
		ircdproto->SendGlobops(OperServ, "%s unloaded module %s", u->nick.c_str(), m->name.c_str());
		notice_lang(Config->s_OperServ, u, OPER_MODULE_UNLOADED, m->name.c_str());
	}

	FOREACH_MOD(I_OnModuleUnload, OnModuleUnload(u, m));

	if (DNSEngine)
		DNSEngine->Cleanup(m);

	DeleteModule(m);
	return MOD_ERR_OK;
}

void ModuleManager::DeleteModule(Module *m)
{
	if (!m || !m->handle)
		return;

	ano_module_t handle = m->handle;
	Anope::string filename = m->filename;

	if (handle && dlclose(handle))
		Log() << ano_moderr();

	if (!filename.empty())
		DeleteFile(filename.c_str());
}

bool ModuleManager::Attach(Implementation i, Module *mod)
{
	if (std::find(EventHandlers[i].begin(), EventHandlers[i].end(), mod) != EventHandlers[i].end())
		return false;

	EventHandlers[i].push_back(mod);
	return true;
}

bool ModuleManager::Detach(Implementation i, Module *mod)
{
	std::vector<Module *>::iterator x = std::find(EventHandlers[i].begin(), EventHandlers[i].end(), mod);

	if (x == EventHandlers[i].end())
		return false;

	EventHandlers[i].erase(x);
	return true;
}

void ModuleManager::Attach(Implementation *i, Module *mod, size_t sz)
{
	for (size_t n = 0; n < sz; ++n)
		Attach(i[n], mod);
}

void ModuleManager::DetachAll(Module *mod)
{
	for (size_t n = I_BEGIN + 1; n != I_END; ++n)
		Detach(static_cast<Implementation>(n), mod);
}

bool ModuleManager::SetPriority(Module *mod, Priority s)
{
	for (size_t n = I_BEGIN + 1; n != I_END; ++n)
		SetPriority(mod, static_cast<Implementation>(n), s);

	return true;
}

bool ModuleManager::SetPriority(Module *mod, Implementation i, Priority s, Module **modules, size_t sz)
{
	/** To change the priority of a module, we first find its position in the vector,
	 * then we find the position of the other modules in the vector that this module
	 * wants to be before/after. We pick off either the first or last of these depending
	 * on which they want, and we make sure our module is *at least* before or after
	 * the first or last of this subset, depending again on the type of priority.
	 */

	/* Locate our module. This is O(n) but it only occurs on module load so we're
	 * not too bothered about it
	 */
	size_t source = 0;
	bool found = false;
	for (size_t x = 0, end = EventHandlers[i].size(); x != end; ++x)
		if (EventHandlers[i][x] == mod)
		{
			source = x;
			found = true;
			break;
		}

	/* Eh? this module doesnt exist, probably trying to set priority on an event
	 * theyre not attached to.
	 */
	if (!found)
		return false;

	size_t swap_pos = 0;
	bool swap = true;
	switch (s)
	{
		/* Dummy value */
		case PRIORITY_DONTCARE:
			swap = false;
			break;
		/* Module wants to be first, sod everything else */
		case PRIORITY_FIRST:
			swap_pos = 0;
			break;
		/* Module is submissive and wants to be last... awww. */
		case PRIORITY_LAST:
			if (EventHandlers[i].empty())
				swap_pos = 0;
			else
				swap_pos = EventHandlers[i].size() - 1;
			break;
		/* Place this module after a set of other modules */
		case PRIORITY_AFTER:
			/* Find the latest possible position */
			swap_pos = 0;
			swap = false;
			for (size_t x = 0, end = EventHandlers[i].size(); x != end; ++x)
				for (size_t n = 0; n < sz; ++n)
					if (modules[n] && EventHandlers[i][x] == modules[n] && x >= swap_pos && source <= swap_pos)
					{
						swap_pos = x;
						swap = true;
					}
			break;
		/* Place this module before a set of other modules */
		case PRIORITY_BEFORE:
			swap_pos = EventHandlers[i].size() - 1;
			swap = false;
			for (size_t x = 0, end = EventHandlers[i].size(); x != end; ++x)
				for (size_t n = 0; n < sz; ++n)
					if (modules[n] && EventHandlers[i][x] == modules[n] && x <= swap_pos && source >= swap_pos)
					{
						swap = true;
						swap_pos = x;
					}
	}

	/* Do we need to swap? */
	if (swap && swap_pos != source)
	{
		/* Suggestion from Phoenix, "shuffle" the modules to better retain call order */
		int incrmnt = 1;

		if (source > swap_pos)
			incrmnt = -1;

		for (unsigned j = source; j != swap_pos; j += incrmnt)
		{
			if (j + incrmnt > EventHandlers[i].size() - 1 || j + incrmnt < 0)
				continue;

			std::swap(EventHandlers[i][j], EventHandlers[i][j + incrmnt]);
		}
	}

	return true;
}

/** Delete all callbacks attached to a module
 * @param m The module
 */
void ModuleManager::ClearCallBacks(Module *m)
{
	while (!m->CallBacks.empty())
		delete m->CallBacks.front();
}

/** Unloading all modules, NEVER call this when Anope isn't shutting down.
 * Ever.
 */
void ModuleManager::UnloadAll()
{
	for (size_t i = MT_BEGIN + 1; i != MT_END; ++i)
	{
		for (std::list<Module *>::iterator it = Modules.begin(), it_end = Modules.end(); it != it_end; )
		{
			Module *m = *it++;

			if (static_cast<MODType>(i) == m->type)
				DeleteModule(m);
		}
	}
}

/** Register a service
 * @oaram s The service
 * @return true if it was successfully registeed, else false (service name colision)
 */
bool ModuleManager::RegisterService(Service *s)
{
	return ModuleManager::ServiceProviders.insert(std::make_pair(s->name, s)).second;
}

/** Unregister a service
 * @param s The service
 * @return true if it was unregistered successfully
 */
bool ModuleManager::UnregisterService(Service *s)
{
	return ModuleManager::ServiceProviders.erase(s->name);
}

/** Get a service
 * @param name The service name
 * @param s The service
 * @return The service
 */
Service *ModuleManager::GetService(const Anope::string &name)
{
	std::map<Anope::string, Service *>::const_iterator it = ModuleManager::ServiceProviders.find(name);

	if (it != ModuleManager::ServiceProviders.end())
		return it->second;
	return NULL;
}

