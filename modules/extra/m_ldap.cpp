/* RequiredLibraries: ldap,lber */

#include "module.h"
#include "ldapapi.h"
#include <ldap.h>

static Pipe *me;

class LDAPService : public LDAPProvider, public Thread, public Condition
{
	Anope::string server;
	int port;
	Anope::string admin_binddn;
	Anope::string admin_pass;

	LDAP *con;

	time_t last_connect;

	LDAPMod **BuildMods(const LDAPMods &attributes)
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

	void FreeMods(LDAPMod **mods)
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

	void Reconnect()
	{
		/* Only try one connect a minute. It is an expensive blocking operation */
		if (last_connect > Anope::CurTime - 60)
			throw LDAPException("Unable to connect to LDAP service " + this->name + ": reconnecting too fast");
		last_connect = Anope::CurTime;

		ldap_unbind_ext(this->con, NULL, NULL);
		int i = ldap_initialize(&this->con, this->server.c_str());
		if (i != LDAP_SUCCESS)
			throw LDAPException("Unable to connect to LDAP service " + this->name + ": " + ldap_err2string(i));
	}

 public:
	typedef std::map<int, LDAPInterface *> query_queue;
	typedef std::vector<std::pair<LDAPInterface *, LDAPResult *> > result_queue;
	query_queue queries;
	result_queue results;

	LDAPService(Module *o, const Anope::string &n, const Anope::string &s, int po, const Anope::string &b, const Anope::string &p) : LDAPProvider(o, "ldap/" + n), server(s), port(po), admin_binddn(b), admin_pass(p), last_connect(0)
	{
		int i = ldap_initialize(&this->con, this->server.c_str());
		if (i != LDAP_SUCCESS)
			throw LDAPException("Unable to connect to LDAP service " + this->name + ": " + ldap_err2string(i));
		static const int version = LDAP_VERSION3;
		i = ldap_set_option(this->con, LDAP_OPT_PROTOCOL_VERSION, &version);
		if (i != LDAP_OPT_SUCCESS)
			throw LDAPException("Unable to set protocol version for " + this->name + ": " + ldap_err2string(i));
	}

