/*
 *   IRC - Internet Relay Chat, atheme2anope.c
 *   (C) Copyright 2009, the Anope team (team@anope.org) 
 *
 *
 *   This program is free software; you can redistribute it and/or 
modify
 *   it under the terms of the GNU General Public License (see it online
 *   at http://www.gnu.org/copyleft/gpl.html) as published by the Free
 *   Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 */

#include "module.h"

#define ATHEMEDATABASE "atheme.db"

#define MU_NEVEROP     0x00000002
#define MU_NOOP        0x00000004
#define MU_HIDEMAIL    0x00000010
#define MU_NOMEMO      0x00000040
#define MU_PRIVATE     0x00002000

static int CNicks = 0;
static int CChannels = 0;
static int CAkills = 0;

typedef void (*ProcessLine)(char *line);
FILE *athemedb;
void WriteDatabase();
void WriteNick(char *line);
NickAlias *makenick(const char *nick);
NickAlias *makealias(const char *nick, NickCore *nc);
int parseFlags(ChannelInfo *ci, char *flags);
void addAccess(ChannelInfo *ci, NickCore *nc, int level);
void addAkick(ChannelInfo *ci, char *akick, char *time);
void WriteChannel(char *line);
void WriteAkill(char *line);

int AnopeInit(int argc, char **argv)
{
	moduleAddAuthor("Anope");
	moduleAddVersion("$Id$");
	moduleSetType(SUPPORTED);

	athemedb = fopen(ATHEMEDATABASE, "r");

	if (!athemedb)
	{
		alog("[atheme2anope] error: unable to read from atheme database");
	}
	else
	{
		alog("[atheme2anope] converting databases...");
		WriteDatabase();
		fclose(athemedb);
		alog("[atheme2anope] Done! Converted %d nicks, %d channels, and %d akills. Restarting...",
			CNicks, CChannels, CAkills);

		quitmsg = calloc(50, 1);
		sprintf(quitmsg, "%s", "Shutting down to convert databases...");

		save_data = 1;
		delayed_quit = 1;
	}

	return MOD_STOP;
}

void AnopeFini(void)
{
}


void WriteDatabase(void)
{
	char line[BUFSIZE];
	char *tok = NULL;
	ProcessLine function = NULL;

	while (!feof(athemedb))
	{
		memset(&line, 0, sizeof(line));
		fgets(line, sizeof(line), athemedb);

		if (tok)
			free(tok);

		tok = myStrGetToken(line, ' ', 0);

		if (tok)
		{
			if (!strcmp(tok, "MU"))
			{
				function = WriteNick;
			}
			else if (!strcmp(tok, "MC"))
			{
				function = WriteChannel;
			}
			else if (!strcmp(tok, "KID"))
			{
				function = NULL;
			}
			else if (!strcmp(tok, "KL"))
			{
				function = WriteAkill;
			}
			if (function)
				function(line);
		}
	}

	if (tok)
		free(tok);
}

