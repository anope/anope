#include "module.h"
#include "ssl.h"
#include "xmlrpc.h"

std::vector<XMLRPCListenSocket *> listen_sockets;

class MyXMLRPCClientSocket : public XMLRPCClientSocket
{
	/* Used to skip the (optional) HTTP header,  which we really don't care about */
	bool in_query;
 public:
	MyXMLRPCClientSocket(XMLRPCListenSocket *ls, int fd, const sockaddrs &addr) : Socket(fd, ls->IsIPv6()), XMLRPCClientSocket(ls, addr), in_query(false)
	{
	}

	bool Read(const Anope::string &message)
	{
		if (message.find("xml version") != Anope::string::npos)
			this->in_query = true;
		else if (!this->in_query)
			return true;

		this->query += message;
		Log(LOG_DEBUG) << "m_xmlrpc: " << message;

		if (message.find("</methodCall>") != Anope::string::npos)
		{
			Log(LOG_DEBUG) << "m_xmlrpc: Processing message";
			this->HandleMessage();
		}

		return true;
	}

	bool GetData(Anope::string &tag, Anope::string &data)
	{
		if (this->query.empty())
			return false;

		Anope::string prev, cur;
		bool istag;

		do
		{
			prev = cur;
			cur.clear();

			int len = 0;
			istag = false;

			if (this->query[0] == '<')
			{
				len = this->query.find_first_of('>');
				istag = true;
			}
			else if (this->query[0] != '>')
			{
				len = this->query.find_first_of('<');
			}

			if (len)
			{
				if (istag)
				{
					cur = this->query.substr(1, len - 1);
					this->query.erase(0, len + 1);
					while (!this->query.empty() && this->query[0] == ' ')
						this->query.erase(this->query.begin());
				}
				else
				{
					cur = this->query.substr(0,len);
					this->query.erase(0, len);
				}
			}
		}
		while (istag && !this->query.empty());

		tag = prev;
		data = cur;
		return !istag && !data.empty();
	}
	
	void HandleMessage();
};

class MyXMLRPCListenSocket : public XMLRPCListenSocket
{
 public:
	MyXMLRPCListenSocket(const Anope::string &bindip, int port, bool ipv6, const Anope::string &u, const Anope::string &p, const std::vector<Anope::string> &a) : XMLRPCListenSocket(bindip, port, ipv6, u, p, a)
	{
		listen_sockets.push_back(this);
	}

	~MyXMLRPCListenSocket()
	{
		std::vector<XMLRPCListenSocket *>::iterator it = std::find(listen_sockets.begin(), listen_sockets.end(), this);
		if (it != listen_sockets.end())
			listen_sockets.erase(it);
	}
	
	ClientSocket *OnAccept(int fd, const sockaddrs &addr)
	{
		MyXMLRPCClientSocket *socket = new MyXMLRPCClientSocket(this, fd, addr);

		socket->SetFlag(SF_DEAD);
		for (unsigned i = 0, j = this->allowed.size(); i < j; ++i)
		{
			try
			{
				cidr cidr_mask(this->allowed[i]);
				if (cidr_mask.match(socket->clientaddr))
				{
					Log() << "m_xmlrpc: Accepted connection " << fd << " from " << addr.addr();
					socket->UnsetFlag(SF_DEAD);
					break;
				}
			}
			catch (const SocketException &ex)
			{
				Log() << "m_xmlrpc: Error checking incoming connection against mask " << this->allowed[i] << ": " << ex.GetReason();
			}
		}

		if (socket->HasFlag(SF_DEAD))
			Log() << "m_xmlrpc: Dropping disallowed connection " << fd << " from " << addr.addr();

		return socket;
	}
};

class MyXMLRPCServiceInterface : public XMLRPCServiceInterface
{
	std::deque<XMLRPCEvent *> events;

 public:
	MyXMLRPCServiceInterface(Module *creator, const Anope::string &sname) : XMLRPCServiceInterface(creator, sname) { }

	void Register(XMLRPCEvent *event)
	{
		this->events.push_back(event);
	}

	void Unregister(XMLRPCEvent *event)
	{
		std::deque<XMLRPCEvent *>::iterator it = std::find(this->events.begin(), this->events.end(), event);

		if (it != this->events.end())
			this->events.erase(it);
	}

	void Reply(XMLRPCClientSocket *source, XMLRPCRequest *request)
	{
		if (!request->id.empty())
			request->reply("id", request->id);

		Anope::string reply = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<methodCall>\n<methodName>" + request->name + "</methodName>\n<params>\n<param>\n<value>\n<struct>\n";
		for (std::map<Anope::string, Anope::string>::const_iterator it = request->get_replies().begin(); it != request->get_replies().end(); ++it)
		{
			reply += "<member>\n<name>" + it->first + "</name>\n<value>\n<string>" + this->Sanitize(it->second) + "</string>\n</value>\n</member>\n";
		}
		reply += "</struct>\n</value>\n</param>\n</params>\n</methodCall>";

		source->Write("HTTP/1.1 200 OK");
		source->Write("Connection: close");
		source->Write("Content-Type: text/xml");
		source->Write("Content-Length: " + stringify(reply.length()));
		source->Write("Server: Anope IRC Services version " + Anope::VersionShort());
		source->Write("");
		
		source->Write(reply);

		Log(LOG_DEBUG) << reply;
	}

