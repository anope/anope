/* Modular support
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#include "services.h"
#include "modules.h"
#include "users.h"
#include "config.h"
#include "event.h"

#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <dirent.h>
#include <sys/types.h>
#include <dlfcn.h>
#endif

std::list<Module *> ModuleManager::Modules;

void ModuleDef::Depends(const Anope::string &modname)
{
	dependencies.push_back(modname);
}

const std::vector<Anope::string> &ModuleDef::GetDependencies()
{
	return dependencies;
}

#ifdef _WIN32
void ModuleManager::CleanupRuntimeDirectory()
{
	Anope::string dirbuf = Anope::DataDir + "/runtime";

	Log(LOG_DEBUG) << "Cleaning out Module run time directory (" << dirbuf << ") - this may take a moment please wait";

	DIR *dirp = opendir(dirbuf.c_str());
	if (!dirp)
	{
		Log(LOG_DEBUG) << "Cannot open directory (" << dirbuf << ")";
		return;
	}

	for (dirent *dp; (dp = readdir(dirp));)
	{
		if (!dp->d_ino)
			continue;
		if (Anope::string(dp->d_name).equals_cs(".") || Anope::string(dp->d_name).equals_cs(".."))
			continue;
		Anope::string filebuf = dirbuf + "/" + dp->d_name;
		unlink(filebuf.c_str());
	}

	closedir(dirp);
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
static ModuleReturn moduleCopyFile(const Anope::string &name, Anope::string &output)
{
	Anope::string input = Anope::ModuleDir + "/modules/" + name + ".so";

	struct stat s;
	if (stat(input.c_str(), &s) == -1)
		return MOD_ERR_NOEXIST;
	else if (!S_ISREG(s.st_mode))
		return MOD_ERR_NOEXIST;

	std::ifstream source(input.c_str(), std::ios_base::in | std::ios_base::binary);
	if (!source.is_open())
		return MOD_ERR_NOEXIST;

	char *tmp_output = strdup(output.c_str());
	int target_fd = mkstemp(tmp_output);
	if (target_fd == -1 || close(target_fd) == -1)
	{
		free(tmp_output);
		source.close();
		return MOD_ERR_FILE_IO;
	}
	output = tmp_output;
	free(tmp_output);

	Log(LOG_DEBUG_2) << "Runtime module location: " << output;

	std::ofstream target(output.c_str(), std::ios_base::in | std::ios_base::binary);
	if (!target.is_open())
	{
		source.close();
		return MOD_ERR_FILE_IO;
	}

	int want = s.st_size;
	char buffer[1024];
	while (want > 0 && !source.fail() && !target.fail())
	{
		source.read(buffer, std::min(want, static_cast<int>(sizeof(buffer))));
		int read_len = source.gcount();

		target.write(buffer, read_len);
		want -= read_len;
	}

	source.close();
	target.close();

	return !source.fail() && !target.fail() ? MOD_ERR_OK : MOD_ERR_FILE_IO;
}
#endif

ModuleReturn ModuleManager::LoadModule(const Anope::string &modname, User *u)
{
	if (modname.empty())
		return MOD_ERR_PARAMS;

	if (FindModule(modname))
		return MOD_ERR_EXISTS;

	Log(LOG_DEBUG) << "Trying to load module: " << modname;

#ifdef _WIN32
	/* Generate the filename for the temporary copy of the module */
	Anope::string pbuf = Anope::DataDir + "/runtime/" + modname + ".so.XXXXXX";

	/* Don't skip return value checking! -GD */
	ModuleReturn ret = moduleCopyFile(modname, pbuf);
	if (ret != MOD_ERR_OK)
	{
		if (ret == MOD_ERR_NOEXIST)
			Log(LOG_TERMINAL) << "Error while loading " << modname << " (file does not exist)";
		else if (ret == MOD_ERR_FILE_IO)
			Log(LOG_TERMINAL) << "Error while loading " << modname << " (file IO error, check file permissions and diskspace)";
		return ret;
	}
#else
	Anope::string pbuf = Anope::ModuleDir + "/modules/" + modname + ".so";