void WriteNick(char *line)
{
	char *tok;
	NickAlias *na = NULL, *na2;
	unsigned int flags;
	int i;
	time_t t = time(NULL);

	tok = strtok(line, " ");

	if (tok)
	{
		if (!strcmp(tok, "MU"))
		{
			tok = strtok(NULL, " ");

			if ((na = findnick(tok)))
			{
				delnick(na);
			}

			flags = 0;

			na = makenick(tok);

			tok = strtok(NULL, " ");
			enc_encrypt(tok, strlen(tok), na->nc->pass, PASSMAX - 1);

			tok = strtok(NULL, " ");
			na->nc->email = sstrdup(tok);

			tok = strtok(NULL, " ");
			na->time_registered = atol(tok);

			tok = strtok(NULL, " ");
			na->last_seen = atol(tok);

			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");
			tok = strtok(NULL, " ");

			tok = strtok(NULL, " ");
			flags = atol(tok);

			if (flags & MU_NOMEMO)
			{
				na->nc->memos.memomax = 0;
			}
			if (flags & MU_NEVEROP || flags & MU_NOOP)
			{
				na->nc->flags |= NI_AUTOOP;
			}
			if (flags & 0x00000010)
			{
				na->nc->flags |= NI_HIDE_EMAIL;
			}
			if (flags & MU_PRIVATE)
			{
				na->nc->flags |= NI_PRIVATE;
			}
		}
		else if (!strcmp(tok, "MD"))
		{
			tok = strtok(NULL, " ");

			tok = strtok(NULL, " ");
			if ((na = findnick(tok)))
			{
				tok = strtok(NULL, " ");
				
				if (!strcmp(tok, "private:host:vhost"))
				{
					tok = strtok(NULL, "\n");

					if (na->last_usermask)
						free(na->last_usermask);
					
					na->last_usermask = sstrdup(tok);
				}
				else if (!strcmp(tok, "private:usercloak"))
				{
					tok = strtok(NULL, "\n");
					addHostCore(na->nick, NULL, tok, "atheme2anope", t);
				}
				else if (!stricmp(tok, "greet"))
				{
					tok = strtok(NULL, "\n");
					na->nc->greet = sstrdup(tok);
				}
				else if (!stricmp(tok, "icq"))
				{
					tok = strtok(NULL, " ");
					na->nc->icq = atoi(tok);
				}
				else if (!stricmp(tok, "url"))
				{
					tok = strtok(NULL, " ");
					na->nc->url = sstrdup(tok);
				}
				else if (!stricmp(tok, "private:freeze:freezer"))
				{
					na->nc->flags |= NI_SUSPENDED;
					na->nc->flags |= NI_SECURE;
					na->nc->flags &= ~(NI_KILLPROTECT | NI_KILL_QUICK | NI_KILL_IMMED);
				}
				else if (!stricmp(tok, "private:freeze:reason"))
				{
					tok = strtok(NULL, "\n");

					na->last_quit = sstrdup(tok);
				}
			}
		}
		else if (!strcmp(tok, "SO"))
		{
			tok = strtok(NULL, " ");
			na->nc->flags |= NI_SERVICES_OPER;
		}
		else if (!strcmp(tok, "MN"))
		{
			tok = strtok(NULL, " ");

			if ((na = findnick(tok)))
			{
				tok = strtok(NULL, " ");

				if (strcmp(na->nick, tok))
				{
					for (i = 0; i < na->nc->aliases.count; i++)
					{
						na2 = na->nc->aliases.list[i];

						if (!stricmp(na2->nick, tok))
							break;
					}

					if (na->nc->aliases.count == i) {
						na = makealias(tok, na->nc);

						tok = strtok(NULL, " ");
						na->time_registered = atol(tok);

						tok = strtok(NULL, " ");
						na->last_seen = atol(tok);
					}
				}
				else
				{
					tok = strtok(NULL, " ");
					na->time_registered = atol(tok);

					tok = strtok(NULL, " ");
					na->last_seen = atol(tok);
				}
			}
		}
	}
}

NickAlias *makenick(const char *nick)
{
	NickAlias *na;
	NickCore *nc;

	nc = scalloc(1, sizeof(NickCore));
	nc->display = sstrdup(nick);
	slist_init(&nc->aliases);
	insert_core(nc);

	if (debug)
		alog("[atheme2anope] debug: Group %s has been created", nc->display);

	na = makealias(nick, nc);

	nc->channelmax = CSMaxReg;
	nc->accesscount = 0;
	nc->access = NULL;
	nc->language = NSDefLanguage;
	nc->flags |= NSDefFlags;

	return na;
}

NickAlias *makealias(const char *nick, NickCore *nc)
{
	NickAlias *na;

	na = scalloc(1, sizeof(NickAlias));
	na->nick = sstrdup(nick);
	na->last_usermask = sstrdup("*@*");
	na->last_realname = sstrdup("atheme2anope");
	na->nc = nc;
	slist_add(&nc->aliases, na);
	alpha_insert_alias(na);

	if (debug)
		alog("[atheme2anope] debug: Nick %s has been created", na->nick);
	CNicks++;

	return na;
}

int parseFlags(ChannelInfo *ci, char *flags)
{
	unsigned char mode;
	int add = 0;
	int max = 1;

	while (flags && (mode = *flags++))
	{
		switch (mode)
		{
			case '+':
				add = 1;
				continue;
			case '-':
				add = 0;
				continue;
			case 'O':
			case 'o':
				if (add && max < 5)
					max = 5;
				continue;
			case 'V':
			case 'v':
				if (add && max < 3)
					max = 3;
				continue;
			case 'H':
			case 'h':
				if (add && max < 4)
					max = 4;
				continue;
			case 'a':
				if (add && max < 10)
					max = 10;
				continue;
			case 'q':
				if (add && max < 9999)
					max = 9999;
				continue;
			case 'b':
				if (add)
					max = -9999;
		}
	}

	return max;
}

void addAccess(ChannelInfo *ci, NickCore *nc, int level)
{
	ChanAccess *access;

	ci->accesscount++;
	ci->access = srealloc(ci->access, sizeof(ChanAccess) * ci->accesscount);

	access = &ci->access[ci->accesscount - 1];
	access->in_use = 1;
	access->nc = nc;
	access->level = level;
	access->last_seen = 0;

	if (debug)
		alog("[atheme2anope] debug: Added nick %s to %s access list at level %d", nc->display, ci->name, level);
}

void addAkick(ChannelInfo *ci, char *akick, char *time)
{
	time_t t = atol(time);
	AutoKick *autokick;

	ci->akickcount++;
	ci->akick = srealloc(ci->akick, sizeof(AutoKick) * ci->akickcount);
	autokick = &ci->akick[ci->akickcount - 1];
	autokick->flags = AK_USED;
	autokick->u.mask = sstrdup(akick);
	autokick->creator = sstrdup("atheme2anope");
	autokick->addtime = t;
	autokick->reason = NULL;

	if (debug)
		alog("[atheme2anope] debug: Added autokick %s to %s with time %s", akick, ci->name, time);
}

