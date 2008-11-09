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

void ModuleManager::LoadModuleList(int total_modules, char **module_list)
{
    int idx;
    Module *m;
    int status = 0;
    for (idx = 0; idx < total_modules; idx++) {
        m = findModule(module_list[idx]);
        if (!m) {
            status = loadModule(module_list[idx], NULL);
            mod_current_module = NULL;
            mod_current_user = NULL;
        }
    }
}
