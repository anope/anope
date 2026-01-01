// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"
#include "modules/botserv/badwords.h"

namespace
{
	Anope::string TypeToString(BotServ::BadWordType bw)
	{
		switch (bw)
		{
			case BotServ::BW_ANY:
				return "ANY";
			case BotServ::BW_SINGLE:
				return "SINGLE";
			case BotServ::BW_START:
				return "START";
			case BotServ::BW_END:
				return "END";
		}
		return ""; // Should never happen.
	}

	BotServ::BadWordType StringToType(const Anope::string &bw)
	{
		if (bw.equals_ci("ANY") || bw.equals_ci("0"))
			return BotServ::BW_ANY;
		if (bw.equals_ci("SINGLE") || bw.equals_ci("1"))
			return BotServ::BW_SINGLE;
		if (bw.equals_ci("START") || bw.equals_ci("2"))
			return BotServ::BW_START;
		if (bw.equals_ci("END") || bw.equals_ci("3"))
			return BotServ::BW_END;

		return BotServ::BW_ANY; // Should never happen.
	}
}

struct BadWordImpl final
	: BotServ::BadWord
	, Serializable
{
	BadWordImpl()
		: Serializable(BOTSERV_BAD_WORDS_TYPE)
	{
	}
	~BadWordImpl() override;
};

struct BadWordTypeImpl final
	: Serialize::Type
{
	BadWordTypeImpl()
		: Serialize::Type(BOTSERV_BAD_WORDS_TYPE)
	{
	}

	void Serialize(Serializable *obj, Serialize::Data &data) const override
	{
		const auto *bw = static_cast<const BadWordImpl *>(obj);
		data.Store("ci", bw->chan);
		data.Store("word", bw->word);
		data.Store("type", TypeToString(bw->type));
	}

	Serializable *Unserialize(Serializable *obj, Serialize::Data &) const override;
};

struct BadWordsImpl final
	: BotServ::BadWords
{
	Serialize::Reference<ChannelInfo> ci;
	typedef std::vector<BadWordImpl *> list;
	Serialize::Checker<list> badwords;

	BadWordsImpl(Extensible *obj)
		: ci(anope_dynamic_static_cast<ChannelInfo *>(obj))
		, badwords(BOTSERV_BAD_WORDS_TYPE)
	{
	}

	~BadWordsImpl() override;

	BotServ::BadWord *AddBadWord(const Anope::string &word, BotServ::BadWordType type) override
	{
		auto *bw = new BadWordImpl();
		bw->chan = ci->name;
		bw->word = word;
		bw->type = type;

		this->badwords->push_back(bw);

		FOREACH_MOD(OnBadWordAdd, (ci, bw));

		return bw;
	}

	BotServ::BadWord *GetBadWord(unsigned index) const override
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

		FOREACH_MOD(OnBadWordDel, (ci, (*this->badwords)[index]));

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
			ci->Shrink<BotServ::BadWords>(BOTSERV_BAD_WORDS_EXT);
	}
};

BadWordsImpl::~BadWordsImpl()
{
	for (list::iterator it = badwords->begin(); it != badwords->end();)
	{
		auto *bw = *it;
		++it;
		delete bw;
	}
}

BadWordImpl::~BadWordImpl()
{
	ChannelInfo *ci = ChannelInfo::Find(chan);
	if (ci)
	{
		BadWordsImpl *badwords = ci->GetExt<BadWordsImpl>(BOTSERV_BAD_WORDS_EXT);
		if (badwords)
		{
			BadWordsImpl::list::iterator it = std::find(badwords->badwords->begin(), badwords->badwords->end(), this);
			if (it != badwords->badwords->end())
				badwords->badwords->erase(it);
		}
	}
}

Serializable *BadWordTypeImpl::Unserialize(Serializable *obj, Serialize::Data &data) const
{
	Anope::string sci, sword;

	data["ci"] >> sci;
	data["word"] >> sword;

	ChannelInfo *ci = ChannelInfo::Find(sci);
	if (!ci)
		return NULL;

	Anope::string n;
	data["type"] >> n;

	BadWordImpl *bw;
	if (obj)
		bw = anope_dynamic_static_cast<BadWordImpl *>(obj);
	else
		bw = new BadWordImpl();
	bw->chan = sci;
	bw->word = sword;
	bw->type = StringToType(n);

	BadWordsImpl *bws = ci->Require<BadWordsImpl>(BOTSERV_BAD_WORDS_EXT);
	if (!obj)
		bws->badwords->push_back(bw);

	return bw;
}

