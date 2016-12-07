/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"
#include "modules/botserv/kick.h"
#include "modules/botserv/badwords.h"
#include "modules/botserv/info.h"

static Module *me;

enum TTBType
{
	TTB_BOLDS,
	TTB_COLORS,
	TTB_REVERSES,
	TTB_UNDERLINES,
	TTB_BADWORDS,
	TTB_CAPS,
	TTB_FLOOD,
	TTB_REPEAT,
	TTB_ITALICS,
	TTB_AMSGS,
	TTB_SIZE
};

class KickerDataImpl : public KickerData
{
	friend class KickerDataType;

	Serialize::Storage<ChanServ::Channel *> channel;

	Serialize::Storage<bool> amsgs,
	     badwords,
	     bolds,
	     caps,
	     colors,
	     flood,
	     italics,
	     repeat,
	     reverses,
	     underlines;

	Serialize::Storage<int16_t> ttb_bolds,
	        ttb_colors,
	        ttb_reverses,
	        ttb_underlines,
	        ttb_badwords,
	        ttb_caps,
	        ttb_flood,
	        ttb_repeat,
	        ttb_italics,
	        ttb_amsgs;

	Serialize::Storage<int16_t> capsmin,
	        capspercent,
	        floodlines,
	        floodsecs,
	        repeattimes;

	Serialize::Storage<bool> dontkickops,
	     dontkickvoices;

 public:
	using KickerData::KickerData;

	ChanServ::Channel *GetChannel() override;
	void SetChannel(ChanServ::Channel *) override;

	bool GetAmsgs() override;
	void SetAmsgs(const bool &) override;

	bool GetBadwords() override;
	void SetBadwords(const bool &) override;

	bool GetBolds() override;
	void SetBolds(const bool &) override;

	bool GetCaps() override;
	void SetCaps(const bool &) override;

	bool GetColors() override;
	void SetColors(const bool &) override;

	bool GetFlood() override;
	void SetFlood(const bool &) override;

	bool GetItalics() override;
	void SetItalics(const bool &) override;

	bool GetRepeat() override;
	void SetRepeat(const bool &) override;

	bool GetReverses() override;
	void SetReverses(const bool &) override;

	bool GetUnderlines() override;
	void SetUnderlines(const bool &) override;

	int16_t GetTTBBolds() override;
	void SetTTBBolds(const int16_t &) override;

	int16_t GetTTBColors() override;
	void SetTTBColors(const int16_t &) override;

	int16_t GetTTBReverses() override;
	void SetTTBReverses(const int16_t &) override;

	int16_t GetTTBUnderlines() override;
	void SetTTBUnderlines(const int16_t &) override;

	int16_t GetTTBBadwords() override;
	void SetTTBBadwords(const int16_t &) override;

	int16_t GetTTBCaps() override;
	void SetTTBCaps(const int16_t &) override;
	
	int16_t GetTTBFlood() override;
	void SetTTBFlood(const int16_t &) override;

	int16_t GetTTBRepeat() override;
	void SetTTBRepeat(const int16_t &) override;

	int16_t GetTTBItalics() override;
	void SetTTBItalics(const int16_t &) override;

	int16_t GetTTBAmsgs() override;
	void SetTTBAmsgs(const int16_t &) override;

	int16_t GetCapsMin() override;
	void SetCapsMin(const int16_t &) override;

	int16_t GetCapsPercent() override;
	void SetCapsPercent(const int16_t &) override;

	int16_t GetFloodLines() override;
	void SetFloodLines(const int16_t &) override;

	int16_t GetFloodSecs() override;
	void SetFloodSecs(const int16_t &) override;

	int16_t GetRepeatTimes() override;
	void SetRepeatTimes(const int16_t &) override;

	bool GetDontKickOps() override;
	void SetDontKickOps(const bool &) override;

	bool GetDontKickVoices() override;
	void SetDontKickVoices(const bool &) override;
};

class KickerDataType : public Serialize::Type<KickerDataImpl>
{
 public:
	Serialize::ObjectField<KickerDataImpl, ChanServ::Channel *> channel;

	Serialize::Field<KickerDataImpl, bool> amsgs,
	                                       badwords,
	                                       bolds,
	                                       caps,
	                                       colors,
	                                       flood,
	                                       italics,
	                                       repeat,
	                                       reverses,
	                                       underlines;

	Serialize::Field<KickerDataImpl, int16_t> ttb_bolds,
	                                          ttb_colors,
	                                          ttb_reverses,
	                                          ttb_underlines,
	                                          ttb_badwords,
	                                          ttb_caps,
	                                          ttb_flood,
	                                          ttb_repeat,
	                                          ttb_italics,
	                                          ttb_amsgs,
	                                          capsmin,
	                                          capspercent,
	                                          floodlines,
	                                          floodsecs,
	                                          repeattimes;

	Serialize::Field<KickerDataImpl, bool> dontkickops,
	                                       dontkickvoices;

	KickerDataType(Module *owner) : Serialize::Type<KickerDataImpl>(owner)
		, channel(this, "channel", &KickerDataImpl::channel, true)
		, amsgs(this, "amsgs", &KickerDataImpl::amsgs)
		, badwords(this, "badwords", &KickerDataImpl::badwords)
		, bolds(this, "bolds", &KickerDataImpl::bolds)
		, caps(this, "caps", &KickerDataImpl::caps)
		, colors(this, "colors", &KickerDataImpl::colors)
		, flood(this, "flood", &KickerDataImpl::flood)
		, italics(this, "italics", &KickerDataImpl::italics)
		, repeat(this, "repeat", &KickerDataImpl::repeat)
		, reverses(this, "reverses", &KickerDataImpl::reverses)
		, underlines(this, "underlines", &KickerDataImpl::underlines)
		, ttb_bolds(this, "ttb_bolds", &KickerDataImpl::ttb_bolds)
		, ttb_colors(this, "ttb_colors", &KickerDataImpl::ttb_colors)
		, ttb_reverses(this, "ttb_reverses", &KickerDataImpl::ttb_reverses)
		, ttb_underlines(this, "ttb_underlines", &KickerDataImpl::ttb_underlines)
		, ttb_badwords(this, "ttb_badwords", &KickerDataImpl::ttb_badwords)
		, ttb_caps(this, "ttb_caps", &KickerDataImpl::ttb_caps)
		, ttb_flood(this, "ttb_flood", &KickerDataImpl::ttb_flood)
		, ttb_repeat(this, "ttb_repeat", &KickerDataImpl::ttb_repeat)
		, ttb_italics(this, "ttb_italics", &KickerDataImpl::ttb_italics)
		, ttb_amsgs(this, "ttb_amsgs", &KickerDataImpl::ttb_amsgs)
		, capsmin(this, "capsmin", &KickerDataImpl::capsmin)
		, capspercent(this, "capspercent", &KickerDataImpl::capspercent)
		, floodlines(this, "floodlines", &KickerDataImpl::floodlines)
		, floodsecs(this, "floodsecs", &KickerDataImpl::floodsecs)
		, repeattimes(this, "repeattimes", &KickerDataImpl::repeattimes)
		, dontkickops(this, "dontkickops", &KickerDataImpl::dontkickops)
		, dontkickvoices(this, "dontkickvoices", &KickerDataImpl::dontkickvoices)
	{
	}
};

