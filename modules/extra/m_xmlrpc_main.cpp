#include "module.h"
#include "xmlrpc.h"

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

	void SendMessage(const Anope::string &source, const Anope::string &msg)
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
		if (request->name == "login")
			this->DoLogin(iface, source, request);
		else if (request->name == "command")
			this->DoCommand(iface, source, request);
		else if (request->name == "checkAuthentication")
			this->DoCheckAuthentication(iface, source, request);
		else if (request->name == "stats")
			this->DoStats(iface, source, request);
		else if (request->name == "channel")
			this->DoChannel(iface, source, request);
		else if (request->name == "user")
			this->DoUser(iface, source, request);
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

				mod_run_cmd(bi, *u, command);

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

	void DoLogin(XMLRPCServiceInterface *iface, XMLRPCClientSocket *source, XMLRPCRequest *request)
	{
		if (source->logged_in)
			request->reply("result", "Logged in");
		else
		{
			XMLRPCListenSocket *ls = static_cast<XMLRPCListenSocket *>(source->LS);

			if (ls->username.empty() && ls->password.empty())
				request->reply("result", "Logged in");
			else if (request->data.size() > 1 && request->data[0] == ls->username && request->data[1] == ls->password)
				request->reply("result", "Logged in");
			else
				request->reply("error", "Invalid credentials");
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
			else if (enc_check_password(password, na->nc->pass) == 1)
			{
				request->reply("result", "Success");
				request->reply("account", na->nc->display);
			}
			else
				request->reply("error", "Invalid password");
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
			request->reply("bancount", stringify((c && c->bans ? c->bans->count : 0)));
			if (c->bans && c->bans->count)
			{
				int i = 0;
				for (Entry *entry = c->bans->entries; entry; entry = entry->next, ++i)
					request->reply("ban" + stringify(i), iface->Sanitize(entry->mask));
			}
			request->reply("exceptcount", stringify((c && c->excepts ? c->excepts->count : 0)));
			if (c->excepts && c->excepts->count)
			{
				int i = 0;
				for (Entry *entry = c->excepts->entries; entry; entry = entry->next, ++i)
					request->reply("except" + stringify(i), iface->Sanitize(entry->mask));
			}
			request->reply("invitecount", stringify((c && c->invites ? c->invites->count : 0)));
			if (c->invites && c->invites->count)
			{
				int i = 0;
				for (Entry *entry = c->invites->entries; entry; entry = entry->next, ++i)
					request->reply("invite" + stringify(i), iface->Sanitize(entry->mask));
			}

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
				request->reply("account", iface->Sanitize(u->Account()->display));

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

