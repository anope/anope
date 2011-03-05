/* RequiredLibraries: ldap */

#include "module.h"
#include "ldap.h"
#include <ldap.h>

static Pipe *me;

class LDAPService : public LDAPProvider, public Thread, public Condition
{
	Anope::string server;
	int port;
	Anope::string binddn;
	Anope::string password;

	LDAP *con;

 public:
	typedef std::map<int, LDAPInterface *> query_queue;
	typedef std::vector<std::pair<LDAPInterface *, LDAPResult *> > result_queue;
	query_queue queries;
	result_queue results;

	LDAPService(Module *o, const Anope::string &n, const Anope::string &s, int po, const Anope::string &b, const Anope::string &p) : LDAPProvider(o, "ldap/" + n), server(s), port(po), binddn(b), password(p)
	{
		if (ldap_initialize(&this->con, this->server.c_str()) != LDAP_SUCCESS)
			throw LDAPException("Unable to connect to LDAP service " + this->name + ": " + Anope::LastError());
		static const int version = LDAP_VERSION3;
		if (ldap_set_option(this->con, LDAP_OPT_PROTOCOL_VERSION, &version) != LDAP_OPT_SUCCESS)
			throw LDAPException("Unable to set protocol version for " + this->name + ": " + Anope::LastError());

		threadEngine.Start(this);
	}

	~LDAPService()
	{
		this->Lock();

		for (query_queue::iterator it = this->queries.begin(), it_end = this->queries.end(); it != it_end; ++it)
		{
			ldap_abandon_ext(this->con, it->first, NULL, NULL);
			delete it->second;
		}
		this->queries.clear();

		for (result_queue::iterator it = this->results.begin(), it_end = this->results.end(); it != it_end; ++it)
		{
			it->second->error = "LDAP Interface is going away";
			it->first->OnError(*it->second);
		}
		this->results.clear();

		this->Unlock();

		ldap_unbind_ext(this->con, NULL, NULL);
	}

	LDAPQuery Bind(LDAPInterface *i, const Anope::string &who, const Anope::string &pass)
	{
		berval cred;
		cred.bv_val = strdup(pass.c_str());
		cred.bv_len = pass.length();

		LDAPQuery msgid;
		int ret = ldap_sasl_bind(con, who.c_str(), LDAP_SASL_SIMPLE, &cred, NULL, NULL, &msgid);
		free(cred.bv_val);
		if (ret != LDAP_SUCCESS)
			throw LDAPException(ldap_err2string(ret));

		if (i != NULL)
		{
			this->Lock();
			this->queries[msgid] = i;
			this->Unlock();
		}
		this->Wakeup();

		return msgid;
	}

	LDAPQuery Search(LDAPInterface *i, const Anope::string &base, const Anope::string &filter)
	{
		if (i == NULL)
			throw LDAPException("No interface");

		LDAPQuery msgid;
		int ret = ldap_search_ext(this->con, base.c_str(), LDAP_SCOPE_SUBTREE, filter.c_str(), NULL, 0, NULL, NULL, NULL, 0, &msgid);
		if (ret != LDAP_SUCCESS)
			throw LDAPException(ldap_err2string(ret));

		this->Lock();
		this->queries[msgid] = i;
		this->Unlock();
		this->Wakeup();

		return msgid;
	}

	void Run()
	{
		while (!this->GetExitState())
		{
			if (this->queries.empty())
			{
				this->Lock();
				this->Wait();
				this->Unlock();
				if (this->GetExitState())
					break;
			}

			static struct timeval tv = { 1, 0 };
			LDAPMessage *result;
			int type = ldap_result(this->con, LDAP_RES_ANY, 1, &tv, &result);
			if (type <= 0 || this->GetExitState())
				continue;

			int cur_id = ldap_msgid(result);

			this->Lock();

			query_queue::iterator it = this->queries.find(cur_id);
			if (it == this->queries.end())
			{
				this->Unlock();
				continue;
			}
			LDAPInterface *i = it->second;
			this->queries.erase(it);

			this->Unlock();

			LDAPResult *ldap_result = new LDAPResult();
			ldap_result->id = cur_id;

			for (LDAPMessage *cur = ldap_first_message(this->con, result); cur; cur = ldap_next_message(this->con, cur))
			{
				int cur_type = ldap_msgtype(cur);

				LDAPAttributes attributes;

				switch (cur_type)
				{
					case LDAP_RES_BIND:
					{
						ldap_result->type = LDAPResult::QUERY_BIND;

						int errcode = -1;
						int parse_result = ldap_parse_result(this->con, cur, &errcode, NULL, NULL, NULL, NULL, 0);
						if (parse_result != LDAP_SUCCESS)
							ldap_result->error = ldap_err2string(parse_result);
						else if (errcode != LDAP_SUCCESS)
							ldap_result->error = ldap_err2string(errcode);
						break;
					}
					case LDAP_RES_SEARCH_ENTRY:
					{
						ldap_result->type = LDAPResult::QUERY_SEARCH;

						BerElement *ber = NULL;
						for (char *attr = ldap_first_attribute(this->con, cur, &ber); attr; attr = ldap_next_attribute(this->con, cur, ber))
						{
							berval **vals = ldap_get_values_len(this->con, cur, attr);
							int count = ldap_count_values_len(vals);

							std::vector<Anope::string> attrs;
							for (int j = 0; j < count; ++j)
								attrs.push_back(vals[j]->bv_val);
							attributes[attr] = attrs;

							ldap_memfree(attr);
						}
						if (ber != NULL)
							ber_free(ber, 0);

						break;
					}
					default:
						Log(LOG_DEBUG) << "m_ldap: Unknown msg type " << cur_type;
						continue;
				}

				ldap_result->messages.push_back(attributes);
			}

			ldap_msgfree(result);

			this->Lock();
			this->results.push_back(std::make_pair(i, ldap_result));
			this->Unlock();

			me->Notify();
		}
	}