ChanServ::Channel *KickerDataImpl::GetChannel()
{
	return Get(&KickerDataType::channel);
}

void KickerDataImpl::SetChannel(ChanServ::Channel *channel)
{
	Set(&KickerDataType::amsgs, channel);
}

bool KickerDataImpl::GetAmsgs()
{
	return Get(&KickerDataType::amsgs);
}

void KickerDataImpl::SetAmsgs(const bool &b)
{
	Set(&KickerDataType::amsgs, b);
}

bool KickerDataImpl::GetBadwords()
{
	return Get(&KickerDataType::badwords);
}

void KickerDataImpl::SetBadwords(const bool &b)
{
	Set(&KickerDataType::badwords, b);
}

bool KickerDataImpl::GetBolds()
{
	return Get(&KickerDataType::bolds);
}

void KickerDataImpl::SetBolds(const bool &b)
{
	Set(&KickerDataType::bolds, b);
}

bool KickerDataImpl::GetCaps()
{
	return Get(&KickerDataType::caps);
}

void KickerDataImpl::SetCaps(const bool &b)
{
	Set(&KickerDataType::caps, b);
}

bool KickerDataImpl::GetColors()
{
	return Get(&KickerDataType::colors);
}

void KickerDataImpl::SetColors(const bool &b)
{
	Set(&KickerDataType::colors, b);
}

bool KickerDataImpl::GetFlood()
{
	return Get(&KickerDataType::flood);
}

void KickerDataImpl::SetFlood(const bool &b)
{
	Set(&KickerDataType::flood, b);
}

bool KickerDataImpl::GetItalics()
{
	return Get(&KickerDataType::italics);
}

void KickerDataImpl::SetItalics(const bool &b)
{
	Set(&KickerDataType::italics, b);
}

bool KickerDataImpl::GetRepeat()
{
	return Get(&KickerDataType::repeat);
}

void KickerDataImpl::SetRepeat(const bool &b)
{
	Set(&KickerDataType::repeat, b);
}

bool KickerDataImpl::GetReverses()
{
	return Get(&KickerDataType::reverses);
}

void KickerDataImpl::SetReverses(const bool &b)
{
	Set(&KickerDataType::reverses, b);
}

bool KickerDataImpl::GetUnderlines()
{
	return Get(&KickerDataType::underlines);
}

void KickerDataImpl::SetUnderlines(const bool &b)
{
	Set(&KickerDataType::underlines, b);
}

int16_t KickerDataImpl::GetTTBBolds()
{
	return Get(&KickerDataType::ttb_bolds);
}

void KickerDataImpl::SetTTBBolds(const int16_t &i)
{
	Set(&KickerDataType::ttb_bolds, i);
}

int16_t KickerDataImpl::GetTTBColors()
{
	return Get(&KickerDataType::ttb_colors);
}

void KickerDataImpl::SetTTBColors(const int16_t &i)
{
	Set(&KickerDataType::ttb_colors, i);
}

int16_t KickerDataImpl::GetTTBReverses()
{
	return Get(&KickerDataType::ttb_reverses);
}

void KickerDataImpl::SetTTBReverses(const int16_t &i)
{
	Set(&KickerDataType::ttb_reverses, i);
}

int16_t KickerDataImpl::GetTTBUnderlines()
{
	return Get(&KickerDataType::ttb_underlines);
}

void KickerDataImpl::SetTTBUnderlines(const int16_t &i)
{
	Set(&KickerDataType::ttb_underlines, i);
}

int16_t KickerDataImpl::GetTTBBadwords()
{
	return Get(&KickerDataType::ttb_badwords);
}

void KickerDataImpl::SetTTBBadwords(const int16_t &i)
{
	Set(&KickerDataType::ttb_badwords, i);
}

int16_t KickerDataImpl::GetTTBCaps()
{
	return Get(&KickerDataType::ttb_caps);
}

void KickerDataImpl::SetTTBCaps(const int16_t &i)
{
	Set(&KickerDataType::ttb_caps, i);
}
	
int16_t KickerDataImpl::GetTTBFlood()
{
	return Get(&KickerDataType::ttb_flood);
}

void KickerDataImpl::SetTTBFlood(const int16_t &i)
{
	Set(&KickerDataType::ttb_flood, i);
}

int16_t KickerDataImpl::GetTTBRepeat()
{
	return Get(&KickerDataType::ttb_repeat);
}

void KickerDataImpl::SetTTBRepeat(const int16_t &i)
{
	Set(&KickerDataType::ttb_repeat, i);
}

int16_t KickerDataImpl::GetTTBItalics()
{
	return Get(&KickerDataType::ttb_italics);
}

void KickerDataImpl::SetTTBItalics(const int16_t &i)
{
	Set(&KickerDataType::ttb_italics, i);
}

int16_t KickerDataImpl::GetTTBAmsgs()
{
	return Get(&KickerDataType::ttb_amsgs);
}

void KickerDataImpl::SetTTBAmsgs(const int16_t &i)
{
	Set(&KickerDataType::ttb_amsgs, i);
}

int16_t KickerDataImpl::GetCapsMin()
{
	return Get(&KickerDataType::capsmin);
}

void KickerDataImpl::SetCapsMin(const int16_t &i)
{
	Set(&KickerDataType::capsmin, i);
}

int16_t KickerDataImpl::GetCapsPercent()
{
	return Get(&KickerDataType::capspercent);
}

void KickerDataImpl::SetCapsPercent(const int16_t &i)
{
	Set(&KickerDataType::capspercent, i);
}

int16_t KickerDataImpl::GetFloodLines()
{
	return Get(&KickerDataType::floodlines);
}

void KickerDataImpl::SetFloodLines(const int16_t &i)
{
	Set(&KickerDataType::floodlines, i);
}

int16_t KickerDataImpl::GetFloodSecs()
{
	return Get(&KickerDataType::floodsecs);
}

void KickerDataImpl::SetFloodSecs(const int16_t &i)
{
	Set(&KickerDataType::floodsecs, i);
}

int16_t KickerDataImpl::GetRepeatTimes()
{
	return Get(&KickerDataType::repeattimes);
}

