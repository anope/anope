/* OperServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandOSConfig : public Command
{
	void ChangeHash(ConfigDataHash &hash, const Anope::string &block, const Anope::string &iname, const Anope::string &value)
	{
		ConfigDataHash::iterator it = hash.find(block);
		
		KeyValList &list = it->second;
		for (unsigned i = 0; i < list.size(); ++i)
		{
			const Anope::string &item = list[i].first;
			if (item == iname)
			{
				list[i] = std::make_pair(item, value);
				break;
			}
		}
	}

 public:
	CommandOSConfig(Module *creator) : Command(creator, "operserv/config", 1, 4, "operserv/config")
	{
		this->SetDesc(_("View and change configuration file settings"));
		this->SetSyntax(_("{\037MODIFY\037|\037VIEW\037} [\037block name\037 \037item name\037 \037item value\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &what = params[0];

		if (what.equals_ci("MODIFY") && params.size() > 3)
		{
			ConfigItems configitems(Config);			

			for (unsigned i = 0; !configitems.Values[i].tag.empty(); ++i)
			{
				ConfigItems::Item *v = &configitems.Values[i];
				if (v->tag.equals_cs(params[1]) && v->value.equals_cs(params[2]))
				{
					try
					{
						ValueItem vi(params[3]);
						if (!v->validation_function(Config, v->tag, v->value, vi))	
							throw ConfigException("Parameter failed to validate.");

						int dt = v->datatype;

						if (dt & DT_NORELOAD)
							throw ConfigException("This item can not be changed while Anope is running.");
						bool allow_wild = dt & DT_ALLOW_WILD;
						dt &= ~(DT_ALLOW_NEWLINE | DT_ALLOW_WILD);

						/* Yay for *massive* copypaste from config.cpp */
						switch (dt)
						{
							case DT_NOSPACES:
							{
								ValueContainerString *vcs = debug_cast<ValueContainerString *>(v->val);
								Config->ValidateNoSpaces(vi.GetValue(), v->tag, v->value);
								vcs->Set(vi.GetValue());
								break;
							}
							case DT_HOSTNAME:
							{
								ValueContainerString *vcs = debug_cast<ValueContainerString *>(v->val);
								Config->ValidateHostname(vi.GetValue(), v->tag, v->value);
								vcs->Set(vi.GetValue());
								break;
							}
							case DT_IPADDRESS:
							{
								ValueContainerString *vcs = debug_cast<ValueContainerString *>(v->val);
								Config->ValidateIP(vi.GetValue(), v->tag, v->value, allow_wild);
								vcs->Set(vi.GetValue());
								break;
							}
							case DT_STRING:
							{
								ValueContainerString *vcs = debug_cast<ValueContainerString *>(v->val);
								vcs->Set(vi.GetValue());
								break;
							}
							case DT_INTEGER:
							{
								int val = vi.GetInteger();
								ValueContainerInt *vci = debug_cast<ValueContainerInt *>(v->val);
								vci->Set(&val, sizeof(int));
								break;
							}
							case DT_UINTEGER:
							{
								unsigned val = vi.GetInteger();
								ValueContainerUInt *vci = debug_cast<ValueContainerUInt *>(v->val);
								vci->Set(&val, sizeof(unsigned));
								break;
							}
							case DT_LUINTEGER:
							{
								unsigned long val = vi.GetInteger();
								ValueContainerLUInt *vci = debug_cast<ValueContainerLUInt *>(v->val);
								vci->Set(&val, sizeof(unsigned long));
								break;
							}
							case DT_TIME:
							{
								time_t time = dotime(vi.GetValue());
								ValueContainerTime *vci = debug_cast<ValueContainerTime *>(v->val);
								vci->Set(&time, sizeof(time_t));
								break;
							}
							case DT_BOOLEAN:
							{
								bool val = vi.GetBool();
								ValueContainerBool *vcb = debug_cast<ValueContainerBool *>(v->val);
								vcb->Set(&val, sizeof(bool));
								break;
							}
							default:
								break;
						}
					}
					catch (const ConfigException &ex)
					{
						source.Reply(_("Error changing configuration value: ") + ex.GetReason());
						return;
					}

					ChangeHash(Config->config_data, params[1], params[2], params[3]);

					Log(LOG_ADMIN, source.u, this) << "to change the configuration value of " << params[1] << ":" << params[2] << " to " << params[3];
					source.Reply(_("Value of %s:%s changed to %s"), params[1].c_str(), params[2].c_str(), params[3].c_str());
					return;
				}
			}

			source.Reply("There is no configuration value named %s:%s", params[1].c_str(), params[2].c_str());
		}
		else if (what.equals_ci("VIEW"))
		{
			/* Blocks we should show */
			const Anope::string show_blocks[] = { "botserv", "chanserv", "defcon", "global", "memoserv", "nickserv", "networkinfo", "operserv", "options", "" };

			Log(LOG_ADMIN, source.u, this) << "VIEW";

			for (ConfigDataHash::const_iterator it = Config->config_data.begin(), it_end = Config->config_data.end(); it != it_end; ++it)
			{
				const Anope::string &bname = it->first;
				const KeyValList &list = it->second;

				bool ok = false;
				for (unsigned i = 0; !show_blocks[i].empty(); ++i)
					if (bname == show_blocks[i])
						ok = true;
				if (ok == false)
					continue;

				source.Reply(_("%s settings:"), bname.c_str());
				for (unsigned i = 0; i < list.size(); ++i)
				{
					const Anope::string &first = list[i].first, second = list[i].second;

					source.Reply(_("  Name: %-15s Value: %s"), first.c_str(), second.c_str());
				}
			}

			source.Reply(_("End of configuration."));
		}
		else
			this->OnSyntaxError(source, what);

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to change and view configuration settings.\n"
				"Settings changed by this command are temporary and will not be reflected\n"
				"back into the configuration file, and will be lost if Anope is shut down,\n"
				"restarted, or the RELOAD command is used.\n"
				" \n"
				"Example:\n"
				"     \002MODIFY nickserv forcemail no\002"));
		return true;
	}
};

class OSConfig : public Module
{
	CommandOSConfig commandosconfig;

 public:
	OSConfig(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandosconfig(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandosconfig);
	}
};

MODULE_INIT(OSConfig)
