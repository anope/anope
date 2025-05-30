/*
 *
 * (C) 2011-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/* RequiredLibraries: ldap_r|ldap,lber */

#include "module.h"
#include "modules/ldap.h"

#include <condition_variable>
#include <mutex>

#ifdef _WIN32
# include <Winldap.h>
# include <WinBer.h>
# include <wininet.h>
# define LDAP_OPT_SUCCESS LDAP_SUCCESS
# define LDAP_OPT_NETWORK_TIMEOUT LDAP_OPT_SEND_TIMEOUT
# define LDAP_STR(X) const_cast<PSTR>((X).c_str())
# define LDAP_SASL_SIMPLE static_cast<PSTR>(0)
# define LDAP_TIME(X) reinterpret_cast<PLDAP_TIMEVAL>(&(X))
# define LDAP_VENDOR_VERSION_MAJOR (LDAP_VERSION / 100)
# define LDAP_VENDOR_VERSION_MINOR (LDAP_VERSION / 10 % 10)
# define LDAP_VENDOR_VERSION_PATCH (LDAP_VERSION / 10)
# define ldap_first_message ldap_first_entry
# define ldap_next_message ldap_next_entry
# define ldap_unbind_ext(LDAP, UNUSED1, UNUSED2) ldap_unbind(LDAP)
# pragma comment(lib, "Wldap32.lib")
# pragma comment(lib, "Wininet.lib")
#else
# include <ldap.h>
# define LDAP_STR(X) ((X).c_str())
# define LDAP_TIME(X) (&(X))
#endif

#if defined LDAP_API_FEATURE_X_OPENLDAP_REENTRANT && !LDAP_API_FEATURE_X_OPENLDAP_REENTRANT
# error Anope requires OpenLDAP to be built as reentrant.
#endif


class LDAPService;
static Pipe *me;

class LDAPRequest
{
public:
	LDAPService *service;
	LDAPInterface *inter;
	LDAPMessage *message = nullptr; /* message returned by ldap_ */
	LDAPResult *result = nullptr; /* final result */
	struct timeval tv;
	QueryType type = QUERY_UNKNOWN;

	LDAPRequest(LDAPService *s, LDAPInterface *i)
		: service(s)
		, inter(i)
	{
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
	}

	virtual ~LDAPRequest()
	{
		delete result;
		if (inter != NULL)
			inter->OnDelete();
		if (message != NULL)
			ldap_msgfree(message);
	}

	virtual int run() = 0;
};

class LDAPBind final
	: public LDAPRequest
{
	Anope::string who, pass;

public:
	LDAPBind(LDAPService *s, LDAPInterface *i, const Anope::string &w, const Anope::string &p)
		: LDAPRequest(s, i)
		, who(w)
		, pass(p)
	{
		type = QUERY_BIND;
	}

	int run() override;
};

class LDAPSearchRequest final
	: public LDAPRequest
{
	Anope::string base;
	Anope::string filter;

public:
	LDAPSearchRequest(LDAPService *s, LDAPInterface *i, const Anope::string &b, const Anope::string &f)
		: LDAPRequest(s, i)
		, base(b)
		, filter(f)
	{
		type = QUERY_SEARCH;
	}

	int run() override;
};

class LDAPAdd final
	: public LDAPRequest
{
	Anope::string dn;
	LDAPMods attributes;

public:
	LDAPAdd(LDAPService *s, LDAPInterface *i, const Anope::string &d, const LDAPMods &attr)
		: LDAPRequest(s, i)
		, dn(d)
		, attributes(attr)
	{
		type = QUERY_ADD;
	}

	int run() override;
};

class LDAPDel final
	: public LDAPRequest
{
	Anope::string dn;

public:
	LDAPDel(LDAPService *s, LDAPInterface *i, const Anope::string &d)
		: LDAPRequest(s, i)
		, dn(d)
	{
		type = QUERY_DELETE;
	}

	int run() override;
};

