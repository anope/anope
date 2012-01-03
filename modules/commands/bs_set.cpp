/* BotServ core functions
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

class CommandBSSet : public Command
{
 public:
	CommandBSSet(Module *creator) : Command(creator, "botserv/set", 3, 3)
	{
		this->SetDesc(_("Configures bot options"));
		this->SetSyntax(_("\037(channel | bot)\037 \037option\037 \037settings\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
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
				source.Reply(BOT_DOES_NOT_EXIST, chan.c_str());
				return;
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
				this->OnSyntaxError(source, "PRIVATE");
			return;
		}
		else if (!(ci = cs_findchan(chan)))
			source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
		else if (!u->HasPriv("botserv/administration") && !ci->AccessFor(u).HasPriv("SET"))
			source.Reply(ACCESS_DENIED);
		else
		{
			bool override = !ci->AccessFor(u).HasPriv("SET");
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
					this->OnSyntaxError(source, "DONTKICKOPS");
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
					this->OnSyntaxError(source, "DONTKICKVOICE");
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
					this->OnSyntaxError(source, "FANTASY");
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
					this->OnSyntaxError(source, "GREET");
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
					this->OnSyntaxError(source, "NOBOT");
			}
			else
				this->OnSyntaxError(source, "");
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		if (subcommand.empty())
		{
			this->SendSyntax(source);
			source.Reply(" ");
			source.Reply(_("Configures bot options.  \037option\037 can be one of:\n"
					" \n"
					"    DONTKICKOPS      To protect ops against bot kicks\n"
					"    DONTKICKVOICES   To protect voices against bot kicks\n"
					"    GREET            Enable greet messages\n"
					"    FANTASY          Enable fantaisist commands\n"
					" \n"
					"Type \002%s%s HELP SET \037option\037\002 for more information\n"
					"on a specific option.\n"
					"Note: access to this command is controlled by the\n"
					"level SET."), Config->UseStrictPrivMsgString.c_str(), source.owner->nick.c_str());
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
		else
			return false;

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		if (subcommand.empty())
			Command::OnSyntaxError(source, "");
		else if (subcommand.equals_ci("PRIVATE"))
			this->SendSyntax(source, "\037botname\037 PRIVATE {\037ON|OFF\037}");
		else if (subcommand.equals_ci("DONTKICKOPS"))
			this->SendSyntax(source, "\037channel\037 DONTKICKOPS {\037ON|OFF\037}");
		else if (subcommand.equals_ci("DONTKICKVOICES"))
			this->SendSyntax(source, "\037channel\037 DONTKICKVOICES {\037ON|OFF\037}");
		else if (subcommand.equals_ci("FANTASY"))
			this->SendSyntax(source, "\037channel\037 FANTASY {\037ON|OFF\037}");
		else if (subcommand.equals_ci("GREET"))
			this->SendSyntax(source, "\037channel\037 GREET {\037ON|OFF\037}");
		else if (subcommand.equals_ci("NOBOT"))
			this->SendSyntax(source, "\037channel\037 NOBOT {\037ON|OFF\037}");
		else
			this->OnSyntaxError(source, "");
	}
};

class BSSet : public Module
{
	CommandBSSet commandbsset;

 public:
	BSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandbsset(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(BSSet)
