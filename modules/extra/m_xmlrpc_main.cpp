#include "module.h"
#include "xmlrpc.h"

// XXX We no longer need this, we need to modify CommandSource to allow commands to go back here
class XMLRPCUser : public User
{
	Anope::string out;
	dynamic_reference<NickAlias> na;

 public:
	XMLRPCUser(const Anope::string &nnick) : User(nnick, Config->ServiceUser, Config->ServiceHost, ""), na(findnick(nick))
	{
		this->realname = "XMLRPC User";
		this->server = Me;
	}

	void SendMessage(BotInfo *, Anope::string msg)
	{
		this->out += msg + "\n";
	}

	NickCore *Account()
	{
		return (na ? na->nc : NULL);
	}

	bool IsIdentified(bool CheckNick = false)
	{
		return na;
	}

	bool IsRecognized(bool CheckSecure = false)
	{
		return na;
	}

	const Anope::string &GetOut()
	{
		return this->out;
	}
};

class MyXMLRPCEvent : public XMLRPCEvent
{
 public:
	void Run(XMLRPCServiceInterface *iface, XMLRPCClientSocket *source, XMLRPCRequest *request)
	{
		if (request->name == "command")
			this->DoCommand(iface, source, request);
		else if (request->name == "checkAuthentication")
			this->DoCheckAuthentication(iface, source, request);
		else if (request->name == "stats")
			this->DoStats(iface, source, request);
		else if (request->name == "channel")
			this->DoChannel(iface, source, request);
		else if (request->name == "user")
			this->DoUser(iface, source, request);
		else if (request->name == "opers")
			this->DoOperType(iface, source, request);
	}

	void DoCommand(XMLRPCServiceInterface *iface, XMLRPCClientSocket *source, XMLRPCRequest *request)
	{
		Anope::string service = request->data.size() > 0 ? request->data[0] : "";
		Anope::string user = request->data.size() > 1 ? request->data[1] : "";
		Anope::string command = request->data.size() > 2 ? request->data[2] : "";

		if (service.empty() || user.empty() || command.empty())
			request->reply("error", "Invalid parameters");
		else
		{
			BotInfo *bi = findbot(service);
			if (!bi)
				request->reply("error", "Invalid service");
			else
			{
				request->reply("result", "Success");

				dynamic_reference<User> u = finduser(user);
				bool created = false;
				if (!u)
				{
					u = new XMLRPCUser(user);
					created = true;
					request->reply("online", "no");
				}
				else
					request->reply("online", "yes");

				mod_run_cmd(bi, *u, NULL, command);

				if (created && u)
				{
					XMLRPCUser *myu = debug_cast<XMLRPCUser *>(*u);
					if (!myu->GetOut().empty())
						request->reply("return", iface->Sanitize(myu->GetOut()));
					delete *u;
				}
			}
		}
	}

	void DoCheckAuthentication(XMLRPCServiceInterface *iface, XMLRPCClientSocket *source, XMLRPCRequest *request)
	{
		Anope::string username = request->data.size() > 0 ? request->data[0] : "";
		Anope::string password = request->data.size() > 1 ? request->data[1] : "";

		if (username.empty() || password.empty())
			request->reply("error", "Invalid parameters");
		else
		{
			NickAlias *na = findnick(username);

			if (!na)
				request->reply("error", "Invalid account");
			else
			{
				EventReturn MOD_RESULT;
				FOREACH_RESULT(I_OnCheckAuthentication, OnCheckAuthentication(NULL, NULL, std::vector<Anope::string>(), na->nc->display, password));
				if (MOD_RESULT == EVENT_ALLOW)
				{
					request->reply("result", "Success");
					request->reply("account", na->nc->display);
				}
				else
					request->reply("error", "Invalid password");
			}
		}
	}

	void DoStats(XMLRPCServiceInterface *iface, XMLRPCClientSocket *source, XMLRPCRequest *request)
	{
		if (SGLine)
			request->reply("sglinecount", stringify(SGLine->GetCount()));
		if (SQLine)
			request->reply("sqlinecount", stringify(SQLine->GetCount()));
		if (SNLine)
			request->reply("snlinecount", stringify(SNLine->GetCount()));
		if (SZLine)
			request->reply("szlinecount", stringify(SZLine->GetCount()));
		request->reply("uptime", stringify(Anope::CurTime - start_time));
		request->reply("uplinkname", Me->GetLinks().front()->GetName());
		{
			Anope::string buf;
			for (unsigned j = 0; !Capab_Info[j].Token.empty(); ++j)
				if (Capab.HasFlag(Capab_Info[j].Flag))
					buf += " " + Capab_Info[j].Token;
			request->reply("uplinkcapab", buf);
		}
		request->reply("usercount", stringify(usercnt));
		request->reply("maxusercount", stringify(maxusercnt));
		request->reply("channelcount", stringify(ChannelList.size()));
	}

