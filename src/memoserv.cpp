/* MemoServ functions.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"

static bool SendMemoMail(NickCore *nc, MemoInfo *mi, Memo *m);

Memo::Memo() : Flags<MemoFlag>(MemoFlagStrings) { }

/**
 * MemoServ initialization.
 * @return void
 */
void ms_init()
{
	if (!Config->s_MemoServ.empty())
		ModuleManager::LoadModuleList(Config->MemoServCoreModules);
}

/*************************************************************************/
/**
 * check_memos:  See if the given user has any unread memos, and send a
 *               NOTICE to that user if so (and if the appropriate flag is
 *               set).
 * @param u User Struct
 * @return void
 */
void check_memos(User *u)
{
	if (Config->s_MemoServ.empty())
		return;

	if (!u)
	{
		Log() << "check_memos called with NULL values";
		return;
	}

	const NickCore *nc = u->Account();
	if (!nc || !u->IsRecognized() || !nc->HasFlag(NI_MEMO_SIGNON))
		return;

	unsigned i = 0, end = nc->memos.memos.size(), newcnt = 0;
	for (; i < end; ++i)
	{
		if (nc->memos.memos[i]->HasFlag(MF_UNREAD))
			++newcnt;
	}
	if (newcnt > 0)
	{
		u->SendMessage(MemoServ, newcnt == 1 ? _("You have 1 new memo.") : _("You have %d new memos."), newcnt);
		if (newcnt == 1 && (nc->memos.memos[i - 1]->HasFlag(MF_UNREAD)))
			u->SendMessage(MemoServ, _("Type \002%s%s READ LAST\002 to read it."), Config->UseStrictPrivMsgString.c_str(), Config->s_MemoServ.c_str());
		else if (newcnt == 1)
		{
			for (i = 0; i < end; ++i)
			{
				if (nc->memos.memos[i]->HasFlag(MF_UNREAD))
					break;
			}
			u->SendMessage(MemoServ, _("Type \002%s%s READ %d\002 to read it."), Config->UseStrictPrivMsgString.c_str(), Config->s_MemoServ.c_str(), i);
		}
		else
			u->SendMessage(MemoServ, _("Type \002%s%s LIST NEW\002 to list them."), Config->UseStrictPrivMsgString.c_str(), Config->s_MemoServ.c_str());
	}
	if (nc->memos.memomax > 0 && nc->memos.memos.size() >= nc->memos.memomax)
	{
		if (nc->memos.memos.size() > nc->memos.memomax)
			u->SendMessage(MemoServ, _("Warning: You are over your maximum number of memos (%d).  You will be unable to receive any new memos until you delete some of your current ones."), nc->memos.memomax);
		else
			u->SendMessage(MemoServ, _("Warning: You have reached your maximum number of memos (%d).  You will be unable to receive any new memos until you delete some of your current ones."), nc->memos.memomax);
	}
}

/*************************************************************************/
/*********************** MemoServ private routines ***********************/
/*************************************************************************/

/**
 * Return the MemoInfo corresponding to the given nick or channel name.
 * @param name Name to check
 * @param ischan - the result its a channel will be stored in here
 * @param isforbid - the result if its forbidden will be stored in here
 * @return `ischan' 1 if the name was a channel name, else 0.
 * @return `isforbid' 1 if the name is forbidden, else 0.
 */
MemoInfo *getmemoinfo(const Anope::string &name, bool &ischan, bool &isforbid)
{
	if (name[0] == '#')
	{
		ischan = true;
		ChannelInfo *ci = cs_findchan(name);
		if (ci)
		{
			if (!ci->HasFlag(CI_FORBIDDEN))
			{
				isforbid = false;
				return &ci->memos;
			}
			else
			{
				isforbid = true;
				return NULL;
			}
		}
		else
		{
			isforbid = false;
			return NULL;
		}
	}
	else
	{
		ischan = false;
		NickAlias *na = findnick(name);
		if (na)
		{
			if (!na->HasFlag(NS_FORBIDDEN))
			{
				isforbid = false;
				return &na->nc->memos;
			}
			else
			{
				isforbid = true;
				return NULL;
			}
		}
		else
		{
			isforbid = false;
			return NULL;
		}
	}
}