void KickerDataImpl::SetRepeatTimes(const int16_t &i)
{
	Set(&KickerDataType::repeattimes, i);
}

bool KickerDataImpl::GetDontKickOps()
{
	return Get(&KickerDataType::dontkickops);
}

void KickerDataImpl::SetDontKickOps(const bool &b)
{
	Set(&KickerDataType::dontkickops, b);
}

bool KickerDataImpl::GetDontKickVoices()
{
	return Get(&KickerDataType::dontkickvoices);
}

void KickerDataImpl::SetDontKickVoices(const bool &b)
{
	Set(&KickerDataType::dontkickvoices, b);
}

class CommandBSKick : public Command
{
 public:
	CommandBSKick(Module *creator) : Command(creator, "botserv/kick", 0)
	{
		this->SetDesc(_("Configures kickers"));
		this->SetSyntax(_("\037option\037 \037channel\037 {\037ON|OFF\037} [\037settings\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Configures bot kickers."
		               " Use of this command requires the \002SET\002 privilege on \037channel\037.\n"
		               "\n"
		               "Available kickers:"));

		Anope::string this_name = source.command;
		for (CommandInfo::map::const_iterator it = source.service->commands.begin(), it_end = source.service->commands.end(); it != it_end; ++it)
		{
			const Anope::string &c_name = it->first;
			const CommandInfo &info = it->second;

			if (c_name.find_ci(this_name + " ") == 0)
			{
				ServiceReference<Command> command(info.name);
				if (command)
				{
					source.command = c_name;
					command->OnServHelp(source);
				}
			}
		}

		CommandInfo *help = source.service->FindCommand("generic/help");
		if (help)
			source.Reply(_("See \002{0}{1} {2} {3} \037option\037\002 for more information on a specific option."),
		                       Config->StrictPrivmsg, source.service->nick, help->cname, this_name);

		return true;
	}
};

class CommandBSKickBase : public Command
{
 public:
	CommandBSKickBase(Module *creator, const Anope::string &cname, int minarg, int maxarg) : Command(creator, cname, minarg, maxarg)
	{
	}

	virtual void Execute(CommandSource &source, const std::vector<Anope::string> &params) override anope_abstract;

	virtual bool OnHelp(CommandSource &source, const Anope::string &subcommand) override anope_abstract;

 protected:
	bool CheckArguments(CommandSource &source, const std::vector<Anope::string> &params, ChanServ::Channel* &ci)
	{
		const Anope::string &chan = params[0];
		const Anope::string &option = params[1];

		ci = ChanServ::Find(chan);

		if (Anope::ReadOnly)
			source.Reply(_("Sorry, kicker configuration is temporarily disabled."));
		else if (ci == NULL)
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
		else if (option.empty())
			this->OnSyntaxError(source, "");
		else if (!option.equals_ci("ON") && !option.equals_ci("OFF"))
			this->OnSyntaxError(source, "");
		else if (!source.AccessFor(ci).HasPriv("SET") && !source.HasPriv("botserv/administration"))
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
		else if (!ci->GetBot())
			source.Reply(_("There is no bot assigned to \002{0}\002."), ci->GetName());
		else
			return true;

		return false;
	}

	bool CheckTTB(CommandSource &source, const Anope::string &ttb, int16_t &i)
	{
		i = 0;
		if (ttb.empty())
			return true;

		try
		{
			i = convertTo<int16_t>(ttb);
			if (i < 0)
				throw ConvertException();
		}
		catch (const ConvertException &)
		{
			i = 0;
			source.Reply(_("\002{0}\002 can not be taken as times to ban. Times to ban must be a positive integer."), ttb);
			return false;
		}

		return true;
	}

	void Process(CommandSource &source, ChanServ::Channel *ci, const Anope::string &param, const Anope::string &ttb, void (KickerData::*setter)(const bool &), void (KickerData::*ttbsetter)(const int16_t &), const Anope::string &optname)
	{
		KickerData *kd = ci->GetRef<KickerData *>();

		if (param.equals_ci("ON"))
		{
			int16_t i;
			if (!CheckTTB(source, ttb, i))
				return;

			(kd->*setter)(true);
			(kd->*ttbsetter)(i);

			if (i)
				source.Reply(_("Bot will now kick for \002{0}\002, and will place a ban after \002{1}\002 kicks for the same user."), optname, i);
			else
				source.Reply(_("Bot will now kick for \002{0}\002."), optname);

			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enable the " << optname << " kicker";
		}
		else if (param.equals_ci("OFF"))
		{
			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to disable the " << optname << " kicker";

			(kd->*setter)(false);
			(kd->*ttbsetter)(0);

			source.Reply(_("Bot won't kick for \002{0}\002 anymore."), optname);
		}
		else
			this->OnSyntaxError(source, "");
	}
};

class CommandBSKickAMSG : public CommandBSKickBase
{
 public:
	CommandBSKickAMSG(Module *creator) : CommandBSKickBase(creator, "botserv/kick/amsg", 2, 3)
	{
		this->SetDesc(_("Configures AMSG kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChanServ::Channel *ci;
		if (CheckArguments(source, params, ci))
			Process(source, ci, params[1], params.size() > 2 ? params[2] : "", &KickerData::SetAmsgs, &KickerData::SetTTBAmsgs, "amsgs");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sets the AMSG kicker on or off on \037channel\037. When enabled, the bot will kick users who send the same message to multiple channels where {0} bots are.\n"
		               "\n"
		               "\037ttb\037 is the number of times a user can be kicked before they get banned. Don't give ttb to disable the ban system."),
		               source.service->nick);
		return true;
	}
};

