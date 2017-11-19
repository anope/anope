/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
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
#include "modules/botserv/badwords.h"

class BadWordImpl : public BadWord
{
	friend class BadWordsType;

	Serialize::Storage<ChanServ::Channel *> channel;
	Serialize::Storage<Anope::string> word;
	Serialize::Storage<BadWordType> type;

 public:
	using BadWord::BadWord;

	ChanServ::Channel *GetChannel() override;
	void SetChannel(ChanServ::Channel *c) override;

	Anope::string GetWord() override;
	void SetWord(const Anope::string &w) override;

	BadWordType GetType() override;
	void SetType(BadWordType t) override;
};

class BadWordsType : public Serialize::Type<BadWordImpl>
{
 public:
	Serialize::ObjectField<BadWordImpl, ChanServ::Channel *> channel;
	Serialize::Field<BadWordImpl, Anope::string> word;
	Serialize::Field<BadWordImpl, BadWordType> type;

	BadWordsType(Module *me) : Serialize::Type<BadWordImpl>(me)
		, channel(this, "channel", &BadWordImpl::channel, true)
		, word(this, "word", &BadWordImpl::word)
		, type(this, "type", &BadWordImpl::type)
	{
	}
};

ChanServ::Channel *BadWordImpl::GetChannel()
{
	return Get(&BadWordsType::channel);
}

void BadWordImpl::SetChannel(ChanServ::Channel *c)
{
	Set(&BadWordsType::channel, c);
}

Anope::string BadWordImpl::GetWord()
{
	return Get(&BadWordsType::word);
}

void BadWordImpl::SetWord(const Anope::string &w)
{
	Set(&BadWordsType::word, w);
}

BadWordType BadWordImpl::GetType()
{
	return Get(&BadWordsType::type);
}

void BadWordImpl::SetType(BadWordType t)
{
	Set(&BadWordsType::type, t);
}

struct BadWordsImpl : BadWords
{
	BadWordsImpl(Module *me) : BadWords(me) { }

	BadWord* AddBadWord(ChanServ::Channel *ci, const Anope::string &word, BadWordType type) override
	{
		BadWord *bw = Serialize::New<BadWord *>();
		bw->SetChannel(ci);
		bw->SetWord(word);
		bw->SetType(type);

		EventManager::Get()->Dispatch(&Event::BadWordEvents::OnBadWordAdd, ci, bw);

		return bw;
	}

	std::vector<BadWord *> GetBadWords(ChanServ::Channel *ci) override
	{
		return ci->GetRefs<BadWord *>();
	}

	BadWord* GetBadWord(ChanServ::Channel *ci, unsigned index) override
	{
		auto bw = GetBadWords(ci);
		return index < bw.size() ? bw[index] : nullptr;
	}

	unsigned GetBadWordCount(ChanServ::Channel *ci) override
	{
		return GetBadWords(ci).size();
	}

	void EraseBadWord(ChanServ::Channel *ci, unsigned index) override
	{
		auto bws = GetBadWords(ci);
		if (index >= bws.size())
			return;

		BadWord *bw = bws[index];
		EventManager::Get()->Dispatch(&Event::BadWordEvents::OnBadWordDel, ci, bw);

		delete bw;
	}

	void ClearBadWords(ChanServ::Channel *ci) override
	{
		for (BadWord *bw : GetBadWords(ci))
			delete bw;
	}
};

class CommandBSBadwords : public Command
{
	ServiceReference<BadWords> badwords;
	
	void DoList(CommandSource &source, ChanServ::Channel *ci, const Anope::string &word)
	{
		logger.Command(source, ci, _("{source} used {command} on {channel} to list badwords"));

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Word")).AddColumn(_("Type"));

		if (!badwords->GetBadWordCount(ci))
		{
			source.Reply(_("The bad word list of \002{0}\002 is empty."), ci->GetName());
			return;
		}

		if (!word.empty() && word.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			NumberList(word, false,
				[&](unsigned int num)
				{
					if (!num || num > badwords->GetBadWordCount(ci))
						return;

					BadWord *b = badwords->GetBadWord(ci, num - 1);
					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(num);
					entry["Word"] = b->GetWord();
					entry["Type"] = b->GetType() == BW_SINGLE ? "(SINGLE)" : (b->GetType() == BW_START ? "(START)" : (b->GetType() == BW_END ? "(END)" : ""));
					list.AddEntry(entry);
				},
				[](){});
		}
		else
		{
			for (unsigned i = 0, end = badwords->GetBadWordCount(ci); i < end; ++i)
			{
				BadWord *b = badwords->GetBadWord(ci, i);

				if (!word.empty() && !Anope::Match(b->GetWord(), word))
					continue;

				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(i + 1);
				entry["Word"] = b->GetWord();
				entry["Type"] = b->GetType() == BW_SINGLE ? "(SINGLE)" : (b->GetType() == BW_START ? "(START)" : (b->GetType() == BW_END ? "(END)" : ""));
				list.AddEntry(entry);
			}
		}

		if (list.IsEmpty())
		{
			source.Reply(_("No matching entries on the bad word list of \002{0}\002."), ci->GetName());
			return;
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		source.Reply(_("Bad words list for \002{0}\002:"), ci->GetName());

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("End of bad words list."));
	}

