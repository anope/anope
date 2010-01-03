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

static std::string GetLevelName(int level)
{
	switch (level)
	{
		case 0:
			return "INVITE";
		case 1:
			return "AKICK";
		case 2:
			return "SET";
		case 3:
			return "UNBAN";
		case 4:
			return "AUTOOP";
		case 5:
			return "AUTODEOP";
		case 6:
			return "AUTOVOICE";
		case 7:
			return "OPDEOP";
		case 8:
			return "LIST";
		case 9:
			return "CLEAR";
		case 10:
			return "NOJOIN";
		case 11:
			return "CHANGE";
		case 12:	
			return "MEMO";
		case 13:
			return "ASSIGN";
		case 14:
			return "BADWORDS";
		case 15:
			return "NOKICK";
		case 16:
			return "FANTASIA";
		case 17:
			return "SAY";
		case 18:
			return "GREET";
		case 19:
			return "VOICEME";
		case 20:	
			return "VOICE";
		case 21:
			return "GETKEY";
		case 22:
			return "AUTOHALFOP";
		case 23:
			return "AUTOPROTECT";
		case 24:
			return "OPDEOPME";
		case 25:
			return "HALFOPME";
		case 26:
			return "HALFOP";
		case 27:
			return "PROTECTME";
		case 28:	
			return "PROTECT";
		case 29:	
			return "KICKME";
		case 30:
			return "KICK";
		case 31:
			return "SIGNKICK";
		case 32:
			return "BANME";
		case 33:
			return "BAN";
		case 34:
			return "TOPIC";
		case 35:
			return "INFO";
		default:
			return "INVALID";
	}
}

void process_mlock_modes(std::ofstream &fs, size_t m, const std::string &ircd)
{
	/* this is the same in all protocol modules */
		if (m &        0x1) fs << " INVITE";        // CMODE_i
		if (m &        0x2) fs << " MODERATED";     // CMODE_m
		if (m &        0x4) fs << " NOEXTERNAL";    // CMODE_n
		if (m &        0x8) fs << " PRIVATE";       // CMODE_p
		if (m &       0x10) fs << " SECRET";        // CMODE_s
		if (m &       0x20) fs << " TOPIC";         // CMODE_t
		if (m &       0x40) fs << " KEY";           // CMODE_k
		if (m &       0x80) fs << " LIMIT";         // CMODE_l
		if (m &      0x200) fs << " REGISTERED";    // CMODE_r

	if (ircd == "unreal" || ircd == "inspircd")
	{
		if (m &      0x100) fs << " REGISTEREDONLY"; // CMODE_R
		if (m &      0x400) fs << " BLOCKCOLOR";     // CMODE_c
		if (m &     0x2000) fs << " NOKNOCK";        // CMODE_K
		if (m &     0x4000) fs << " REDIRECT";       // CMODE_L
		if (m &     0x8000) fs << " OPERONLY";       // CMODE_O
		if (m &    0x10000) fs << " NOKICK";         // CMODE_Q
		if (m &    0x20000) fs << " STRIPCOLOR";     // CMODE_S
		if (m &    0x80000) fs << " FLOOD";          // CMODE_f
		if (m &   0x100000) fs << " FILTER";         // CMODE_G
		if (m &   0x200000) fs << " NOCTCP";         // CMODE_C
		if (m &   0x400000) fs << " AUDITORIUM";     // CMODE_u
		if (m &   0x800000) fs << " SSL";            // CMODE_z
		if (m &  0x1000000) fs << " NONICK";         // CMODE_N
		if (m &  0x4000000) fs << " REGMODERATED";   // CMODE_M
	}

	if (ircd == "unreal")
	{
		if (m &      0x800) fs << " ADMINONLY";       // CMODE_A
		if (m &     0x1000) fs << "";                 // old CMODE_H (removed in 3.2)
		if (m &    0x40000) fs << " NOINVITE";        // CMODE_f
		if (m &  0x2000000) fs << " NONOTICE";        // CMODE_T
		if (m &  0x8000000) fs << " JOINFLOOD";       // CMODE_j
	} // if (unreal)
	if (ircd == "inspircd" )
	{
		if (m &      0x800) fs << " ALLINVITE";        // CMODE_A
		if (m &     0x1000) fs << " NONOTICE";         // CMODE_T
		/* for some reason, there is no CMODE_P in 1.8.x and no CMODE_V in the 1.9.1 protocol module
		   we are ignoring this flag until we find a solution for this problem, 
		   so the +V/+P mlock mode is lost on convert 
		   anope 1.8: if (m &    0x40000) fs << " NOINVITE";         // CMODE_V
		   anope 1.9: if (m &    0x40000) fs << " PERM";             // CMODE_P
		*/
		if (m &  0x2000000) fs << " JOINFLOOD";        // CMODE_j
		if (m &  0x8000000) fs << " BLOCKCAPS";        // CMODE_B
		if (m & 0x10000000) fs << " NICKFLOOD";        // CMODE_F
		if (m & 0x20000000) fs << "";                  // CMODE_g (mode +g <badword>) ... can't be mlocked in older version
		if (m & 0x40000000) fs << "";                  // CMODE_J (mode +J [seconds] ... can't be mlocked in older versions
	} // if (inspircd)
}

