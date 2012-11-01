/* OperServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandOSMode : public Command
{
 public:
	CommandOSMode(Module *creator) : Command(creator, "operserv/mode", 2, 2)
	{
		this->SetDesc(_("Change channel modes"));
		this->SetSyntax(_("\037channel\037 \037modes\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &target = params[0];
		const Anope::string &modes = params[1];

		Channel *c = findchan(target);
		if (!c)
			source.Reply(CHAN_X_NOT_IN_USE, target.c_str());
		else if (c->bouncy_modes)
			source.Reply(_("Services is unable to change modes. Are your servers' U:lines configured correctly?"));
		else
		{
			spacesepstream sep(modes);
			Anope::string mode;
			int add = 1;
			Anope::string log_modes, log_params;

			sep.GetToken(mode);
			for (unsigned i = 0; i < mode.length(); ++i)
			{
				char ch = mode[i];

				if (ch == '+')
				{
					add = 1;
					log_modes += "+";
					continue;
				}
				else if (ch == '-')
				{
					add = 0;
					log_modes += "-";
					continue;
				}

				ChannelMode *cm = ModeManager::FindChannelModeByChar(ch);
				if (!cm)
					continue;

				Anope::string param, param_log;
				if (cm->Type != MODE_REGULAR)
				{
					if (!sep.GetToken(param))
						continue;

					param_log = param;

					if (cm->Type == MODE_STATUS)
					{
						User *targ = finduser(param);
						if (targ == NULL || c->FindUser(targ) == NULL)
							continue;
						param = targ->GetUID();
					}
				}

				log_modes += cm->ModeChar;
				if (!param.empty())
					log_params += " " + param_log;

				if (add)
					c->SetMode(source.service, cm, param, false);
				else
					c->RemoveMode(source.service, cm, param, false);
			}

			if (!log_modes.replace_all_cs("+", "").replace_all_cs("-", "").empty())
				Log(LOG_ADMIN, source, this) << log_modes << log_params << " on " << c->name;
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows Services operators to change modes for any channel.\n"
				"Parameters are the same as for the standard /MODE command."));
		return true;
	}
};

class CommandOSUMode : public Command
{
 public:
	CommandOSUMode(Module *creator) : Command(creator, "operserv/umode", 2, 2)
	{
		this->SetDesc(_("Change user modes"));
		this->SetSyntax(_("\037user\037 \037modes\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &target = params[0];
		const Anope::string &modes = params[1];

		User *u2 = finduser(target);
		if (!u2)
			source.Reply(NICK_X_NOT_IN_USE, target.c_str());
		else
		{
			u2->SetModes(source.service, "%s", modes.c_str());
			source.Reply(_("Changed usermodes of \002%s\002 to %s."), u2->nick.c_str(), modes.c_str());

			u2->SendMessage(source.service, _("\002%s\002 changed your usermodes to %s."), source.GetNick().c_str(), modes.c_str());

			Log(LOG_ADMIN, source, this) << modes << " on " << target;
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows Services operators to change modes for any user.\n"
				"Parameters are the same as for the standard /MODE command."));
		return true;
	}
};

class OSMode : public Module
{
	CommandOSMode commandosmode;
	CommandOSUMode commandosumode;

 public:
	OSMode(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandosmode(this), commandosumode(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(OSMode)