	void DoAdd(CommandSource &source, ChanServ::Channel *ci, const Anope::string &word)
	{
		size_t pos = word.rfind(' ');
		BadWordType bwtype = BW_ANY;
		Anope::string realword = word;

		if (pos != Anope::string::npos)
		{
			Anope::string opt = word.substr(pos + 1);
			if (!opt.empty())
			{
				if (opt.equals_ci("SINGLE"))
					bwtype = BW_SINGLE;
				else if (opt.equals_ci("START"))
					bwtype = BW_START;
				else if (opt.equals_ci("END"))
					bwtype = BW_END;
			}
			realword = word.substr(0, pos);
		}

		unsigned badwordsmax = Config->GetModule(this->module)->Get<unsigned>("badwordsmax");
		if (badwords->GetBadWordCount(ci) >= badwordsmax)
		{
			source.Reply(_("Sorry, you can only have \002{0}\002 bad words entries on a channel."), badwordsmax);
			return;
		}

		bool casesensitive = Config->GetModule(this->module)->Get<bool>("casesensitive");

		for (BadWord *bw : badwords->GetBadWords(ci))
			if ((casesensitive && realword.equals_cs(bw->GetWord())) || (!casesensitive && realword.equals_ci(bw->GetWord())))
			{
				source.Reply(_("\002{0}\002 already exists in \002{1}\002 bad words list."), bw->GetWord(), ci->GetName());
				return;
			}

		logger.Command(source, ci, _("{source} used {command} on {channel} to add {0}"), realword);
		badwords->AddBadWord(ci, realword, bwtype);

		source.Reply(_("\002{0}\002 added to \002{1}\002 bad words list."), realword, ci->GetName());
	}

	void DoDelete(CommandSource &source, ChanServ::Channel *ci, const Anope::string &word)
	{
		if (!badwords->GetBadWordCount(ci))
		{
			source.Reply(_("Bad word list for \002{0}\002 is empty."), ci->GetName());
			return;
		}

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (!word.empty() && isdigit(word[0]) && word.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			unsigned int deleted = 0;

			NumberList(word, true,
				[&](unsigned int num)
				{
					if (!num || num > badwords->GetBadWordCount(ci))
						return;

					logger.Command(source, ci, _("{source} used {command} on {channel} to remove {0}"), badwords->GetBadWord(ci, num - 1)->GetWord());

					++deleted;
					badwords->EraseBadWord(ci, num - 1);
				},
				[&]()
				{
					if (!deleted)
						source.Reply(_("No matching entries on the bad word list of \002{0}\002."), ci->GetName());
					else if (deleted == 1)
						source.Reply(_("Deleted \0021\002 entry from bad word list of \002{0}\002."), ci->GetName());
					else
						source.Reply(_("Deleted \002{0}\002 entries from the bad word list of \002{1}\002."), deleted, ci->GetName());
				});
		}
		else
		{
			unsigned i, end;
			BadWord *bw;

			for (i = 0, end = badwords->GetBadWordCount(ci); i < end; ++i)
			{
				bw = badwords->GetBadWord(ci, i);

				if (word.equals_ci(bw->GetWord()))
					break;
			}

			if (i == end)
			{
				source.Reply(_("\002{0}\002 was not found on the bad word list of \002{1}\002."), word, ci->GetName());
				return;
			}

			logger.Command(source, ci, _("{source} used {command} on {channel} to remove {3}"), bw->GetWord());

			source.Reply(_("\002{0}\002 deleted from \002{1}\002 bad words list."), bw->GetWord(), ci->GetName());

			bw->Delete();
		}
	}

	void DoClear(CommandSource &source, ChanServ::Channel *ci)
	{
		logger.Command(source, ci, _("{source} used {command} on {channel} to clear the badwords list"));

		badwords->ClearBadWords(ci);
		source.Reply(_("Bad words list is now empty."));
	}