	void SetExitState()
	{
		ModuleManager::UnregisterService(this);
		Thread::SetExitState();
	}
};

class ModuleLDAP : public Module, public Pipe
{
	std::map<Anope::string, LDAPService *> LDAPServices;
 public:

	ModuleLDAP(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		me = this;

		Implementation i[] = { I_OnReload, I_OnModuleUnload };
		ModuleManager::Attach(i, this, 2);

		OnReload(true);
	}

	~ModuleLDAP()
	{
		for (std::map<Anope::string, LDAPService *>::iterator it = this->LDAPServices.begin(); it != this->LDAPServices.end(); ++it)
		{
			it->second->SetExitState();
			it->second->Wakeup();
		}
		LDAPServices.clear();
	}

	void OnReload(bool startup)
	{
		ConfigReader config;
		int i, num;

		for (std::map<Anope::string, LDAPService *>::iterator it = this->LDAPServices.begin(); it != this->LDAPServices.end();)
		{
			const Anope::string &cname = it->first;
			LDAPService *s = it->second;
			++it;

			for (i = 0, num = config.Enumerate("ldap"); i < num; ++i)
			{
				if (config.ReadValue("ldap", "name", "main", i) == cname)
				{
					break;
				}
			}

			if (i == num)
			{
				Log(LOG_NORMAL, "ldap") << "LDAP: Removing server connection " << cname;

				s->SetExitState();
				s->Wakeup();
				this->LDAPServices.erase(cname);
			}
		}

		for (i = 0, num = config.Enumerate("ldap"); i < num; ++i)
		{
			Anope::string connname = config.ReadValue("ldap", "name", "main", i);

			if (this->LDAPServices.find(connname) == this->LDAPServices.end())
			{
				Anope::string server = config.ReadValue("ldap", "server", "127.0.0.1", i);
				int port = config.ReadInteger("ldap", "port", "389", i, true);
				Anope::string binddn = config.ReadValue("ldap", "binddn", "", i);
				Anope::string password = config.ReadValue("ldap", "password", "", i);

				try
				{
					LDAPService *ss = new LDAPService(this, connname, server, port, binddn, password);
					this->LDAPServices.insert(std::make_pair(connname, ss));
					ModuleManager::RegisterService(ss);

					Log(LOG_NORMAL, "ldap") << "LDAP: Successfully connected to server " << connname << " (" << server << ")";
				}
				catch (const LDAPException &ex)
				{
					Log(LOG_NORMAL, "ldap") << "LDAP: " << ex.GetReason();
				}
			}
		}
	}

	void OnModuleUnload(User *, Module *m)
	{
		for (std::map<Anope::string, LDAPService *>::iterator it = this->LDAPServices.begin(); it != this->LDAPServices.end(); ++it)
		{
			LDAPService *s = it->second;
			s->Lock();
			for (LDAPService::query_queue::iterator it2 = s->queries.begin(); it2 != s->queries.end();)
			{
				int msgid = it2->first;
				LDAPInterface *i = it2->second;
				++it2;
				if (i->owner == m)
					s->queries.erase(msgid);
			}
			for (unsigned i = s->results.size(); i > 0; --i)
			{
				LDAPInterface *li = s->results[i - 1].first;
				if (li->owner == m)
					s->results.erase(s->results.begin() + i - 1);
			}
			s->Unlock();
		} 
	}

	void OnNotify()
	{
		for (std::map<Anope::string, LDAPService *>::iterator it = this->LDAPServices.begin(); it != this->LDAPServices.end(); ++it)
		{
			LDAPService *s = it->second;

			s->Lock();
			LDAPService::result_queue results = s->results;
			s->results.clear();
			s->Unlock();

			for (unsigned i = 0; i < results.size(); ++i)
			{
				LDAPInterface *li = results[i].first;
				LDAPResult *r = results[i].second;

				if (!r->error.empty())
					li->OnError(*r);
				else
					li->OnResult(*r);
			}
		} 
	}
};

MODULE_INIT(ModuleLDAP)

