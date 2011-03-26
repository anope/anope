/* BotServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

class CommandBSKick : public Command
{
 public:
	CommandBSKick() : Command("KICK", 3, 6)
	{
		this->SetDesc(_("Configures kickers"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		const Anope::string &option = params[1];
		const Anope::string &value = params[2];
		const Anope::string &ttb = params.size() > 3 ? params[3] : "";

		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (readonly)
			source.Reply(_("Sorry, kicker configuration is temporarily disabled."));
		else if (chan.empty() || option.empty() || value.empty())
			SyntaxError(source, "KICK", _("KICK \037channel\037 \037option\037 {\037ON|\037} [\037settings\037]"));
		else if (!value.equals_ci("ON") && !value.equals_ci("OFF"))
			SyntaxError(source, "KICK", _("KICK \037channel\037 \037option\037 {\037ON|\037} [\037settings\037]"));
		else if (!check_access(u, ci, CA_SET) && !u->HasPriv("botserv/administration"))
			source.Reply(_(ACCESS_DENIED));
		else if (!ci->bi)
			source.Reply(_(BOT_NOT_ASSIGNED));
		else
		{
			bool override = !check_access(u, ci, CA_SET);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << option << " " << value;

			if (option.equals_ci("BADWORDS"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_BADWORDS] = convertTo<int16>(ttb);
							if (ci->ttb[TTB_BADWORDS] < 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							/* reset the value back to 0 - TSL */
							ci->ttb[TTB_BADWORDS] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_BADWORDS] = 0;

					ci->botflags.SetFlag(BS_KICK_BADWORDS);
					if (ci->ttb[TTB_BADWORDS])
						source.Reply(_("Bot will now kick \002bad words\002, and will place a ban after \n"
											"%d kicks for the same user. Use the BADWORDS command\n"
											"to add or remove a bad word."), ci->ttb[TTB_BADWORDS]);
					else
						source.Reply(_("Bot will now kick \002bad words\002. Use the BADWORDS command\n"
								"to add or remove a bad word."));
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_BADWORDS);
					source.Reply(_("Bot won't kick \002bad words\002 anymore."));
				}
			}
			else if (option.equals_ci("BOLDS"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_BOLDS] = convertTo<int16>(ttb);
							if (ci->ttb[TTB_BOLDS] < 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_BOLDS] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_BOLDS] = 0;
					ci->botflags.SetFlag(BS_KICK_BOLDS);
					if (ci->ttb[TTB_BOLDS])
						source.Reply(_("Bot will now kick \002bolds\002, and will place a ban after\n%d kicks to the same user."), ci->ttb[TTB_BOLDS]);
					else
						source.Reply(_("Bot will now kick \002bolds\002."));
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_BOLDS);
					source.Reply(_("Bot won't kick \002bolds\002 anymore."));
				}
			}
			else if (option.equals_ci("CAPS"))
			{
				if (value.equals_ci("ON"))
				{
					Anope::string min = params.size() > 4 ? params[4] : "";
					Anope::string percent = params.size() > 5 ? params[5] : "";

					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_CAPS] = convertTo<int16>(ttb);
							if (ci->ttb[TTB_CAPS] < 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_CAPS] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_CAPS] = 0;

					ci->capsmin = 10;
					try
					{
						ci->capsmin = convertTo<int16>(min);
					}
					catch (const ConvertException &) { }
					if (ci->capsmin < 1)
						ci->capsmin = 10;

					ci->capspercent = 25;
					try
					{
						ci->capspercent = convertTo<int16>(percent);
					}
					catch (const ConvertException &) { }
					if (ci->capspercent < 1 || ci->capspercent > 100)
						ci->capspercent = 25;

					ci->botflags.SetFlag(BS_KICK_CAPS);
					if (ci->ttb[TTB_CAPS])
						source.Reply(_("Bot will now kick \002caps\002 (they must constitute at least\n"
								"%d characters and %d%% of the entire message), and will \n"
								"place a ban after %d kicks for the same user."), ci->capsmin, ci->capspercent, ci->ttb[TTB_CAPS]);
					else
						source.Reply(_("Bot will now kick \002caps\002 (they must constitute at least\n"
								"%d characters and %d%% of the entire message)."), ci->capsmin, ci->capspercent);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_CAPS);
					source.Reply(_("Bot won't kick \002caps\002 anymore."));
				}
			}
			else if (option.equals_ci("COLORS"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_COLORS] = convertTo<int16>(ttb);
							if (ci->ttb[TTB_COLORS] < 1)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_COLORS] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_COLORS] = 0;

					ci->botflags.SetFlag(BS_KICK_COLORS);
					if (ci->ttb[TTB_COLORS])
						source.Reply(_("Bot will now kick \002colors\002, and will place a ban after %d\nkicks for the same user."), ci->ttb[TTB_COLORS]);
					else
						source.Reply(_("Bot will now kick \002colors\002."));
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_COLORS);
					source.Reply(_("Bot won't kick \002colors\002 anymore."));
				}
			}
			else if (option.equals_ci("FLOOD"))
			{
				if (value.equals_ci("ON"))
				{
					Anope::string lines = params.size() > 4 ? params[4] : "";
					Anope::string secs = params.size() > 5 ? params[5] : "";

					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_FLOOD] = convertTo<int16>(ttb);
							if (ci->ttb[TTB_FLOOD] < 1)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_FLOOD] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_FLOOD] = 0;

					ci->floodlines = 6;
					try
					{
						ci->floodlines = convertTo<int16>(lines);
					}
					catch (const ConvertException &) { }
					if (ci->floodlines < 2)
						ci->floodlines = 6;

					ci->floodsecs = 10;
					try
					{
						ci->floodsecs = convertTo<int16>(secs);
					}
					catch (const ConvertException &) { }
					if (ci->floodsecs < 1)
						ci->floodsecs = 10;
					if (ci->floodsecs > Config->BSKeepData)
						ci->floodsecs = Config->BSKeepData;

					ci->botflags.SetFlag(BS_KICK_FLOOD);
					if (ci->ttb[TTB_FLOOD])
						source.Reply(_("Bot will now kick \002flood\002 (%d lines in %d seconds and\nwill place a ban after %d kicks for the same user."), ci->floodlines, ci->floodsecs, ci->ttb[TTB_FLOOD]);
					else
						source.Reply(_("Bot will now kick \002flood\002 (%d lines in %d seconds)."), ci->floodlines, ci->floodsecs);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_FLOOD);
					source.Reply(_("Bot won't kick \002flood\002 anymore."));
				}
			}
			else if (option.equals_ci("REPEAT"))
			{
				if (value.equals_ci("ON"))
				{
					Anope::string times = params.size() > 4 ? params[4] : "";

					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_REPEAT] = convertTo<int16>(ttb);
							if (ci->ttb[TTB_REPEAT])
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_REPEAT] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_REPEAT] = 0;

					ci->repeattimes = 3;
					try
					{
						ci->repeattimes = convertTo<int16>(times);
					}
					catch (const ConvertException &) { }
					if (ci->repeattimes < 2)
						ci->repeattimes = 3;

					ci->botflags.SetFlag(BS_KICK_REPEAT);
					if (ci->ttb[TTB_REPEAT])
						source.Reply(_("Bot will now kick \002repeats\002 (users that say %d times\n"
								"the same thing), and will place a ban after %d \n"
								"kicks for the same user."), ci->repeattimes, ci->ttb[TTB_REPEAT]);
					else
						source.Reply(_("Bot will now kick \002repeats\002 (users that say %d times\n"
							"the same thing)."), ci->repeattimes);
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_REPEAT);
					source.Reply(_("Bot won't kick \002repeats\002 anymore."));
				}
			}
			else if (option.equals_ci("REVERSES"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_REVERSES] = convertTo<int16>(ttb);
							if (ci->ttb[TTB_REVERSES] < 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_REVERSES] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_REVERSES] = 0;
					ci->botflags.SetFlag(BS_KICK_REVERSES);
					if (ci->ttb[TTB_REVERSES])
						source.Reply(_("Bot will now kick \002reverses\002, and will place a ban after %d\nkicks for the same user."), ci->ttb[TTB_REVERSES]);
					else
						source.Reply(_("Bot will now kick \002reverses\002."));
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_REVERSES);
					source.Reply(_("Bot won't kick \002reverses\002 anymore."));
				}
			}
			else if (option.equals_ci("UNDERLINES"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_UNDERLINES] = convertTo<int16>(ttb);
							if (ci->ttb[TTB_REVERSES] < 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_UNDERLINES] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_UNDERLINES] = 0;

					ci->botflags.SetFlag(BS_KICK_UNDERLINES);
					if (ci->ttb[TTB_UNDERLINES])
						source.Reply(_("Bot will now kick \002underlines\002, and will place a ban after %d\nkicks for the same user."), ci->ttb[TTB_UNDERLINES]);
					else
						source.Reply(_("Bot will now kick \002underlines\002."));
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_UNDERLINES);
					source.Reply(_("Bot won't kick \002underlines\002 anymore."));
				}
			}
			else if (option.equals_ci("ITALICS"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_ITALICS] = convertTo<int16>(ttb);
							if (ci->ttb[TTB_ITALICS] < 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_ITALICS] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_ITALICS] = 0;

					ci->botflags.SetFlag(BS_KICK_ITALICS);
					if (ci->ttb[TTB_ITALICS])
						source.Reply(_("Bot will now kick \002italics\002, and will place a ban after\n%d kicks for the same user."), ci->ttb[TTB_ITALICS]);
					else
						source.Reply(_("Bot will now kick \002italics\002."));
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_ITALICS);
					source.Reply(_("Bot won't kick \002italics\002 anymore."));
				}
			}
			else if (option.equals_ci("AMSGS"))
			{
				if (value.equals_ci("ON"))
				{
					if (!ttb.empty())
					{
						try
						{
							ci->ttb[TTB_AMSGS] = convertTo<int16>(ttb);
							if (ci->ttb[TTB_AMSGS] < 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							ci->ttb[TTB_AMSGS] = 0;
							source.Reply(_("\002%s\002 cannot be taken as times to ban."), ttb.c_str());
							return MOD_CONT;
						}
					}
					else
						ci->ttb[TTB_AMSGS] = 0;

					ci->botflags.SetFlag(BS_KICK_AMSGS);
					if (ci->ttb[TTB_AMSGS])
						source.Reply(_("Bot will now kick for \002amsgs\002, and will place a ban after %d\nkicks for the same user."), ci->ttb[TTB_AMSGS]);
					else
						source.Reply(_("Bot will now kick for \002amsgs\002"));
				}
				else
				{
					ci->botflags.UnsetFlag(BS_KICK_AMSGS);
					source.Reply(_("Bot won't kick for \002amsgs\002 anymore."));
				}
			}
			else
				source.Reply(_(UNKNOWN_OPTION), option.c_str(), this->name.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		if (subcommand.empty())
			source.Reply(_("Syntax: \002KICK \037channel\037 \037option\037 \037parameters\037\002\n"
					" \n"
					"Configures bot kickers.  \037option\037 can be one of:\n"
					" \n"
					"    AMSGS         Sets if the bot kicks for amsgs\n"
					"    BOLDS         Sets if the bot kicks bolds\n"
					"    BADWORDS      Sets if the bot kicks bad words\n"
					"    CAPS          Sets if the bot kicks caps\n"
					"    COLORS        Sets if the bot kicks colors\n"
					"    FLOOD         Sets if the bot kicks flooding users\n"
					"    REPEAT        Sets if the bot kicks users who repeat\n"
					"                       themselves\n"
					"    REVERSES      Sets if the bot kicks reverses\n"
					"    UNDERLINES    Sets if the bot kicks underlines\n"
					"    ITALICS       Sets if the bot kicks italics\n"
					" \n"
					"Type \002%s%s HELP KICK \037option\037\002 for more information\n"
					"on a specific option.\n"
					" \n"
					"Note: access to this command is controlled by the\n"
					"level SET."), Config->UseStrictPrivMsgString.c_str(), ChanServ->nick.c_str());
		else if (subcommand.equals_ci("BADWORDS"))
			source.Reply(_("Syntax: \002KICK \037#channel\037 BADWORDS {\037ON|OFF\037} [\037ttb\037]\002\n"
					"Sets the bad words kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who say certain words\n"
					"on the channels.\n"
					"You can define bad words for your channel using the\n"
					"\002BADWORDS\002 command. Type \002%s%s HELP BADWORDS\002 for\n"
					"more information.\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."), Config->UseStrictPrivMsgString.c_str(), ChanServ->nick.c_str());
		else if (subcommand.equals_ci("BOLDS"))
			source.Reply(_("Syntax: \002KICK \037channel\037 BOLDS {\037ON|OFF\037} [\037ttb\037]\002\n"
					"Sets the bolds kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who use bolds.\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("CAPS"))
			source.Reply(_("Syntax: \002KICK \037channel\037 CAPS {\037ON|OFF\037} [\037ttb\037 [\037min\037 [\037percent\037]]]\002\n"
					"Sets the caps kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who are talking in\n"
					"CAPS.\n"
					"The bot kicks only if there are at least \002min\002 caps\n"
					"and they constitute at least \002percent\002%% of the total \n"
					"text line (if not given, it defaults to 10 characters\n"
					"and 25%%).\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("COLORS"))
			source.Reply(_("Syntax: \002KICK \037channel\037 COLORS {\037ON|OFF\037} [\037ttb\037]\002\n"
					"Sets the colors kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who use colors.\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("FLOOD"))
			source.Reply(_("Syntax: \002KICK \037channel\037 FLOOD {\037ON|OFF\037} [\037ttb\037 [\037ln\037 [\037secs\037]]]\002\n"
					"Sets the flood kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who are flooding\n"
					"the channel using at least \002ln\002 lines in \002secs\002 seconds\n"
					"(if not given, it defaults to 6 lines in 10 seconds).\n"
					" \n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("REPEAT"))
			source.Reply(_("Syntax: \002KICK \037#channel\037 REPEAT {\037ON|OFF\037} [\037ttb\037 [\037num\037]]\002\n"
					"Sets the repeat kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who are repeating\n"
					"themselves \002num\002 times (if num is not given, it\n"
					"defaults to 3).\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("REVERSES"))
			source.Reply(_("Syntax: \002KICK \037channel\037 REVERSES {\037ON|OFF\037} [\037ttb\037]\002\n"
					"Sets the reverses kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who use reverses.\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("UNDERLINES"))
			source.Reply(_("Syntax: \002KICK \037channel\037 UNDERLINES {\037ON|OFF\037} [\037ttb\037]\002\n"
					"Sets the underlines kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who use underlines.\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("ITALICS"))
			source.Reply(_("Syntax: \002KICK \037channel\037 ITALICS {\037ON|OFF\037} [\037ttb\037]\002\n"
					"Sets the italics kicker on or off. When enabled, this\n"
					"option tells the bot to kick users who use italics.\n"
					"ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else if (subcommand.equals_ci("AMSGS"))
			source.Reply(_("Syntax: \002KICK \037channel\037 AMSGS {\037ON|OFF\037} [\037ttb\037]\002\n"
					"Sets the amsg kicker on or off. When enabled, the bot will\n"
					"kick users who send the same message to multiple channels\n"
					"where BotServ bots are.\n"
					"Ttb is the number of times a user can be kicked\n"
					"before it get banned. Don't give ttb to disable\n"
					"the ban system once activated."));
		else
			return false;

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "KICK", _("KICK \037channel\037 \037option\037 {\037ON|\037} [\037settings\037]"));
	}
};

class BSKick : public Module
{
	CommandBSKick commandbskick;

 public:
	BSKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(BotServ, &commandbskick);

		ModuleManager::Attach(I_OnPrivmsg, this);
	}

	EventReturn OnPrivmsg(User *u, ChannelInfo *ci, Anope::string &msg, bool &Allow)
	{
		Anope::string m, ch;
		time_t time;

		if (u->GetExtRegular("bs_kick_lastmsg", m) && u->GetExtRegular("bs_kick_lasttime", time) && u->GetExtRegular("bs_kick_lastchan", ch))
		{
			if (time == Anope::CurTime && m == msg && ch != ci->name)
			{
				for (UChannelList::iterator it = u->chans.begin(); it != u->chans.end();)
				{
					Channel *c = (*it)->chan;
					++it;

					if (c->ci != NULL && c->ci->botflags.HasFlag(BS_KICK_AMSGS))
					{
						check_ban(c->ci, u, TTB_AMSGS);
						bot_kick(c->ci, u, _("Don't use AMSGs!"));
					}
				}

				return EVENT_CONTINUE;
			}
		}
		
		u->Extend("bs_kick_lastmsg", new ExtensibleItemRegular<Anope::string>(msg));
		u->Extend("bs_kick_lasttime", new ExtensibleItemRegular<time_t>(Anope::CurTime));
		u->Extend("bs_kick_lastchan", new ExtensibleItemRegular<Anope::string>(ci->name));

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(BSKick)