 public:
	CommandBSBadwords(Module *creator) : Command(creator, "botserv/badwords", 2, 3)
	{
		this->SetDesc(_("Maintains the bad words list"));
		this->SetSyntax(_("\037channel\037 ADD \037word\037 [\037SINGLE\037 | \037START\037 | \037END\037]"));
		this->SetSyntax(_("\037channel\037 DEL {\037word\037 | \037entry-num\037 | \037list\037}"));
		this->SetSyntax(_("\037channel\037 LIST [\037mask\037 | \037list\037]"));
		this->SetSyntax(_("\037channel\037 CLEAR"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &cmd = params[1];
		const Anope::string &word = params.size() > 2 ? params[2] : "";
		bool need_args = cmd.equals_ci("LIST") || cmd.equals_ci("CLEAR");

		if (!need_args && word.empty())
		{
			this->OnSyntaxError(source);
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("BADWORDS") && !source.HasOverridePriv("botserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "BADWORDS", ci->GetName());
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, bad words list modification is temporarily disabled."));
			return;
		}

		if (cmd.equals_ci("ADD"))
			this->DoAdd(source, ci, word);
		else if (cmd.equals_ci("DEL"))
			this->DoDelete(source, ci, word);
		else if (cmd.equals_ci("LIST"))
			this->DoList(source, ci, word);
		else if (cmd.equals_ci("CLEAR"))
			this->DoClear(source, ci);
		else
			this->OnSyntaxError(source);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (subcommand.equals_ci("ADD"))
		{
			source.Reply(_("Adds \037word\037 to the bad words list for \037channel\037. If \002SINGLE\002 is specified then the user must say the enire word for it to be considered a match."
			               " If \002START\002 is specified then the user only must say a word that starts with \037word\037."
			               " Likewise if \002END\002 is specified then the user must only say a word that ends with \037word\037."
			               " If no argument is specified, then any word which contains \037word\037 will be considered a match.\n"
			               "\n"
			               "Examples:\n"
			               "         {command} #anope ADD raw SINGLE\n"
			               "         Adds the bad word \"raw\" to the bad word list of #anope."),
			               "command"_kw = source.GetCommand());
		}
		else if (subcommand.equals_ci("DEL"))
		{
			source.Reply(_("Removes \037word\037 from the bad words list for \037channel\037. If a number or list of numbers is given, those entries are deleted.\n"
			               "\n"
			               "Examples:\n"
			               "         {command} #anope DEL raw\n"
			               "         Removes the bad word \"raw\" from the bad word list of #anope.\n"
			               "\n"
			               "         {command} #anope DEL 2-5,7-9\n"
			               "         Removes bad words entries numbered 2 through 5 and 7 through 9 on #anope."),
			               "command"_kw = source.GetCommand());

		}
		else if (subcommand.equals_ci("LIST"))
		{
			source.Reply(_("Lists the badwords for \037channel\037."
			               " If a wildcard mask is given, only those entries matching the mask are displayed."
			               " If a list of entry numbers is given, only those entries are shown.\n"
			               "\n"
			               "Examples:\n"
			               "         {command} #anope LIST\n"
			               "         Lists the bad words for #anope.\n"
			               "\n"
			               "         {command} #anope LIST 2-5,7-9\n"
			               "         Lists bad words entries numbered 2 thorough 5 and 7 through 9 on #anope."),
			               "command"_kw = source.GetCommand());
		}
		else if (subcommand.equals_ci("CLEAR"))
		{
			source.Reply(_("Clears the bad words for \037channel\037."
			               "\n"
			               "\n"
			               "Example:\n"
			               "         {command} #anope CLEAR\n"
			               "         Clears the bad word list for #anope."),
			               "command"_kw = source.GetCommand());
		}
		else
		{
			source.Reply(_("Maintains the bad words list for a channel."
		                       " When a word on the bad words list is said, action may be taken against the offending user.\n"
		                       "\n"
		                       "Use of this command requires the \002{0}\002 privilege on \037channel\037."),
			               "BADWORDS");

			CommandInfo *help = source.service->FindCommand("generic/help");
			if (help)
				source.Reply(_("\n"
				               "For help configuring the bad word kicker use \002{msg}{service} HELP KICK BADWORDS\002.\n"//XXX
				               "\n"
				               "The \002ADD\002 command adds \037word\037 to the badwords list.\n"
				               "\002{msg}{service} {help} {command} ADD\002 for more information.\n"
				               "\n"
				               "The \002DEL\002 command removes \037word\037 from the badwords list.\n"
				               "\002{msg}{service} {help} {command} DEL\002 for more information.\n"
				               "\n"
				               "The \002LIST\002 and \002CLEAR\002 commands show and clear the bad words list, respectively.\n"
				               "\002{msg}{service} {help} {command} LIST\002 and \002{msg}{service} {help} {command} CLEAR\002 for more information.\n"),
				               "msg"_kw = Config->StrictPrivmsg, "service"_kw = source.service->nick, "command"_kw = source.GetCommand(), "help"_kw = help->cname);
		}
		return true;
	}
};

class BSBadwords : public Module
{
	CommandBSBadwords commandbsbadwords;
	BadWordsImpl badwords;
	BadWordsType bwtype;

 public:
	BSBadwords(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandbsbadwords(this)
		, badwords(this)
		, bwtype(this)
	{
	}
};

MODULE_INIT(BSBadwords)