#endif

	dlerror();
	void *handle = dlopen(pbuf.c_str(), RTLD_NOW);
	const char *err = dlerror();
	if (!handle)
	{
		if (err && *err)
			Log() << err;
		return MOD_ERR_NOLOAD;
	}

	dlerror();
	AnopeModule *module = static_cast<AnopeModule *>(dlsym(handle, "AnopeMod"));
	err = dlerror();
	if (!module || !module->init || !module->fini)
	{
		Log() << "No module symbols function found, not an Anope module";
		if (err && *err)
			Log(LOG_DEBUG) << err;
		dlclose(handle);
		return MOD_ERR_NOLOAD;
	}

	ModuleDef *def = module->init();
	
	/* Create module. */
	Anope::string nick;
	if (u)
		nick = u->nick;

	Module *m;

	ModuleReturn moderr = MOD_ERR_OK;
	try
	{
		m = def->Create(modname, nick);
	}
	catch (const ModuleException &ex)
	{
		Log() << "Error while loading " << modname << ": " << ex.GetReason();
		moderr = MOD_ERR_EXCEPTION;
	}
	
	if (moderr != MOD_ERR_OK)
	{
		if (dlclose(handle))
			Log() << dlerror();
		return moderr;
	}

	m->filename = pbuf;
	m->handle = handle;
	m->def = def;
	m->module = module;

	ModuleVersion v = m->GetVersion();
	if (v.GetMajor() < Anope::VersionMajor() || (v.GetMajor() == Anope::VersionMajor() && v.GetMinor() < Anope::VersionMinor()))
	{
		Log() << "Module " << modname << " is compiled against an older version of Anope " << v.GetMajor() << "." << v.GetMinor() << ", this is " << Anope::VersionShort();
		DeleteModule(m);
		return MOD_ERR_VERSION;
	}
	else if (v.GetMajor() > Anope::VersionMajor() || (v.GetMajor() == Anope::VersionMajor() && v.GetMinor() > Anope::VersionMinor()))
	{
		Log() << "Module " << modname << " is compiled against a newer version of Anope " << v.GetMajor() << "." << v.GetMinor() << ", this is " << Anope::VersionShort();
		DeleteModule(m);
		return MOD_ERR_VERSION;
	}
	else if (v.GetPatch() < Anope::VersionPatch())
	{
		Log() << "Module " << modname << " is compiled against an older version of Anope, " << v.GetMajor() << "." << v.GetMinor() << "." << v.GetPatch() << ", this is " << Anope::VersionShort();
		DeleteModule(m);
		return MOD_ERR_VERSION;
	}
	else if (v.GetPatch() > Anope::VersionPatch())
	{
		Log() << "Module " << modname << " is compiled against a newer version of Anope, " << v.GetMajor() << "." << v.GetMinor() << "." << v.GetPatch() << ", this is " << Anope::VersionShort();
		DeleteModule(m);
		return MOD_ERR_VERSION;
	}
	else
		Log(LOG_DEBUG_2) << "Module " << modname << " is compiled against current version of Anope " << Anope::VersionShort();

	/* Initialize config */
	try
	{
		m->OnReload(Config);
	}
	catch (const ModuleException &ex)
	{
		Log() << "Module " << modname << " couldn't load:" << ex.GetReason();
		moderr = MOD_ERR_EXCEPTION;
	}
	catch (const ConfigException &ex)
	{
		Log() << "Module " << modname << " couldn't load due to configuration problems: " << ex.GetReason();
		moderr = MOD_ERR_EXCEPTION;
	}
	
	if (moderr != MOD_ERR_OK)
	{
		DeleteModule(m);
		return moderr;
	}

	Log(LOG_DEBUG) << "Module " << modname << " loaded.";

	Event::OnModuleLoad(&Event::ModuleLoad::OnModuleLoad, u, m);

	return MOD_ERR_OK;
}

ModuleReturn ModuleManager::UnloadModule(Module *m, User *u)
{
	if (!m)
		return MOD_ERR_PARAMS;

	Event::OnModuleUnload(&Event::ModuleUnload::OnModuleUnload, u, m);

	return DeleteModule(m);
}

Module *ModuleManager::FindModule(const Anope::string &name)
{
	for (std::list<Module *>::const_iterator it = Modules.begin(), it_end = Modules.end(); it != it_end; ++it)
	{
		Module *m = *it;

		if (m->name.equals_ci(name))
			return m;
	}

	return NULL;
}

Module *ModuleManager::FindFirstOf(ModType type)
{
	for (std::list<Module *>::const_iterator it = Modules.begin(), it_end = Modules.end(); it != it_end; ++it)
	{
		Module *m = *it;

		if (m->type & type)
			return m;
	}

	return NULL;
}

void ModuleManager::RequireVersion(int major, int minor, int patch)
{
	if (Anope::VersionMajor() > major)
		return;
	else if (Anope::VersionMajor() == major)
	{
		if (minor == -1)
			return;
		else if (Anope::VersionMinor() > minor)
			return;
		else if (Anope::VersionMinor() == minor)
		{
			if (patch == -1)
				return;
			else if (Anope::VersionPatch() > patch)
				return;
			else if (Anope::VersionPatch() == patch)
				return;
		}
	}

	throw ModuleException("This module requires version " + stringify(major) + "." + stringify(minor) + "." + stringify(patch) + " - this is " + Anope::VersionShort());
}

ModuleReturn ModuleManager::DeleteModule(Module *m)
{
	if (!m || !m->handle)
		return MOD_ERR_PARAMS;

	Serialize::Unregister(m);

	void *handle = m->handle;
	Anope::string filename = m->filename;

	Log(LOG_DEBUG) << "Unloading module " << m->name;

	ModuleDef *def = m->def;
	AnopeModule *module = m->module;

	def->Destroy(m);
	module->fini(def);

	dlerror();
	if (dlclose(handle))
		Log() << dlerror();

#ifdef _WIN32
	if (!filename.empty())
		unlink(filename.c_str());
#endif

	return MOD_ERR_OK;
}

void ModuleManager::UnloadAll()
{
	std::vector<Anope::string> modules;
	for (size_t i = 1, j = 0; i != MT_END; j |= i, i <<= 1)
		for (std::list<Module *>::iterator it = Modules.begin(), it_end = Modules.end(); it != it_end; ++it)
		{
			Module *m = *it;
			if ((m->type & j) == m->type)
				modules.push_back(m->name);
		}

	for (unsigned i = 0; i < modules.size(); ++i)
	{
		Module *m = FindModule(modules[i]);
		if (m != NULL)
			UnloadModule(m, NULL);
	}
}