class BadwordsDelCallback final
	: public NumberList
{
	CommandSource &source;
	ChannelInfo *ci;
	BotServ::BadWords *bw;
	Command *c;
	unsigned deleted = 0;
	Anope::string lastdeleted;
	bool override = false;
public:
	BadwordsDelCallback(CommandSource &_source, ChannelInfo *_ci, Command *_c, const Anope::string &list) : NumberList(list, true), source(_source), ci(_ci), c(_c)
	{
		if (!source.AccessFor(ci).HasPriv("BADWORDS") && source.HasPriv("botserv/administration"))
			this->override = true;
		bw = ci->Require<BotServ::BadWords>(BOTSERV_BAD_WORDS_EXT);
	}

	~BadwordsDelCallback() override
	{
		switch (deleted)
		{
			case 0:
				source.Reply(_("No matching entries on %s bad words list."), ci->name.c_str());
				break;

			case 1:
				source.Reply(_("Deleted %s from %s bad words list."), lastdeleted.c_str(), ci->name.c_str());
				break;

			default:
				source.Reply(deleted, N_("Deleted %d entry from %s bad words list.", "Deleted %d entries from %s bad words list."), deleted, ci->name.c_str());
				break;
		}
	}

	void HandleNumber(unsigned Number) override
	{
		if (!bw || !Number || Number > bw->GetBadWordCount())
			return;

		lastdeleted = bw->GetBadWord(Number - 1)->word;
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, c, ci) << "DEL " << lastdeleted;
		++deleted;
		bw->EraseBadWord(Number - 1);
	}
};