	Anope::string Sanitize(const Anope::string &string)
	{
		static struct special_chars
		{
			Anope::string character;
			Anope::string replace;

			special_chars(const Anope::string &c, const Anope::string &r) : character(c), replace(r) { }
		}
		special[] = {
			special_chars("&", "&amp;"),
			special_chars("\"", "&quot;"),
			special_chars("<", "&lt;"),
			special_chars(">", "&qt;"),
			special_chars("'", "&#39;"),
			special_chars("\n", "&#xA;"),
			special_chars("\002", ""), // bold
			special_chars("\003", ""), // color
			special_chars("\035", ""), // italics
			special_chars("\037", ""), // underline
			special_chars("\026", ""), // reverses
			special_chars("", "")
		};

		Anope::string ret = string;
		for (int i = 0; special[i].character.empty() == false; ++i)
			ret = ret.replace_all_cs(special[i].character, special[i].replace);
		return ret;
	}

	void RunXMLRPC(XMLRPCClientSocket *source, XMLRPCRequest *request)
	{
		for (unsigned i = 0; i < this->events.size(); ++i)
		{
			XMLRPCEvent *e = this->events[i];

			e->Run(this, source, request);
			if (!request->get_replies().empty())
				this->Reply(source, request);
		}
	}
};

class ModuleXMLRPC;
static ModuleXMLRPC *me;
class ModuleXMLRPC : public Module
{
	service_reference<SSLService, Base> sslref;

 public:
	MyXMLRPCServiceInterface xmlrpcinterface;

	ModuleXMLRPC(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED), sslref("ssl"), xmlrpcinterface(this, "xmlrpc")
	{
		me = this;


		Implementation i[] = { I_OnReload };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
		
		OnReload();
	}

	~ModuleXMLRPC()
	{
		/* Clean up our sockets and our listening sockets */
		for (std::map<int, Socket *>::const_iterator it = SocketEngine::Sockets.begin(), it_end = SocketEngine::Sockets.end(); it != it_end;)
		{
			Socket *s = it->second;
			++it;

			ClientSocket *cs = dynamic_cast<ClientSocket *>(s);
			if (cs != NULL)
			{
				for (unsigned i = 0; i < listen_sockets.size(); ++i)
					if (cs->LS == listen_sockets[i])
					{
						delete cs;
						break;
					}
			}
		}

		for (unsigned i = 0; i < listen_sockets.size(); ++i)
			delete listen_sockets[i];
		listen_sockets.clear();
	}

	void OnReload()
	{
		ConfigReader config;

		for (unsigned i = 0; i < listen_sockets.size(); ++i)
			delete listen_sockets[i];
		listen_sockets.clear();

		for (int i = 0; i < config.Enumerate("m_xmlrpc"); ++i)
		{
			Anope::string bindip = config.ReadValue("m_xmlrpc", "bindip", "0.0.0.0", i);
			int port = config.ReadInteger("m_xmlrpc", "port", 0, i);
			bool ipv6 = config.ReadFlag("m_xmlrpc", "ipv6", "no", i);
			bool ssl = config.ReadFlag("m_xmlrpc", "ssl", "no", i);
			Anope::string allowed = config.ReadValue("m_xmlrpc", "allowed", "127.0.0.1", i);
			Anope::string username = config.ReadValue("m_xmlrpc", "username", "", i);
			Anope::string password = config.ReadValue("m_xmlrpc", "password", "", i);
			std::vector<Anope::string> allowed_vector = BuildStringVector(allowed, ' ');

			if (bindip.empty() || port < 1)
				continue;

			if (ssl && !sslref)
			{
				Log() << "m_xmlrpc: Could not enable SSL, is m_ssl loaded?";
				ssl = false;
			}

			try
			{
				MyXMLRPCListenSocket *xmls = new MyXMLRPCListenSocket(bindip, port, ipv6, username, password, allowed_vector);
				if (ssl)
					sslref->Init(xmls);
			}
			catch (const SocketException &ex)
			{
				Log() << "m_xmlrpc " << ex.GetReason();
			}
		}
	}
};

void MyXMLRPCClientSocket::HandleMessage()
{
	Anope::string name, data;
	XMLRPCRequest request;

	while (this->GetData(name, data))
	{
		Log(LOG_DEBUG) << "m_xmlrpc: Tag name: " << name << ", data: " << data;
		if (name == "methodName")
			request.name = data;
		else if (name == "name" && data == "id")
		{
			this->GetData(name, data);
			request.id = data;
		}
		else if (name == "string")
			request.data.push_back(data);
	}

	me->xmlrpcinterface.RunXMLRPC(this, &request);
}

MODULE_INIT(ModuleXMLRPC)