void WriteChannel(char *line)
{
	char *tok, *akick;
	ChannelInfo *ci;
	NickCore *nc;
	int level;

	tok = strtok(line, " ");

	if (tok)
	{
		if (!strcmp(tok, "MC"))
		{
			tok = strtok(NULL, " ");

			if ((ci = cs_findchan(tok)))
			{
				delchan(ci);
			}

			ci = makechan(tok);
			CChannels++;
			ci->bantype = CSDefBantype;
			ci->flags = CSDefFlags;
			ci->mlock_on = ircd->defmlock;
			ci->memos.memomax = MSMaxMemos;
			ci->desc = sstrdup("Unknown");
			strscpy(ci->last_topic_setter, s_ChanServ, NICKMAX);
			ci->bi = NULL;
			ci->botflags = BSDefFlags;

			tok = strtok(NULL, " ");

			tok = strtok(NULL, " ");
			if ((nc = findcore(tok)))
			{
				ci->founder = nc;
			}
			else
			{
				alog("[atheme2anope] warning!! Channel %s has unfound user %s as founder", ci->name, tok);
				delchan(ci);
				return;
			}

			ci->founder->channelcount++;

			tok = strtok(NULL, " ");
			ci->time_registered = atol(tok);

			tok = strtok(NULL, " ");
			ci->last_used = atol(tok);

			if (debug)
				alog("[atheme2anope] debug: Channel %s added (founder %s)", ci->name, ci->founder->display);
		}
		else if (!stricmp(tok, "CA"))
		{
			tok = strtok(NULL, " ");

			if ((ci = cs_findchan(tok)))
			{
				tok = strtok(NULL, " ");

				if ((nc = findcore(tok)))
				{
					tok = strtok(NULL, " ");
					level = parseFlags(ci, tok);
					addAccess(ci, nc, level);
				}
				else
				{
					akick = tok;
					tok = strtok(NULL, " ");

					if (!strcmp(tok, "+b"))
					{
						tok = strtok(NULL, " ");

						addAkick(ci, akick, tok);
					}
				}
			}
		}
		else if (!stricmp(tok, "MD"))
		{
			tok = strtok(NULL, " ");

			tok = strtok(NULL, " ");
			if ((ci = cs_findchan(tok)))
			{
				tok = strtok(NULL, " ");
				
				if (!strcmp(tok, "private:topic:setter"))
				{
					tok = strtok(NULL, "\n");
					strscpy(ci->last_topic_setter, tok, NICKMAX);
				}
				else if (!strcmp(tok, "private:topic:text"))
				{
					tok = strtok(NULL, "\n");
					ci->last_topic = sstrdup(tok);
				}
				else if (!strcmp(tok, "private:topic:ts"))
				{
					tok = strtok(NULL, " ");
					ci->last_topic_time = atol(tok);
				}
				else if (!strcmp(tok, "description"))
				{
					tok = strtok(NULL, "\n");

					if (ci->desc)
						free(ci->desc);

					ci->desc = sstrdup(tok);
				}
				else if (!strcmp(tok, "private:entrymsg"))
				{
					tok = strtok(NULL, "\n");

					ci->entry_message = sstrdup(tok);
				}
				else if (!strcmp(tok, "private:close:closer"))
				{
					tok = strtok(NULL, "\n");

					ci->flags |= CI_SUSPENDED;
					ci->forbidby = sstrdup(tok);
				}
				else if (!strcmp(tok, "private:close:reason"))
				{
					tok = strtok(NULL, "\n");

					ci->forbidreason = sstrdup(tok);
				}
			}
		}
	}
}

void WriteAkill(char *line)
{
	char *tok;
	Akill *entry;
	time_t t = time(NULL), j;

	tok = strtok(line, " ");

	if (tok)
	{
		if (!strcmp(tok, "KL"))
		{
			tok = strtok(NULL, " ");

			if (!strcmp(tok, "*"))
			{
				entry = scalloc(sizeof(Akill), 1);

				tok = strtok(NULL, " ");
				entry->host = sstrdup(tok);

				tok = strtok(NULL, " ");
				if (!stricmp(tok, "0"))
					entry->expires = 0;
				else
				{
					j = (time_t)tok;
					entry->expires = (t + j);
				}

				tok = strtok(NULL, " ");
				j = (time_t)tok;
				entry->seton = j;

				tok = strtok(NULL, " ");
				entry->user = sstrdup("*");

				tok = strtok(NULL, "\n");
				entry->reason = sstrdup(tok);

				if (debug)
					alog("[atheme2anope] debug: Added akill on %s for %s to expire at %lu", entry->host, entry->reason, (long unsigned int)entry->expires);
				CAkills++;

				slist_add(&akills, entry);
			}
		}
	}
}

