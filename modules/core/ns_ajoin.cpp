/* NickServ core functions
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

class CommandNSAJoin : public Command
{
	void DoList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		std::vector<std::pair<Anope::string, Anope::string> > channels;
		source.u->Account()->GetExtRegular("ns_ajoin_channels", channels);

		if (channels.empty())
			source.Reply(_("Your auto join list is empty."));
		else
		{
			source.Reply(_("Your auto join list:\n"
					"  Num   Channel      Key"));
			for (unsigned i = 0; i < channels.size(); ++i)
				source.Reply(" %3d    %-12s %s", i + 1, channels[i].first.c_str(), channels[i].second.c_str());
		}
	}

	void DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		std::vector<std::pair<Anope::string, Anope::string> > channels;
		source.u->Account()->GetExtRegular("ns_ajoin_channels", channels);

		if (channels.size() >= Config->AJoinMax)
			source.Reply(_("Your auto join list is full."));
		else if (ircdproto->IsChannelValid(params[1]) == false)
 			source.Reply(LanguageString::CHAN_X_INVALID, params[1].c_str());
		else
		{
			channels.push_back(std::make_pair(params[1], params.size() > 2 ? params[2] : ""));
			source.Reply(_("Added %s to your auto join list."), params[1].c_str());
			source.u->Account()->Extend("ns_ajoin_channels", new ExtensibleItemRegular<std::vector<
				std::pair<Anope::string, Anope::string> > >(channels));
		}
	}

	void DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		std::vector<std::pair<Anope::string, Anope::string> > channels;
		source.u->Account()->GetExtRegular("ns_ajoin_channels", channels);

		unsigned i;
		for (i = 0; i < channels.size(); ++i)
			if (channels[i].first.equals_ci(params[1]))
				break;
		
		if (i == channels.size())
			source.Reply(_("%s was not found on your auto join list."), params[1].c_str());
		else
		{
			channels.erase(channels.begin() + i);
			source.u->Account()->Extend("ns_ajoin_channels", new ExtensibleItemRegular<std::vector<
				std::pair<Anope::string, Anope::string> > >(channels));
			source.Reply(_("%s was removed from your auto join list."), params[1].c_str());
		}
	}

 public:
	CommandNSAJoin() : Command("AJOIN", 1, 3)
	{
		this->SetDesc("Manage your auto join list");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (params[0].equals_ci("LIST"))
			this->DoList(source, params);
		else if (params.size() < 2)
			this->OnSyntaxError(source, "");
		else if (params[0].equals_ci("ADD"))
			this->DoAdd(source, params);
		else if (params[0].equals_ci("DEL"))
			this->DoDel(source, params);
		else
			this->OnSyntaxError(source, "");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002AJOIN {ADD | DEL | LIST} [\037channel\037] [\037key\037]\002\n"
				" \n"
				"This command manages your auto join list. When you identify\n"
				"you will automatically join the channels on your auto join list"));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "AJOIN", _("AJOIN {ADD | DEL | LIST} [\037channel\037] [\037key\037]"));
	}
};

class NSAJoin : public Module
{
	CommandNSAJoin commandnsajoin;

 public:
	NSAJoin(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsajoin);

		Implementation i[] = { I_OnNickIdentify, I_OnDatabaseWriteMetadata, I_OnDatabaseReadMetadata };
		ModuleManager::Attach(i, this, 3);
	}

	void OnNickIdentify(User *u)
	{
		std::vector<std::pair<Anope::string, Anope::string> > channels;
		u->Account()->GetExtRegular("ns_ajoin_channels", channels);

		for (unsigned i = 0; i < channels.size(); ++i)
		{
			Channel *c = findchan(channels[i].first);
			ChannelInfo *ci = c != NULL? c->ci : cs_findchan(channels[i].first);
			if (c == NULL && ci != NULL)
				c = ci->c;

			bool need_invite = false;
			Anope::string key = channels[i].second;
			
			if (ci != NULL)
			{
				if (ci->HasFlag(CI_SUSPENDED) || ci->HasFlag(CI_FORBIDDEN))
					continue;
			}
			if (c != NULL)
			{
				if (c->FindUser(u) != NULL)
					continue;
				else if (c->HasMode(CMODE_OPERONLY) && !u->HasMode(UMODE_OPER))
					continue;
				else if (c->HasMode(CMODE_ADMINONLY) && !u->HasMode(UMODE_ADMIN))
					continue;
				else if (c->HasMode(CMODE_SSL) && !u->HasMode(UMODE_SSL))
					continue;
				else if (matches_list(c, u, CMODE_BAN) == true && matches_list(c, u, CMODE_EXCEPT) == false)
					need_invite = true;
				else if (c->HasMode(CMODE_INVITE) && matches_list(c, u, CMODE_INVITEOVERRIDE) == false)
					need_invite = true;
					
				if (c->HasMode(CMODE_KEY))
				{
					Anope::string k;
					if (c->GetParam(CMODE_KEY, k))
					{
						if (check_access(u, ci, CA_GETKEY))
							key = k;
						else if (key != k)
							need_invite = true;
					}
				}
				if (c->HasMode(CMODE_LIMIT))
				{
					Anope::string l;
					if (c->GetParam(CMODE_LIMIT, l))
					{
						try
						{
							unsigned limit = convertTo<unsigned>(l);
							if (c->users.size() >= limit)
								need_invite = true;
						}
						catch (const ConvertException &) { }
					}
				}
			}

			if (need_invite)
			{
				if (!check_access(u, ci, CA_INVITE))
					continue;
				ircdproto->SendInvite(NickServ, channels[i].first, u->nick);
			}

			ircdproto->SendSVSJoin(NickServ->nick, u->nick, channels[i].first, key);
		}
	}

	void OnDatabaseWriteMetadata(void (*WriteMetadata)(const Anope::string &, const Anope::string &), NickCore *nc)
	{
		std::vector<std::pair<Anope::string, Anope::string> > channels;
		nc->GetExtRegular("ns_ajoin_channels", channels);

		Anope::string chans;
		for (unsigned i = 0; i < channels.size(); ++i)
			chans += " " + channels[i].first + "," + channels[i].second;

		if (!chans.empty())
		{
			chans.erase(chans.begin());
			WriteMetadata("NS_AJOIN", chans);
		}
	}

	EventReturn OnDatabaseReadMetadata(NickCore *nc, const Anope::string &key, const std::vector<Anope::string> &params)
	{
		if (key == "NS_AJOIN")
		{
			std::vector<std::pair<Anope::string, Anope::string> > channels;
			nc->GetExtRegular("ns_ajoin_channels", channels);

			for (unsigned i = 0; i < params.size(); ++i)
			{
				Anope::string chan, chankey;
				commasepstream sep(params[i]);
				sep.GetToken(chan);
				sep.GetToken(chankey);

				channels.push_back(std::make_pair(chan, chankey));
			}

			nc->Extend("ns_ajoin_channels", new ExtensibleItemRegular<std::vector<
				std::pair<Anope::string, Anope::string> > >(channels));

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(NSAJoin)