	void DoChannel(XMLRPCServiceInterface *iface, XMLRPCClientSocket *source, XMLRPCRequest *request)
	{
		if (request->data.empty())
			return;

		Channel *c = findchan(request->data[0]);

		request->reply("name", iface->Sanitize(c ? c->name : request->data[0]));

		if (c)
		{
			request->reply("bancount", stringify(c->HasMode(CMODE_BAN)));
			int count = 0;
			std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> its = c->GetModeList(CMODE_BAN);
			for (; its.first != its.second; ++its.first)
				request->reply("ban" + stringify(++count), iface->Sanitize(its.first->second));

			request->reply("exceptcount", stringify(c->HasMode(CMODE_EXCEPT)));
			count = 0;
			its = c->GetModeList(CMODE_EXCEPT);
			for (; its.first != its.second; ++its.first)
				request->reply("except" + stringify(++count), iface->Sanitize(its.first->second));

			request->reply("invitecount", stringify(c->HasMode(CMODE_INVITEOVERRIDE)));
			count = 0;
			its = c->GetModeList(CMODE_INVITEOVERRIDE);
			for (; its.first != its.second; ++its.first)
				request->reply("invite" + stringify(++count), iface->Sanitize(its.first->second));

			Anope::string users;
			for (CUserList::const_iterator it = c->users.begin(); it != c->users.end(); ++it)
			{
				UserContainer *uc = *it;
				users += uc->Status->BuildModePrefixList() + uc->user->nick + " ";
			}
			if (!users.empty())
			{
				users.erase(users.length() - 1);
				request->reply("users", iface->Sanitize(users));
			}

			if (!c->topic.empty())
				request->reply("topic", iface->Sanitize(c->topic));

			if (!c->topic_setter.empty())
				request->reply("topicsetter", iface->Sanitize(c->topic_setter));

			request->reply("topictime", stringify(c->topic_time));
		}
	}

	void DoUser(XMLRPCServiceInterface *iface, XMLRPCClientSocket *source, XMLRPCRequest *request)
	{
		if (request->data.empty())
			return;

		User *u = finduser(request->data[0]);

		request->reply("nick", iface->Sanitize(u ? u->nick : request->data[0]));

		if (u)
		{
			request->reply("ident", iface->Sanitize(u->GetIdent()));
			request->reply("vident", iface->Sanitize(u->GetVIdent()));
			request->reply("host", iface->Sanitize(u->host));
			if (!u->vhost.empty())
				request->reply("vhost", iface->Sanitize(u->vhost));
			if (!u->chost.empty())
				request->reply("chost", iface->Sanitize(u->chost));
			if (u->ip())
				request->reply("ip", u->ip.addr());
			request->reply("timestamp", stringify(u->timestamp));
			request->reply("signon", stringify(u->my_signon));
			if (u->Account())
			{
				request->reply("account", iface->Sanitize(u->Account()->display));
				if (u->Account()->o)
					request->reply("opertype", iface->Sanitize(u->Account()->o->ot->GetName()));
			}

			Anope::string channels;
			for (UChannelList::const_iterator it = u->chans.begin(); it != u->chans.end(); ++it)
			{
				ChannelContainer *cc = *it;
				channels += cc->Status->BuildModePrefixList() + cc->chan->name + " ";
			}
			if (!channels.empty())
			{
				channels.erase(channels.length() - 1);
				request->reply("channels", channels);
			}
		}
	}

	void DoOperType(XMLRPCServiceInterface *iface, XMLRPCClientSocket *source, XMLRPCRequest *request)
	{
		for (std::list<OperType *>::const_iterator it = Config->MyOperTypes.begin(), it_end = Config->MyOperTypes.end(); it != it_end; ++it)
		{
			Anope::string perms;
			for (std::list<Anope::string>::const_iterator it2 = (*it)->GetPrivs().begin(), it2_end = (*it)->GetPrivs().end(); it2 != it2_end; ++it2)
				perms += " " + *it2;
			for (std::list<Anope::string>::const_iterator it2 = (*it)->GetCommands().begin(), it2_end = (*it)->GetCommands().end(); it2 != it2_end; ++it2)
				perms += " " + *it2;
			request->reply((*it)->GetName(), perms);
		}
	}
};

class ModuleXMLRPCMain : public Module
{
	service_reference<XMLRPCServiceInterface> xmlrpc;

	MyXMLRPCEvent stats;

 public:
	ModuleXMLRPCMain(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), xmlrpc("xmlrpc")
	{
		if (!xmlrpc)
			throw ModuleException("Unable to find xmlrpc reference, is m_xmlrpc loaded?");

		xmlrpc->Register(&stats);
	}

	~ModuleXMLRPCMain()
	{
		if (xmlrpc)
			xmlrpc->Unregister(&stats);
	}
};

MODULE_INIT(ModuleXMLRPCMain)