/*************************************************************************/

/**
 * Split from do_send, this way we can easily send a memo from any point
 * @param source Where replies should go
 * @param name Target of the memo
 * @param text Memo Text
 * @param z type see info
 *    0 - reply to user
 *    1 - silent
 *    2 - silent with no delay timer
 *    3 - reply to user and request read receipt
 * @return void
 */
void memo_send(CommandSource &source, const Anope::string &name, const Anope::string &text, int z)
{
	if (Config->s_MemoServ.empty())
		return;

	User *u = source.u;

	bool ischan, isforbid;
	MemoInfo *mi;
	Anope::string sender = u && u->Account() ? u->Account()->display : "";
	bool is_servoper = u != NULL && u->IsServicesOper();

	if (readonly)
		u->SendMessage(MemoServ, _(MEMO_SEND_DISABLED));
	else if (text.empty())
	{
		if (!z)
			SyntaxError(source, "SEND", _(MEMO_SEND_SYNTAX));

		if (z == 3)
			SyntaxError(source, "RSEND", _("{\037nick\037 | \037channel\037} \037memo-text\037")); // XXX?
	}
	else if (!u->IsIdentified() && !u->IsRecognized())
	{
		if (!z || z == 3)
			source.Reply(_(NICK_IDENTIFY_REQUIRED), Config->UseStrictPrivMsgString.c_str(), Config->s_NickServ.c_str());
	}
	else if (!(mi = getmemoinfo(name, ischan, isforbid)))
	{
		if (!z || z == 3)
		{
			if (isforbid)
				source.Reply(ischan ? _(CHAN_X_FORBIDDEN) : _(NICK_X_FORBIDDEN), name.c_str());
			else
				source.Reply(ischan ? _(CHAN_X_NOT_REGISTERED) : _(NICK_X_NOT_REGISTERED), name.c_str());
		}
	}
	else if (z != 2 && Config->MSSendDelay > 0 && u && u->lastmemosend + Config->MSSendDelay > Anope::CurTime)
	{
		u->lastmemosend = Anope::CurTime;
		if (!z)
			source.Reply(_("Please wait %d seconds before using the SEND command again."), Config->MSSendDelay);

		if (z == 3)
			source.Reply(_("Please wait %d seconds before using the RSEND command again."), Config->MSSendDelay);
	}
	else if (!mi->memomax && !is_servoper)
	{
		if (!z || z == 3)
			source.Reply(_("%s cannot receive memos."), name.c_str());
	}
	else if (mi->memomax > 0 && mi->memos.size() >= mi->memomax && !is_servoper)
	{
		if (!z || z == 3)
			source.Reply(_("%s currently has too many memos and cannot receive more."), name.c_str());
	}
	else
	{
		if (!z || z == 3)
			source.Reply(_("Memo sent to \002%s\002."), name.c_str());
		if (!u->IsServicesOper() && mi->HasIgnore(u))
			return;

		u->lastmemosend = Anope::CurTime;
		Memo *m = new Memo();
		mi->memos.push_back(m);
		m->sender = sender;
		m->time = Anope::CurTime;
		m->text = text;
		m->SetFlag(MF_UNREAD);
		/* Set notify sent flag - DrStein */
		if (z == 2)
			m->SetFlag(MF_NOTIFYS);
		/* Set receipt request flag */
		if (z == 3)
			m->SetFlag(MF_RECEIPT);
		if (!ischan)
		{
			NickCore *nc = findnick(name)->nc;

			FOREACH_MOD(I_OnMemoSend, OnMemoSend(u, nc, m));

			if (Config->MSNotifyAll)
			{
				if (nc->HasFlag(NI_MEMO_RECEIVE))
				{
					for (std::list<NickAlias *>::iterator it = nc->aliases.begin(), it_end = nc->aliases.end(); it != it_end; ++it)
					{
						NickAlias *na = *it;
						User *user = finduser(na->nick);
						if (user && user->IsIdentified())
							user->SendMessage(MemoServ, _(MEMO_NEW_MEMO_ARRIVED), sender.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->s_MemoServ.c_str(), mi->memos.size());
					}
				}
				else
				{
					if ((u = finduser(name)) && u->IsIdentified() && nc->HasFlag(NI_MEMO_RECEIVE))
						u->SendMessage(MemoServ, _(MEMO_NEW_MEMO_ARRIVED), sender.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->s_MemoServ.c_str(), mi->memos.size());
				} /* if (flags & MEMO_RECEIVE) */
			}
			/* if (MSNotifyAll) */
			/* let's get out the mail if set in the nickcore - certus */
			if (nc->HasFlag(NI_MEMO_MAIL))
				SendMemoMail(nc, mi, m);
		}
		else
		{
			Channel *c;

			FOREACH_MOD(I_OnMemoSend, OnMemoSend(u, cs_findchan(name), m));

			if (Config->MSNotifyAll && (c = findchan(name)))
			{
				for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
				{
					UserContainer *cu = *it;

					if (check_access(cu->user, c->ci, CA_MEMO))
					{
						if (cu->user->Account() && cu->user->Account()->HasFlag(NI_MEMO_RECEIVE))
							cu->user->SendMessage(MemoServ, _(MEMO_NEW_X_MEMO_ARRIVED), c->ci->name.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->s_MemoServ.c_str(), c->ci->name.c_str(), mi->memos.size());
					}
				}
			} /* MSNotifyAll */
		} /* if (!ischan) */
	} /* if command is valid */
}