class LDAPModify final
	: public LDAPRequest
{
	Anope::string base;
	LDAPMods attributes;

public:
	LDAPModify(LDAPService *s, LDAPInterface *i, const Anope::string &b, const LDAPMods &attr)
		: LDAPRequest(s, i)
		, base(b)
		, attributes(attr)
	{
		type = QUERY_MODIFY;
	}

	int run() override;
};

class LDAPService final
	: public LDAPProvider
	, public Thread
{
	Anope::string server;
	Anope::string admin_binddn;
	Anope::string admin_pass;

	LDAP *con;

	time_t last_connect = 0;

public:
	std::condition_variable_any condvar;
	std::mutex mutex;

	static LDAPMod **BuildMods(const LDAPMods &attributes)
	{
		LDAPMod **mods = new LDAPMod*[attributes.size() + 1];
		memset(mods, 0, sizeof(LDAPMod*) * (attributes.size() + 1));
		for (unsigned x = 0; x < attributes.size(); ++x)
		{
			const LDAPModification &l = attributes[x];
			mods[x] = new LDAPMod();

			if (l.op == LDAPModification::LDAP_ADD)
				mods[x]->mod_op = LDAP_MOD_ADD;
			else if (l.op == LDAPModification::LDAP_DEL)
				mods[x]->mod_op = LDAP_MOD_DELETE;
			else if (l.op == LDAPModification::LDAP_REPLACE)
				mods[x]->mod_op = LDAP_MOD_REPLACE;
			else if (l.op != 0)
				throw LDAPException("Unknown LDAP operation");
			mods[x]->mod_type = strdup(l.name.c_str());
			mods[x]->mod_values = new char*[l.values.size() + 1];
			memset(mods[x]->mod_values, 0, sizeof(char *) * (l.values.size() + 1));
			for (unsigned j = 0, c = 0; j < l.values.size(); ++j)
				if (!l.values[j].empty())
					mods[x]->mod_values[c++] = strdup(l.values[j].c_str());
		}
		return mods;
	}

	static void FreeMods(LDAPMod **mods)
	{
		for (int i = 0; mods[i] != NULL; ++i)
		{
			free(mods[i]->mod_type);
			for (int j = 0; mods[i]->mod_values[j] != NULL; ++j)
				free(mods[i]->mod_values[j]);
			delete [] mods[i]->mod_values;
		}
		delete [] mods;
	}

private:
#ifdef _WIN32
	// Windows LDAP does not implement this so we need to do it.
	int ldap_initialize(LDAP **ldap, const char *url)
	{
		URL_COMPONENTS urlComponents;
		memset(&urlComponents, 0, sizeof(urlComponents));
		urlComponents.dwStructSize = sizeof(urlComponents);

		urlComponents.lpszScheme = new char[8];
		urlComponents.dwSchemeLength = 8;

		urlComponents.lpszHostName = new char[1024];
		urlComponents.dwHostNameLength = 1024;

		if (!InternetCrackUrlA(url, 0, 0, &urlComponents))
		{
			delete[] urlComponents.lpszScheme;
			delete[] urlComponents.lpszHostName;
			return LDAP_CONNECT_ERROR; // Malformed url.
		}

		unsigned long port = 389; // Default plaintext port.
		bool secure = false; // LDAP defaults to plaintext.
		if (urlComponents.dwSchemeLength > 0)
		{
			const Anope::string scheme(urlComponents.lpszScheme);
			if (scheme.equals_ci("ldaps"))
			{
				port = 636; // Default encrypted port.
				secure = true;
			}
			else if (!scheme.equals_ci("ldap"))
			{
				delete[] urlComponents.lpszScheme;
				delete[] urlComponents.lpszHostName;
				return LDAP_CONNECT_ERROR; // Invalid protocol.
			}
		}

		if (urlComponents.nPort > 0)
		{
			port = urlComponents.nPort;
		}

		*ldap = ldap_sslinit(urlComponents.lpszHostName, port, secure);
		delete[] urlComponents.lpszScheme;
		delete[] urlComponents.lpszHostName;
		if (!*ldap)
		{
			return LdapGetLastError(); // Something went wrong, find out what.
		}

		// We're connected to the LDAP server!
		return LDAP_SUCCESS;
	}
#endif

	void Connect()
	{
		int i = ldap_initialize(&this->con, this->server.c_str());
		if (i != LDAP_SUCCESS)
			throw LDAPException("Unable to connect to LDAP service " + this->name + ": " + ldap_err2string(i));

		const int version = LDAP_VERSION3;
		i = ldap_set_option(this->con, LDAP_OPT_PROTOCOL_VERSION, &version);
		if (i != LDAP_OPT_SUCCESS)
			throw LDAPException("Unable to set protocol version for " + this->name + ": " + ldap_err2string(i));

		const struct timeval tv = { 0, 0 };
		i = ldap_set_option(this->con, LDAP_OPT_NETWORK_TIMEOUT, &tv);
		if (i != LDAP_OPT_SUCCESS)
			throw LDAPException("Unable to set timeout for " + this->name + ": " + ldap_err2string(i));
	}

	void Reconnect()
	{
		/* Only try one connect a minute. It is an expensive blocking operation */
		if (last_connect > Anope::CurTime - 60)
			throw LDAPException("Unable to connect to LDAP service " + this->name + ": reconnecting too fast");
		last_connect = Anope::CurTime;

		ldap_unbind_ext(this->con, NULL, NULL);

		Connect();
	}

	void QueueRequest(LDAPRequest *r)
	{
		this->mutex.lock();
		this->queries.push_back(r);
		this->condvar.notify_all();
		this->mutex.unlock();
	}

public:
	typedef std::vector<LDAPRequest *> query_queue;
	query_queue queries, results;
	std::mutex process_mutex; /* held when processing requests not in either queue */

	LDAPService(Module *o, const Anope::string &n, const Anope::string &s, const Anope::string &b, const Anope::string &p) : LDAPProvider(o, n), server(s), admin_binddn(b), admin_pass(p)
	{
		Connect();
	}

	~LDAPService()
	{
		/* At this point the thread has stopped so we don't need to hold process_mutex */

		this->mutex.lock();

		for (auto *req : this->queries)
		{
			/* queries have no results yet */
			req->result = new LDAPResult();
			req->result->type = req->type;
			req->result->error = "LDAP Interface is going away";
			if (req->inter)
				req->inter->OnError(*req->result);

			delete req;
		}
		this->queries.clear();

		for (const auto *req : this->queries)
		{
			/* even though this may have already finished successfully we return that it didn't */
			req->result->error = "LDAP Interface is going away";
			if (req->inter)
				req->inter->OnError(*req->result);

			delete req;
		}

		this->mutex.unlock();

		ldap_unbind_ext(this->con, NULL, NULL);
	}

	void BindAsAdmin(LDAPInterface *i) override
	{
		this->Bind(i, this->admin_binddn, this->admin_pass);
	}

	void Bind(LDAPInterface *i, const Anope::string &who, const Anope::string &pass) override
	{
		auto *b = new LDAPBind(this, i, who, pass);
		QueueRequest(b);
	}

	void Search(LDAPInterface *i, const Anope::string &base, const Anope::string &filter) override
	{
		if (i == NULL)
			throw LDAPException("No interface");

		auto *s = new LDAPSearchRequest(this, i, base, filter);
		QueueRequest(s);
	}

	void Add(LDAPInterface *i, const Anope::string &dn, LDAPMods &attributes) override
	{
		auto *add = new LDAPAdd(this, i, dn, attributes);
		QueueRequest(add);
	}

	void Del(LDAPInterface *i, const Anope::string &dn) override
	{
		auto *del = new LDAPDel(this, i, dn);
		QueueRequest(del);
	}

	void Modify(LDAPInterface *i, const Anope::string &base, LDAPMods &attributes) override
	{
		auto *mod = new LDAPModify(this, i, base, attributes);
		QueueRequest(mod);
	}

private:
	void BuildReply(int res, LDAPRequest *req)
	{
		LDAPResult *ldap_result = req->result = new LDAPResult();
		req->result->type = req->type;

		if (res != LDAP_SUCCESS)
		{
			ldap_result->error = ldap_err2string(res);
			return;
		}

		if (req->message == NULL)
		{
			return;
		}

		/* a search result */

		for (LDAPMessage *cur = ldap_first_message(this->con, req->message); cur; cur = ldap_next_message(this->con, cur))
		{
			LDAPAttributes attributes;

			char *dn = ldap_get_dn(this->con, cur);
			if (dn != NULL)
			{
				attributes["dn"].push_back(dn);
				ldap_memfree(dn);
				dn = NULL;
			}

			BerElement *ber = NULL;

			for (char *attr = ldap_first_attribute(this->con, cur, &ber); attr; attr = ldap_next_attribute(this->con, cur, ber))
			{
				berval **vals = ldap_get_values_len(this->con, cur, attr);
				int count = ldap_count_values_len(vals);

				std::vector<Anope::string> attrs;
				for (int j = 0; j < count; ++j)
					attrs.push_back(vals[j]->bv_val);
				attributes[attr] = attrs;

				ldap_value_free_len(vals);
				ldap_memfree(attr);
			}

			if (ber != NULL)
				ber_free(ber, 0);

			ldap_result->messages.push_back(attributes);
		}
	}

	void SendRequests()
	{
		process_mutex.lock();

		query_queue q;
		this->mutex.lock();
		queries.swap(q);
		this->mutex.unlock();

		if (q.empty())
		{
			process_mutex.unlock();
			return;
		}

		for (auto *req : q)
		{
			int ret = req->run();

			if (ret == LDAP_SERVER_DOWN || ret == LDAP_TIMEOUT)
			{
				/* try again */
				try
				{
					Reconnect();
				}
				catch (const LDAPException &)
				{
				}

				ret = req->run();
			}

			BuildReply(ret, req);

			this->mutex.lock();
			results.push_back(req);
			this->mutex.unlock();
		}

		me->Notify();

		process_mutex.unlock();
	}

public:
	void Run() override
	{
		while (!this->GetExitState())
		{
			this->mutex.lock();
			/* Queries can be non empty if one is pushed during SendRequests() */
			if (queries.empty())
				this->condvar.wait(this->mutex);
			this->mutex.unlock();

			SendRequests();
		}
	}

	LDAP *GetConnection()
	{
		return con;
	}
};

