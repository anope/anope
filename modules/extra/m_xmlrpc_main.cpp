#include "module.h"
#include "xmlrpc.h"

class MyXMLRPCEvent : public XMLRPCEvent
{
 public:
	void Run(XMLRPCServiceInterface *iface, XMLRPCClientSocket *source, XMLRPCRequest *request) anope_override
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

 private:
	void DoCommand(XMLRPCServiceInterface *iface, XMLRPCClientSocket *, XMLRPCRequest *request)
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

				NickAlias *na = findnick(user);

				Anope::string out;

				struct XMLRPCommandReply : CommandReply
				{
					Anope::string &str;

					XMLRPCommandReply(Anope::string &s) : str(s) { }

					void SendMessage(const BotInfo *source, const Anope::string &msg) anope_override
					{
						str += msg + "\n";
					};
				}
				reply(out);

				CommandSource source(user, NULL, na ? *na->nc : NULL, &reply);
				source.c = NULL;
				source.owner = bi;
				source.service = bi;

				RunCommand(source, command);

				if (!out.empty())
					request->reply("return", iface->Sanitize(out));
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
			const NickAlias *na = findnick(username);

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
		request->reply("uptime", stringify(Anope::CurTime - start_time));
		request->reply("uplinkname", Me->GetLinks().front()->GetName());
		{
			Anope::string buf;
			for (std::set<Anope::string>::iterator it = Capab.begin(); it != Capab.end(); ++it)
				buf += " " + *it;
			if (!buf.empty())
				buf.erase(buf.begin());
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
			request->reply("topicts", stringify(c->topic_ts));
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
			if (!u->ip.empty())
				request->reply("ip", u->ip);
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
	ModuleXMLRPCMain(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED), xmlrpc("XMLRPCServiceInterface", "xmlrpc")
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

