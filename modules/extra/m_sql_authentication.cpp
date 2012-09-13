#include "module.h"
#include "sql.h"

class SQLAuthenticationResult : public SQLInterface
{
	dynamic_reference<Command> cmd;
	CommandSource source;
	std::vector<Anope::string> params;
	dynamic_reference<User> user;
	Anope::string account;

 public:
	SQLAuthenticationResult(Module *m, Command *c, CommandSource &s, const std::vector<Anope::string> &p, User *u, const Anope::string &a) : SQLInterface(m), cmd(c), source(s), params(p), user(u), account(a) { }

	void OnResult(const SQLResult &r) anope_override
	{
		if (user && cmd)
		{
			Anope::string email;

			if (r.Rows() > 0)
			{
				user->Extend("m_sql_authentication_success", NULL);

				try
				{
					email = r.Get(0, "email");
				}
				catch (const SQLException &) { }
			}
			else
				user->Extend("m_sql_authentication_failed", NULL);

			BotInfo *bi = findbot(Config->NickServ);
			NickAlias *na = findnick(account);
			if (na == NULL)
			{
				na = new NickAlias(account, new NickCore(account));
				if (Config->NSAddAccessOnReg)
					na->nc->AddAccess(create_mask(user));
		
				if (bi)
					user->SendMessage(bi, _("Your account \002%s\002 has been successfully created."), na->nick.c_str());
			}

			if (!email.empty() && email != na->nc->email)
			{
				na->nc->email = email;
				if (bi)
					user->SendMessage(bi, _("Your email has been updated to \002%s\002."), email.c_str());
			}

			cmd->Execute(source, params);
		}

		delete this;
	}

	void OnError(const SQLResult &r) anope_override
	{
		Log() << "m_sql_authentication: Error executing query " << r.GetQuery().query << ": " << r.GetError();

		if (user && cmd)
		{
			user->Extend("m_sql_authentication_failed", NULL);
			cmd->Execute(source, params);
		}

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

	EventReturn OnCheckAuthentication(Command *c, CommandSource *source, const std::vector<Anope::string> &params, const Anope::string &account, const Anope::string &password) anope_override
	{
		if (!source || !source->GetUser())
			return EVENT_CONTINUE;
		else if (!this->SQL)
		{
			Log() << "m_sql_authentication: Unable to find SQL engine";
			return EVENT_CONTINUE;
		}
		else if (source->GetUser()->HasExt("m_sql_authentication_success"))
		{
			source->GetUser()->Shrink("m_sql_authentication_success");
			return EVENT_ALLOW;
		}
		else if (source->GetUser()->HasExt("m_sql_authentication_failed"))
		{
			source->GetUser()->Shrink("m_sql_authentication_failed");
			return EVENT_CONTINUE;
		}

		SQLQuery q(this->query);
		q.setValue("a", account);
		q.setValue("p", password);
		q.setValue("n", source->GetNick());
		q.setValue("i", source->GetUser()->ip);

		this->SQL->Run(new SQLAuthenticationResult(this, c, *source, params, source->GetUser(), account), q);

		Log(LOG_DEBUG) << "m_sql_authentication: Checking authentication for " << account;

		return EVENT_STOP;
	}
};

MODULE_INIT(ModuleSQLAuthentication)