class ModuleLDAP final
	: public Module
	, public Pipe
{
	std::map<Anope::string, LDAPService *> LDAPServices;

public:

	ModuleLDAP(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR)
	{
		me = this;

		Log(this) << "Module was compiled against LDAP (" << LDAP_VENDOR_NAME << ") version " << LDAP_VENDOR_VERSION_MAJOR << "." << LDAP_VENDOR_VERSION_MINOR << "." << LDAP_VENDOR_VERSION_PATCH;
	}

	~ModuleLDAP()
	{
		for (std::map<Anope::string, LDAPService *>::iterator it = this->LDAPServices.begin(); it != this->LDAPServices.end(); ++it)
		{
			it->second->SetExitState();
			it->second->condvar.notify_all();
			it->second->Join();
			delete it->second;
		}
		LDAPServices.clear();
	}

	void OnReload(Configuration::Conf &config) override
	{
		const auto &conf = config.GetModule(this);

		for (std::map<Anope::string, LDAPService *>::iterator it = this->LDAPServices.begin(); it != this->LDAPServices.end();)
		{
			const Anope::string &cname = it->first;
			LDAPService *s = it->second;
			int i;

			++it;

			for (i = 0; i < conf.CountBlock("ldap"); ++i)
				if (conf.GetBlock("ldap", i).Get<const Anope::string>("name", "ldap/main") == cname)
					break;

			if (i == conf.CountBlock("ldap"))
			{
				Log(LOG_NORMAL, "ldap") << "LDAP: Removing server connection " << cname;

				s->SetExitState();
				s->condvar.notify_all();
				s->Join();
				delete s;
				this->LDAPServices.erase(cname);
			}
		}

		for (int i = 0; i < conf.CountBlock("ldap"); ++i)
		{
			const auto &ldap = conf.GetBlock("ldap", i);

			const Anope::string &connname = ldap.Get<const Anope::string>("name", "ldap/main");

			if (this->LDAPServices.find(connname) == this->LDAPServices.end())
			{
				const Anope::string &server = ldap.Get<const Anope::string>("server", "127.0.0.1");
				const Anope::string &admin_binddn = ldap.Get<const Anope::string>("admin_binddn");
				const Anope::string &admin_password = ldap.Get<const Anope::string>("admin_password");

				try
				{
					auto *ss = new LDAPService(this, connname, server, admin_binddn, admin_password);
					ss->Start();
					this->LDAPServices.emplace(connname, ss);

					Log(LOG_NORMAL, "ldap") << "LDAP: Successfully initialized server " << connname << " (" << server << ")";
				}
				catch (const LDAPException &ex)
				{
					Log(LOG_NORMAL, "ldap") << "LDAP: " << ex.GetReason();
				}
			}
		}
	}

	void OnModuleUnload(User *, Module *m) override
	{
		for (std::map<Anope::string, LDAPService *>::iterator it = this->LDAPServices.begin(); it != this->LDAPServices.end(); ++it)
		{
			LDAPService *s = it->second;

			s->process_mutex.lock();
			s->mutex.lock();

			for (unsigned int i = s->queries.size(); i > 0; --i)
			{
				LDAPRequest *req = s->queries[i - 1];
				LDAPInterface *li = req->inter;

				if (li && li->owner == m)
				{
					s->queries.erase(s->queries.begin() + i - 1);
					delete req;
				}
			}
			for (unsigned int i = s->results.size(); i > 0; --i)
			{
				LDAPRequest *req = s->results[i - 1];
				LDAPInterface *li = req->inter;

				if (li && li->owner == m)
				{
					s->results.erase(s->results.begin() + i - 1);
					delete req;
				}
			}

			s->mutex.unlock();
			s->process_mutex.unlock();
		}
	}

	void OnNotify() override
	{
		for (std::map<Anope::string, LDAPService *>::iterator it = this->LDAPServices.begin(); it != this->LDAPServices.end(); ++it)
		{
			LDAPService *s = it->second;

			LDAPService::query_queue results;
			s->mutex.lock();
			results.swap(s->results);
			s->mutex.unlock();

			for (const auto *req : results)
			{
				LDAPInterface *li = req->inter;
				LDAPResult *r = req->result;

				if (li != NULL)
				{
					if (!r->getError().empty())
					{
						Log(this) << "Error running LDAP query: " << r->getError();
						li->OnError(*r);
					}
					else
						li->OnResult(*r);
				}

				delete req;
			}
		}
	}
};