class CommandBSBadwords final
	: public Command
{
private:
	void DoList(CommandSource &source, ChannelInfo *ci, const Anope::string &word)
	{
		bool override = !source.AccessFor(ci).HasPriv("BADWORDS");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "LIST";

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Word")).AddColumn(_("Type"));
		list.SetFlexible(_("{number}: \002{word}\002 -- type: {type}"));

		auto *bw = ci->GetExt<BotServ::BadWords>(BOTSERV_BAD_WORDS_EXT);
		if (!bw || !bw->GetBadWordCount())
		{
			source.Reply(_("%s bad words list is empty."), ci->name.c_str());
			return;
		}
		else if (!word.empty() && word.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class BadwordsListCallback final
				: public NumberList
			{
				ListFormatter &list;
				BotServ::BadWords *bw;
			public:
				BadwordsListCallback(ListFormatter &_list, BotServ::BadWords *_bw, const Anope::string &numlist) : NumberList(numlist, false), list(_list), bw(_bw)
				{
				}

				void HandleNumber(unsigned Number) override
				{
					if (!Number || Number > bw->GetBadWordCount())
						return;

					const auto *b = bw->GetBadWord(Number - 1);
					ListFormatter::ListEntry entry;
					entry["Number"] = Anope::ToString(Number);
					entry["Word"] = b->word;
					entry["Type"] = TypeToString(b->type);
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
				const auto *b = bw->GetBadWord(i);

				if (!word.empty() && !Anope::Match(b->word, word))
					continue;

				ListFormatter::ListEntry entry;
				entry["Number"] = Anope::ToString(i + 1);
				entry["Word"] = b->word;
				entry["Type"] =  TypeToString(b->type);
				list.AddEntry(entry);
			}
		}

		if (list.IsEmpty())
			source.Reply(_("No matching entries on %s bad words list."), ci->name.c_str());
		else
		{
			source.Reply(_("Bad words list for %s:"), ci->name.c_str());
			list.SendTo(source);
			source.Reply(_("End of bad words list."));
		}
	}

	void DoAdd(CommandSource &source, ChannelInfo *ci, const Anope::string &word)
	{
		size_t pos = word.rfind(' ');
		auto bwtype = BotServ::BW_ANY;
		Anope::string realword = word;
		auto *badwords = ci->Require<BotServ::BadWords>(BOTSERV_BAD_WORDS_EXT);

		if (pos != Anope::string::npos)
		{
			Anope::string opt = word.substr(pos + 1);
			if (!opt.empty())
				bwtype = StringToType(opt);
			realword = word.substr(0, pos);
		}

		unsigned badwordsmax = Config->GetModule(this->module).Get<unsigned>("badwordsmax");
		if (badwords->GetBadWordCount() >= badwordsmax)
		{
			source.Reply(_("You can only have %d bad words entries on a channel."), badwordsmax);
			return;
		}

		bool casesensitive = Config->GetModule(this->module).Get<bool>("casesensitive");

		for (unsigned i = 0, end = badwords->GetBadWordCount(); i < end; ++i)
		{
			const auto *bw = badwords->GetBadWord(i);

			if ((casesensitive && realword.equals_cs(bw->word)) || (!casesensitive && realword.equals_ci(bw->word)))
			{
				source.Reply(_("\002%s\002 already exists in %s bad words list."), bw->word.c_str(), ci->name.c_str());
				return;
			}
		}

		bool override = !source.AccessFor(ci).HasPriv("BADWORDS");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "ADD " << realword;
		badwords->AddBadWord(realword, bwtype);

		source.Reply(_("\002%s\002 added to %s bad words list."), realword.c_str(), ci->name.c_str());
	}

	void DoDelete(CommandSource &source, ChannelInfo *ci, const Anope::string &word)
	{
		auto *badwords = ci->GetExt<BotServ::BadWords>(BOTSERV_BAD_WORDS_EXT);

		if (!badwords || !badwords->GetBadWordCount())
		{
			source.Reply(_("%s bad words list is empty."), ci->name.c_str());
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
			const BotServ::BadWord *badword;

			for (i = 0, end = badwords->GetBadWordCount(); i < end; ++i)
			{
				badword = badwords->GetBadWord(i);

				if (word.equals_ci(badword->word))
					break;
			}

			if (i == end)
			{
				source.Reply(_("\002%s\002 not found on %s bad words list."), word.c_str(), ci->name.c_str());
				return;
			}

			bool override = !source.AccessFor(ci).HasPriv("BADWORDS");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "DEL " << badword->word;

			source.Reply(_("\002%s\002 deleted from %s bad words list."), badword->word.c_str(), ci->name.c_str());

			badwords->EraseBadWord(i);
		}

		badwords->Check();
	}

	void DoClear(CommandSource &source, ChannelInfo *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("BADWORDS");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "CLEAR";

		auto *badwords = ci->GetExt<BotServ::BadWords>(BOTSERV_BAD_WORDS_EXT);
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
		const Anope::string &cmd = params[1];
		const Anope::string &word = params.size() > 2 ? params[2] : "";
		bool need_args = cmd.equals_ci("LIST") || cmd.equals_ci("CLEAR");

		if (!need_args && word.empty())
		{
			this->OnSyntaxError(source, cmd);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!source.AccessFor(ci).HasPriv("BADWORDS") && !source.HasPriv("botserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, ci, word);
		else if (cmd.equals_ci("DEL"))
			return this->DoDelete(source, ci, word);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, ci, word);
		else if (cmd.equals_ci("CLEAR"))
			return this->DoClear(source, ci);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Maintains the \002bad words list\002 for a channel. The bad "
				"words list determines which words are to be kicked "
				"when the bad words kicker is enabled. For more information, "
				"type \002%s\032KICK\032%s\002."
				"\n\n"
				"The \002ADD\002 command adds the given word to the "
				"bad words list. If SINGLE is specified, a kick will be "
				"done only if a user says the entire word. If START is "
				"specified, a kick will be done if a user says a word "
				"that starts with \037word\037. If END is specified, a kick "
				"will be done if a user says a word that ends with "
				"\037word\037. If you don't specify anything, a kick will "
				"be issued every time \037word\037 is said by a user. This "
				"will be shown in the \002LIST\002 output as ANY."
				"\n\n"
				"The \002DEL\002 command removes the given word from the "
				"bad words list. If a list of entry numbers is given, those "
				"entries are deleted.  (See the example for LIST below.)"
				"\n\n"
				"The \002LIST\002 command displays the bad words list. If "
				"a wildcard mask is given, only those entries matching the "
				"mask are displayed. If a list of entry numbers is given, "
				"only those entries are shown; for example:\n"
				"   \002#channel\032LIST\0322-5,7-9\002\n"
				"      Lists bad words entries numbered 2 through 5 and\n"
				"      7 through 9."
				"\n\n"
				"The \002CLEAR\002 command clears all entries from the "
				"bad words list."
			),
			source.service->GetQueryCommand("generic/help").c_str(),
			source.command.nobreak().c_str());
		return true;
	}
};

class BSBadwords final
	: public Module
{
	CommandBSBadwords commandbsbadwords;
	ExtensibleItem<BadWordsImpl> badwords;
	BadWordTypeImpl badword_type;

public:
	BSBadwords(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandbsbadwords(this)
		, badwords(this, BOTSERV_BAD_WORDS_EXT)
	{
	}
};

MODULE_INIT(BSBadwords)