class CommandBSKickBadwords : public CommandBSKickBase
{
 public:
	CommandBSKickBadwords(Module *creator) : CommandBSKickBase(creator, "botserv/kick/badwords", 2, 3)
	{
		this->SetDesc(_("Configures badwords kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChanServ::Channel *ci;
		if (CheckArguments(source, params, ci))
			Process(source, ci, params[1], params.size() > 2 ? params[2] : "", &KickerData::SetBadwords, &KickerData::SetTTBBadwords, "badwords");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		ServiceBot *bi;
		Anope::string name;
		CommandInfo *help;
		source.Reply(_("Sets the bad words kicker on or off on \037channel\037. When enabled, the bot will kick users who say certain words on the channel."));
		if (Command::FindCommandFromService("botserv/badwords", bi, name) && (help = bi->FindCommand("generic/help")))
			source.Reply(_("You can define bad words for your channel using the \002{0}\002 command. See \002{1}{2} {3} {4}\002 for more information."),
			               name, Config->StrictPrivmsg, bi->nick, help->cname, name);
		source.Reply(_("\n"
		               "\037ttb\037 is the number of times a user can be kicked before they get banned. Don't give ttb to disable the ban system."));
		return true;
	}
};

class CommandBSKickBolds : public CommandBSKickBase
{
 public:
	CommandBSKickBolds(Module *creator) : CommandBSKickBase(creator, "botserv/kick/bolds", 2, 3)
	{
		this->SetDesc(_("Configures bolds kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChanServ::Channel *ci;
		if (CheckArguments(source, params, ci))
			Process(source, ci, params[1], params.size() > 2 ? params[2] : "", &KickerData::SetBolds, &KickerData::SetTTBBolds, "bolds");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sets the bolds kicker on or off on \037channel\037. When enabled, the bot will kick users who use \002bolds\002.\n"
		               "\n"
		               "\037ttb\037 is the number of times a user can be kicked before they get banned. Don't give ttb to disable the ban system."));
		return true;
	}
};

class CommandBSKickCaps : public CommandBSKickBase
{
 public:
	CommandBSKickCaps(Module *creator) : CommandBSKickBase(creator, "botserv/kick/caps", 2, 5)
	{
		this->SetDesc(_("Configures caps kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037 [\037min\037 [\037percent\037]]]\002"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChanServ::Channel *ci;
		if (!CheckArguments(source, params, ci))
			return;

		KickerData *kd = GetKickerData(ci);

		if (params[1].equals_ci("ON"))
		{
			const Anope::string &ttb = params.size() > 2 ? params[2] : "",
				&min = params.size() > 3 ? params[3] : "",
				&percent = params.size() > 4 ? params[4] : "";

			int16_t i;
			if (!CheckTTB(source, ttb, i))
				return;

			kd->SetCaps(true);
			kd->SetTTBCaps(i);

			kd->SetCapsMin(10);
			try
			{
				kd->SetCapsMin(convertTo<int16_t>(min));
			}
			catch (const ConvertException &) { }
			if (kd->GetCapsMin() < 1)
				kd->SetCapsMin(10);

			try
			{
				kd->SetCapsPercent(convertTo<int16_t>(percent));
			}
			catch (const ConvertException &) { }
			if (kd->GetCapsPercent() < 1 || kd->GetCapsPercent() > 100)
				kd->SetCapsPercent(25);

			if (i)
				source.Reply(_("Bot will now kick for \002caps\002 if they constitute at least {0} characters and {1}% of the entire message, and will place a ban after {2} kicks for the same user."), kd->GetCapsMin(), kd->GetCapsPercent(), i);
			else
				source.Reply(_("Bot will now kick for \002caps\002 if they constitute at least {0} characters and {1}% of the entire message."), kd->GetCapsMin(), kd->GetCapsPercent());
		}
		else
		{
			kd->SetCaps(false);
			kd->SetTTBCaps(0);
			kd->SetCapsMin(0);
			kd->SetCapsPercent(0);
			source.Reply(_("Bot won't kick for \002caps\002 anymore."));
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sets the caps kicker on or off on \037channel\037. When enabled, the bot will kick users who talk in CAPS."
		                " The bot kicks only if there are at least \002min\002 caps and they constitute at least \002percent\002% of the total text line."
		                " (if not given, it defaults to 10 characters and 25%).\n"
		                "\n"
		                "\037ttb\037 is the number of times a user can be kicked before they get banned. Don't give ttb to disable the ban system."));
		return true;
	}
};

class CommandBSKickColors : public CommandBSKickBase
{
 public:
	CommandBSKickColors(Module *creator) : CommandBSKickBase(creator, "botserv/kick/colors", 2, 3)
	{
		this->SetDesc(_("Configures color kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChanServ::Channel *ci;
		if (CheckArguments(source, params, ci))
			Process(source, ci, params[1], params.size() > 2 ? params[2] : "", &KickerData::SetColors, &KickerData::SetTTBColors, "colors");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sets the colors kicker on or off on \037channel\037. When enabled, the bot will kick users who use \00304c\00307o\00308l\00303o\00306r\00312s\017.\n"
		               "\n"
		                "\037ttb\037 is the number of times a user can be kicked before they get banned. Don't give ttb to disable the ban system."));
		return true;
	}
};

class CommandBSKickFlood : public CommandBSKickBase
{
 public:
	CommandBSKickFlood(Module *creator) : CommandBSKickBase(creator, "botserv/kick/flood", 2, 5)
	{
		this->SetDesc(_("Configures flood kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037 [\037lines\037 [\037seconds\037]]]\002"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChanServ::Channel *ci;
		if (!CheckArguments(source, params, ci))
			return;

		KickerData *kd = GetKickerData(ci);

		if (params[1].equals_ci("ON"))
		{
			const Anope::string &ttb = params.size() > 2 ? params[2] : "",
						&lines = params.size() > 3 ? params[3] : "",
						&secs = params.size() > 4 ? params[4] : "";

			int16_t i;
			if (!CheckTTB(source, ttb, i))
				return;

			kd->SetFlood(true);
			kd->SetTTBFlood(i);

			kd->SetFloodLines(6);
			try
			{
				kd->SetFloodLines(convertTo<int16_t>(lines));
			}
			catch (const ConvertException &) { }
			if (kd->GetFloodLines() < 2)
				kd->SetFloodLines(6);

			kd->SetFloodSecs(10);
			try
			{
				kd->SetFloodSecs(convertTo<int16_t>(secs));
			}
			catch (const ConvertException &) { }
			if (kd->GetFloodSecs() < 1)
				kd->SetFloodSecs(10);
			if (kd->GetFloodSecs() > Config->GetModule(me)->Get<time_t>("keepdata"))
				kd->SetFloodSecs(Config->GetModule(me)->Get<time_t>("keepdata"));

			if (i)
				source.Reply(_("Bot will now kick for \002flood\002 ({0} lines in {1} seconds and will place a ban after {2} kicks for the same user."), kd->GetFloodLines(), kd->GetFloodSecs(), i);
			else
				source.Reply(_("Bot will now kick for \002flood\002 ({0} lines in {1} seconds)."), kd->GetFloodLines(), kd->GetFloodSecs());
		}
		else if (params[1].equals_ci("OFF"))
		{
			kd->SetFlood(false);
			kd->SetFloodLines(0);
			kd->SetFloodSecs(0);
			source.Reply(_("Bot won't kick for \002flood\002 anymore."));
		}
		else
		{
			this->OnSyntaxError(source, params[1]);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sets the flood kicker on or off on \037channel\037. When enabled, the bot to kick users who are flooding the channel with at least \037lines\037 lines in \037seconds\037 seconds. If not given, it defaults to 6 lines in 10 seconds.\n"
		               "\n"
		                "\037ttb\037 is the number of times a user can be kicked before they get banned. Don't give ttb to disable the ban system."));
		return true;
	}
};

class CommandBSKickItalics : public CommandBSKickBase
{
 public:
	CommandBSKickItalics(Module *creator) : CommandBSKickBase(creator, "botserv/kick/italics", 2, 3)
	{
		this->SetDesc(_("Configures italics kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChanServ::Channel *ci;
		if (CheckArguments(source, params, ci))
			Process(source, ci, params[1], params.size() > 2 ? params[2] : "", &KickerData::SetItalics, &KickerData::SetTTBItalics, "itlaics");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sets the italics kicker on or off on \037channel\037. When enabled, the bot will kick users who use \035italics\035."
		               "\n"
		               "\037ttb\037 is the number of times a user can be kicked before they get banned. Don't give ttb to disable the ban system."));
		return true;
	}
};

class CommandBSKickRepeat : public CommandBSKickBase
{
 public:
	CommandBSKickRepeat(Module *creator) : CommandBSKickBase(creator, "botserv/kick/repeat", 2, 4)
	{
		this->SetDesc(_("Configures repeat kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037 [\037num\037]]\002"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChanServ::Channel *ci;
		if (!CheckArguments(source, params, ci))
			return;

		KickerData *kd = GetKickerData(ci);

		if (params[1].equals_ci("ON"))
		{
			const Anope::string &ttb = params.size() > 2 ? params[2] : "",
						&times = params.size() > 3 ? params[3] : "";

			int16_t i;
			if (!CheckTTB(source, ttb, i))
				return;

			kd->SetRepeat(true);
			kd->SetTTBRepeat(i);
			kd->SetRepeatTimes(3);
			try
			{
				kd->SetRepeatTimes(convertTo<int16_t>(times));
			}
			catch (const ConvertException &) { }
			if (kd->GetRepeatTimes() < 1)
				kd->SetRepeatTimes(3);

			if (i)
			{
				if (kd->GetRepeatTimes() != 1)
					source.Reply(_("Bot will now kick for \002repeats\002 (users that say the same thing {0} times),"
							" and will place a ban after {1} kicks for the same user."), kd->GetRepeatTimes(), i);
				else
					source.Reply(_("Bot will now kick for \002repeats\002 (users that say the same thing {0} time),"
							" and will place a ban after {1} kicks for the same user."), kd->GetRepeatTimes(), i);
			}
			else
			{
				if (kd->GetRepeatTimes() != 1)
					source.Reply(_("Bot will now kick for \002repeats\002 (users that say the same thing {0} times)."), kd->GetRepeatTimes());
				else
					source.Reply(_("Bot will now kick for \002repeats\002 (users that say the same thing {0} time)."), kd->GetRepeatTimes());

			}
		}
		else if (params[1].equals_ci("OFF"))
		{
			kd->SetRepeat(false);
			kd->SetTTBRepeat(0);
			kd->SetRepeatTimes(0);
			source.Reply(_("Bot won't kick for \002repeats\002 anymore."));
		}
		else
		{
			this->OnSyntaxError(source, params[1]);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sets the repeat kicker on or off. When enabled, the bot will kick users who repeat themselves \037num\037 times. If \037num\037 is not given, it defaults to 3.\n"
		               "\n"
		               "\037ttb\037 is the number of times a user can be kicked before they get banned. Don't give ttb to disable the ban system."));
		return true;
	}
};

class CommandBSKickReverses : public CommandBSKickBase
{
 public:
 	CommandBSKickReverses(Module *creator) : CommandBSKickBase(creator, "botserv/kick/reverses", 2, 3)
	{
		this->SetDesc(_("Configures reverses kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChanServ::Channel *ci;
		if (CheckArguments(source, params, ci))
			Process(source, ci, params[1], params.size() > 2 ? params[2] : "", &KickerData::SetReverses, &KickerData::SetTTBReverses, "reverses");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sets the reverses kicker on or off. When enabled, the bot will kick users who use \026reverses\026.\n"
		                "\n"
		                "\037ttb\037 is the number of times a user can be kicked before they get banned. Don't give ttb to disable the ban system."));
		return true;
	}
};

class CommandBSKickUnderlines : public CommandBSKickBase
{
 public:
	CommandBSKickUnderlines(Module *creator) : CommandBSKickBase(creator, "botserv/kick/underlines", 2, 3)
	{
		this->SetDesc(_("Configures underlines kicker"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037} [\037ttb\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChanServ::Channel *ci;
		if (CheckArguments(source, params, ci))
			Process(source, ci, params[1], params.size() > 2 ? params[2] : "", &KickerData::SetUnderlines, &KickerData::SetTTBUnderlines, "underlines");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sets the underlines kicker on or off. When enabled, the bot will kick users who use \037underlines\037\n"
		               "\n"
		               "\037ttb\037 is the number of times a user can be kicked before they get banned. Don't give ttb to disable the ban system."));
		return true;
	}
};

class CommandBSSetDontKickOps : public Command
{
 public:
	CommandBSSetDontKickOps(Module *creator, const Anope::string &sname = "botserv/set/dontkickops") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("To protect ops against bot kicks"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChanServ::Channel *ci = ChanServ::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), params[0]);
			return;
		}

		ChanServ::AccessGroup access = source.AccessFor(ci);
		if (!source.HasPriv("botserv/administration") && !access.HasPriv("SET"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, bot option setting is temporarily disabled."));
			return;
		}

		KickerData *kd = GetKickerData(ci);

		if (params[1].equals_ci("ON"))
		{
			bool override = !access.HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enable dontkickops";

			kd->SetDontKickOps(true);
			source.Reply(_("Bot \002won't kick ops\002 on channel \002{0}\002."), ci->GetName());
		}
		else if (params[1].equals_ci("OFF"))
		{
			bool override = !access.HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to disable dontkickops";

			kd->SetDontKickOps(false);
			source.Reply(_("Bot \002will kick ops\002 on channel \002{0}\002."), ci->GetName());
		}
		else
		{
			this->OnSyntaxError(source, source.command);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Enables or disables \002op protection\002 mode on a channel."
		               " When it is enabled, ops won't be kicked by the bot, even if they don't have the \002{0}\002 privilege.\n"
		               "\n"
		               "Use of this command requires the \002{1}\002 privilege on \037channel\037."),
		               "NOKICK", "SET");
		return true;
	}
};

class CommandBSSetDontKickVoices : public Command
{
 public:
	CommandBSSetDontKickVoices(Module *creator, const Anope::string &sname = "botserv/set/dontkickvoices") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("To protect voices against bot kicks"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChanServ::Channel *ci = ChanServ::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), params[0]);
			return;
		}

