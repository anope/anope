
#include "module.h"
#include "xmlrpc.h"
#include "httpd.h"

class MyXMLRPCServiceInterface : public XMLRPCServiceInterface, public HTTPPage
{
	std::deque<XMLRPCEvent *> events;

 public:
	MyXMLRPCServiceInterface(Module *creator, const Anope::string &sname) : XMLRPCServiceInterface(creator, sname), HTTPPage("/xmlrpc", "text/xml") { }

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

	Anope::string Sanitize(const Anope::string &string) anope_override
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

 private:
	static bool GetData(Anope::string &content, Anope::string &tag, Anope::string &data)
	{
		if (content.empty())
			return false;

		Anope::string prev, cur;
		bool istag;

		do
		{
			prev = cur;
			cur.clear();

			int len = 0;
			istag = false;

			if (content[0] == '<')
			{
				len = content.find_first_of('>');
				istag = true;
			}
			else if (content[0] != '>')
			{
				len = content.find_first_of('<');
			}

			if (len)
			{
				if (istag)
				{
					cur = content.substr(1, len - 1);
					content.erase(0, len + 1);
					while (!content.empty() && content[0] == ' ')
						content.erase(content.begin());
				}
				else
				{
					cur = content.substr(0,len);
					content.erase(0, len);
				}
			}
		}
		while (istag && !content.empty());

		tag = prev;
		data = cur;
		return !istag && !data.empty();
	}

 public:
	bool OnRequest(HTTPProvider *provider, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply) anope_override
	{
		Anope::string content = message.content, tname, data;
		XMLRPCRequest request(reply);

		while (GetData(content, tname, data))
		{
			Log(LOG_DEBUG) << "m_xmlrpc: Tag name: " << tname << ", data: " << data;
			if (tname == "methodName")
				request.name = data;
			else if (tname == "name" && data == "id")
			{
				GetData(content, tname, data);
				request.id = data;
			}
			else if (tname == "string")
				request.data.push_back(data);
		}

		for (unsigned i = 0; i < this->events.size(); ++i)
		{
			XMLRPCEvent *e = this->events[i];

			if (!e->Run(this, client, request))
				return false;
			else if (!request.get_replies().empty())
			{
				this->Reply(request);
				return true;
			}
		}

		reply.error = HTTP_PAGE_NOT_FOUND;
		reply.Write("Unrecognized query");
		return true;
	}

	void Reply(XMLRPCRequest &request)
	{
		if (!request.id.empty())
			request.reply("id", request.id);

		Anope::string r = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<methodCall>\n<methodName>" + request.name + "</methodName>\n<params>\n<param>\n<value>\n<struct>\n";
		for (std::map<Anope::string, Anope::string>::const_iterator it = request.get_replies().begin(); it != request.get_replies().end(); ++it)
			r += "<member>\n<name>" + it->first + "</name>\n<value>\n<string>" + this->Sanitize(it->second) + "</string>\n</value>\n</member>\n";
		r += "</struct>\n</value>\n</param>\n</params>\n</methodCall>";

		request.r.Write(r);
	}
};

class ModuleXMLRPC : public Module
{
	ServiceReference<HTTPProvider> httpref;
 public:
	MyXMLRPCServiceInterface xmlrpcinterface;

	ModuleXMLRPC(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR),
		httpref("HTTPProvider", "httpd/main"), xmlrpcinterface(this, "xmlrpc")
	{
		if (!httpref)
			throw ModuleException("Unable to find http reference, is m_httpd loaded?");
		httpref->RegisterPage(&xmlrpcinterface);

		Implementation i[] = { I_OnReload };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	~ModuleXMLRPC()
	{
		if (httpref)
			httpref->UnregisterPage(&xmlrpcinterface);
	}
};

MODULE_INIT(ModuleXMLRPC)

