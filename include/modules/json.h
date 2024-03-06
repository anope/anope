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

#include <nlohmann/json.hpp>
#include "httpd.h"

/* Basic struct for an authentication ticket. */
struct JSONTicket
{
    std::string ticket;
    std::string user;
    time_t expire;
};

/* JSONRequest Class */
class JSONRequest
{
	std::map<Anope::string, Anope::string> replies;

public:
	Anope::string name;
	Anope::string id;
    nlohmann::json data;
	HTTPReply& r;

	JSONRequest(HTTPReply &_r) : r(_r) { }
	inline void reply(const Anope::string &dname, const Anope::string &ddata) { this->replies.insert(std::make_pair(dname, ddata)); }
	inline const std::map<Anope::string, Anope::string> &get_replies() { return this->replies; }
};

/* JSONTicketInterface Class*/
class JSONTicketInterface
{
    std::vector<JSONTicket> _tickets;
    
public:
    JSONTicketInterface() {};
    
    void createTicket(JSONTicket &t, std::string u)
    {
        // purge expired
        purgeExpired();

        // check if the user has a valid ticker, if yes update
        // if no, create.
        for (JSONTicket &ticket : _tickets)
        {
            if (ticket.user == u)
            { 
                // add another 3600 seconds
                ticket.expire = Anope::CurTime + 3600;
                t = ticket;
                
                return;
            }
        }

        t = {
            Anope::Random(32).c_str(),
            u,
            Anope::CurTime + 3600
        };

        _tickets.push_back(t);
    }

    bool validateTicket(std::string ticket, std::string u)
    {
        // purge expired
        purgeExpired();
        
        for (const JSONTicket& t : _tickets)
        {
            if (t.user == u && t.ticket == ticket) 
            {
                return true;
            }
        }

        return false;
    }

private:
    void purgeExpired()
    {
        _tickets.erase(std::remove_if(_tickets.begin(), _tickets.end(), [](const JSONTicket& t)
        {
            return Anope::CurTime > t.expire;
        }), _tickets.end());
    }
};

class JSONServiceInterface;

class JSONEvent
{
public:
	virtual ~JSONEvent() { }
	virtual bool Run(JSONServiceInterface *iface, HTTPClient *client, JSONRequest &request) = 0;
};

class JSONServiceInterface : public Service
{
public:
    JSONTicketInterface ticketiface;
	JSONServiceInterface(Module *creator, const Anope::string &sname) : Service(creator, "JSONServiceInterface", sname) { }
	virtual void Register(JSONEvent *event) = 0;
	virtual void Unregister(JSONEvent *event) = 0;
	virtual Anope::string Sanitize(const Anope::string &string) = 0;
	virtual void Reply(JSONRequest &request) = 0;
};