		ChanServ::AccessGroup access = source.AccessFor(ci);
		if (!source.HasPriv("botserv/administration") && !access.HasPriv("SET"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, bot option setting is temporarily disabled."));
			return;
		}

		KickerData *kd = GetKickerData(ci);

		if (params[1].equals_ci("ON"))
		{
			bool override = !access.HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enable dontkickvoices";

			kd->SetDontKickVoices(true);
			source.Reply(_("Bot \002won't kick voices\002 on channel %s."), ci->GetName().c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			bool override = !access.HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to disable dontkickvoices";

			kd->SetDontKickVoices(false);
			source.Reply(_("Bot \002will kick voices\002 on channel %s."), ci->GetName().c_str());
		}
		else
		{
			this->OnSyntaxError(source, source.command);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Enables or disables \002voice protection\002 mode on a channel."
		               " When it is enabled, ops won't be kicked by the bot, even if they don't have the \002{0}\002 privilege.\n"
		               "\n"
		               "Use of this command requires the \002{1}\002 privilege on \037channel\037."),
		               "NOKICK", "SET");
		return true;
	}
};

struct BanData
{
	struct Data
	{
		Anope::string mask;
		time_t last_use;
		int16_t ttb[TTB_SIZE];

		Data()
		{
			last_use = 0;
			for (int i = 0; i < TTB_SIZE; ++i)
				this->ttb[i] = 0;
		}
	};

