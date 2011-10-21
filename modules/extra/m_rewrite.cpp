/*
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

struct Rewrite
{
	Anope::string client, source_message, target_message;

	bool Matches(const std::vector<Anope::string> &message)
	{
		std::vector<Anope::string> sm = BuildStringVector(this->source_message);

		for (unsigned i = 0; i < sm.size(); ++i)
			if (sm[i] != "$" && !sm[i].equals_ci(message[i]))
				return false;
		
		return true;
	}

	Anope::string Process(const std::vector<Anope::string> &params)
	{
		spacesepstream sep(this->target_message);
		Anope::string token, message;

		while (sep.GetToken(token))
		{
			if (token[0] != '$')
				message += " " + token;
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
};

class ModuleRewrite : public Module
{
	std::vector<Rewrite> rewrites;

 public:
	ModuleRewrite(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnBotPrivmsg };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
		ModuleManager::SetPriority(this, PRIORITY_FIRST);

		this->OnReload();
	}

	void OnReload()
	{
		ConfigReader config;

		this->rewrites.clear();

		for (int i = 0; i < config.Enumerate("rewrite"); ++i)
		{
			Rewrite rw;

			rw.client = config.ReadValue("rewrite", "client", "", i);
			rw.source_message = config.ReadValue("rewrite", "source_message", "", i),
			rw.target_message = config.ReadValue("rewrite", "target_message", "", i);

			if (rw.client.empty() || rw.source_message.empty() || rw.target_message.empty())
				continue;

			this->rewrites.push_back(rw);
		}
	}

	EventReturn OnBotPrivmsg(User *u, BotInfo *bi, Anope::string &message)
	{
		std::vector<Anope::string> tokens = BuildStringVector(message);
		for (unsigned i = 0; i < this->rewrites.size(); ++i)
		{
			Rewrite &rw = this->rewrites[i];

			if (rw.client == bi->nick && rw.Matches(tokens))
			{
				Anope::string new_message = rw.Process(tokens);
				Log(LOG_DEBUG) << "m_rewrite: Rewrote '" << message << "' to '" << new_message << "'";
				message = new_message;
				break;
			}
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ModuleRewrite)
