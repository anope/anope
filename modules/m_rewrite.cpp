/*
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

struct Rewrite
{
	Anope::string client, source_message, target_message, desc;

	bool Matches(const std::vector<Anope::string> &message)
	{
		std::vector<Anope::string> sm;
		spacesepstream(this->source_message).GetTokens(sm);

		for (unsigned i = 0; i < sm.size(); ++i)
			if (i >= message.size() || (sm[i] != "$" && !sm[i].equals_ci(message[i])))
				return false;
		
		return true;
	}

	Anope::string Process(CommandSource &source, const std::vector<Anope::string> &params)
	{
		spacesepstream sep(this->target_message);
		Anope::string token, message;

		while (sep.GetToken(token))
		{
			if (token[0] != '$')
				message += " " + token;
			else if (token == "$me")
				message += " " + source.GetNick();
			else
			{
				int num = -1, end = -1;
				try
				{
					Anope::string num_str = token.substr(1);
					size_t hy = num_str.find('-');
					if (hy == Anope::string::npos)
					{
						num = convertTo<int>(num_str);
						end = num + 1;
					}
					else
					{
						num = convertTo<int>(num_str.substr(0, hy));
						if (hy == num_str.length() - 1)
							end = params.size();
						else
							end = convertTo<int>(num_str.substr(hy + 1)) + 1;
					}
				}
				catch (const ConvertException &)
				{
					continue;
				}

				for (int i = num; i < end && static_cast<unsigned>(i) < params.size(); ++i)
					message += " " +  params[i];
			}
		}

		message.trim();
		return message;
	}

	static std::vector<Rewrite> rewrites;
	
	static Rewrite *Find(const Anope::string &client, const Anope::string &cmd)
	{
		for (unsigned i = 0; i < rewrites.size(); ++i)
		{
			Rewrite &r = rewrites[i];

			if ((client.empty() || r.client.equals_ci(client)) && (r.source_message.equals_ci(cmd) || r.source_message.find_ci(cmd + " ") == 0))
				return &r;
		}

		return NULL;
	}

	static Rewrite *Match(const Anope::string &client, const std::vector<Anope::string> &params)
	{
		for (unsigned i = 0; i < rewrites.size(); ++i)
		{
			Rewrite &r = rewrites[i];

			if ((client.empty() || r.client.equals_ci(client)) && r.Matches(params))
				return &r;
		}

		return NULL;
	}
};

std::vector<Rewrite> Rewrite::rewrites;

class RewriteCommand : public Command
{
 public:
	RewriteCommand(Module *creator) : Command(creator, "rewrite", 0, 0) { }

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		std::vector<Anope::string> full_params = params;
		full_params.insert(full_params.begin(), source.command);

		Rewrite *r = Rewrite::Match(!source.c ? source.service->nick : "", full_params);
		if (r != NULL)
		{
			Anope::string new_message = r->Process(source, full_params);
			Log(LOG_DEBUG) << "m_rewrite: Rewrote '" << source.command << (!params.empty() ? " " + params[0] : "") << "' to '" << new_message << "' using '" << r->source_message << "'";
			source.service = BotInfo::Find(r->client);
			if (!source.service)
				return;
			RunCommand(source, new_message);
		}
		else
			Log() << "m_rewrite: Unable to rewrite '" << source.command << (!params.empty() ? " " + params[0] : "") << "'";
	}

	void OnServHelp(CommandSource &source) anope_override
	{
		Rewrite *r = Rewrite::Find(!source.c ? source.service->nick : "", source.command);
		if (r != NULL && !r->desc.empty())
		{
			this->SetDesc(r->desc);
			Command::OnServHelp(source);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		Rewrite *r = Rewrite::Find(!source.c ? source.service->nick : "", source.command);
		if (r != NULL && !r->desc.empty())
		{
			source.Reply(r->desc);
			size_t sz = r->target_message.find(' ');
			source.Reply(_("This command is an alias to the command %s."), sz != Anope::string::npos ? r->target_message.substr(0, sz).c_str() : r->target_message.c_str());
			return true;
		}
		return false;
	}
};

class ModuleRewrite : public Module
{
	RewriteCommand cmdrewrite;

 public:
	ModuleRewrite(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR), cmdrewrite(this)
	{
		Implementation i[] = { I_OnReload };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnReload(ServerConfig *conf, ConfigReader &reader) anope_override
	{

		Rewrite::rewrites.clear();

		for (int i = 0; i < reader.Enumerate("command"); ++i)
		{
			if (!reader.ReadFlag("command", "rewrite", "no", i))
				continue;

			Rewrite rw;

			rw.client = reader.ReadValue("command", "service", "", i);
			rw.source_message = reader.ReadValue("command", "rewrite_source", "", i),
			rw.target_message = reader.ReadValue("command", "rewrite_target", "", i);
			rw.desc = reader.ReadValue("command", "rewrite_description", "", i);

			if (rw.client.empty() || rw.source_message.empty() || rw.target_message.empty())
				continue;

			Rewrite::rewrites.push_back(rw);
		}
	}
};

MODULE_INIT(ModuleRewrite)
