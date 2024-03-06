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
 *   "method": "command",
 *   "service": "chanserv",
 *   "command": "op",
 *   "params": ["#help", "some_user"],
 *   "ticket": {
 *       "ticket": "brEJHU976XlOLikD2q0ky5EHbwDdb2Jb",
 *       "username": "irc_nickname"
 *   }
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

static Module *me;

class JSONIdentifyRequest : public IdentifyRequest
{
	JSONRequest request;
	HTTPReply repl;
	Reference<HTTPClient> client;
	Reference<JSONServiceInterface> xinterface;

 public:
	JSONIdentifyRequest(Module *m, JSONRequest& req, HTTPClient *c, JSONServiceInterface* iface, const Anope::string &acc, const Anope::string &pass) : IdentifyRequest(m, acc, pass), request(req), repl(request.r), client(c), xinterface(iface) { }

	void OnSuccess() anope_override
	{
		if (!xinterface || !client)
        {
			return;
        }

		JSONTicket new_ticket;
        request.r = this->repl;

        xinterface->ticketiface.createTicket(new_ticket, GetAccount().c_str());

		request.reply("result", "success");
		request.reply("account", GetAccount());
        request.reply("ticket", new_ticket.ticket.c_str());
        request.reply("expires", std::to_string((long)new_ticket.expire));

		xinterface->Reply(request);
		client->SendReply(&request.r);
	}

	void OnFail() anope_override
	{
		if (!xinterface || !client)
        {
			return;
        }

		request.r = this->repl;

		request.reply("error", "Invalid password");

		xinterface->Reply(request);
		client->SendReply(&request.r);
	}
};

class MyJSONEvent : public JSONEvent
{
public:
	bool Run(JSONServiceInterface *iface, HTTPClient *client, JSONRequest &request) anope_override
	{
        // we at the very least need a method
        if (!request.data.contains("method"))
        {
            request.reply("error", "Missing or malformed data.");
            return false;
        }

        if (request.data["method"] == "auth")
        {
            // don't worry about ticket check we we are authing
            return this->DoCheckAuthentication(iface, client, request);
        }
        else
        {
            // make sure we have ticket object with both ticket id, and nickname
            if (!request.data.contains("ticket") 
                || request.data["ticket"]["ticket"].empty() 
                || request.data["ticket"]["username"].empty())
            {
                request.reply("error", "Invalid auth ticket.");
                return true;
            }
            
            Anope::string user = request.data["ticket"]["username"].get_ref<std::string&>().c_str();

            // validate the ticket
            if (iface->ticketiface.validateTicket(request.data["ticket"]["ticket"], request.data["ticket"]["username"]))
            {
                // run the command
                if (request.data["method"] == "command")
                {
                    this->DoCommand(iface, client, request);
                }
            }
            else
            {
                request.reply("error", "Invalid auth ticket.");
                return true;
            }
        }

		return true;
	}

private:
	void DoCommand(JSONServiceInterface *iface, HTTPClient *client, JSONRequest &request)
	{
        Anope::string service = request.data.contains("service") ? request.data["service"].get_ref<std::string&>().c_str() : "";
		Anope::string command = request.data.contains("command") ? request.data["command"].get_ref<std::string&>().c_str() : "";
        Anope::string user = request.data.contains("ticket") ? request.data["ticket"]["username"].get_ref<std::string&>().c_str() : "";
        Anope::string params = "";

        // if we have params
        if (request.data.contains("params") && request.data["params"].is_array())
        {
            std::string p = " " + std::accumulate(request.data["params"].begin(), request.data["params"].end(), std::string(),
                [](const std::string& a, const std::string& b) {
                    return a + (a.empty() ? "" : " ") + b;
                });

            params = p.c_str();
        }

		if (service.empty() || user.empty() || command.empty())
        {
			request.reply("error", "Invalid parameters");
        }
		else
		{
			BotInfo *bi = BotInfo::Find(service, true);
			if (!bi)
            {
				request.reply("error", "Invalid service");
            }
			else
			{
				request.reply("result", "success");

				NickAlias *na = NickAlias::Find(user);
				Anope::string out;

				struct JSONommandReply : CommandReply
				{
					Anope::string &str;
					JSONommandReply(Anope::string &s) : str(s) { }

					void SendMessage(BotInfo *, const Anope::string &msg) anope_override
					{
						str += msg + "\n";
					};
				}
				reply(out);

                User *u = User::Find(user, true);
				CommandSource source(user, u, na ? *na->nc : NULL, &reply, bi);
				Command::Run(source, command + params);

				if (!out.empty())
                {
					request.reply("return", iface->Sanitize(out));
                }
			}
		}
	}

	bool DoCheckAuthentication(JSONServiceInterface *iface, HTTPClient *client, JSONRequest &request)
	{
        Anope::string username = request.data.contains("username") ? request.data["username"].get_ref<std::string&>().c_str() : "";
		Anope::string password = request.data.contains("password") ? request.data["password"].get_ref<std::string&>().c_str() : "";

		if (username.empty() || password.empty())
        {
			request.reply("error", "Invalid parameters");
        }
		else
		{
			JSONIdentifyRequest *req = new JSONIdentifyRequest(me, request, client, iface, username, password);
			FOREACH_MOD(OnCheckAuthentication, (NULL, req));
            req->Dispatch();
			return false;
		}

		return true;
	}
};

class ModuleJSONMain : public Module
{
	ServiceReference<JSONServiceInterface> json;
	MyJSONEvent stats;

public:
	ModuleJSONMain(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA), json("JSONServiceInterface", "json")
	{
		me = this;

		if (!json)
        {
			throw ModuleException("Unable to find api reference, is m_api loaded?");
        }

		json->Register(&stats);
	}

	~ModuleJSONMain()
	{
		if (json)
        {
            json->Unregister(&stats);
        }
	}
};

MODULE_INIT(ModuleJSONMain)
