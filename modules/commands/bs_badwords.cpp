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
#include "modules/bs_badwords.h"

static EventHandlers<Event::BadWordEvents> *bwevents;

struct BadWordImpl : BadWord, Serializable
{
	BadWordImpl() : Serializable("BadWord") { }
	~BadWordImpl();

	void Serialize(Serialize::Data &data) const override
	{
		data["ci"] << this->chan;
		data["word"] << this->word;
		data.SetType("type", Serialize::Data::DT_INT); data["type"] << this->type;
	}

	static Serializable* Unserialize(Serializable *obj, Serialize::Data &);
};

struct BadWordsImpl : BadWords
{
	Serialize::Reference<ChanServ::Channel> ci;
	typedef std::vector<BadWordImpl *> list;
	Serialize::Checker<list> badwords;

	BadWordsImpl(Extensible *obj) : ci(anope_dynamic_static_cast<ChanServ::Channel *>(obj)), badwords("BadWord") { }

	~BadWordsImpl();

	BadWord* AddBadWord(const Anope::string &word, BadWordType type) override
	{
		BadWordImpl *bw = new BadWordImpl();
		bw->chan = ci->name;
		bw->word = word;
		bw->type = type;

		this->badwords->push_back(bw);

		(*bwevents)(&Event::BadWordEvents::OnBadWordAdd, ci, bw);

		return bw;
	}

	BadWord* GetBadWord(unsigned index) const override
	{
		if (this->badwords->empty() || index >= this->badwords->size())
			return NULL;

		BadWordImpl *bw = (*this->badwords)[index];
		bw->QueueUpdate();
		return bw;
	}

	unsigned GetBadWordCount() const override
	{
		return this->badwords->size();
	}

	void EraseBadWord(unsigned index) override
	{
		if (this->badwords->empty() || index >= this->badwords->size())
			return;

		(*bwevents)(&Event::BadWordEvents::OnBadWordDel, ci, (*this->badwords)[index]);

		delete this->badwords->at(index);
	}

	void ClearBadWords() override
	{
		while (!this->badwords->empty())
			delete this->badwords->back();
	}

	void Check() override
	{
		if (this->badwords->empty())
			ci->Shrink<BadWords>("badwords");
	}
};

BadWordsImpl::~BadWordsImpl()
{
	for (list::iterator it = badwords->begin(); it != badwords->end();)
	{
		BadWord *bw = *it;
		++it;
		delete bw;
	}
}

BadWordImpl::~BadWordImpl()
{
	ChanServ::Channel *ci = ChanServ::Find(chan);
	if (ci)
	{
		BadWordsImpl *badwords = ci->GetExt<BadWordsImpl>("badwords");
		if (badwords)
		{
			BadWordsImpl::list::iterator it = std::find(badwords->badwords->begin(), badwords->badwords->end(), this);
			if (it != badwords->badwords->end())
				badwords->badwords->erase(it);
		}
	}
}

Serializable* BadWordImpl::Unserialize(Serializable *obj, Serialize::Data &data)
{
	Anope::string sci, sword;

	data["ci"] >> sci;
	data["word"] >> sword;

	ChanServ::Channel *ci = ChanServ::Find(sci);
	if (!ci)
		return NULL;

	unsigned int n;
	data["type"] >> n;

	BadWordImpl *bw;
	if (obj)
		bw = anope_dynamic_static_cast<BadWordImpl *>(obj);
	else
		bw = new BadWordImpl();
	bw->chan = sci;
	bw->word = sword;
	bw->type = static_cast<BadWordType>(n);

	BadWordsImpl *bws = ci->Require<BadWordsImpl>("badwords");
	bws->badwords->push_back(bw);

	return bw;
}

class BadwordsDelCallback : public NumberList
{
	CommandSource &source;
	ChanServ::Channel *ci;
	BadWords *bw;
	Command *c;
	unsigned deleted;
	bool override;
 public:
	BadwordsDelCallback(CommandSource &_source, ChanServ::Channel *_ci, Command *_c, const Anope::string &list) : NumberList(list, true), source(_source), ci(_ci), c(_c), deleted(0), override(false)
	{
		if (!source.AccessFor(ci).HasPriv("BADWORDS") && source.HasPriv("botserv/administration"))
			this->override = true;
		bw = ci->Require<BadWords>("badwords");
	}

	~BadwordsDelCallback()
	{
		if (!deleted)
			source.Reply(_("No matching entries on the bad word list of \002{0}\002."), ci->name);
		else if (deleted == 1)
			source.Reply(_("Deleted \0021\002 entry from bad word list of \002{0}\002."), ci->name);
		else
			source.Reply(_("Deleted \002{0}\002 entries from the bad word list of \002{1}\002."), deleted, ci->name);
	}

	void HandleNumber(unsigned Number) override
	{
		if (!bw || !Number || Number > bw->GetBadWordCount())
			return;

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, c, ci) << "DEL " << bw->GetBadWord(Number - 1)->word;
		++deleted;
		bw->EraseBadWord(Number - 1);
	}
};

