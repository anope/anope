/*
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

struct Rewrite final
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
				Anope::string num_str = token.substr(1);
				size_t hy = num_str.find('-');
				if (hy == Anope::string::npos)
				{
					auto n = Anope::TryConvert<int>(num_str);
					if (!n.has_value())
						continue;

					num = n.value();
					end = num + 1;
				}
				else
				{
					auto n = Anope::TryConvert<int>(num_str.substr(0, hy));
					if (!n.has_value())
						continue;

					num = n.value();
					if (hy == num_str.length() - 1)
						end = params.size();
					else
					{
						n = Anope::TryConvert<int>(num_str.substr(hy + 1));
						if (!n.has_value())
							continue;

						end = n.value() + 1;
					}
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
		for (auto &r : rewrites)
		{
			if ((client.empty() || r.client.equals_ci(client)) && (r.source_message.equals_ci(cmd) || r.source_message.find_ci(cmd + " ") == 0))
				return &r;
		}

		return NULL;
	}

	static Rewrite *Match(const Anope::string &client, const std::vector<Anope::string> &params)
	{
		for (auto &r : rewrites)
		{
			if ((client.empty() || r.client.equals_ci(client)) && r.Matches(params))
				return &r;
		}

		return NULL;
	}
};

std::vector<Rewrite> Rewrite::rewrites;

class RewriteCommand final
	: public Command
{
public:
	RewriteCommand(Module *creator) : Command(creator, "rewrite", 0, 0) { }

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		std::vector<Anope::string> full_params = params;
		full_params.insert(full_params.begin(), source.command);

		Rewrite *r = Rewrite::Match(!source.c ? source.service->nick : "", full_params);
		if (r != NULL)
		{
			Anope::string new_message = r->Process(source, full_params);
			Log(LOG_DEBUG) << "rewrite: Rewrote '" << source.command << (!params.empty() ? " " + params[0] : "") << "' to '" << new_message << "' using '" << r->source_message << "'";
			source.service = BotInfo::Find(r->client, true);
			if (!source.service)
				return;
			Command::Run(source, new_message);
		}
		else
			Log() << "rewrite: Unable to rewrite '" << source.command << (!params.empty() ? " " + params[0] : "") << "'";
	}

	void OnServHelp(CommandSource &source, HelpWrapper &help) override
	{
		Rewrite *r = Rewrite::Find(!source.c ? source.service->nick : "", source.command);
		if (r != NULL && !r->desc.empty())
		{
			this->SetDesc(r->desc);
			Command::OnServHelp(source, help);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
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

class ModuleRewrite final
	: public Module
{
	RewriteCommand cmdrewrite;

public:
	ModuleRewrite(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR | EXTRA), cmdrewrite(this)
	{
	}

	void OnReload(Configuration::Conf &conf) override
	{
		Rewrite::rewrites.clear();

		for (int i = 0; i < conf.CountBlock("command"); ++i)
		{
			const auto &block = conf.GetBlock("command", i);

			if (!block.Get<bool>("rewrite"))
				continue;

			Rewrite rw;

			rw.client = block.Get<const Anope::string>("service");
			rw.source_message = block.Get<const Anope::string>("rewrite_source");
			rw.target_message = block.Get<const Anope::string>("rewrite_target");
			rw.desc = block.Get<const Anope::string>("rewrite_description");

			if (rw.client.empty() || rw.source_message.empty() || rw.target_message.empty())
				continue;

			Rewrite::rewrites.push_back(rw);
		}
	}
};

MODULE_INIT(ModuleRewrite)
