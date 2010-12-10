#include "module.h"
#include "ssl.h"
#include "xmlrpc.h"

class MyXMLRPCClientSocket : public XMLRPCClientSocket
{
 public:
	MyXMLRPCClientSocket(XMLRPCListenSocket *ls, int fd, const sockaddrs &addr) : XMLRPCClientSocket(ls, fd, addr)
	{
		if (ls->username.empty() && ls->password.empty())
			this->logged_in = true;
	}

	bool Read(const Anope::string &message)
	{
		this->query += message;
		Log(LOG_DEBUG) << "m_xmlrpc: " << message;

		if (message.find("</methodCall>") != Anope::string::npos)
		{
			Log(LOG_DEBUG) << "m_xmlrpc: Processing message";
			this->HandleMessage();
			this->query.clear();
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
	MyXMLRPCListenSocket(const Anope::string &bindip, int port, bool ipv6, const Anope::string &u, const Anope::string &p, const std::vector<Anope::string> &a) : XMLRPCListenSocket(bindip, port, ipv6, u, p, a) { }
	
	ClientSocket *OnAccept(int fd, const sockaddrs &addr)
	{
		MyXMLRPCClientSocket *socket = new MyXMLRPCClientSocket(this, fd, addr);
		if (std::find(this->allowed.begin(), this->allowed.end(), addr.addr()) != this->allowed.end())
			Log() << "m_xmlrpc: Accepted connection " << fd << " from " << addr.addr();
		else
		{
			Log() << "m_xmlrpc: Dropping disallowed connection " << fd << " from " << addr.addr();
			socket->SetFlag(SF_DEAD);
		}
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
		
		source->Write(reply);
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
			special_chars("\35", ""), // italics
			special_chars("\031", ""), // underline
			special_chars("\26", ""), // reverses
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
	std::vector<MyXMLRPCListenSocket *> listen_sockets;
	service_reference<SSLService> sslref;

 public:
	MyXMLRPCServiceInterface xmlrpcinterface;

	ModuleXMLRPC(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), sslref("ssl"), xmlrpcinterface(this, "xmlrpc")
	{
		me = this;

		OnReload(false);

		ModuleManager::RegisterService(&this->xmlrpcinterface);

		Implementation i[] = { I_OnReload };
		ModuleManager::Attach(i, this, 1);
	}

	~ModuleXMLRPC()
	{
		for (unsigned i = 0; i < this->listen_sockets.size(); ++i)
			delete this->listen_sockets[i];
		this->listen_sockets.clear();
	}

	void OnReload(bool)
	{
		ConfigReader config;

		for (unsigned i = 0; i < this->listen_sockets.size(); ++i)
			delete this->listen_sockets[i];
		this->listen_sockets.clear();

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
				{
					sslref->Init(xmls);
				}
				this->listen_sockets.push_back(xmls);
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

	if (request.name == "login" || this->logged_in)
		me->xmlrpcinterface.RunXMLRPC(this, &request);
}

MODULE_INIT(ModuleXMLRPC)

