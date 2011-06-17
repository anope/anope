/* Modular support
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "modules.h"

message_map MessageMap;
std::list<Module *> Modules;

/** Message constructor, adds the message to Anope
 * @param n The message name
 * @param f A callback function
 */
Message::Message(const Anope::string &n, bool (*f)(const Anope::string &, const std::vector<Anope::string> &)) : name(n), func(f)
{
	MessageMap.insert(std::make_pair(this->name, this));
}

/** Message destructor
 */
Message::~Message()
{
	message_map::iterator it = MessageMap.find(this->name);

	if (it == MessageMap.end())
		return;

	message_map::iterator upper = MessageMap.upper_bound(this->name);

	for (; it != upper; ++it)
	{
		if (it->second == this)
		{
			MessageMap.erase(it);
			break;
		}
	}
}

/** Find a message in the message table
 * @param name The name of the message were looking for
 * @return NULL if we cant find it, or a pointer to the Message if we can
 */
std::vector<Message *> Anope::FindMessage(const Anope::string &name)
{
	std::vector<Message *> messages;

	message_map::iterator it = MessageMap.find(name);

	if (it == MessageMap.end())
		return messages;

	message_map::iterator upper = MessageMap.upper_bound(name);

	for (; it != upper; ++it)
		messages.push_back(it->second);

	return messages;
}

/*******************************************************************************
 * Command Functions
 *******************************************************************************/

int Module::AddCommand(BotInfo *bi, Command *c)
{
	if (!bi || !c)
		return MOD_ERR_PARAMS;

	c->module = this;
	c->service = bi;

	std::pair<CommandMap::iterator, bool> it = bi->Commands.insert(std::make_pair(c->name, c));

	if (it.second != true)
	{
		Log() << "Error creating command " << c->name << ". Command already exists!";
		return MOD_ERR_EXISTS;
	}

	return MOD_ERR_OK;
}

/**
 * Delete a command from the service given.
 * @param cmdTable the cmdTable for the services to remove the command from
 * @param name the name of the command to delete from the service
 * @return returns MOD_ERR_OK on success
 */
int Module::DelCommand(BotInfo *bi, Command *c)
{
	if (!bi || !c)
		return MOD_ERR_PARAMS;

	if (!bi->Commands.erase(c->name))
		return MOD_ERR_NOEXIST;
	
	c->module = NULL;
	c->service = NULL;

	return MOD_ERR_OK;
}

Service::Service(Module *o, const Anope::string &n) : owner(o), name(n)
{
}

Service::~Service()
{
	ModuleManager::UnregisterService(this);
}