int LDAPBind::run()
{
	berval cred;
	cred.bv_val = strdup(pass.c_str());
	cred.bv_len = pass.length();

	int i = ldap_sasl_bind_s(service->GetConnection(), LDAP_STR(who), LDAP_SASL_SIMPLE, &cred, NULL, NULL, NULL);

	free(cred.bv_val);

	return i;
}

int LDAPSearchRequest::run()
{
	return ldap_search_ext_s(service->GetConnection(), LDAP_STR(base), LDAP_SCOPE_SUBTREE, LDAP_STR(filter), NULL, 0, NULL, NULL, LDAP_TIME(tv), 0, &message);
}

int LDAPAdd::run()
{
	LDAPMod **mods = LDAPService::BuildMods(attributes);
	int i = ldap_add_ext_s(service->GetConnection(), LDAP_STR(dn), mods, NULL, NULL);
	LDAPService::FreeMods(mods);
	return i;
}

int LDAPDel::run()
{
	return ldap_delete_ext_s(service->GetConnection(), LDAP_STR(dn), NULL, NULL);
}

int LDAPModify::run()
{
	LDAPMod **mods = LDAPService::BuildMods(attributes);
	int i = ldap_modify_ext_s(service->GetConnection(), LDAP_STR(base), mods, NULL, NULL);
	LDAPService::FreeMods(mods);
	return i;
}

MODULE_INIT(ModuleLDAP)
