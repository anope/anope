/*
 *   Copyright (C) 2003-2009 Anope Team <team@anope.org>
 *   Copyright (C) 2005-2006 Florian Schulze <certus@anope.org>
 *   Copyright (C) 2008-2009 Robin Burchell <w00t@inspircd.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License (see it online
 *   at http://www.gnu.org/copyleft/gpl.html) as published by the Free
 *   Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 */

#include "db-convert.h"

int main(int argc, char *argv[])
{
	dbFILE *f;
	std::ofstream fs;
	std::string hashm;

	printf("\n"C_LBLUE"Anope 1.8.x -> 1.9.x database converter"C_NONE"\n\n");

	hashm = "plain"; // XXX
	/*
	while (hashm != "md5" && hashm != "sha1" && hashm != "oldmd5" && hashm != "plain")
	{
		if (!hashm.empty())
			std::cout << "Select a valid option, idiot. Thanks!" << std::endl;
		std::cout << "Which hash method did you use? (md5, sha1, oldmd5, plain)" << std::endl << "? ";
		std::cin >> hashm;
	}
	*/

	std::cout << "You selected " << hashm << std::endl;

	fs.open("anope.db");
	if (!fs.is_open())
	{
		printf("\n"C_LBLUE"Could not open anope.db for write"C_NONE"\n\n");
		exit(1);
	}

	// VERSHUN ONE
	fs << "VER 1" << std::endl;

	/* Section I: Nicks */
	/* Ia: First database */
	if ((f = open_db_read("NickServ", "nick.db", 14)))
	{

		NickAlias *na, **nalast, *naprev;
		NickCore *nc, **nclast, *ncprev;
		int16 tmp16;
		int32 tmp32;
		int i, j, c;

		printf("Trying to merge nicks...\n");

		/* Nick cores */
		for (i = 0; i < 1024; i++) {
			nclast = &nclists[i];
			ncprev = NULL;

			while ((c = getc_db(f)) == 1) {
				if (c != 1) {
					printf("Invalid format in nickserv db.\n");
					exit(0);
				}

				nc = (NickCore *)calloc(1, sizeof(NickCore));
				nc->aliascount = 0;
				nc->unused = 0;

				*nclast = nc;
				nclast = &nc->next;
				nc->prev = ncprev;
				ncprev = nc;

				READ(read_string(&nc->display, f));
				READ(read_buffer(nc->pass, f));
				READ(read_string(&nc->email, f));
				READ(read_string(&nc->greet, f));
				READ(read_uint32(&nc->icq, f));
				READ(read_string(&nc->url, f));
				READ(read_uint32(&nc->flags, f));
				READ(read_uint16(&nc->language, f));
				READ(read_uint16(&nc->accesscount, f));
				if (nc->accesscount) {
					char **access;
					access = (char **)calloc(sizeof(char *) * nc->accesscount, 1);
					nc->access = access;
					for (j = 0; j < nc->accesscount; j++, access++)
						READ(read_string(access, f));
				}
				READ(read_int16(&nc->memos.memocount, f));
				READ(read_int16(&nc->memos.memomax, f));
				if (nc->memos.memocount) {
					Memo *memos;
					memos = (Memo *)calloc(sizeof(Memo) * nc->memos.memocount, 1);
					nc->memos.memos = memos;
					for (j = 0; j < nc->memos.memocount; j++, memos++) {
						READ(read_uint32(&memos->number, f));
						READ(read_uint16(&memos->flags, f));
						READ(read_int32(&tmp32, f));
						memos->time = tmp32;
						READ(read_buffer(memos->sender, f));
						READ(read_string(&memos->text, f));
					}
				}
				READ(read_uint16(&nc->channelcount, f));
				READ(read_int16(&tmp16, f));
			} /* getc_db() */
			*nclast = NULL;
		} /* for() loop */

		/* Nick aliases */
		for (i = 0; i < 1024; i++) {
			char *s = NULL;

			nalast = &nalists[i];
			naprev = NULL;

			while ((c = getc_db(f)) == 1) {
				if (c != 1) {
					printf("Invalid format in nick db.\n");
					exit(0);
				}

				na = (NickAlias *)calloc(1, sizeof(NickAlias));

				READ(read_string(&na->nick, f));
				char *mask;
				char *real;
				char *quit;
				READ(read_string(&mask, f));
				READ(read_string(&real, f));
				READ(read_string(&quit, f));

				READ(read_int32(&tmp32, f));
				na->time_registered = tmp32;
				READ(read_int32(&tmp32, f));
				na->last_seen = tmp32;
				READ(read_uint16(&na->status, f));
				READ(read_string(&s, f));
				na->nc = findcore(s, 0);
				na->nc->aliascount++;
				free(s);

				if (!na->nc->last_quit && quit)
					na->nc->last_quit = strdup(quit);
				if (!na->nc->last_realname && real)
					na->nc->last_realname = strdup(real);
				if (!na->nc->last_usermask && mask)
					na->nc->last_usermask = strdup(mask);

				// Convert nick NOEXPIRE to group NOEXPIRE
				if (na->status & 0x0004)
				{
					na->nc->flags |= 0x00100000;
				}

				// Convert nick FORBIDDEN to group FORBIDDEN
				if (na->status & 0x0002)
				{
					na->nc->flags |= 0x80000000;
				}

				free(mask);
				free(real);
				free(quit);

				*nalast = na;
				nalast = &na->next;
				na->prev = naprev;
				naprev = na;
			} /* getc_db() */
			*nalast = NULL;
		} /* for() loop */
		close_db(f); /* End of section Ia */
	}

	/* CLEAN THE CORES */
	int i;
	for (i = 0; i < 1024; i++) {
		NickCore *ncnext;
		for (NickCore *nc = nclists[i]; nc; nc = ncnext) {
			ncnext = nc->next;
			if (nc->aliascount < 1) {
				printf("Deleting core %s (%s).\n", nc->display, nc->email);
				delcore(nc);
			}
		}
	}

	/* Nick cores */
	for (i = 0; i < 1024; i++)
	{
		NickAlias *na;
		NickCore *nc;
		char **access;
		Memo *memos;
		int j;
		char cpass[5000]; // if it's ever this long, I will commit suicide
		for (nc = nclists[i]; nc; nc = nc->next)
		{
			if (nc->aliascount < 1)
			{
				std::cout << "Skipping core with 0 or less aliases (wtf?)" << std::endl;
				continue;
			}

			if (nc->flags & 0x80000000)
			{
				std::cout << "Skipping forbidden nick " << nc->display << std::endl;
				continue;
			}

			// Enc pass
			b64_encode(nc->pass, hashm == "plain" ? strlen(nc->pass) : 32, (char *)cpass, 5000);

			fs << "NC " << nc->display << " " << hashm << ":" << cpass << " " << nc->email;
			fs << " " << GetLanguageID(nc->language) << nc->memos.memomax << " " << nc->channelcount << std::endl;

			std::cout << "Wrote account for " << nc->display << " passlen " << strlen(cpass) << std::endl;
			if (nc->greet)
				fs << "MD NC greet :" << nc->greet << std::endl;
			if (nc->icq)
				fs << "MD NC icq :" << nc->icq << std::endl;
			if (nc->url)
				fs << "MD NC url :" << nc->url << std::endl;

			if (nc->accesscount)
			{
				for (j = 0, access = nc->access; j < nc->accesscount; j++, access++)
					fs << "MD NC access " << *access << std::endl;
			}

			fs << "MD NC flags "
					<< ((nc->flags & NI_KILLPROTECT  ) ? "KILLPROTECT "  : "")
					<< ((nc->flags & NI_SECURE       ) ? "SECURE "       : "")
					<< ((nc->flags & NI_MSG          ) ? "MSG "          : "")
					<< ((nc->flags & NI_MEMO_HARDMAX ) ? "MEMO_HARDMAX " : "")
					<< ((nc->flags & NI_MEMO_SIGNON  ) ? "MEMO_SIGNON "  : "")
					<< ((nc->flags & NI_MEMO_RECEIVE ) ? "MEMO_RECEIVE " : "")
					<< ((nc->flags & NI_PRIVATE      ) ? "PRIVATE "      : "")
					<< ((nc->flags & NI_HIDE_EMAIL   ) ? "HIDE_EMAIL "   : "")
					<< ((nc->flags & NI_HIDE_MASK    ) ? "HIDE_MASK "    : "")
					<< ((nc->flags & NI_HIDE_QUIT    ) ? "HIDE_QUIT "    : "")
					<< ((nc->flags & NI_KILL_QUICK   ) ? "KILL_QUICK "   : "")
					<< ((nc->flags & NI_KILL_IMMED   ) ? "KILL_IMMED "   : "")
					<< ((nc->flags & NI_MEMO_MAIL    ) ? "MEMO_MAIL "    : "")
					<< ((nc->flags & NI_HIDE_STATUS  ) ? "HIDE_STATUS "  : "")
					<< ((nc->flags & NI_SUSPENDED    ) ? "SUSPENDED "    : "")
					<< ((nc->flags & NI_AUTOOP       ) ? "AUTOOP "       : "")
					<< ((nc->flags & NI_NOEXPIRE     ) ? "NOEXPIRE "     : "")
					<< ((nc->flags & NI_FORBIDDEN    ) ? "FORBIDDEN "    : "") << std::endl;
			if (nc->memos.memocount)
			{
				memos = nc->memos.memos;
				for (j = 0; j < nc->memos.memocount; j++, memos++)
					fs << "ME " << memos->number << " " << memos->flags << " " << memos->time << " " << memos->sender << " :" << memos->text << std::endl;
			}

			/* we could do this in a seperate loop, I'm doing it here for tidiness. */
			for (int tmp = 0; tmp < 1024; tmp++)
			{
				for (na = nalists[tmp]; na; na = na->next)
				{
					if (!na->nc)
					{
						std::cout << "Skipping alias with no core (wtf?)" << std::endl;
						continue;
					}

					if (na->nc != nc)
						continue;

					std::cout << "Writing: " << na->nc->display << "'s nick: " << na->nick << std::endl;

					fs <<  "NA " << na->nc->display << " " << na->nick << " " << na->time_registered << " " << na->last_seen << std::endl;
					if (nc->last_usermask)
						fs << "MD NA last_usermask :" << nc->last_usermask << std::endl;
					if (nc->last_realname)
						fs << "MD NA last_realname :" << nc->last_realname << std::endl;
					if (nc->last_quit)
						fs << "MD NA last_quit :" << nc->last_quit << std::endl;
					if ((na->status & NS_FORBIDDEN) || (na->status & NS_NO_EXPIRE)) 
					{
						fs << "MD NA flags "
							<< ((na->status & NS_FORBIDDEN) ? "FORBIDDEN " : "")
							<< ((na->status & NS_NO_EXPIRE) ? "NOEXPIRE"   : "") << std::endl;
					}
				}
			}
		}
	}
	/* Section II: Chans */
	// IIa: First database
	if ((f = open_db_read("ChanServ", "chan.db", 16)))
	{
		ChannelInfo *ci, **last, *prev;
		int c;

		printf("Trying to merge channels...\n");

		for (i = 0; i < 256; i++)
		{
			int16 tmp16;
			int32 tmp32;
			int n_levels;
			char *s;
			int n_ttb;

			last = &chanlists[i];
			prev = NULL;

			while ((c = getc_db(f)) == 1)
			{
				int j;

				if (c != 1)
				{
					printf("Invalid format in chans.db.\n");
					exit(0);
				}

				ci = (ChannelInfo *)calloc(sizeof(ChannelInfo), 1);
				*last = ci;
				last = &ci->next;
				ci->prev = prev;
				prev = ci;
				READ(read_buffer(ci->name, f));
				READ(read_string(&ci->founder, f));
				READ(read_string(&ci->successor, f));
				READ(read_buffer(ci->founderpass, f));
				READ(read_string(&ci->desc, f));
				if (!ci->desc)
					ci->desc = strdup("");
				std::cout << "Read " << ci->name << " founder " << (ci->founder ? ci->founder : "N/A") << std::endl;
				READ(read_string(&ci->url, f));
				READ(read_string(&ci->email, f));
				READ(read_int32(&tmp32, f));
				ci->time_registered = tmp32;
				READ(read_int32(&tmp32, f));
				ci->last_used = tmp32;
				READ(read_string(&ci->last_topic, f));
				READ(read_buffer(ci->last_topic_setter, f));
				READ(read_int32(&tmp32, f));
				ci->last_topic_time = tmp32;
				READ(read_uint32(&ci->flags, f));
				// Temporary flags cleanup
				ci->flags &= ~0x80000000;
				READ(read_string(&ci->forbidby, f));
				READ(read_string(&ci->forbidreason, f));
				READ(read_int16(&tmp16, f));
				ci->bantype = tmp16;
				READ(read_int16(&tmp16, f));
				n_levels = tmp16;
				ci->levels = (int16 *)calloc(36 * sizeof(*ci->levels), 1);
				for (j = 0; j < n_levels; j++)
				{
					if (j < 36)
						READ(read_int16(&ci->levels[j], f));
					else
						READ(read_int16(&tmp16, f));
				}
				READ(read_uint16(&ci->accesscount, f));
				if (ci->accesscount)
				{
					ci->access = (ChanAccess *)calloc(ci->accesscount, sizeof(ChanAccess));
					for (j = 0; j < ci->accesscount; j++)
					{
						READ(read_uint16(&ci->access[j].in_use, f));
						if (ci->access[j].in_use)
						{
							READ(read_int16(&ci->access[j].level, f));
							READ(read_string(&s, f));
							if (s)
							{
								ci->access[j].nc = findcore(s, 0);
								free(s);
							}
							if (ci->access[j].nc == NULL)
								ci->access[j].in_use = 0;
							READ(read_int32(&tmp32, f));
							ci->access[j].last_seen = tmp32;
						}
					}
				}
				else
				{
					ci->access = NULL;
				}
				READ(read_uint16(&ci->akickcount, f));
				if (ci->akickcount)
				{
					ci->akick = (AutoKick *)calloc(ci->akickcount, sizeof(AutoKick));
					for (j = 0; j < ci->akickcount; j++)
					{
						SAFE(read_uint16(&ci->akick[j].flags, f));
						if (ci->akick[j].flags & 0x0001)
						{
							SAFE(read_string(&s, f));
							if (ci->akick[j].flags & 0x0002)
							{
								ci->akick[j].u.nc = findcore(s, 0);
								if (!ci->akick[j].u.nc)
									ci->akick[j].flags &= ~0x0001;
								free(s);
							}
							else
							{
								ci->akick[j].u.mask = s;
							}
							SAFE(read_string(&s, f));
							if (ci->akick[j].flags & 0x0001)
								ci->akick[j].reason = s;
							else if (s)
								free(s);
							SAFE(read_string(&s, f));
							if (ci->akick[j].flags & 0x0001)
							{
								ci->akick[j].creator = s;
							}
							else if (s)
							{
								free(s);
							}
							SAFE(read_int32(&tmp32, f));
							if (ci->akick[j].flags & 0x0001)
								ci->akick[j].addtime = tmp32;
						}
					}
				}
				else
				{
					ci->akick = NULL;
				}
				READ(read_uint32(&ci->mlock_on, f));
				READ(read_uint32(&ci->mlock_off, f));
				READ(read_uint32(&ci->mlock_limit, f));
				READ(read_string(&ci->mlock_key, f));
				READ(read_string(&ci->mlock_flood, f));
				READ(read_string(&ci->mlock_redirect, f));
				READ(read_int16(&ci->memos.memocount, f));
				READ(read_int16(&ci->memos.memomax, f));
				if (ci->memos.memocount)
				{
					Memo *memos;
					memos = (Memo *)calloc(sizeof(Memo) * ci->memos.memocount, 1);
					ci->memos.memos = memos;
					for (j = 0; j < ci->memos.memocount; j++, memos++)
					{
						READ(read_uint32(&memos->number, f));
						READ(read_uint16(&memos->flags, f));
						READ(read_int32(&tmp32, f));
						memos->time = tmp32;
						READ(read_buffer(memos->sender, f));
						READ(read_string(&memos->text, f));
					}
				}
				READ(read_string(&ci->entry_message, f));

				// BotServ options
				READ(read_string(&ci->bi, f));
				READ(read_int32(&tmp32, f));
				ci->botflags = tmp32;
				READ(read_int16(&tmp16, f));
				n_ttb = tmp16;
				ci->ttb = (int16 *)calloc(2 * 8, 1);
				for (j = 0; j < n_ttb; j++)
				{
					if (j < 8)
						READ(read_int16(&ci->ttb[j], f));
					else
						READ(read_int16(&tmp16, f));
				}
				for (j = n_ttb; j < 8; j++)
					ci->ttb[j] = 0;
				READ(read_int16(&tmp16, f));
				ci->capsmin = tmp16;
				READ(read_int16(&tmp16, f));
				ci->capspercent = tmp16;
				READ(read_int16(&tmp16, f));
				ci->floodlines = tmp16;
				READ(read_int16(&tmp16, f));
				ci->floodsecs = tmp16;
				READ(read_int16(&tmp16, f));
				ci->repeattimes = tmp16;

				READ(read_uint16(&ci->bwcount, f));
				if (ci->bwcount)
				{
					ci->badwords = (BadWord *)calloc(ci->bwcount, sizeof(BadWord));
					for (j = 0; j < ci->bwcount; j++)
					{
						SAFE(read_uint16(&ci->badwords[j].in_use, f));
						if (ci->badwords[j].in_use)
						{
							SAFE(read_string(&ci->badwords[j].word, f));
							SAFE(read_uint16(&ci->badwords[j].type, f));
						}
					}
				}
				else
				{
					ci->badwords = NULL;
				}
			}
			*last = NULL;
		}
	}

	close_db(f);


	ChannelInfo *ci;

	for (i = 0; i < 256; i++)
	{
		for (ci = chanlists[i]; ci; ci = ci->next)
		{
			int j;

		/*	commented because channels without founder can be forbidden
			if (!ci->founder)
			{
				std::cout << "Skipping channel with no founder (wtf?)" << std::endl;
				continue;
			}*/

			fs << "CH " << ci->name << " " << ci->time_registered << " " << ci->last_used;
			fs << " " << ci->bantype << " " << ci->memos.memomax << std::endl;
			if (ci->founder)
				fs << "MD CH founder " << ci->founder << std::endl;
			if (ci->successor)
				fs << "MD CH successor " << ci->successor << std::endl;
			fs << "MD CH lvl";
			for (j = 0; j < 36; j++)
			{
				fs << " " <<ci->levels[j];
			}
			fs << std::endl;
			fs << "MD CH flags " 
					<< ((ci->flags & CI_KEEPTOPIC     ) ? "KEEPTOPIC "     : "")
					<< ((ci->flags & CI_SECUREOPS     ) ? "SECUREOPS "     : "")
					<< ((ci->flags & CI_PRIVATE       ) ? "PRIVATE "       : "")
					<< ((ci->flags & CI_TOPICLOCK     ) ? "TOPICLOCK "     : "")
					<< ((ci->flags & CI_RESTRICTED    ) ? "RESTRICTED "    : "")
					<< ((ci->flags & CI_PEACE         ) ? "PEACE "         : "")
					<< ((ci->flags & CI_SECURE        ) ? "SECURE "        : "")
					<< ((ci->flags & CI_FORBIDDEN     ) ? "FORBIDDEN "     : "")
					<< ((ci->flags & CI_NO_EXPIRE     ) ? "NOEXPIRE "      : "")
					<< ((ci->flags & CI_MEMO_HARDMAX  ) ? "MEMO_HARDMAX "  : "")
					<< ((ci->flags & CI_OPNOTICE      ) ? "OPNOTICE "      : "")
					<< ((ci->flags & CI_SECUREFOUNDER ) ? "SECUREFOUNDER " : "")
					<< ((ci->flags & CI_SIGNKICK      ) ? "SIGNKICK "      : "")
					<< ((ci->flags & CI_SIGNKICK_LEVEL) ? "SIGNKICKLEVEL " : "")
					<< ((ci->flags & CI_XOP           ) ? "XOP "           : "")
					<< ((ci->flags & CI_SUSPENDED     ) ? "SUSPENDED"      : "") << std::endl;
			if (ci->desc)
				fs << "MD CH desc :" << ci->desc << std::endl;
			if (ci->url)
				fs << "MD CH url :" << ci->url << std::endl;
			if (ci->email)
				fs << "MD CH email :" << ci->email << std::endl;
			if (ci->last_topic)  // MD CH topic <setter> <time> :topic
				fs << "MD CH topic " << ci->last_topic_setter << " " << ci->last_topic_time <<  " :" << ci->last_topic << std::endl;
			if (ci->flags & CI_FORBIDDEN)
				fs << "MD CH forbid " << ci->forbidby << " :" << ci->forbidreason << std::endl;

			for (j = 0; j < ci->accesscount; j++)
			{  // MD CH access <display> <level> <last_seen>
				if (ci->access[j].in_use)
					fs << "MD CH access "
						<< ci->access[j].nc->display << " " << ci->access[j].level << " " 
						<< ci->access[j].last_seen << " " << std::endl;
			}

			for (j = 0; j < ci->akickcount; j++)
			{  // MD CH akick <USED/NOTUSED> <STUCK/UNSTUCK> <NICK/MASK> <akick> <creator> <addtime> :<reason>
				fs << "MD CH akick "
					<< ((ci->akick[j].flags & AK_USED) ? "USED " : "NOTUSED ")
					<< ((ci->akick[j].flags & AK_STUCK) ? "STUCK " : "UNSTUCK" )
					<< ((ci->akick[j].flags & AK_ISNICK) ? "NICK " : "MASK ")
					<< ((ci->akick[j].flags & AK_ISNICK) ? ci->akick[j].u.nc->display : ci->akick[j].u.mask )
					<< " " << ci->akick[j].creator << " " << ci->akick[j].addtime << " :" << ci->akick[j].reason
					<< std::endl;
			}

			/* TODO: convert to new mlock modes */
			if (ci->mlock_on)
				fs << "MD CH mlock_on " << ci->mlock_on << std::endl;
			if (ci->mlock_off)
				fs << "MD CH mlock_off " << ci->mlock_off << std::endl;
			if (ci->mlock_limit)
				fs << "MD CH mlock_limit " << ci->mlock_limit << std::endl;
			if (ci->mlock_key)
				fs << "MD CH mlock_key" << ci->mlock_key << std::endl;
			if (ci->mlock_flood)
				fs << "MD CH mlock_flood" << ci->mlock_flood << std::endl;
			if (ci->mlock_redirect)
				fs << "MD CH mlock_redirect" << ci->mlock_redirect << std::endl;

			if (ci->memos.memocount)
			{
				Memo *memos;
				memos = ci->memos.memos;
				for (j = 0; j < ci->memos.memocount; j++, memos++)
				{
					fs << "ME " << memos->number << " " << memos->flags << " " 
						<< memos->time << " " << memos->sender << " :" << memos->text << std::endl;
				}
			}

			if (ci->entry_message)
				fs << "MD CH entry_message :" << ci->entry_message << std::endl;
			if (ci->bi)  // here is "bi" a *Char, not a pointer to BotInfo !
				fs << "MD CH bot " << ci->bi << std::endl;
			if (ci->botflags)
				fs << "MD CH botflags "
					<< (( ci->botflags & BS_DONTKICKOPS     ) ? "DONTKICKOPS "    : "" )
					<< (( ci->botflags & BS_DONTKICKVOICES  ) ? "DONTKICKVOICES "  : "")
					<< (( ci->botflags & BS_FANTASY         ) ? "FANTASY "         : "")
					<< (( ci->botflags & BS_SYMBIOSIS       ) ? "SYMBIOSIS "       : "")
					<< (( ci->botflags & BS_GREET           ) ? "GREET "           : "")
					<< (( ci->botflags & BS_NOBOT           ) ? "NOBOT "           : "")
					<< (( ci->botflags & BS_KICK_BOLDS      ) ? "KICK_BOLDS "      : "")
					<< (( ci->botflags & BS_KICK_COLORS     ) ? "KICK_COLORS "     : "")
					<< (( ci->botflags & BS_KICK_REVERSES   ) ? "KICK_REVERSES "   : "")
					<< (( ci->botflags & BS_KICK_UNDERLINES ) ? "KICK_UNDERLINES " : "")
					<< (( ci->botflags & BS_KICK_BADWORDS   ) ? "KICK_BADWORDS "   : "")
					<< (( ci->botflags & BS_KICK_CAPS       ) ? "KICK_CAPS "       : "")
					<< (( ci->botflags & BS_KICK_FLOOD      ) ? "KICK_FLOOD "      : "")
					<< (( ci->botflags & BS_KICK_REPEAT     ) ? "KICK_REPEAT "     : "") << std::endl;
			fs << "MD CH TTB";
			for (j = 0; j < 8; j++)
				fs << " " << ci->ttb[j];
			fs << std::endl;
			if (ci->capsmin)
				fs << "MD CH capsmin " << ci->capsmin << std::endl;
			if (ci->capspercent)
				fs << "MD CH capspercent " << ci->capspercent << std::endl;
			if (ci->floodlines)
				fs << "MD CH floodlines " << ci->floodlines << std::endl;
			if (ci->floodsecs)
				fs << "MD CH floodsecs " << ci->floodsecs << std::endl;
			if (ci->repeattimes)
				fs << "MD CH repeattimes " << ci->repeattimes << std::endl;
			for (j = 0; j < ci->bwcount; j++)
			{
				fs << "MD CH badword "
					<< (( ci->badwords[j].type == 0 ) ? "ANY "    : "" )
					<< (( ci->badwords[j].type == 1 ) ? "SINGLE " : "" )
					<< (( ci->badwords[j].type == 3 ) ? "START "  : "" )
					<< (( ci->badwords[j].type == 4 ) ? "END "    : "" )
					<< " " << ci->badwords[j].word << std::endl;
			}

		} /* for (chanlists[i]) */
	} /* for (i) */

	/* Section III: Bots */
	/* IIIa: First database */
	if ((f = open_db_read("Botserv", "bot.db", 10))) {
		BotInfo *bi;
		int c;
		int32 tmp32;
		int16 tmp16;

		printf("Trying to merge bots...\n");

		while ((c = getc_db(f)) == 1) {
			if (c != 1) {
				printf("Invalid format in %s.\n", "bot.db");
				exit(0);
			}

			bi = (BotInfo *)calloc(sizeof(BotInfo), 1);
			READ(read_string(&bi->nick, f));
			READ(read_string(&bi->user, f));
			READ(read_string(&bi->host, f));
			READ(read_string(&bi->real, f));
			SAFE(read_int16(&tmp16, f));
			bi->flags = tmp16;
			READ(read_int32(&tmp32, f));
			bi->created = tmp32;
			READ(read_int16(&tmp16, f));
			bi->chancount = tmp16;
			insert_bot(bi);
		}
	}

	/* IIIc: Saving */
	BotInfo *bi;
	for (i = 0; i < 256; i++) 
	{
		for (bi = botlists[i]; bi; bi = bi->next) 
		{
			std::cout << "Writing Bot " << bi->nick << "!" << bi->user << "@" << bi->host << std::endl;
			fs << "BI " << bi->nick << " " << bi->user << " " << bi->host << " " 
				<< bi->created << " " << bi->chancount << " :" << bi->real << std::endl;
			fs << "MD BI flags "
					<< (( bi->flags & BI_PRIVATE  ) ? "PRIVATE "  : "" )
					<< (( bi->flags & BI_CHANSERV ) ? "CHANSERV " : "" )
					<< (( bi->flags & BI_BOTSERV  ) ? "BOTSERV "  : "" )
					<< (( bi->flags & BI_HOSTSERV  ) ? "HOSTSERV " : "" )
					<< (( bi->flags & BI_OPERSERV ) ? "OPERSERV " : "" )
					<< (( bi->flags & BI_MEMOSERV ) ? "MEMOSERV " : "" )
					<< (( bi->flags & BI_NICKSERV ) ? "NICKSERV " : "" )
					<< (( bi->flags & BI_GLOBAL   ) ? "GLOBAL "   : "" ) << std::endl;
		} // for (botflists[i])
	} // for (i)

	/* Section IV: Hosts */
	/* IVa: First database */
	HostCore *hc, *firsthc;
	if ((f = open_db_read("HostServ", "hosts.db", 3))) {
		int c;
		int32 tmp32;

		printf("Trying to merge hosts...\n");

		while ((c = getc_db(f)) == 1) {
			if (c != 1) {
				printf("Invalid format in %s.\n", "hosts.db");
				exit(0);
			}
			hc = (HostCore *)calloc(1, sizeof(HostCore));
			READ(read_string(&hc->nick, f));
			READ(read_string(&hc->vIdent, f));
			READ(read_string(&hc->vHost, f));
			READ(read_string(&hc->creator, f));
			READ(read_int32(&tmp32, f));
			hc->time = tmp32;
			hc->next = firsthc;
			if (firsthc)
				firsthc->last = hc;
			hc->last = NULL;
			firsthc = hc;
		}
	}
	/* IVb: Saving */
	for (hc = firsthc; hc; hc = hc->next) 
	{  // because vIdent can sometimes be empty, we put it at the end of the list 
		std::cout << "Writing vHost for " << hc->nick << " (" << hc->vIdent << "@" << hc->vHost << ")" << std::endl;
		fs << "HI " << hc->nick << " " << hc->creator << " " << hc->time << " " 
				<< hc->vHost << " " << hc->vIdent << std::endl;
	} // for (hc)


	/* MERGING DONE \o/ HURRAY! */
	fs.close();
	printf("\n\nMerging is now done. I give NO guarantee for your DBs.\n");
	return 0;
} /* End of main() */