	~LDAPService()
	{
		this->Lock();

		for (query_queue::iterator it = this->queries.begin(), it_end = this->queries.end(); it != it_end; ++it)
		{
			ldap_abandon_ext(this->con, it->first, NULL, NULL);
			it->second->OnDelete();
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
	
	LDAPQuery BindAsAdmin(LDAPInterface *i)
	{
		return this->Bind(i, this->admin_binddn, this->admin_pass);
	}

	LDAPQuery Bind(LDAPInterface *i, const Anope::string &who, const Anope::string &pass) anope_override
	{
		berval cred;
		cred.bv_val = strdup(pass.c_str());
		cred.bv_len = pass.length();

		LDAPQuery msgid;
		int ret = ldap_sasl_bind(con, who.c_str(), LDAP_SASL_SIMPLE, &cred, NULL, NULL, &msgid);
		free(cred.bv_val);
		if (ret != LDAP_SUCCESS)
		{
			if (ret == LDAP_SERVER_DOWN || ret == LDAP_TIMEOUT)
			{
				this->Reconnect();
				return this->Bind(i, who, pass);
			}
			else
				throw LDAPException(ldap_err2string(ret));
		}

		if (i != NULL)
		{
			this->Lock();
			this->queries[msgid] = i;
			this->Unlock();
		}
		this->Wakeup();

		return msgid;
	}

	LDAPQuery Search(LDAPInterface *i, const Anope::string &base, const Anope::string &filter) anope_override
	{
		if (i == NULL)
			throw LDAPException("No interface");

		LDAPQuery msgid;
		int ret = ldap_search_ext(this->con, base.c_str(), LDAP_SCOPE_SUBTREE, filter.c_str(), NULL, 0, NULL, NULL, NULL, 0, &msgid);
		if (ret != LDAP_SUCCESS)
		{
			if (ret == LDAP_SERVER_DOWN || ret == LDAP_TIMEOUT)
			{
				this->Reconnect();
				return this->Search(i, base, filter);
			}
			else
				throw LDAPException(ldap_err2string(ret));
		}

		this->Lock();
		this->queries[msgid] = i;
		this->Unlock();
		this->Wakeup();

		return msgid;
	}

	LDAPQuery Add(LDAPInterface *i, const Anope::string &dn, LDAPMods &attributes) anope_override
	{
		LDAPMod **mods = this->BuildMods(attributes);
		LDAPQuery msgid;
		int ret = ldap_add_ext(this->con, dn.c_str(), mods, NULL, NULL, &msgid);
		this->FreeMods(mods);

		if (ret != LDAP_SUCCESS)
		{
			if (ret == LDAP_SERVER_DOWN || ret == LDAP_TIMEOUT)
			{
				this->Reconnect();
				return this->Add(i, dn, attributes);
			}
			else
				throw LDAPException(ldap_err2string(ret));
		}

		if (i != NULL)
		{
			this->Lock();
			this->queries[msgid] = i;
			this->Unlock();
		}
		this->Wakeup();

		return msgid;
	}

	LDAPQuery Del(LDAPInterface *i, const Anope::string &dn) anope_override
	{
		LDAPQuery msgid;
		int ret = ldap_delete_ext(this->con, dn.c_str(), NULL, NULL, &msgid);

		if (ret != LDAP_SUCCESS)
		{
			if (ret == LDAP_SERVER_DOWN || ret == LDAP_TIMEOUT)
			{
				this->Reconnect();
				return this->Del(i, dn);
			}
			else
				throw LDAPException(ldap_err2string(ret));
		}

		if (i != NULL)
		{
			this->Lock();
			this->queries[msgid] = i;
			this->Unlock();
		}
		this->Wakeup();

		return msgid;
	}

	LDAPQuery Modify(LDAPInterface *i, const Anope::string &base, LDAPMods &attributes) anope_override
	{
		LDAPMod **mods = this->BuildMods(attributes);
		LDAPQuery msgid;
		int ret = ldap_modify_ext(this->con, base.c_str(), mods, NULL, NULL, &msgid);
		this->FreeMods(mods);

		if (ret != LDAP_SUCCESS)
		{
			if (ret == LDAP_SERVER_DOWN || ret == LDAP_TIMEOUT)
			{
				this->Reconnect();
				return this->Modify(i, base, attributes);
			}
			else
				throw LDAPException(ldap_err2string(ret));
		}

		if (i != NULL)
		{
			this->Lock();
			this->queries[msgid] = i;
			this->Unlock();
		}
		this->Wakeup();

		return msgid;
	}

	void Run() anope_override
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
			int rtype = ldap_result(this->con, LDAP_RES_ANY, 1, &tv, &result);
			if (rtype <= 0 || this->GetExitState())
				continue;

			int cur_id = ldap_msgid(result);

			this->Lock();

			query_queue::iterator it = this->queries.find(cur_id);
			if (it == this->queries.end())
			{
				this->Unlock();
				ldap_msgfree(result);
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

				char *dn = ldap_get_dn(this->con, cur);
				if (dn != NULL)
				{
					attributes["dn"].push_back(dn);
					ldap_memfree(dn);
					dn = NULL;
				}

				switch (cur_type)
				{
					case LDAP_RES_BIND:
						ldap_result->type = LDAPResult::QUERY_BIND;
						break;
					case LDAP_RES_SEARCH_ENTRY:
						ldap_result->type = LDAPResult::QUERY_SEARCH;
						break;
					case LDAP_RES_ADD:
						ldap_result->type = LDAPResult::QUERY_ADD;
						break;
					case LDAP_RES_DELETE:
						ldap_result->type = LDAPResult::QUERY_DELETE;
						break;
					case LDAP_RES_MODIFY:
						ldap_result->type = LDAPResult::QUERY_MODIFY;
						break;
					case LDAP_RES_SEARCH_RESULT:
						// If we get here and ldap_result->type is LDAPResult::QUERY_UNKNOWN
						// then the result set is empty
						ldap_result->type = LDAPResult::QUERY_SEARCH;
						break;
					default:
						Log(LOG_DEBUG) << "m_ldap: Unknown msg type " << cur_type;
						continue;
				}

				switch (cur_type)
				{
					case LDAP_RES_BIND:
					{
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

						break;
					}
					case LDAP_RES_ADD:
					case LDAP_RES_DELETE:
					case LDAP_RES_MODIFY:
					{
						int errcode = -1;
						int parse_result = ldap_parse_result(this->con, cur, &errcode, NULL, NULL, NULL, NULL, 0);
						if (parse_result != LDAP_SUCCESS)
							ldap_result->error = ldap_err2string(parse_result);
						else if (errcode != LDAP_SUCCESS)
							ldap_result->error = ldap_err2string(errcode);
						break;
					}
					default:
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
};

class ModuleLDAP : public Module, public Pipe
{
	std::map<Anope::string, LDAPService *> LDAPServices;
 public:

	ModuleLDAP(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR)
	{
		me = this;

		Implementation i[] = { I_OnReload, I_OnModuleUnload };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	~ModuleLDAP()
	{
		for (std::map<Anope::string, LDAPService *>::iterator it = this->LDAPServices.begin(); it != this->LDAPServices.end(); ++it)
		{
			it->second->SetExitState();
			it->second->Wakeup();
			it->second->Join();
			delete it->second;
		}
		LDAPServices.clear();
	}

	void OnReload(Configuration::Conf *conf) anope_override
	{
		int i, num;

		for (std::map<Anope::string, LDAPService *>::iterator it = this->LDAPServices.begin(); it != this->LDAPServices.end();)
		{
			const Anope::string &cname = it->first;
			LDAPService *s = it->second;
			++it;

			for (i = 0; i < Config->CountBlock("ldap"); ++i)
				if (Config->GetBlock("ldap", i)->Get<const Anope::string &>("name", "ldap/main") == cname)
					break;

			if (i == num)
			{
				Log(LOG_NORMAL, "ldap") << "LDAP: Removing server connection " << cname;

				s->SetExitState();
				s->Wakeup();
				this->LDAPServices.erase(cname);
			}
		}

		for (i = 0; i < Config->CountBlock("ldap"); ++i)
		{
			const Anope::string &connname = Config->GetBlock("ldap", i)->Get<const Anope::string &>("name", "ldap/main");

			if (this->LDAPServices.find(connname) == this->LDAPServices.end())
			{
				const Anope::string &server = Config->GetBlock("ldap", i)->Get<const Anope::string &>("server", "127.0.0.1");
				int port = Config->GetBlock("ldap", i)->Get<int>("port", "389");
				const Anope::string &admin_binddn = Config->GetBlock("ldap", i)->Get<const Anope::string &>("admin_binddn");
				const Anope::string &admin_password = Config->GetBlock("ldap", i)->Get<const Anope::string &>("admin_password");

				try
				{
					LDAPService *ss = new LDAPService(this, connname, server, port, admin_binddn, admin_password);
					ss->Start();
					this->LDAPServices.insert(std::make_pair(connname, ss));

					Log(LOG_NORMAL, "ldap") << "LDAP: Successfully connected to server " << connname << " (" << server << ")";
				}
				catch (const LDAPException &ex)
				{
					Log(LOG_NORMAL, "ldap") << "LDAP: " << ex.GetReason();
				}
			}
		}
	}

	void OnModuleUnload(User *, Module *m) anope_override
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
				{
					i->OnDelete();
					s->queries.erase(msgid);
				}
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

	void OnNotify() anope_override
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

