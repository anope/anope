/* BotServ core functions
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
#include "botserv.h"

class CommandBSSet : public Command
{
 public:
	CommandBSSet() : Command("SET", 3, 3)
	{
		this->SetFlag(CFLAG_STRIP_CHANNEL);
		this->SetDesc(_("Configures bot options"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		const Anope::string &option = params[1];
		const Anope::string &value = params[2];

		User *u = source.u;
		ChannelInfo *ci;

		if (readonly)
			source.Reply(_("Sorry, bot option setting is temporarily disabled."));
		else if (u->HasCommand("botserv/set/private") && option.equals_ci("PRIVATE"))
		{
			BotInfo *bi;

			if (!(bi = findbot(chan)))
			{
				source.Reply(_(BOT_DOES_NOT_EXIST), chan.c_str());
				return MOD_CONT;
			}

			if (value.equals_ci("ON"))
			{
				bi->SetFlag(BI_PRIVATE);
				source.Reply(_("Private mode of bot %s is now \002on\002."), bi->nick.c_str());
			}
			else if (value.equals_ci("OFF"))
			{
				bi->UnsetFlag(BI_PRIVATE);
				source.Reply(_("Private mode of bot %s is now \002off\002."), bi->nick.c_str());
			}
			else
				SyntaxError(source, "SET PRIVATE", _("SET \037botname\037 PRIVATE {\037ON|\037}"));
			return MOD_CONT;
		}
		else if (!(ci = cs_findchan(chan)))
			source.Reply(_(CHAN_X_NOT_REGISTERED), chan.c_str());
		else if (!u->HasPriv("botserv/administration") && !check_access(u, ci, CA_SET))
			source.Reply(_(ACCESS_DENIED));
		else
		{
			bool override = !check_access(u, ci, CA_SET);
			Log(override ? LOG_ADMIN : LOG_COMMAND, u, this, ci) << option << " " << value;

			if (option.equals_ci("DONTKICKOPS"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_DONTKICKOPS);
					source.Reply(_("Bot \002won't kick ops\002 on channel %s."), ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_DONTKICKOPS);
					source.Reply(_("Bot \002will kick ops\002 on channel %s."), ci->name.c_str());
				}
				else
					SyntaxError(source, "SET DONTKICKOPS", _("SET \037channel\037 DONTKICKOPS {\037ON|\037}"));
			}
			else if (option.equals_ci("DONTKICKVOICES"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_DONTKICKVOICES);
					source.Reply(_("Bot \002won't kick voices\002 on channel %s."), ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_DONTKICKVOICES);
					source.Reply(_("Bot \002will kick voices\002 on channel %s."), ci->name.c_str());
				}
				else
					SyntaxError(source, "SET DONTKICKVOICES", _("SET \037channel\037 DONTKICKVOICES {\037ON|\037}"));
			}
			else if (option.equals_ci("FANTASY"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_FANTASY);
					source.Reply(_("Fantasy mode is now \002on\002 on channel %s."), ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_FANTASY);
					source.Reply(_("Fantasy mode is now \002off\002 on channel %s."), ci->name.c_str());
				}
				else
					SyntaxError(source, "SET FANTASY", _("SET \037channel\037 FANTASY {\037ON|\037}"));
			}
			else if (option.equals_ci("GREET"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_GREET);
					source.Reply(_("Greet mode is now \002on\002 on channel %s."), ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_GREET);
					source.Reply(_("Greet mode is now \002off\002 on channel %s."), ci->name.c_str());
				}
				else
					SyntaxError(source, "SET GREET", _("SET \037channel\037 GREET {\037ON|\037}"));
			}
			else if (u->HasCommand("botserv/set/nobot") && option.equals_ci("NOBOT"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_NOBOT);
					if (ci->bi)
						ci->bi->UnAssign(u, ci);
					source.Reply(_("No Bot mode is now \002on\002 on channel %s."), ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_NOBOT);
					source.Reply(_("No Bot mode is now \002off\002 on channel %s."), ci->name.c_str());
				}
				else
					SyntaxError(source, "SET NOBOT", _("SET \037botname\037 NOBOT {\037ON|\037}"));
			}
			else if (option.equals_ci("SYMBIOSIS"))
			{
				if (value.equals_ci("ON"))
				{
					ci->botflags.SetFlag(BS_SYMBIOSIS);
					source.Reply(_("Symbiosis mode is now \002on\002 on channel %s."), ci->name.c_str());
				}
				else if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_SYMBIOSIS);
					source.Reply(_("Symbiosis mode is now \002off\002 on channel %s."), ci->name.c_str());
				}
				else
					SyntaxError(source, "SET SYMBIOSIS", _("SET \037channel\037 SYMBIOSIS {\037ON|\037}"));
			}
			else if (option.equals_ci("MSG"))
			{
				if (value.equals_ci("OFF"))
				{
					ci->botflags.UnsetFlag(BS_MSG_PRIVMSG);
					ci->botflags.UnsetFlag(BS_MSG_NOTICE);
					ci->botflags.UnsetFlag(BS_MSG_NOTICEOPS);
					source.Reply(_("Fantasy replies will no longer be sent to %s."), ci->name.c_str());
				}
				else if (value.equals_ci("PRIVMSG"))
				{
					ci->botflags.SetFlag(BS_MSG_PRIVMSG);
					ci->botflags.UnsetFlag(BS_MSG_NOTICE);
					ci->botflags.UnsetFlag(BS_MSG_NOTICEOPS);
					source.Reply(_("Fantasy replies will be sent via PRIVMSG to %s."), ci->name.c_str());
				}
				else if (value.equals_ci("NOTICE"))
				{
					ci->botflags.UnsetFlag(BS_MSG_PRIVMSG);
					ci->botflags.SetFlag(BS_MSG_NOTICE);
					ci->botflags.UnsetFlag(BS_MSG_NOTICEOPS);
					source.Reply(_("Fantasy replies will be sent via NOTICE to %s."), ci->name.c_str());
				}
				else if (value.equals_ci("NOTICEOPS"))
				{
					ci->botflags.UnsetFlag(BS_MSG_PRIVMSG);
					ci->botflags.UnsetFlag(BS_MSG_NOTICE);
					ci->botflags.SetFlag(BS_MSG_NOTICEOPS);
					source.Reply(_("Fantasy replies will be sent via NOTICE to channel ops on %s."), ci->name.c_str());
				}
				else
					SyntaxError(source, "SET MSG", _("SET \037channel\037 MSG {\037OFF|PRIVMSG|NOTICE|\037}"));
			}
			else
				source.Reply(_(UNKNOWN_OPTION), option.c_str(), Config->UseStrictPrivMsgString.c_str(), BotServ->nick.c_str(), this->name.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		if (subcommand.empty())
		{
			source.Reply(_("Syntax: \002SET \037(channel | bot)\037 \037option\037 \037parameters\037\002\n"
					" \n"
					"Configures bot options.  \037option\037 can be one of:\n"
					" \n"
					"    DONTKICKOPS      To protect ops against bot kicks\n"
					"    DONTKICKVOICES   To protect voices against bot kicks\n"
					"    GREET            Enable greet messages\n"
					"    FANTASY          Enable fantaisist commands\n"
					"    SYMBIOSIS        Allow the bot to act as a real bot\n"
					"    MSG              Configure how fantasy commands should be replied to\n"
					" \n"
					"Type \002%s%s HELP SET \037option\037\002 for more information\n"
					"on a specific option.\n"
					"Note: access to this command is controlled by the\n"
					"level SET."), Config->UseStrictPrivMsgString.c_str(), Config->s_BotServ.c_str());
			User *u = source.u;
			if (u->IsServicesOper())
				source.Reply(_("These options are reserved to Services Operators:\n"
						" \n"
						"    NOBOT            Prevent a bot from being assigned to \n"
						"                        a channel\n"
						"    PRIVATE          Prevent a bot from being assigned by\n"
						"                        non IRC operators"));
		}
		else if (subcommand.equals_ci("DONTKICKOPS"))
			source.Reply(_("Syntax: \002SET \037channel\037 DONTKICKOPS {\037ON|OFF\037}\n"
					" \n"
					"Enables or disables \002ops protection\002 mode on a channel.\n"
					"When it is enabled, ops won't be kicked by the bot\n"
					"even if they don't match the NOKICK level."));
		else if (subcommand.equals_ci("DONTKICKVOICES"))
			source.Reply(_("Syntax: \002SET \037channel\037 DONTKICKVOICES {\037ON|OFF\037}\n"
					" \n"
					"Enables or disables \002voices protection\002 mode on a channel.\n"
					"When it is enabled, voices won't be kicked by the bot\n"
					"even if they don't match the NOKICK level."));
		else if (subcommand.equals_ci("FANTASY"))
			source.Reply(_("Syntax: \002SET \037channel\037 FANTASY {\037ON|OFF\037}\n"
					"Enables or disables \002fantasy\002 mode on a channel.\n"
					"When it is enabled, users will be able to use\n"
					"commands !op, !deop, !voice, !devoice,\n"
					"!kick, !kb, !unban, !seen on a channel (find how \n"
					"to use them; try with or without nick for each, \n"
					"and with a reason for some?).\n"
					" \n"
					"Note that users wanting to use fantaisist\n"
					"commands MUST have enough level for both\n"
					"the FANTASIA and another level depending\n"
					"of the command if required (for example, to use \n"
					"!op, user must have enough access for the OPDEOP\n"
					"level)."));
		else if (subcommand.equals_ci("GREET"))
			source.Reply(_("Syntax: \002SET \037channel\037 GREET {\037ON|OFF\037}\n"
					" \n"
					"Enables or disables \002greet\002 mode on a channel.\n"
					"When it is enabled, the bot will display greet\n"
					"messages of users joining the channel, provided\n"
					"they have enough access to the channel."));
		else if (subcommand.equals_ci("SYMBIOSIS"))
			source.Reply(_("Syntax: \002SET \037channel\037 SYMBIOSIS {\037ON|OFF\037}\n"
					" \n"
					"Enables or disables \002symbiosis\002 mode on a channel.\n"
					"When it is enabled, the bot will do everything\n"
					"normally done by %s on channels, such as MODEs,\n"
					"KICKs, and even the entry message."), Config->s_ChanServ.c_str());
		else if (subcommand.equals_ci("NOBOT"))
			source.Reply(_("Syntax: \002SET \037channel\037 NOBOT {\037ON|OFF\037}\002\n"
					" \n"
					"This option makes a channel be unassignable. If a bot \n"
					"is already assigned to the channel, it is unassigned\n"
					"automatically when you enable the option."));
		else if (subcommand.equals_ci("PRIVATE"))
			source.Reply(_("Syntax: \002SET \037bot-nick\037 PRIVATE {\037ON|OFF\037}\002\n"
					"This option prevents a bot from being assigned to a\n"
					"channel by users that aren't IRC operators."));
		else if (subcommand.equals_ci("MSG"))
			source.Reply(_("Syntax: \002SET \037channel\037 MSG {\037OFF|PRIVMSG|NOTICE|NOTICEOPS\037}\002\n"
					" \n"
					"Configures how fantasy commands should be returned to the channel. Off disables\n"
					"fantasy from replying to the channel. Privmsg, notice, and noticeops message the\n"
					"channel, notice the channel, and notice the channel ops respectively.\n"
					" \n"
					"Note that replies over one line will not use this setting to prevent spam, and will\n"
					"go directly to the user who executed it."));
		else
			return false;

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SET", _("SET \037(channel | bot)\037 \037option\037 \037settings\037"));
	}
};

class BSSet : public Module
{
	CommandBSSet commandbsset;

 public:
	BSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		if (!botserv)
			throw ModuleException("BotServ is not loaded!");

		this->AddCommand(botserv->Bot(), &commandbsset);
	}
};

MODULE_INIT(BSSet)
