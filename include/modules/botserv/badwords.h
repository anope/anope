/*
 * Anope IRC Services
 *
 * Copyright (C) 2013-2017 Anope Team <team@anope.org>
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

/** Flags for badwords
 */
enum BadWordType
{
	/* Always kicks if the word is said */
	BW_ANY,
	/* User must say the entire word */
	BW_SINGLE,
	/* The word has to start with the badword */
	BW_START,
	/* The word has to end with the badword */
	BW_END
};

/* Structure used to contain bad words. */
class BadWord : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;
 public:
	static constexpr const char *NAME = "badword";
	 
	virtual ~BadWord() = default;

	virtual ChanServ::Channel *GetChannel() anope_abstract;
	virtual void SetChannel(ChanServ::Channel *) anope_abstract;

	virtual Anope::string GetWord() anope_abstract;
	virtual void SetWord(const Anope::string &) anope_abstract;

	virtual BadWordType GetType() anope_abstract;
	virtual void SetType(BadWordType) anope_abstract;
};

class BadWords : public Service
{
 public:
	static constexpr const char *NAME = "badwords";
	
	BadWords(Module *me) : Service(me, NAME) { }

	/** Add a badword to the badword list
	 * @param word The badword
	 * @param type The type (SINGLE START END)
	 * @return The badword
	 */
	virtual BadWord* AddBadWord(ChanServ::Channel *, const Anope::string &word, BadWordType type) anope_abstract;

	virtual std::vector<BadWord *> GetBadWords(ChanServ::Channel *ci) anope_abstract;

	/** Get a badword structure by index
	 * @param index The index
	 * @return The badword
	 */
	virtual BadWord* GetBadWord(ChanServ::Channel *, unsigned index) anope_abstract;

	/** Get how many badwords are on this channel
	 * @return The number of badwords in the vector
	 */
	virtual unsigned GetBadWordCount(ChanServ::Channel *) anope_abstract;

	/** Remove a badword
	 * @param index The index of the badword
	 */
	virtual void EraseBadWord(ChanServ::Channel *, unsigned index) anope_abstract;

	/** Clear all badwords from the channel
	 */
	virtual void ClearBadWords(ChanServ::Channel *) anope_abstract;
};

namespace Event
{
	struct CoreExport BadWordEvents : Events
	{
		static constexpr const char *NAME = "badwords";

		using Events::Events;

		/** Called before a badword is added to the badword list
		 * @param ci The channel
		 * @param bw The badword
		 */
		virtual void OnBadWordAdd(ChanServ::Channel *ci, const BadWord *bw) anope_abstract;

		/** Called before a badword is deleted from a channel
		 * @param ci The channel
		 * @param bw The badword
		 */
		virtual void OnBadWordDel(ChanServ::Channel *ci, const BadWord *bw) anope_abstract;
	};
}