 private:
	typedef Anope::map<Data> data_type;
	data_type data_map;

 public:
	Data &get(const Anope::string &key)
	{
		return this->data_map[key];
	}

	bool empty() const
	{
		return this->data_map.empty();
	}

	void purge()
	{
		time_t keepdata = Config->GetModule(me)->Get<time_t>("keepdata");
		for (data_type::iterator it = data_map.begin(), it_end = data_map.end(); it != it_end;)
		{
			const Anope::string &user = it->first;
			Data &bd = it->second;
			++it;

			if (Anope::CurTime - bd.last_use > keepdata)
				data_map.erase(user);
		}
	}
};

struct UserData
{
	UserData()
	{
		last_use = last_start = Anope::CurTime;
	}

	/* Data validity */
	time_t last_use;

	/* for flood kicker */
	int16_t lines = 0;
	time_t last_start;

	/* for repeat kicker */
	Anope::string lasttarget;
	int16_t times = 0;

	Anope::string lastline;
};

class BanDataPurger : public Timer
{
 public:
	BanDataPurger(Module *o) : Timer(o, 300, Anope::CurTime, true) { }

	void Tick(time_t) override
	{
		Log(LOG_DEBUG) << "bs_main: Running bandata purger";

		for (channel_map::iterator it = ChannelList.begin(), it_end = ChannelList.end(); it != it_end; ++it)
		{
			Channel *c = it->second;

			BanData *bd = c->GetExt<BanData>("bandata");
			if (bd != NULL)
			{
				bd->purge();
				if (bd->empty())
					c->Shrink<BanData>("bandata");
			}
		}
	}
};