int main(int argc, char *argv[])
{
	dbFILE *f;
	std::ofstream fs;
	std::string hashm, ircd;

	printf("\n"C_LBLUE"Anope 1.8.x -> 1.9.2+ database converter"C_NONE"\n\n");

	while (hashm != "md5" && hashm != "sha1" && hashm != "oldmd5" && hashm != "plain")
	{
		if (!hashm.empty())
			std::cout << "Select a valid option, thanks!" << std::endl;
		std::cout << "Which hash method did you use? (md5, sha1, oldmd5, plain)" << std::endl << "? ";
		std::cin >> hashm;
	}
	
	while (ircd != "bahamut" && ircd != "charybdis" && ircd != "dreamforge" && ircd != "hybrid"
				&& ircd != "inspircd" && ircd != "plexus2" && ircd != "plexus3" && ircd != "ptlink"
				&& ircd != "rageircd" && ircd != "ratbox" && ircd != "shadowircd" && ircd != "solidircd"
				&& ircd != "ultimate2" && ircd != "ultimate3" && ircd != "unreal" && ircd != "viagra")
	{
		if (!ircd.empty())
			std::cout << "Select a valid option!" << std::endl;
		std::cout << "Which IRCd did you use? (required for converting the mlock modes)" << std::endl;
		std::cout << "(bahamut, charybdis, dreamforge, hybrid, inspircd, plexus2, plexus3, ptlink," << std::endl;
		std::cout << "rageircd, ratbox, shadowircd, solidircd, ultimate2, ultimate3, unreal, viagra)" << std::endl;
		std::cout << "Your IRCd: "; std::cin >> ircd;
	}

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

	head = NULL;
	if ((f = open_db_read("HostServ", "hosts.db", 3)))
	{
		int c;
		HostCore *hc;

		while ((c = getc_db(f)) == 1)
		{
			hc = (HostCore *)calloc(1, sizeof(HostCore));
			READ(read_string(&hc->nick, f));
			READ(read_string(&hc->vIdent, f));
			READ(read_string(&hc->vHost, f));
			READ(read_string(&hc->creator, f));
			READ(read_int32(&hc->time, f));

			hc->next = head;
			head = hc;
		}

		close_db(f);
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

			fs << "NC " << nc->display << " " << hashm << ":" << cpass << " ";
			fs << " " << GetLanguageID(nc->language) << " " << nc->memos.memomax << " " << nc->channelcount << std::endl;

			std::cout << "Wrote account for " << nc->display << " passlen " << strlen(cpass) << std::endl;
			if (nc->email)
				fs << "MD EMAIL " << nc->email << std::endl;
			if (nc->greet)
				fs << "MD GREET :" << nc->greet << std::endl;
			if (nc->icq)
				fs << "MD ICQ :" << nc->icq << std::endl;
			if (nc->url)
				fs << "MD URL :" << nc->url << std::endl;

			if (nc->accesscount)
			{
				for (j = 0, access = nc->access; j < nc->accesscount; j++, access++)
					fs << "MD ACCESS " << *access << std::endl;
			}

			fs << "MD FLAGS "
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
					<< ((nc->flags & NI_FORBIDDEN    ) ? "FORBIDDEN "    : "") << std::endl;
			if (nc->memos.memocount)
			{
				memos = nc->memos.memos;
				for (j = 0; j < nc->memos.memocount; j++, memos++)
				{
					fs << "MD MI " << memos->number << " " << memos->time << " " << memos->sender;
					if (memos->flags & MF_UNREAD)
						fs << " UNREAD";
					if (memos->flags & MF_RECEIPT)
						fs << " RECEIPT";
					if (memos->flags & MF_NOTIFYS)
						fs << " NOTIFYS";
					fs << " :" << memos->text << std::endl;
				}
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
					if (na->last_usermask)
						fs << "MD LAST_USERMASK " << na->last_usermask << std::endl;
					if (na->last_realname)
						fs << "MD LAST_REALNAME :" << na->last_realname << std::endl;
					if (na->last_quit)
						fs << "MD LAST_QUIT :" << na->last_quit << std::endl;
					if ((na->status & NS_FORBIDDEN) || (na->status & NS_NO_EXPIRE)) 
					{
						fs << "MD FLAGS"
							<< ((na->status & NS_FORBIDDEN) ? " FORBIDDEN" : "")
							<< ((na->status & NS_NO_EXPIRE) ? " NOEXPIRE"   : "") << std::endl;
					}

					HostCore *hc = findHostCore(na->nick);
					if (hc)
					{
						fs << "MD VHOST " << hc->creator << " " << hc->time << " " << hc->vHost << " :" << (hc->vIdent ? hc->vIdent : "") << std::endl;
					}
				}
			}
		}
	}


	/* Section II: Bots */
	/* IIa: First database */
	if ((f = open_db_read("Botserv", "bot.db", 10))) {
		std::string input;
		int c, broken = 0;
		int32 created;
		int16 flags, chancount;
		char *nick, *user, *host, *real;

		std::cout << "Trying to convert bots..." << std::endl;

		while (input != "y" && input != "n")
		{
			std::cout << std::endl << "Are you converting a 1.9.0 database? (y/n) " << std::endl << "? ";
			std::cin >> input;
		}
		if (input == "y")
			broken = 1;

		while ((c = getc_db(f)) == 1) {
			READ(read_string(&nick, f));
			READ(read_string(&user, f));
			READ(read_string(&host, f));
			READ(read_string(&real, f));
			SAFE(read_int16(&flags, f));
			READ(read_int32(&created, f));
			READ(read_int16(&chancount, f));

			if (created == 0)
				created = time(NULL); // Unfortunatley, we forgot to store the created bot time in 1.9.1+

			/* fix for the 1.9.0 broken bot.db */
			if (broken)
			{
				flags = 0;
				if (!stricmp(nick, "ChanServ"))
					flags |= BI_CHANSERV;
				if (!stricmp(nick, "BotServ"))
					flags |= BI_BOTSERV;
				if (!stricmp(nick, "HostServ"))
					flags |= BI_HOSTSERV;
				if (!stricmp(nick, "OperServ"))
					flags |= BI_OPERSERV;
				if (!stricmp(nick, "MemoServ"))
					flags |= BI_MEMOSERV;
				if (!stricmp(nick, "NickServ"))
					flags |= BI_NICKSERV;
				if (!stricmp(nick, "Global"))
					flags |= BI_GLOBAL;
			} /* end of 1.9.0 broken database fix */
			std::cout << "Writing Bot " << nick << "!" << user << "@" << host << std::endl;
			fs << "BI " << nick << " " << user << " " << host << " " << created << " " << chancount << " :" << real << std::endl;
			fs << "MD FLAGS"
					<< (( flags & BI_PRIVATE  ) ? " PRIVATE"  : "" )
					<< (( flags & BI_CHANSERV ) ? " CHANSERV" : "" )
					<< (( flags & BI_BOTSERV  ) ? " BOTSERV"  : "" )
					<< (( flags & BI_HOSTSERV ) ? " HOSTSERV" : "" )
					<< (( flags & BI_OPERSERV ) ? " OPERSERV" : "" )
					<< (( flags & BI_MEMOSERV ) ? " MEMOSERV" : "" )
					<< (( flags & BI_NICKSERV ) ? " NICKSERV" : "" )
					<< (( flags & BI_GLOBAL   ) ? " GLOBAL"   : "" ) << std::endl;
		}
		close_db(f);
	}

	/* Section III: Chans */
	// IIIa: First database
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

		close_db(f);
	}

	ChannelInfo *ci;

	for (i = 0; i < 256; i++)
	{
		for (ci = chanlists[i]; ci; ci = ci->next)
		{
			int j;

			fs << "CH " << ci->name << " " << ci->time_registered << " " << ci->last_used;
			fs << " " << ci->bantype << " " << ci->memos.memomax << std::endl;
			if (ci->founder)
				fs << "MD FOUNDER " << ci->founder << std::endl;
			if (ci->successor)
				fs << "MD SUCCESSOR " << ci->successor << std::endl;
			fs << "MD LEVELS";
			for (j = 0; j < 36; j++)
			{
				fs << " " << GetLevelName(j) << " " << ci->levels[j];
			}
			fs << std::endl;
			fs << "MD FLAGS" 
					<< ((ci->flags & CI_KEEPTOPIC     ) ? " KEEPTOPIC"     : "")
					<< ((ci->flags & CI_SECUREOPS     ) ? " SECUREOPS"     : "")
					<< ((ci->flags & CI_PRIVATE       ) ? " PRIVATE"       : "")
					<< ((ci->flags & CI_TOPICLOCK     ) ? " TOPICLOCK"     : "")
					<< ((ci->flags & CI_RESTRICTED    ) ? " RESTRICTED"    : "")
					<< ((ci->flags & CI_PEACE         ) ? " PEACE"         : "")
					<< ((ci->flags & CI_SECURE        ) ? " SECURE"        : "")
					<< ((ci->flags & CI_FORBIDDEN     ) ? " FORBIDDEN"     : "")
					<< ((ci->flags & CI_NO_EXPIRE     ) ? " NO_EXPIRE"     : "")
					<< ((ci->flags & CI_MEMO_HARDMAX  ) ? " MEMO_HARDMAX"  : "")
					<< ((ci->flags & CI_OPNOTICE      ) ? " OPNOTICE"      : "")
					<< ((ci->flags & CI_SECUREFOUNDER ) ? " SECUREFOUNDER" : "")
					<< ((ci->flags & CI_SIGNKICK      ) ? " SIGNKICK"      : "")
					<< ((ci->flags & CI_SIGNKICK_LEVEL) ? " SIGNKICKLEVEL" : "")
					<< ((ci->flags & CI_XOP           ) ? " XOP"           : "")
					<< ((ci->flags & CI_SUSPENDED     ) ? " SUSPENDED"      : "") << std::endl;
			if (ci->desc)
				fs << "MD DESC :" << ci->desc << std::endl;
			if (ci->url)
				fs << "MD URL :" << ci->url << std::endl;
			if (ci->email)
				fs << "MD EMAIL :" << ci->email << std::endl;
			if (ci->last_topic)  // MD CH topic <setter> <time> :topic
				fs << "MD TOPIC " << ci->last_topic_setter << " " << ci->last_topic_time <<  " :" << ci->last_topic << std::endl;
			if (ci->flags & CI_FORBIDDEN)
				fs << "MD FORBID " << ci->forbidby << " :" << ci->forbidreason << std::endl;

			for (j = 0; j < ci->accesscount; j++)
			{  // MD ACCESS <display> <level> <last_seen> <creator> - creator isn't in 1.9.0-1, but is in 1.9.2
				if (ci->access[j].in_use)
					fs << "MD ACCESS "
						<< ci->access[j].nc->display << " " << ci->access[j].level << " " 
						<< ci->access[j].last_seen << " Unknown" << std::endl;
			}

			for (j = 0; j < ci->akickcount; j++)
			{  // MD AKICK <STUCK/UNSTUCK> <NICK/MASK> <akick> <creator> <addtime> :<reason>
				if (ci->akick[j].flags & 0x0001)
				{
					fs << "MD AKICK "
						<< ((ci->akick[j].flags & AK_STUCK) ? "STUCK " : "UNSTUCK " )
						<< ((ci->akick[j].flags & AK_ISNICK) ? "NICK " : "MASK ")
						<< ((ci->akick[j].flags & AK_ISNICK) ? ci->akick[j].u.nc->display : ci->akick[j].u.mask )
						<< " " << ci->akick[j].creator << " " << ci->akick[j].addtime << " :";
						if (ci->akick[j].reason)
							fs << ci->akick[j].reason;
						fs << std::endl;
				}
			}

			if (ci->mlock_on)
			{
				fs << "MD MLOCK_ON";
				process_mlock_modes(fs, ci->mlock_on, ircd);
				fs << std::endl;
			}
			if (ci->mlock_off)
			{
				fs << "MD MLOCK_OFF";
				process_mlock_modes(fs, ci->mlock_off, ircd);
				fs << std::endl;
			}
			if (ci->mlock_limit || ci->mlock_key || ci->mlock_flood || ci->mlock_redirect)
			{
				fs << "MD MLP";
				if (ci->mlock_limit)
					fs << " CMODE_LIMIT " << ci->mlock_limit;
				if (ci->mlock_key)
					fs << " CMODE_KEY " << ci->mlock_key;
				if (ci->mlock_flood)
					fs << " CMODE_FLOOD " << ci->mlock_flood;
				if (ci->mlock_redirect)
					fs << " CMODE_REDIRECT " << ci->mlock_redirect;
				fs << std::endl;
			}
			if (ci->memos.memocount)
			{
				Memo *memos;
				memos = ci->memos.memos;
				for (j = 0; j < ci->memos.memocount; j++, memos++)
				{
					fs << "MD MI " << memos->number << " " << memos->time << " " << memos->sender;
					if (memos->flags & MF_UNREAD)
						fs << " UNREAD";
					if (memos->flags & MF_RECEIPT)
						fs << " RECEIPT";
					if (memos->flags & MF_NOTIFYS)
						fs << " NOTIFYS";
					fs << " :" << memos->text << std::endl;
				}
			}

			if (ci->entry_message)
				fs << "MD ENTRYMSG :" << ci->entry_message << std::endl;
			if (ci->bi)  // here is "bi" a *Char, not a pointer to BotInfo !
				fs << "MD BI NAME " << ci->bi << std::endl;
			if (ci->botflags)
				fs << "MD BI FLAGS"
					<< (( ci->botflags & BS_DONTKICKOPS     ) ? " DONTKICKOPS"    : "" )
					<< (( ci->botflags & BS_DONTKICKVOICES  ) ? " DONTKICKVOICES"  : "")
					<< (( ci->botflags & BS_FANTASY         ) ? " FANTASY"         : "")
					<< (( ci->botflags & BS_SYMBIOSIS       ) ? " SYMBIOSIS"       : "")
					<< (( ci->botflags & BS_GREET           ) ? " GREET"           : "")
					<< (( ci->botflags & BS_NOBOT           ) ? " NOBOT"           : "")
					<< (( ci->botflags & BS_KICK_BOLDS      ) ? " KICK_BOLDS"      : "")
					<< (( ci->botflags & BS_KICK_COLORS     ) ? " KICK_COLORS"     : "")
					<< (( ci->botflags & BS_KICK_REVERSES   ) ? " KICK_REVERSES"   : "")
					<< (( ci->botflags & BS_KICK_UNDERLINES ) ? " KICK_UNDERLINES" : "")
					<< (( ci->botflags & BS_KICK_BADWORDS   ) ? " KICK_BADWORDS"   : "")
					<< (( ci->botflags & BS_KICK_CAPS       ) ? " KICK_CAPS"       : "")
					<< (( ci->botflags & BS_KICK_FLOOD      ) ? " KICK_FLOOD"      : "")
					<< (( ci->botflags & BS_KICK_REPEAT     ) ? " KICK_REPEAT"     : "") << std::endl;
			fs << "MD BI TTB";
			fs << " BOLDS " << ci->ttb[0];
			fs << " COLORS " << ci->ttb[1];
			fs << " REVERSES " << ci->ttb[2];
			fs << " UNDERLINES " << ci->ttb[3];
			fs << " BADWORDS " << ci->ttb[4];
			fs << " CAPS " << ci->ttb[5];
			fs << " FLOOD " << ci->ttb[6];
			fs << " REPEAT " << ci->ttb[7];
			fs << std::endl;
			if (ci->capsmin)
				fs << "MD BI CAPSMINS " << ci->capsmin << std::endl;
			if (ci->capspercent)
				fs << "MD BI CAPSPERCENT " << ci->capspercent << std::endl;
			if (ci->floodlines)
				fs << "MD BI FLOODLINES " << ci->floodlines << std::endl;
			if (ci->floodsecs)
				fs << "MD BI FLOODSECS " << ci->floodsecs << std::endl;
			if (ci->repeattimes)
				fs << "MD BI REPEATTIMES " << ci->repeattimes << std::endl;
			for (j = 0; j < ci->bwcount; j++)
			{
				if (ci->badwords[j].in_use)
				{
					fs << "MD BI BADWORD "
						<< (( ci->badwords[j].type == 0 ) ? "ANY "    : "" )
						<< (( ci->badwords[j].type == 1 ) ? "SINGLE " : "" )
						<< (( ci->badwords[j].type == 3 ) ? "START "  : "" )
						<< (( ci->badwords[j].type == 4 ) ? "END "    : "" )
						<< ":" << ci->badwords[j].word << std::endl;
				}
			}

		} /* for (chanlists[i]) */
	} /* for (i) */

	/*********************************/
	/*    OPERSERV Section           */
	/*********************************/

	if ((f = open_db_read("OperServ", "oper.db", 13)))
	{
		int32 maxusercnt = 0, maxusertime = 0, seton = 0, expires = 0;
		int16 capacity = 0;
		char *user, *host, *by, *reason, *mask;

		std::cout << "Writing operserv data (stats, akills, sglines, szlines)" << std::endl;

		SAFE(read_int32(&maxusercnt, f));
		SAFE(read_int32(&maxusertime, f));
		fs << "OS STATS " << maxusercnt << " " << maxusertime << std::endl;

		/* AKILLS */
		read_int16(&capacity, f);
		for (i = 0; i < capacity; i++) 
		{
			SAFE(read_string(&user, f));
			SAFE(read_string(&host, f));
			SAFE(read_string(&by, f));
			SAFE(read_string(&reason, f));
			SAFE(read_int32(&seton, f));
			SAFE(read_int32(&expires, f));
			fs << "OS AKILL " << user << " " << host << " " << by << " " << seton << " " << expires << " :" << reason << std::endl;
			free(user); free(host); free(by); free(reason);
		}
		/* SGLINES */
		read_int16(&capacity, f);
		for (i = 0; i < capacity; i++) 
		{
			SAFE(read_string(&mask, f));
			SAFE(read_string(&by, f));
			SAFE(read_string(&reason, f));
			SAFE(read_int32(&seton, f));
			SAFE(read_int32(&expires, f));
			fs << "OS SGLINE " << mask << " " << by << " " << seton << " " << expires << " :" << reason << std::endl;
			free(mask); free(by); free(reason);
		}
		/* SQLINES */
		read_int16(&capacity, f);
		for (i = 0; i < capacity; i++) 
		{
			SAFE(read_string(&mask, f));
			SAFE(read_string(&by, f));
			SAFE(read_string(&reason, f));
			SAFE(read_int32(&seton, f));
			SAFE(read_int32(&expires, f));
			fs << "OS SQLINE " << mask << " " << by << " " << seton << " " << expires << " :" << reason << std::endl;
			free(mask); free(by); free(reason);
		}
		/* SZLINES */
		read_int16(&capacity, f);
		for (i = 0; i < capacity; i++) 
		{
			SAFE(read_string(&mask, f));
			SAFE(read_string(&by, f));
			SAFE(read_string(&reason, f));
			SAFE(read_int32(&seton, f));
			SAFE(read_int32(&expires, f));
			fs << "OS SZLINE " << mask << " " << by << " " << seton << " " << expires << " :" << reason << std::endl;
			free(mask); free(by); free(reason);
		}
		close_db(f);
	} // operserv database


	/* CONVERTING DONE \o/ HURRAY! */
	fs.close();
	return 0;
} /* End of main() */
