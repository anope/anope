/*
 *
 * (C) 2016 Adam <Adam@anope.org>
 * 
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#include "services.h"
#include "event.h"

EventManager *EventManager::eventManager = nullptr;

void EventManager::Init()
{
	eventManager = new EventManager();
}

EventManager *EventManager::Get()
{
	return eventManager;
}