class BSKick : public Module
	, public EventHook<Event::ServiceBotEvent>
	, public EventHook<Event::Privmsg>
{
	ExtensibleItem<BanData> bandata;
	ExtensibleItem<UserData> userdata;

	KickerDataType kdtype;

	CommandBSKick commandbskick;
	CommandBSKickAMSG commandbskickamsg;
	CommandBSKickBadwords commandbskickbadwords;
	CommandBSKickBolds commandbskickbolds;
	CommandBSKickCaps commandbskickcaps;
	CommandBSKickColors commandbskickcolors;
	CommandBSKickFlood commandbskickflood;
	CommandBSKickItalics commandbskickitalics;
	CommandBSKickRepeat commandbskickrepeat;
	CommandBSKickReverses commandbskickreverse;
	CommandBSKickUnderlines commandbskickunderlines;

	CommandBSSetDontKickOps commandbssetdontkickops;
	CommandBSSetDontKickVoices commandbssetdontkickvoices;

	BanDataPurger purger;
	
	ServiceReference<BadWords> badwords;

	BanData::Data &GetBanData(User *u, Channel *c)
	{
		BanData *bd = bandata.Require(c);
		return bd->get(u->GetMask());
	}

	UserData *GetUserData(User *u, Channel *c)
	{
		ChanUserContainer *uc = c->FindUser(u);
		if (uc == nullptr)
			return nullptr;

		return userdata.Require(uc);
       }

	template<typename... Args>
	void TakeAction(ChanServ::Channel *ci, User *u, int ttb, TTBType ttbtype, const Anope::string &message, Args&&... args)
	{
		/* Don't ban ulines or protected users */
		if (u->IsProtected())
			return;

		BanData::Data &bd = this->GetBanData(u, ci->c);

		++bd.ttb[ttbtype];
		if (ttb && bd.ttb[ttbtype] >= ttb)
		{
			bd.ttb[ttbtype] = 0;

			Anope::string mask = ci->GetIdealBan(u);

			ci->c->SetMode(NULL, "BAN", mask);
			EventManager::Get()->Dispatch(&Event::BotBan::OnBotBan, u, ci, mask);
		}

		if (!ci->c->FindUser(u))
			return;

		Anope::string buf = Anope::Format(message, std::forward<Args>(args)...);
		ci->c->Kick(ci->GetBot(), u, buf);
	}

 public:
	BSKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::ServiceBotEvent>(this)
		, EventHook<Event::Privmsg>(this)

		, bandata(this, "bandata")
		, userdata(this, "userdata")

		, kdtype(this)

		, commandbskick(this)
		, commandbskickamsg(this)
		, commandbskickbadwords(this)
		, commandbskickbolds(this)
		, commandbskickcaps(this)
		, commandbskickcolors(this)
		, commandbskickflood(this)
		, commandbskickitalics(this)
		, commandbskickrepeat(this)
		, commandbskickreverse(this)
		, commandbskickunderlines(this)

		, commandbssetdontkickops(this)
		, commandbssetdontkickvoices(this)

		, purger(this)
	{
		me = this;
	}

	void OnServiceBot(CommandSource &source, ServiceBot *bi, ChanServ::Channel *ci, InfoFormatter &info) override
	{
		if (!ci)
			return;

		Anope::string enabled = Language::Translate(source.nc, _("Enabled"));
		Anope::string disabled = Language::Translate(source.nc, _("Disabled"));
		KickerData *kd = ci->GetRef<KickerData *>();

		if (kd && kd->GetBadwords())
		{
			if (kd->GetTTBBadwords())
				info[_("Bad words kicker")] = Anope::printf("%s (%d kick(s) to ban)", enabled.c_str(), kd->GetBadwords());
			else
				info[_("Bad words kicker")] = enabled;
		}
		else
		{
			info[_("Bad words kicker")] = disabled;
		}

		if (kd && kd->GetBolds())
		{
			if (kd->GetTTBBolds())
				info[_("Bolds kicker")] = Anope::printf("%s (%d kick(s) to ban)", enabled.c_str(), kd->GetTTBBolds());
			else
				info[_("Bolds kicker")] = enabled;
		}
		else
		{
			info[_("Bolds kicker")] = disabled;
		}

		if (kd && kd->GetCaps())
		{
			if (kd->GetTTBCaps())
				info[_("Caps kicker")] = Anope::printf(_("%s (%d kick(s) to ban; minimum %d/%d%%)"), enabled.c_str(), kd->GetTTBCaps(), kd->GetCapsMin(), kd->GetCapsPercent());
			else
				info[_("Caps kicker")] = Anope::printf(_("%s (minimum %d/%d%%)"), enabled.c_str(), kd->GetCapsMin(), kd->GetCapsPercent());
		}
		else
		{
			info[_("Caps kicker")] = disabled;
		}

		if (kd && kd->GetColors())
		{
			if (kd->GetTTBColors())
				info[_("Colors kicker")] = Anope::printf(_("%s (%d kick(s) to ban)"), enabled.c_str(), kd->GetTTBColors());
			else
				info[_("Colors kicker")] = enabled;
		}
		else
		{
			info[_("Colors kicker")] = disabled;
		}

		if (kd && kd->GetFlood())
		{
			if (kd->GetTTBFlood())
				info[_("Flood kicker")] = Anope::printf(_("%s (%d kick(s) to ban; %d lines in %ds)"), enabled.c_str(), kd->GetTTBFlood(), kd->GetFloodLines(), kd->GetFloodSecs());
			else
				info[_("Flood kicker")] = Anope::printf(_("%s (%d lines in %ds)"), enabled.c_str(), kd->GetFloodLines(), kd->GetFloodSecs());
		}
		else
		{
			info[_("Flood kicker")] = disabled;
		}

		if (kd && kd->GetRepeat())
		{
			if (kd->GetTTBRepeat())
				info[_("Repeat kicker")] = Anope::printf(_("%s (%d kick(s) to ban; %d times)"), enabled.c_str(), kd->GetTTBRepeat(), kd->GetRepeatTimes());
			else
				info[_("Repeat kicker")] = Anope::printf(_("%s (%d times)"), enabled.c_str(), kd->GetRepeatTimes());
		}
		else
		{
			info[_("Repeat kicker")] = disabled;
		}

		if (kd && kd->GetReverses())
		{
			if (kd->GetTTBReverses())
				info[_("Reverses kicker")] = Anope::printf(_("%s (%d kick(s) to ban)"), enabled.c_str(), kd->GetTTBReverses());
			else
				info[_("Reverses kicker")] = enabled;
		}
		else
		{
			info[_("Reverses kicker")] = disabled;
		}

		if (kd && kd->GetUnderlines())
		{
			if (kd->GetTTBUnderlines())
				info[_("Underlines kicker")] = Anope::printf(_("%s (%d kick(s) to ban)"), enabled.c_str(), kd->GetTTBUnderlines());
			else
				info[_("Underlines kicker")] = enabled;
		}
		else
		{
			info[_("Underlines kicker")] = disabled;
		}

		if (kd && kd->GetItalics())
		{
			if (kd->GetTTBItalics())
				info[_("Italics kicker")] = Anope::printf(_("%s (%d kick(s) to ban)"), enabled.c_str(), kd->GetTTBItalics());
			else
				info[_("Italics kicker")] = enabled;
		}
		else
		{
			info[_("Italics kicker")] = disabled;
		}

		if (kd && kd->GetAmsgs())
		{
			if (kd->GetTTBAmsgs())
				info[_("AMSG kicker")] = Anope::printf(_("%s (%d kick(s) to ban)"), enabled.c_str(), kd->GetTTBAmsgs());
			else
				info[_("AMSG kicker")] = enabled;
		}
		else
		{
			info[_("AMSG kicker")] = disabled;
		}

		if (kd && kd->GetDontKickOps())
			info.AddOption(_("Ops protection"));
		if (kd && kd->GetDontKickVoices())
			info.AddOption(_("Voices protection"));
	}

	void OnPrivmsg(User *u, Channel *c, Anope::string &msg) override
	{
		/* Now we can make kicker stuff. We try to order the checks
		 * from the fastest one to the slowest one, since there's
		 * no need to process other kickers if a user is kicked before
		 * the last kicker check.
		 *
		 * But FIRST we check whether the user is protected in any
		 * way.
		 */
		ChanServ::Channel *ci = c->ci;
		if (ci == NULL)
			return;
		KickerData *kd = c->ci->GetRef<KickerData *>();
		if (kd == NULL)
			return;

		if (ci->AccessFor(u).HasPriv("NOKICK"))
			return;
		if (kd->GetDontKickOps() && (c->HasUserStatus(u, "HALFOP") || c->HasUserStatus(u, "OP") || c->HasUserStatus(u, "PROTECT") || c->HasUserStatus(u, "OWNER")))
			return;
		if (kd->GetDontKickVoices() && c->HasUserStatus(u, "VOICE"))
			return;

		Anope::string realbuf = msg;

		/* If it's a /me, cut the CTCP part because the ACTION will cause
		 * problems with the caps or badwords kicker
		 */
		if (realbuf.substr(0, 8).equals_ci("\1ACTION ") && realbuf[realbuf.length() - 1] == '\1')
		{
			realbuf.erase(0, 8);
			realbuf.erase(realbuf.length() - 1);
		}

		if (realbuf.empty())
			return;

		/* Bolds kicker */
		if (kd->GetBolds() && realbuf.find(2) != Anope::string::npos)
		{
			TakeAction(ci, u, kd->GetTTBBolds(), TTB_BOLDS,  _("Don't use bolds on this channel!"));
			return;
		}

		/* Color kicker */
		if (kd->GetColors() && realbuf.find(3) != Anope::string::npos)
		{
			TakeAction(ci, u, kd->GetTTBColors(), TTB_COLORS, _("Don't use colors on this channel!"));
			return;
		}

		/* Reverses kicker */
		if (kd->GetReverses() && realbuf.find(22) != Anope::string::npos)
		{
			TakeAction(ci, u, kd->GetTTBReverses(), TTB_REVERSES, _("Don't use reverses on this channel!"));
			return;
		}

		/* Italics kicker */
		if (kd->GetItalics() && realbuf.find(29) != Anope::string::npos)
		{
			TakeAction(ci, u, kd->GetTTBItalics(), TTB_ITALICS, _("Don't use italics on this channel!"));
			return;
		}

		/* Underlines kicker */
		if (kd->GetUnderlines() && realbuf.find(31) != Anope::string::npos)
		{
			TakeAction(ci, u, kd->GetTTBUnderlines(), TTB_UNDERLINES, _("Don't use underlines on this channel!"));
			return;
		}

		/* Caps kicker */
		if (kd->GetCaps() && realbuf.length() >= static_cast<unsigned>(kd->GetCapsMin()))
		{
			int i = 0, l = 0;

			for (unsigned j = 0, end = realbuf.length(); j < end; ++j)
			{
				if (isupper(realbuf[j]))
					++i;
				else if (islower(realbuf[j]))
					++l;
			}

			/* i counts uppercase chars, l counts lowercase chars. Only
			 * alphabetic chars (so islower || isupper) qualify for the
			 * percentage of caps to kick for; the rest is ignored. -GD
			 */

			if ((i || l) && i >= kd->GetCapsMin() && i * 100 / (i + l) >= kd->GetCapsPercent())
			{
				TakeAction(ci, u, kd->GetTTBCaps(), TTB_CAPS, _("Turn caps lock OFF!"));
				return;
			}
		}

		/* Bad words kicker */
		if (kd->GetBadwords())
		{
			bool mustkick = false;

			/* Normalize the buffer */
			Anope::string nbuf = Anope::NormalizeBuffer(realbuf);
			bool casesensitive = Config->GetModule("botserv/main")->Get<bool>("casesensitive");

			/* Normalize can return an empty string if this only conains control codes etc */
			if (badwords && !nbuf.empty())
				for (unsigned i = 0; i < badwords->GetBadWordCount(ci); ++i)
				{
					BadWord *bw = badwords->GetBadWord(ci, i);

					if (bw->GetWord().empty())
						continue; // Shouldn't happen

					if (bw->GetWord().length() > nbuf.length())
						continue; // This can't ever match

					if (bw->GetType() == BW_ANY && ((casesensitive && nbuf.find(bw->GetWord()) != Anope::string::npos) || (!casesensitive && nbuf.find_ci(bw->GetWord()) != Anope::string::npos)))
						mustkick = true;
					else if (bw->GetType() == BW_SINGLE)
					{
						size_t len = bw->GetWord().length();

						if ((casesensitive && bw->GetWord().equals_cs(nbuf)) || (!casesensitive && bw->GetWord().equals_ci(nbuf)))
							mustkick = true;
						else if (nbuf.find(' ') == len && ((casesensitive && bw->GetWord().equals_cs(nbuf.substr(0, len))) || (!casesensitive && bw->GetWord().equals_ci(nbuf.substr(0, len)))))
							mustkick = true;
						else
						{
							if (len < nbuf.length() && nbuf.rfind(' ') == nbuf.length() - len - 1 && ((casesensitive && nbuf.find(bw->GetWord()) == nbuf.length() - len) || (!casesensitive && nbuf.find_ci(bw->GetWord()) == nbuf.length() - len)))
								mustkick = true;
							else
							{
								Anope::string wordbuf = " " + bw->GetWord() + " ";

								if ((casesensitive && nbuf.find(wordbuf) != Anope::string::npos) || (!casesensitive && nbuf.find_ci(wordbuf) != Anope::string::npos))
									mustkick = true;
							}
						}
					}
					else if (bw->GetType() == BW_START)
					{
						size_t len = bw->GetWord().length();

						if ((casesensitive && nbuf.substr(0, len).equals_cs(bw->GetWord())) || (!casesensitive && nbuf.substr(0, len).equals_ci(bw->GetWord())))
							mustkick = true;
						else
						{
							Anope::string wordbuf = " " + bw->GetWord();

							if ((casesensitive && nbuf.find(wordbuf) != Anope::string::npos) || (!casesensitive && nbuf.find_ci(wordbuf) != Anope::string::npos))
								mustkick = true;
						}
					}
					else if (bw->GetType() == BW_END)
					{
						size_t len = bw->GetWord().length();

						if ((casesensitive && nbuf.substr(nbuf.length() - len).equals_cs(bw->GetWord())) || (!casesensitive && nbuf.substr(nbuf.length() - len).equals_ci(bw->GetWord())))
							mustkick = true;
						else
						{
							Anope::string wordbuf = bw->GetWord() + " ";

							if ((casesensitive && nbuf.find(wordbuf) != Anope::string::npos) || (!casesensitive && nbuf.find_ci(wordbuf) != Anope::string::npos))
								mustkick = true;
						}
					}

					if (mustkick)
					{
						if (Config->GetModule(me)->Get<bool>("gentlebadwordreason"))
							TakeAction(ci, u, kd->GetTTBBadwords(), TTB_BADWORDS, _("Watch your language!"));
						else
							TakeAction(ci, u, kd->GetTTBBadwords(), TTB_BADWORDS, _("Don't use the word \"{0}\" on this channel!"), bw->GetWord());

						return;
					}
				} /* for */
		} /* if badwords */

		UserData *ud = GetUserData(u, c);

		if (ud)
		{
			/* Flood kicker */
			if (kd->GetFlood())
			{
				if (Anope::CurTime - ud->last_start > kd->GetFloodSecs())
				{
					ud->last_start = Anope::CurTime;
					ud->lines = 0;
				}

				++ud->lines;
				if (ud->lines >= kd->GetFloodLines())
				{
					TakeAction(ci, u, kd->GetTTBFlood(), TTB_FLOOD, _("Stop flooding!"));
					return;
				}
			}

			/* Repeat kicker */
			if (kd->GetRepeat())
			{
				if (!ud->lastline.equals_ci(realbuf))
					ud->times = 0;
				else
					++ud->times;

				if (ud->times >= kd->GetRepeatTimes())
				{
					TakeAction(ci, u, kd->GetTTBRepeat(), TTB_REPEAT, _("Stop repeating yourself!"));
					return;
				}
			}

			if (ud->lastline.equals_ci(realbuf) && !ud->lasttarget.empty() && !ud->lasttarget.equals_ci(ci->GetName()))
			{
				for (User::ChanUserList::iterator it = u->chans.begin(); it != u->chans.end();)
				{
					Channel *chan = it->second->chan;
					++it;

					if (chan->ci && kd->GetAmsgs() && !chan->ci->AccessFor(u).HasPriv("NOKICK"))
					{
						TakeAction(ci, u, kd->GetTTBAmsgs(), TTB_AMSGS, _("Don't use AMSGs!"));
						return;
					}
				}
			}

			ud->lasttarget = ci->GetName();
			ud->lastline = realbuf;
		}
	}
};

MODULE_INIT(BSKick)
