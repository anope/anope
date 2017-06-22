/*
 * Anope IRC Services
 *
 * Copyright (C) 2008-2017 Anope Team <team@anope.org>
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

	Anope::Logger.Debug("Cleaning out Module run time directory ({0}) - this may take a moment please wait", dirbuf);

	DIR *dirp = opendir(dirbuf.c_str());
	if (!dirp)
	{
		Anope::Logger.Debug("Cannot open directory ({0})", dirbuf);
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
 * @return ModuleReturn::OK on success
 */
static ModuleReturn moduleCopyFile(const Anope::string &name, Anope::string &output)
{
	Anope::string input = Anope::ModuleDir + "/modules/" + name + ".so";

	struct stat s;
	if (stat(input.c_str(), &s) == -1)
		return ModuleReturn::NOEXIST;
	else if (!S_ISREG(s.st_mode))
		return ModuleReturn::NOEXIST;

	std::ifstream source(input.c_str(), std::ios_base::in | std::ios_base::binary);
	if (!source.is_open())
		return ModuleReturn::NOEXIST;

	char *tmp_output = strdup(output.c_str());
	int target_fd = mkstemp(tmp_output);
	if (target_fd == -1 || close(target_fd) == -1)
	{
		free(tmp_output);
		source.close();
		return ModuleReturn::FILE_IO;
	}
	output = tmp_output;
	free(tmp_output);

	Anope::Logger.Debug2("Runtime module location: {0}", output);

	std::ofstream target(output.c_str(), std::ios_base::in | std::ios_base::binary);
	if (!target.is_open())
	{
		source.close();
		return ModuleReturn::FILE_IO;
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

	return !source.fail() && !target.fail() ? ModuleReturn::OK : MOD_ERR_FILE_IO;
}
#endif

ModuleReturn ModuleManager::LoadModule(const Anope::string &modname, User *u)
{
	if (modname.empty())
		return ModuleReturn::PARAMS;

	if (FindModule(modname))
		return ModuleReturn::EXISTS;

	Anope::Logger.Debug("Trying to load module: {0}", modname);

#ifdef _WIN32
	/* Generate the filename for the temporary copy of the module */
	Anope::string pbuf = Anope::DataDir + "/runtime/" + modname.replace_all_cs("/", "_") + ".so.XXXXXX";

	/* Don't skip return value checking! -GD */
	ModuleReturn ret = moduleCopyFile(modname, pbuf);
	if (ret != ModuleReturn::OK)
	{
		if (ret == ModuleReturn::NOEXIST)
			Anope::Logger.Terminal(_("Error while loading {0} (file does not exist)"), modname);
		else if (ret == ModuleReturn::FILE_IO)
			Anope::Logger.Terminal(_("Error while loading {0} (file IO error, check file permissions and diskspace)"), modname);
		return ret;
	}
#else
	Anope::string pbuf = Anope::ModuleDir + "/modules/" + modname.replace_all_cs("/", "_") + ".so";
#endif

	dlerror();
	void *handle = dlopen(pbuf.c_str(), RTLD_NOW);
	const char *err = dlerror();
	if (!handle)
	{
		if (err && *err)
			Anope::Logger.Log(err);
		return ModuleReturn::NOLOAD;
	}

	dlerror();
	AnopeModule *module = static_cast<AnopeModule *>(dlsym(handle, "AnopeMod"));
	err = dlerror();
	if (!module || module->api_version != ANOPE_MODAPI_VER)
	{
		Anope::Logger.Log("No module symbols function found, not an Anope module");
		if (err && *err)
			Anope::Logger.Log(err);
		dlclose(handle);
		return ModuleReturn::NOLOAD;
	}

	try
	{
		ModuleVersion v = module->version();

		if (v.GetMajor() < Anope::VersionMajor() || (v.GetMajor() == Anope::VersionMajor() && v.GetMinor() < Anope::VersionMinor()))
		{
			Anope::Logger.Log(_("Module {0} is compiled against an older version of Anope {1}.{2}, this is {3}"), modname, v.GetMajor(), v.GetMinor(), Anope::VersionShort());
			dlclose(handle);
			return ModuleReturn::VERSION;
		}
		else if (v.GetMajor() > Anope::VersionMajor() || (v.GetMajor() == Anope::VersionMajor() && v.GetMinor() > Anope::VersionMinor()))
		{
			Anope::Logger.Log(_("Module {0} is compiled against a newer version of Anope {1}.{2}, this is {3}"), modname, v.GetMajor(), v.GetMinor(), Anope::VersionShort());
			dlclose(handle);
			return ModuleReturn::VERSION;
		}
		else if (v.GetPatch() < Anope::VersionPatch())
		{
			Anope::Logger.Log(_("Module {0} is compiled against an older version of Anope, {1}.{2}.{3}, this is {4}"), modname, v.GetMajor(), v.GetMinor(), v.GetPatch(), Anope::VersionShort());
			dlclose(handle);
			return ModuleReturn::VERSION;
		}
		else if (v.GetPatch() > Anope::VersionPatch())
		{
			Anope::Logger.Log(_("Module {0} is compiled against a newer version of Anope, {1}.{2}.{3}, this is {4}"), modname, v.GetMajor(), v.GetMinor(), v.GetPatch(), Anope::VersionShort());
			dlclose(handle);
			return ModuleReturn::VERSION;
		}
		else
		{
			Anope::Logger.Debug2("Module {0} is compiled against current version of Anope {1}", Anope::VersionShort());
		}
	}
	catch (const ModuleException &ex)
	{
		/* this error has already been logged */
		dlclose(handle);
		return ModuleReturn::NOLOAD;
	}

	ModuleDef *def = module->init();
	
	/* Create module. */
	Anope::string nick;
	if (u)
		nick = u->nick;

	Module *m;

	ModuleReturn moderr = ModuleReturn::OK;
	try
	{
		m = def->Create(modname, nick);
	}
	catch (const ModuleException &ex)
	{
		Anope::Logger.Log(_("Error while loading {0}: {1}"), modname, ex.GetReason());
		moderr = ModuleReturn::EXCEPTION;
	}
	
	if (moderr != ModuleReturn::OK)
	{
		if (dlclose(handle))
			Anope::Logger.Log(dlerror());
		return moderr;
	}

	m->filename = pbuf;
	m->handle = handle;
	m->def = def;
	m->module = module;

	/* Initialize config */
	try
	{
		m->OnReload(Config);
	}
	catch (const ModuleException &ex)
	{
		Anope::Logger.Log(_("Module {0} couldn't load: {1}"), modname, ex.GetReason());
		moderr = ModuleReturn::EXCEPTION;
	}
	catch (const ConfigException &ex)
	{
		Anope::Logger.Log(_("Module {0} couldn't load due to configuration problems: {1}"), modname, ex.GetReason());
		moderr = ModuleReturn::EXCEPTION;
	}
	
	if (moderr != ModuleReturn::OK)
	{
		DeleteModule(m);
		return moderr;
	}

	Anope::Logger.Log(_("Module {0} loaded"), modname);

	EventManager::Get()->Dispatch(&Event::ModuleLoad::OnModuleLoad, u, m);

	return ModuleReturn::OK;
}

ModuleReturn ModuleManager::UnloadModule(Module *m, User *u)
{
	if (!m)
		return ModuleReturn::PARAMS;

	EventManager::Get()->Dispatch(&Event::ModuleUnload::OnModuleUnload, u, m);

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
		return ModuleReturn::PARAMS;

	Serialize::Unregister(m);

	void *handle = m->handle;
	Anope::string filename = m->filename;

	Anope::Logger.Log("Unloading module {0}", m->name);

	ModuleDef *def = m->def;
	AnopeModule *module = m->module;

	def->Destroy(m);
	module->fini(def);

	dlerror();
	if (dlclose(handle))
		Anope::Logger.Log(dlerror());

#ifdef _WIN32
	if (!filename.empty())
		unlink(filename.c_str());
#endif

	return ModuleReturn::OK;
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

