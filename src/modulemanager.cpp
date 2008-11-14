/* Modular support
 *
 * (C) 2008 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * $Id$
 *
 */
#include "modules.h"
#include "language.h"
#include "version.h"

#if !defined(_WIN32)
	#include <dlfcn.h>
#else
	const char *ano_moderr(void);
#endif

void ModuleManager::LoadModuleList(int total_modules, char **module_list)
{
    int idx;
    Module *m;
    int status = 0;
    for (idx = 0; idx < total_modules; idx++) {
        m = findModule(module_list[idx]);
        if (!m) {
            status = ModuleManager::LoadModule(module_list[idx], NULL);
            mod_current_module = NULL;
            mod_current_user = NULL;
        }
    }
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
static int moduleCopyFile(const char *name, const char *output)
{
    int ch;
    FILE *source, *target;
	int srcfp;
    char input[4096];
    int len;

    strncpy(input, MODULE_PATH, 4095);  /* Get full path with module extension */
    len = strlen(input);
    strncat(input, name, 4095 - len);
    len = strlen(output);
    strncat(input, MODULE_EXT, 4095 - len);

#ifndef _WIN32
	if ((srcfp = mkstemp((char *)output)) == -1)
		return MOD_ERR_FILE_IO;
#else
	if (!mktemp(output))
		return MOD_ERR_FILE_IO;
#endif

	if (debug)
		alog("Runtime module location: %s", output);

	/* Linux/UNIX should ignore the b param, why do we still have seperate
	 * calls for it here? -GD
	 */
#ifndef _WIN32
    if ((source = fopen(input, "r")) == NULL) {
#else
    if ((source = fopen(input, "rb")) == NULL) {
#endif
        return MOD_ERR_NOEXIST;
    }
#ifndef _WIN32
    if ((target = fdopen(srcfp, "w")) == NULL) {
#else
    if ((target = fopen(output, "wb")) == NULL) {
#endif
        return MOD_ERR_FILE_IO;
    }
    while ((ch = fgetc(source)) != EOF) {
        fputc(ch, target);
    }
    fclose(source);
    if (fclose(target) != 0) {
        return MOD_ERR_FILE_IO;
    }
    return MOD_ERR_OK;
}

static bool IsOneOfModuleTypeLoaded(MODType mt)
{
	int idx = 0;
	ModuleHash *current = NULL;
	int pmods = 0;
	
	for (idx = 0; idx != MAX_CMD_HASH; idx++)
	{
		for (current = MODULE_HASH[idx]; current; current = current->next)
		{
			if (current->m->type == mt)
				pmods++;
		}
	}

	/*
	 * 2, because module constructors now add modules to the hash.. so 1 (original module)
	 * and 2 (this module). -- w00t
	 */
	if (pmods == 2)
		return true;

	return false;
}

int ModuleManager::LoadModule(const std::string &modname, User * u)
{
	const char *err;
	Module * (*func) (const std::string &);
	int ret = 0;

	if (modname.empty())
		return MOD_ERR_PARAMS;

	if (findModule(modname.c_str()) != NULL)
		return MOD_ERR_EXISTS;

	if (debug)
		alog("trying to load [%s]", modname.c_str());

	/* Generate the filename for the temporary copy of the module */
	std::string pbuf;
	pbuf = MODULE_PATH;
#ifndef _WIN32
	pbuf += "runtime/";
#else
	pbuf += "runtime\\";
#endif
	pbuf += modname;
	pbuf += MODULE_EXT;
	pbuf += ".";
	pbuf += "XXXXXX";

	/* Don't skip return value checking! -GD */
    if ((ret = moduleCopyFile(modname.c_str(), pbuf.c_str())) != MOD_ERR_OK)
	{
		/* XXX: This used to assign filename here, but I don't think that was correct..
		 * even if it was, it makes life very fucking difficult, so.
		 */
		return ret;
	}

	ano_modclearerr();

	void *handle = ano_modopen(pbuf.c_str());
	if (handle == NULL && (err = ano_moderr()) != NULL)
	{
		alog("%s", err);
		return MOD_ERR_NOLOAD;
	}

	ano_modclearerr();
	func = (Module *(*)(const std::string &))ano_modsym(handle, "init_module");
	if (func == NULL && (err = ano_moderr()) != NULL)
	{
		alog("No magical init function found, not an Anope module");
		ano_modclose(handle);
		return MOD_ERR_NOLOAD;
	}

    if (!func)
	{
		throw CoreException("Couldn't find constructor, yet moderror wasn't set?");
	}

	mod_current_module_name = modname.c_str();

	/* Create module. */
	std::string nick;
	if (u)
		nick = u->nick;
	else
		nick = "";

	Module *m;

	try
	{
		m = func(nick);
	}
	catch (ModuleException &ex)
	{
		alog("Error while loading %s: %s", modname.c_str(), ex.GetReason());
		return MOD_STOP;
	}

	mod_current_module = m;
	mod_current_user = u;
	m->filename = pbuf;
	m->handle = handle;

	if (m->type == PROTOCOL && IsOneOfModuleTypeLoaded(PROTOCOL))
	{
		delete m;
		alog("You cannot load two protocol modules");
		return MOD_STOP;
	}
	else if (m->type == ENCRYPTION && IsOneOfModuleTypeLoaded(ENCRYPTION))
	{
		delete m;
		alog("You cannot load two encryption modules");
		return MOD_STOP;
	}

	mod_current_module_name = NULL;

	if (u)
	{
		ircdproto->SendGlobops(s_OperServ, "%s loaded module %s", u->nick, modname.c_str());
		notice_lang(s_OperServ, u, OPER_MODULE_LOADED, modname.c_str());
	}

	return MOD_ERR_OK;
}

int ModuleManager::UnloadModule(Module *m, User *u)
{
	if (!m || !m->handle)
	{
		if (u)
			notice_lang(s_OperServ, u, OPER_MODULE_REMOVE_FAIL, m->name.c_str());
		return MOD_ERR_PARAMS;
	}

	if (m->GetPermanent() || m->type == PROTOCOL || m->type == ENCRYPTION)
	{
		if (u)
			notice_lang(s_OperServ, u, OPER_MODULE_NO_UNLOAD);
		return MOD_ERR_NOUNLOAD;
	}

	if (u)
	{
		ircdproto->SendGlobops(s_OperServ, "%s unloaded module %s", u->nick, m->name.c_str());
		notice_lang(s_OperServ, u, OPER_MODULE_UNLOADED, m->name.c_str());
	}

	delete m;
	return MOD_ERR_OK;
}

