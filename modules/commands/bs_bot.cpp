/* BotServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/bs_bot.h"

class CommandBSBot : public Command
{
	EventHandlers<Event::BotCreate> OnBotCreate;
	EventHandlers<Event::BotChange> OnBotChange;
	EventHandlers<Event::BotDelete> OnBotDelete;

	void DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &nick = params[1];
		const Anope::string &user = params[2];
		const Anope::string &host = params[3];
		const Anope::string &real = params[4];

		if (ServiceBot::Find(nick, true))
		{
			source.Reply(_("Bot \002{0}\002 already exists."), nick);
			return;
		}

		Configuration::Block *networkinfo = Config->GetBlock("networkinfo");

		if (nick.length() > networkinfo->Get<unsigned>("nicklen"))
		{
			source.Reply(_("Bot nicks may only be \002{0}\002 characters long."), networkinfo->Get<unsigned>("nicklen"));
			return;
		}

		if (user.length() > networkinfo->Get<unsigned>("userlen"))
		{
			source.Reply(_("Bot idents may only be \002{0}\002 characters long."), networkinfo->Get<unsigned>("userlen"));
			return;
		}

		if (host.length() > networkinfo->Get<unsigned>("hostlen"))
		{
			source.Reply(_("Bot hosts may only be \002{0}\002 characters long."), networkinfo->Get<unsigned>("hostlen"));
			return;
		}

		if (!IRCD->IsNickValid(nick))
		{
			source.Reply(_("Bot nicks may only contain valid nick characters."));
			return;
		}

		if (!IRCD->IsIdentValid(user))
		{
			source.Reply(_("Bot idents may only contain valid ident characters."));
			return;
		}

		if (!IRCD->IsHostValid(host))
		{
			source.Reply(_("Bot hosts may only contain valid host characters."));
			return;
		}

		/* We check whether the nick is registered, and inform the user
		* if so. You need to drop the nick manually before you can use
		* it as a bot nick from now on -GD
		*/
		NickServ::Nick *na = NickServ::FindNick(nick);
		if (na)
		{
			source.Reply(_("\002{0}\002 is already registered!"), na->GetNick());
			return;
		}

		User *targ = User::Find(nick, true);
		if (targ)
			targ->Kill(Me, "Nickname is reserved for services");

		ServiceBot *bi = new ServiceBot(nick, user, host, real);

		Log(LOG_ADMIN, source, this) << "ADD " << bi->GetMask() << " " << bi->realname;

		source.Reply(_("\002{0}!{1}@{2}\002 (\002{3}\002) added to the bot list."), bi->nick, bi->GetIdent(), bi->host, bi->realname);

		this->OnBotCreate(&Event::BotCreate::OnBotCreate, bi);
	}

	void DoChange(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &oldnick = params[1];
		const Anope::string &nick = params.size() > 2 ? params[2] : "";
		const Anope::string &user = params.size() > 3 ? params[3] : "";
		const Anope::string &host = params.size() > 4 ? params[4] : "";
		const Anope::string &real = params.size() > 5 ? params[5] : "";

		if (oldnick.empty() || nick.empty())
		{
			this->OnSyntaxError(source, "CHANGE");
			return;
		}

		ServiceBot *bi = ServiceBot::Find(oldnick, true);
		if (!bi)
		{
			source.Reply(_("Bot \002{0}\002 does not exist."), oldnick);
			return;
		}

		if (bi->bi->conf)
		{
			source.Reply(_("Bot \002{0}\002 is not changeable because it is configured in services configuration."), bi->nick.c_str());
			return;
		}

		Configuration::Block *networkinfo = Config->GetBlock("networkinfo");

		if (nick.length() > networkinfo->Get<unsigned>("nicklen"))
		{
			source.Reply(_("Bot nicknames may only be \002{0}\002 characters long."), networkinfo->Get<unsigned>("nicklen"));
			return;
		}

		if (user.length() > networkinfo->Get<unsigned>("userlen"))
		{
			source.Reply(_("Bot usernames may only be \002{0}\002 characters long."), networkinfo->Get<unsigned>("userlen"));
			return;
		}

		if (host.length() > networkinfo->Get<unsigned>("hostlen"))
		{
			source.Reply(_("Bot hostnames may only be \002{0}\002 characters long."), networkinfo->Get<unsigned>("hostlen"));
			return;
		}

		/* Checks whether there *are* changes.
		* Case sensitive because we may want to change just the case.
		* And we must finally check that the nick is not already
		* taken by another bot.
		*/
		if (nick.equals_cs(bi->nick)
			&& (user.empty() || user.equals_cs(bi->GetIdent()))
			&& (host.empty() || host.equals_cs(bi->host))
			&& (real.empty() || real.equals_cs(bi->realname)))
		{
			source.Reply(_("There is no difference between the current settings and the new settings."));
			return;
		}

		if (!IRCD->IsNickValid(nick))
		{
			source.Reply(_("Bot nicknames may only contain valid nickname characters."));
			return;
		}

		if (!user.empty() && !IRCD->IsIdentValid(user))
		{
			source.Reply(_("Bot uesrnames may only contain valid username characters."));
			return;
		}

		if (!host.empty() && !IRCD->IsHostValid(host))
		{
			source.Reply(_("Bot hostnames may only contain valid hostname characters."));
			return;
		}

		ServiceBot *newbi = ServiceBot::Find(nick, true);
		if (newbi && bi != newbi)
		{
			source.Reply(_("Bot \002{0}\002 already exists."), newbi->nick);
			return;
		}

		if (!nick.equals_ci(bi->nick))
		{
			/* We check whether the nick is registered, and inform the user
			* if so. You need to drop the nick manually before you can use
			* it as a bot nick from now on -GD
			*/
			NickServ::Nick *na = NickServ::FindNick(nick);
			if (na)
			{
				source.Reply(_("\002{0}\002 is already registered."), na->GetNick());
				return;
			}

			/* The new nick is really different, so we remove the Q line for the old nick. */
			//XLine x_del(bi->nick);
			//IRCD->SendSQLineDel(&x_del);

			/* Add a Q line for the new nick */
			//XLine x(nick, "Reserved for services");
			//IRCD->SendSQLine(NULL, &x);
		}

		if (!user.empty())
		{
			IRCD->SendQuit(bi, "Quit: Be right back");
			bi->introduced = false;
		}
		else
			IRCD->SendNickChange(bi, nick);

		if (!nick.equals_cs(bi->nick))
		{
			bi->SetNewNick(nick);
			bi->bi->SetNick(nick);
		}

		if (!user.equals_cs(bi->GetIdent()))
		{
			bi->SetIdent(user);
			bi->bi->SetUser(user);
		}
		if (!host.equals_cs(bi->host))
		{
			bi->host = host;
			bi->bi->SetHost(host);
		}
		if (real.equals_cs(bi->realname))
		{
			bi->realname = real;
			bi->bi->SetRealName(real);
		}

		if (!user.empty())
			bi->OnKill();

		source.Reply(_("Bot \002{0}\002 has been changed to \002{1}!{2}@{3}\002 (\002{4}\002)."), oldnick, bi->nick, bi->GetIdent(), bi->host, bi->realname);
		Log(LOG_ADMIN, source, this) << "CHANGE " << oldnick << " to " << bi->GetMask() << " " << bi->realname;

		this->OnBotChange(&Event::BotChange::OnBotChange, bi);
	}

	void DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &nick = params[1];

		if (nick.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return;
		}

		ServiceBot *bi = ServiceBot::Find(nick, true);
		if (!bi)
		{
			source.Reply(_("Bot \002{0}\002 does not exist."), nick);
			return;
		}

		if (bi->bi->conf)
		{
			source.Reply(_("Bot \002{0}\002 is can not be deleted because it is configured in services configuration."), bi->nick);
			return;
		}

		this->OnBotDelete(&Event::BotDelete::OnBotDelete, bi);

		Log(LOG_ADMIN, source, this) << "DEL " << bi->nick;

		source.Reply(_("Bot \002{0}\002 has been deleted."), bi->nick);
		delete bi;
	}

 public:
	CommandBSBot(Module *creator) : Command(creator, "botserv/bot", 1, 6)
		, OnBotCreate(creator)
		, OnBotChange(creator)
		, OnBotDelete(creator)
	{
		this->SetDesc(_("Maintains network bot list"));
		this->SetSyntax(_("\002ADD \037nicknae\037 \037username\037 \037hostname\037 \037realname\037\002"));
		this->SetSyntax(_("\002CHANGE \037oldnickname\037 \037newnickname\037 [\037username\037 [\037hostname\037 [\037realname\037]]]\002"));
		this->SetSyntax(_("\002DEL \037nickname\037\002"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[0];

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		if (cmd.equals_ci("ADD"))
		{
			if (!source.HasCommand("botserv/bot/add"))
			{
				source.Reply(_("Access denied. You do not have access to the operator command \002{0}\002."), "botserv/bot/add");
				return;
			}

			// ADD nick user host real - 5
			if (params.size() < 5)
			{
				this->OnSyntaxError(source, "ADD");
				return;
			}

			std::vector<Anope::string> tempparams = params;
			// ADD takes less params than CHANGE, so we need to take 6 if given and append it with a space to 5.
			if (tempparams.size() >= 6)
				tempparams[4] = tempparams[4] + " " + tempparams[5];

			this->DoAdd(source, tempparams);
		}
		else if (cmd.equals_ci("CHANGE"))
		{
			// CHANGE oldn newn user host real - 6
			// but only oldn and newn are required
			if (!source.HasCommand("botserv/bot/change"))
			{
				source.Reply(_("Access denied. You do not have access to the operator command \002{0}\002."), "botserv/bot/change");
				return;
			}

			if (params.size() < 3)
			{
				this->OnSyntaxError(source, "CHANGE");
				return;
			}

			this->DoChange(source, params);
		}
		else if (cmd.equals_ci("DEL"))
		{
			// DEL nick
			if (!source.HasCommand("botserv/bot/del"))
			{
				source.Reply(_("Access denied. You do not have access to the operator command \002{0}\002."), "botserv/bot/del");
				return;
			}

			if (params.size() < 1)
			{
				this->OnSyntaxError(source, "DEL");
				return;
			}

			this->DoDel(source, params);
		}
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (subcommand.equals_ci("ADD"))
			source.Reply(_("\002{command} ADD\002 adds a bot with the given \037nickname\037, \037username\037, \037hostname\037 and \037realname\037."
			               " You can not create a bot with a nickname that is currently registered. If an unregistered user is currently using the nick, they will be killed.\n"
			               " Once a bot is created, users will be able to assign the bot to their channels. This command requires the opererator privilege for command \002{0}\002."
			               "\n"
			               "Example:\n"
			               "         {command} ADD Botox Botox services.anope.org Botox\n"
			               "         Adds a service bot with nickname \"Botox\", username \"Botox\", hostname \"services.anope.org\", and realname \"Botox\" to the bot list."),
			               "botserv/bot/add", "command"_kw = source.command);
		else if (subcommand.equals_ci("CHANGE"))
			source.Reply(_("\002{command} CHANGE\002 allows changing the \037nickname\037, \037username\037, \037hostname\037 and \037realname\037 of bot \037oldnickname\037."
			               " If a new username, hostname, or realname is specified, then the bot \037nickname\037 will quit and rejoin all of its channels using the new mask."
			               " Otherwise, the bot simply change nick to \037newnickname\037. All settings on the bot, such as channels and no-bot, are retained."
			               " This command requires the operator privilege for command \002{0}\002."
			               "\n"
			               "Example:\n"
			               "         {command} CHANGE Botox peer connection reset.by peer\n"
			               "          Renames the bot \"Botox\" to \"peer\" with the given username, hostname, and realname."),
			               "botserv/bot/change", "command"_kw = source.command);
		else if (subcommand.equals_ci("DEL"))
			source.Reply(_("\002{command} DEL\002 removes the bot \037nickname\037 from the bot list. The bot will quit from any channels it is in, and will not be replaced."
			               " This command requires the operator privilege for command \002{0}\002.\n"
			               "\n"
			               "Example:\n"
			               "         {command} DEL peer\n"
			               "         Removes the bot \"peer\" from the bot list."),
			               "botserv/bot/del", "command"_kw = source.command);
		else
		{
			source.Reply(_("Allows Services Operators to create, modify, and delete bots that users will be able to use on their channels."));

			CommandInfo *help = source.service->FindCommand("generic/help");
			if (help)
				source.Reply(_("\n"
			                       "The \002ADD\002 command adds a bot with the given \037nickname\037, \037username\037, \037hostname\037 and \037realname\037 to the bot list.\n"
			                       "\002{msg}{service} {help} {command} ADD\002 for more information.\n"
			                       "\n"
			                       "The \002CHANGE\002 command allows changing the \037nickname\037, \037username\037, \037hostname\037 and \037realname\037 of bot \037oldnickname\037.\n"
			                       "\002{msg}{service} {help} {command} CHANGE\002 for more information.\n"
			                       "\n"
			                       "The \002{command} DEL\002 removes the bot \037nickname\037 from the bot list.\n"
			                       "\002{msg}{service} {help} {command} DEL\002 for more information."),
			                       "msg"_kw = Config->StrictPrivmsg, "help"_kw = help->cname, "command"_kw = source.command);
		}
		return true;
	}
};

class BSBot : public Module
{
	CommandBSBot commandbsbot;

 public:
	BSBot(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandbsbot(this)
	{

	}
};

MODULE_INIT(BSBot)
