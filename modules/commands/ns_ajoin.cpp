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

struct AJoinList : std::vector<std::pair<Anope::string, Anope::string> >, ExtensibleItem, Serializable
{
	NickCore *nc;

	AJoinList(NickCore *n) : nc(n) { }

	Anope::string serialize_name() const
	{
		return "AJoinList";
	}

	serialized_data serialize()
	{
		serialized_data sd;

		sd["nc"] << this->nc->display;
		Anope::string channels;
		for (unsigned i = 0; i < this->size(); ++i)
			channels += this->at(i).first + "," + this->at(i).second;
		sd["channels"] << channels;

		return sd;
	}

	static void unserialize(serialized_data &sd)
	{
		NickCore *nc = findcore(sd["nc"].astr());
		if (nc == NULL)
			return;

		AJoinList *aj = new AJoinList(nc);
		nc->Extend("ns_ajoin_channels", aj);

		Anope::string token;
		spacesepstream ssep(sd["channels"].astr());
		while (ssep.GetToken(token))
		{
			size_t c = token.find(',');
			Anope::string chan, key;
			if (c == Anope::string::npos)
				chan = token;
			else
			{
				chan = token.substr(0, c);
				key = token.substr(c + 1);
			}

			aj->push_back(std::make_pair(chan, key));
		}
	}
};

class CommandNSAJoin : public Command
{
	void DoList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		AJoinList *channels = source.u->Account()->GetExt<AJoinList *>("ns_ajoin_channels");

		if (channels == NULL || channels->empty())
			source.Reply(_("Your auto join list is empty."));
		else
		{
			source.Reply(_("Your auto join list:\n"
					"  Num   Channel      Key"));
			for (unsigned i = 0; i < channels->size(); ++i)
				source.Reply(" %3d    %-12s %s", i + 1, channels->at(i).first.c_str(), channels->at(i).second.c_str());
		}
	}

	void DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		AJoinList *channels = source.u->Account()->GetExt<AJoinList *>("ns_ajoin_channels");
		if (channels == NULL)
		{
			channels = new AJoinList(source.u->Account());
			source.u->Account()->Extend("ns_ajoin_channels", channels);
		}

		unsigned i = 0;
		if (channels != NULL)
			for (; i < channels->size(); ++i)
				if (channels->at(i).first.equals_ci(params[1]))
					break;

		if (channels->size() >= Config->AJoinMax)
			source.Reply(_("Your auto join list is full."));
		else if (i != channels->size())
			source.Reply(_("%s is already on your auto join list."), params[1].c_str());
		else if (ircdproto->IsChannelValid(params[1]) == false)
 			source.Reply(CHAN_X_INVALID, params[1].c_str());
		else
		{
			channels->push_back(std::make_pair(params[1], params.size() > 2 ? params[2] : ""));
			source.Reply(_("Added %s to your auto join list."), params[1].c_str());
		}
	}

	void DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		AJoinList *channels = source.u->Account()->GetExt<AJoinList *>("ns_ajoin_channels");

		unsigned i = 0;
		if (channels != NULL)
			for (; i < channels->size(); ++i)
				if (channels->at(i).first.equals_ci(params[1]))
					break;
		
		if (channels == NULL || i == channels->size())
			source.Reply(_("%s was not found on your auto join list."), params[1].c_str());
		else
		{
			channels->erase(channels->begin() + i);
			source.Reply(_("%s was removed from your auto join list."), params[1].c_str());
		}
	}

 public:
	CommandNSAJoin(Module *creator) : Command(creator, "nickserv/ajoin", 1, 3)
	{
		this->SetDesc(_("Manage your auto join list"));
		this->SetSyntax(_("{ADD | DEL | LIST} [\037channel\037] [\037key\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
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

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command manages your auto join list. When you identify\n"
				"you will automatically join the channels on your auto join list"));
		return true;
	}
};

class NSAJoin : public Module
{
	SerializeType ajoinlist_type;
	CommandNSAJoin commandnsajoin;

 public:
	NSAJoin(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		ajoinlist_type("AJoinList", AJoinList::unserialize), commandnsajoin(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnNickIdentify };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnNickIdentify(User *u)
	{
		AJoinList *channels = u->Account()->GetExt<AJoinList *>("ns_ajoin_channels");

		if (channels == NULL)
			return;

		for (unsigned i = 0; i < channels->size(); ++i)
		{
			Channel *c = findchan(channels->at(i).first);
			ChannelInfo *ci = c != NULL ? c->ci : cs_findchan(channels->at(i).first);
			if (c == NULL && ci != NULL)
				c = ci->c;

			bool need_invite = false;
			Anope::string key = channels->at(i).second;
			
			if (ci != NULL)
			{
				if (ci->HasFlag(CI_SUSPENDED))
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
						if (ci->AccessFor(u).HasPriv("GETKEY"))
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
				BotInfo *bi = findbot(Config->NickServ);
				if (!bi || !ci->AccessFor(u).HasPriv("INVITE"))
					continue;
				ircdproto->SendInvite(bi, channels->at(i).first, u->nick);
			}

			ircdproto->SendSVSJoin(Config->NickServ, u->nick, channels->at(i).first, key);
		}
	}
};

MODULE_INIT(NSAJoin)
