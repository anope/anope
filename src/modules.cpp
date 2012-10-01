/* Modular support
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */


#include "services.h"
#include "modules.h"

std::list<Module *> Modules;

CallBack::CallBack(Module *mod, long time_from_now, time_t now, bool repeating) : Timer(time_from_now, now, repeating),  m(mod)
{
}

CallBack::~CallBack()
{
	std::list<CallBack *>::iterator it = std::find(m->CallBacks.begin(), m->CallBacks.end(), this);
	if (it != m->CallBacks.end())
		m->CallBacks.erase(it);
}

