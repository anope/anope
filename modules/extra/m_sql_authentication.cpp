#include "module.h"
#include "sql.h"

static Module *me;

class SQLAuthenticationResult : public SQLInterface
{
	dynamic_reference<User> user;
	IdentifyRequest *req;

 public:
	SQLAuthenticationResult(User *u, IdentifyRequest *r) : SQLInterface(me), user(u), req(r)
	{
		req->Hold(me);
	}

	~SQLAuthenticationResult()
	{
		req->Release(me);
	}

	void OnResult(const SQLResult &r) anope_override
	{
		if (r.Rows() == 0)
		{
			Log(LOG_DEBUG) << "m_sql_authentication: Unsuccessful authentication for " << req->GetAccount();
			delete this;
			return;
		}

		Log(LOG_DEBUG) << "m_sql_authentication: Successful authentication for " << req->GetAccount();

		Anope::string email;
		try
		{
			email = r.Get(0, "email");
		}
		catch (const SQLException &) { }

		const BotInfo *bi = findbot(Config->NickServ);
		NickAlias *na = findnick(req->GetAccount());
		if (na == NULL)
		{
			na = new NickAlias(req->GetAccount(), new NickCore(req->GetAccount()));
			if (user)
			{
				if (Config->NSAddAccessOnReg)
					na->nc->AddAccess(create_mask(user));
		
				if (bi)
					user->SendMessage(bi, _("Your account \002%s\002 has been successfully created."), na->nick.c_str());
			}
		}

		if (!email.empty() && email != na->nc->email)
		{
			na->nc->email = email;
			if (user && bi)
				user->SendMessage(bi, _("Your email has been updated to \002%s\002."), email.c_str());
		}

		req->Success(me);
		delete this;
	}

	void OnError(const SQLResult &r) anope_override
	{
		Log(this->owner) << "m_sql_authentication: Error executing query " << r.GetQuery().query << ": " << r.GetError();
		delete this;
	}
};

class ModuleSQLAuthentication : public Module
{
	Anope::string engine;
	Anope::string query;
	bool disable_register;
	Anope::string disable_reason;

	service_reference<SQLProvider> SQL;

 public:
	ModuleSQLAuthentication(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED)
	{
		this->SetAuthor("Anope");

		me = this;

		Implementation i[] = { I_OnReload, I_OnPreCommand, I_OnCheckAuthentication };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}

	void OnReload() anope_override
	{
		ConfigReader config;

		this->engine = config.ReadValue("m_sql_authentication", "engine", "", 0);
		this->query = config.ReadValue("m_sql_authentication", "query", "", 0);
		this->disable_register = config.ReadFlag("m_sql_authentication", "disable_ns_register", "false", 0);
		this->disable_reason = config.ReadValue("m_sql_authentication", "disable_reason", "", 0);

		this->SQL = service_reference<SQLProvider>("SQLProvider", this->engine);
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) anope_override
	{
		if (this->disable_register && !this->disable_reason.empty() && command->name == "nickserv/register")
		{
			source.Reply(this->disable_reason);
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnCheckAuthentication(User *u, IdentifyRequest *req) anope_override
	{
		if (!this->SQL)
		{
			Log(this) << "Unable to find SQL engine";
			return;
		}

		SQLQuery q(this->query);
		q.setValue("a", req->GetAccount());
		q.setValue("p", req->GetPassword());
		if (u)
		{
			q.setValue("n", u->nick);
			q.setValue("i", u->ip);
		}
		else
		{
			q.setValue("n", "");
			q.setValue("i", "");
		}


		this->SQL->Run(new SQLAuthenticationResult(u, req), q);

		Log(LOG_DEBUG) << "m_sql_authentication: Checking authentication for " << req->GetAccount();
	}
};

MODULE_INIT(ModuleSQLAuthentication)