class CommandBSBadwords : public Command
{
 private:
	void DoList(CommandSource &source, ChanServ::Channel *ci, const Anope::string &word)
	{
		bool override = !source.AccessFor(ci).HasPriv("BADWORDS");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "LIST";
		ListFormatter list(source.GetAccount());
		BadWords *bw = ci->GetExt<BadWords>("badwords");

		list.AddColumn(_("Number")).AddColumn(_("Word")).AddColumn(_("Type"));

		if (!bw || !bw->GetBadWordCount())
		{
			source.Reply(_("The bad word list of \002{0}\002 is empty."), ci->name);
			return;
		}

		if (!word.empty() && word.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class BadwordsListCallback : public NumberList
			{
				ListFormatter &list;
				BadWords *bw;
			 public:
				BadwordsListCallback(ListFormatter &_list, BadWords *_bw, const Anope::string &numlist) : NumberList(numlist, false), list(_list), bw(_bw)
				{
				}

				void HandleNumber(unsigned Number) override
				{
					if (!Number || Number > bw->GetBadWordCount())
						return;

					const BadWord *b = bw->GetBadWord(Number - 1);
					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(Number);
					entry["Word"] = b->word;
					entry["Type"] = b->type == BW_SINGLE ? "(SINGLE)" : (b->type == BW_START ? "(START)" : (b->type == BW_END ? "(END)" : ""));
					this->list.AddEntry(entry);
				}
			}
			nl_list(list, bw, word);
			nl_list.Process();
		}
		else
		{
			for (unsigned i = 0, end = bw->GetBadWordCount(); i < end; ++i)
			{
				const BadWord *b = bw->GetBadWord(i);

				if (!word.empty() && !Anope::Match(b->word, word))
					continue;

				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(i + 1);
				entry["Word"] = b->word;
				entry["Type"] = b->type == BW_SINGLE ? "(SINGLE)" : (b->type == BW_START ? "(START)" : (b->type == BW_END ? "(END)" : ""));
				list.AddEntry(entry);
			}
		}

		if (list.IsEmpty())
		{
			source.Reply(_("No matching entries on the bad word list of \002{0}\002."), ci->name);
			return;
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		source.Reply(_("Bad words list for \002{0}\002:"), ci->name);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("End of bad words list."));
	}

	void DoAdd(CommandSource &source, ChanServ::Channel *ci, const Anope::string &word)
	{
		size_t pos = word.rfind(' ');
		BadWordType bwtype = BW_ANY;
		Anope::string realword = word;
		BadWords *badwords = ci->Require<BadWords>("badwords");

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
		if (badwords->GetBadWordCount() >= badwordsmax)
		{
			source.Reply(_("Sorry, you can only have \002{0}\002 bad words entries on a channel."), badwordsmax);
			return;
		}

		bool casesensitive = Config->GetModule(this->module)->Get<bool>("casesensitive");

		for (unsigned i = 0, end = badwords->GetBadWordCount(); i < end; ++i)
		{
			const BadWord *bw = badwords->GetBadWord(i);

			if ((casesensitive && realword.equals_cs(bw->word)) || (!casesensitive && realword.equals_ci(bw->word)))
			{
				source.Reply(_("\002{0}\002 already exists in \002{1}\002 bad words list."), bw->word, ci->name);
				return;
			}
		}

		bool override = !source.AccessFor(ci).HasPriv("BADWORDS");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "ADD " << realword;
		badwords->AddBadWord(realword, bwtype);

		source.Reply(_("\002{0}\002 added to \002{1}\002 bad words list."), realword, ci->name);
	}

	void DoDelete(CommandSource &source, ChanServ::Channel *ci, const Anope::string &word)
	{
		BadWords *badwords = ci->GetExt<BadWords>("badwords");

		if (!badwords || !badwords->GetBadWordCount())
		{
			source.Reply(_("Bad word list for \002{0}\002 is empty."), ci->name);
			return;
		}

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (!word.empty() && isdigit(word[0]) && word.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			BadwordsDelCallback list(source, ci, this, word);
			list.Process();
		}
		else
		{
			unsigned i, end;
			const BadWord *badword;

			for (i = 0, end = badwords->GetBadWordCount(); i < end; ++i)
			{
				badword = badwords->GetBadWord(i);

				if (word.equals_ci(badword->word))
					break;
			}

			if (i == end)
			{
				source.Reply(_("\002{0}\002 was not found on the bad word list of \002{1}\002."), word, ci->name);
				return;
			}

			bool override = !source.AccessFor(ci).HasPriv("BADWORDS");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "DEL " << badword->word;

			source.Reply(_("\002{0}\002 deleted from \002{1}\002 bad words list."), badword->word, ci->name);

			badwords->EraseBadWord(i);
		}

		badwords->Check();
	}

	void DoClear(CommandSource &source, ChanServ::Channel *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("BADWORDS");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "CLEAR";

		BadWords *badwords = ci->GetExt<BadWords>("badwords");
		if (badwords)
			badwords->ClearBadWords();
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
			this->OnSyntaxError(source, cmd);
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("BADWORDS") && (!need_args || !source.HasPriv("botserv/administration")))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "BADWORDS", ci->name);
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, channel bad words list modification is temporarily disabled."));
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
			this->OnSyntaxError(source, "");
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
			               "command"_kw = source.command);
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
			               "command"_kw = source.command);

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
			               "command"_kw = source.command);
		}
		else if (subcommand.equals_ci("CLEAR"))
		{
			source.Reply(_("Clears the bad words for \037channel\037."
			               "\n"
			               "\n"
			               "Example:\n"
			               "         {command} #anope CLEAR\n"
			               "         Clears the bad word list for #anope."),
			               "command"_kw = source.command);
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
				               "msg"_kw = Config->StrictPrivmsg, "service"_kw = source.service->nick, "command"_kw = source.command, "help"_kw = help->cname);
		}
		return true;
	}
};

class BSBadwords : public Module
{
	CommandBSBadwords commandbsbadwords;
	ExtensibleItem<BadWordsImpl> badwords;
	Serialize::Type badword_type;
	EventHandlers<Event::BadWordEvents> events;

 public:
	BSBadwords(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandbsbadwords(this)
		, badwords(this, "badwords")
		, badword_type("BadWord", BadWordImpl::Unserialize)
		, events(this, "Badwords")
	{
		bwevents = &events;
	}
};

MODULE_INIT(BSBadwords)