/*************************************************************************/

unsigned MemoInfo::GetIndex(Memo *m) const
{
	for (unsigned i = 0; i < this->memos.size(); ++i)
		if (this->memos[i] == m)
			return i;
	return -1;
}

void MemoInfo::Del(unsigned index)
{
	if (index >= this->memos.size())
		return;
	delete this->memos[index];
	this->memos.erase(this->memos.begin() + index);
}

void MemoInfo::Del(Memo *memo)
{
	std::vector<Memo *>::iterator it = std::find(this->memos.begin(), this->memos.end(), memo);

	if (it != this->memos.end())
	{
		delete memo;
		this->memos.erase(it);
	}
}

bool MemoInfo::HasIgnore(User *u)
{
	for (unsigned i = 0; i < this->ignores.size(); ++i)
		if (u->nick.equals_ci(this->ignores[i]) || (u->Account() && u->Account()->display.equals_ci(this->ignores[i])) || Anope::Match(u->GetMask(), Anope::string(this->ignores[i])))
			return true;
	return false;
}

/*************************************************************************/

static bool SendMemoMail(NickCore *nc, MemoInfo *mi, Memo *m)
{
	char message[BUFSIZE];

	snprintf(message, sizeof(message), GetString(nc,
	"Hi %s\n"
	" \n"
	"You've just received a new memo from %s. This is memo number %d.\n"
	" \n"
	"Memo text:\n"
	" \n"
	"%s").c_str(), nc->display.c_str(), m->sender.c_str(), mi->GetIndex(m), m->text.c_str());

	return Mail(nc, GetString(nc, _("New memo")), message);
}

/*************************************************************************/

/* Send receipt notification to sender. */

void rsend_notify(CommandSource &source, Memo *m, const Anope::string &chan)
{
	/* Only send receipt if memos are allowed */
	if (!readonly)
	{
		/* Get nick alias for sender */
		NickAlias *na = findnick(m->sender);

		if (!na)
			return;

		/* Get nick core for sender */
		NickCore *nc = na->nc;

		if (!nc)
			return;

		/* Text of the memo varies if the recepient was a
		   nick or channel */
		char text[256];
		if (!chan.empty())
			snprintf(text, sizeof(text), GetString(na->nc, _("\002[auto-memo]\002 The memo you sent to %s has been viewed.")).c_str(), chan.c_str());
		else
			snprintf(text, sizeof(text), "%s", GetString(na->nc, _("\002[auto-memo]\002 The memo you sent has been viewed.")).c_str());

		/* Send notification */
		memo_send(source, m->sender, text, 2);

		/* Notify recepient of the memo that a notification has
		   been sent to the sender */
		source.Reply(_("A notification memo has been sent to %s informing him/her you have\n"
				"read his/her memo."), nc->display.c_str());
	}

	/* Remove receipt flag from the original memo */
	m->UnsetFlag(MF_RECEIPT);

	return;
}
