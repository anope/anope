/**
 * Anope JSON API
 * json.h
 * synmuffin (jnewing@gmail.com)
 * 
 * Ok here's the deal, this is a total hackjob I just took the xmlrpc module and redid it for JSON, as well
 * as added some basic authentication stuff.
 * 
 * methods
 *  - "auth" to login (and create your ticket, also update)
 *  - "command" to run any command.
 * 
 * examples:
 * 
 * method: auth 
 * this method needs to be called first, if the login is successfully
 * it will return a JSON object with the account name, then unix timestamp of 
 * when the ticket will expire, the result, and the ticket itself.
 * 
 * you will need this ticket to access commands.
 * 
 * [REQUEST]:
 * Headers:
 *  Content-Type: "application/json"
 * 
 * Body:
 * {
 *   "method": "auth",
 *   "username": "irc_nickname",
 *   "password": "chanserv_password"
 * }
 * 
 * [RESPONSE]:
 * {
 *  "account": "irc_nickname",
 *  "expires": "1709697781",
 *  "result": "success",
 *  "ticket": "brEJHU976XlOLikD2q0ky5EHbwDdb2Jb"
 * }
 * 
 * 
 * method: validate 
 * this is a quick method to check current ticket validity.
 * 
 * 
 * [REQUEST]:
 * Headers:
 *  Content-Type: "application/json"
 * 
 * Body:
 * {
 *   "method": "auth",
 *   "ticket": {
 *       "ticket": "brEJHU976XlOLikD2q0ky5EHbwDdb2Jb",
 *       "username": "synmuffin"
 *   }
 * }
 * 
 * [RESPONSE]:
 * {
 *  "valid": "true|false"
 * }
 * 
 * 
 * method: command
 * this is just like running any other services command. give the service, the command name
 * an array of parameters and lastly the ticket object that consistest of the ticket and
 * the uesrname (irc nickame). see example below. 
 * 
 * [REQUEST]:
 * Headers:
 *  Content-Type: "application/json"
 * 
 * Body:
 * {
 *   "ticket": {
 *       "ticket": "brEJHU976XlOLikD2q0ky5EHbwDdb2Jb",
 *       "username": "irc_nickname"
 *   },
 *   "method": "command",
 *   "service": "chanserv",
 *   "command": "op",
 *   "params": ["#help", "some_user"]
 * }
 * 
 * [RESPONSE]:
 * {
 *  "result": "success"
 * }
 *  
 */

#include "module.h"
#include "modules/json.h"
#include "modules/httpd.h"


class MyJSONServiceInterface : public JSONServiceInterface, public HTTPPage
{
  std::deque<JSONEvent *> events;

public:

	MyJSONServiceInterface(Module *creator, const Anope::string &sname) :
    JSONServiceInterface(creator, sname),
    HTTPPage("/api", "application/json")
  {}

	void Register(JSONEvent *event)
	{
		this->events.push_back(event);
	}

	void Unregister(JSONEvent *event)
	{
		std::deque<JSONEvent *>::iterator it = std::find(this->events.begin(), this->events.end(), event);

		if (it != this->events.end())
    {
		  this->events.erase(it);
    }
	}

	Anope::string Sanitize(const Anope::string &string)
	{
		Anope::string ret = string;
		return ret;
	}

	bool OnRequest(HTTPProvider *provider, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply)
	{
		Anope::string content = message.content;
		JSONRequest request(reply);

    GetData(content, request.data);

		for (unsigned i = 0; i < this->events.size(); ++i)
		{
			JSONEvent *e = this->events[i];

			if (!e->Run(this, client, request))
      {
				return false;
      }
			else if (!request.get_replies().empty())
			{
				this->Reply(request);
				return true;
			}
		}

		reply.error = HTTP_PAGE_NOT_FOUND;
		reply.Write("{ 404, \"Method not found.\"}");
		return true;
	}

	void Reply(JSONRequest &request)
	{
    nlohmann::json r;

		if (!request.id.empty())
    {
      r["id"] = request.id.c_str();
    }

    for (auto const& [k, v]: request.get_replies())
    {
      r[k.c_str()] = v.c_str();
    }

    request.r.Write(r.dump());
	}

private:

  static bool GetData(Anope::string &content, nlohmann::json &data)
	{
		if (content.empty())
    {
			return false;
    }

    data = nlohmann::json::parse(content);
    return true;
	}
};

class ModuleJSON : public Module
{
	ServiceReference<HTTPProvider> httpref;

public:
	MyJSONServiceInterface jsoninterface;

	ModuleJSON(const Anope::string &modname, const Anope::string &creator) :
    Module(modname, creator, EXTRA),
		jsoninterface(this, "json")
	{}

	~ModuleJSON()
	{
		if (httpref)
    {
			httpref->UnregisterPage(&jsoninterface);
    }
	}

	void OnReload(Configuration::Conf *conf)
	{
		if (httpref)
    {
			httpref->UnregisterPage(&jsoninterface);
    }

		this->httpref = ServiceReference<HTTPProvider>("HTTPProvider", conf->GetModule(this)->Get<const Anope::string>("server", "httpd/main"));

		if (!httpref)
    {
			throw ConfigException("Unable to find http reference, is m_httpd loaded?");
    }

		httpref->RegisterPage(&jsoninterface);
	}
};

MODULE_INIT(ModuleJSON)
