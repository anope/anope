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

int do_badwords(User * u);
void myBotServHelp(User * u);
int badwords_del_callback(User * u, int num, va_list args);
int badwords_list(User * u, int index, ChannelInfo * ci, int *sent_header);
int badwords_list_callback(User * u, int num, va_list args);

/**
 * Create the command, and tell anope about it.
 * @param argc Argument count
 * @param argv Argument list
 * @return MOD_CONT to allow the module, MOD_STOP to stop it
 **/
int AnopeInit(int argc, char **argv)
{
    Command *c;

    moduleAddAuthor("Anope");
    moduleAddVersion(VERSION_STRING);
    moduleSetType(CORE);

    c = createCommand("BADWORDS", do_badwords, NULL, BOT_HELP_BADWORDS, -1,
                      -1, -1, -1);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);

    moduleSetBotHelp(myBotServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * Add the help response to Anopes /bs help output.
 * @param u The user who is requesting help
 **/
void myBotServHelp(User * u)
{
    notice_lang(s_BotServ, u, BOT_HELP_CMD_BADWORDS);
}

/**
 * The /bs badwords command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_badwords(User * u)
{
    char *chan = strtok(NULL, " ");
    char *cmd = strtok(NULL, " ");
    char *word = strtok(NULL, "");
    ChannelInfo *ci;
    BadWord *bw;

    int i;
    int need_args = (cmd
                     && (!stricmp(cmd, "LIST") || !stricmp(cmd, "CLEAR")));

    if (!cmd || (need_args ? 0 : !word)) {
        syntax_error(s_BotServ, u, "BADWORDS", BOT_BADWORDS_SYNTAX);
    } else if (!(ci = cs_findchan(chan))) {
        notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!check_access(u, ci, CA_BADWORDS)
               && (!need_args || !is_services_admin(u))) {
        notice_lang(s_BotServ, u, ACCESS_DENIED);
    } else if (stricmp(cmd, "ADD") == 0) {

        char *opt, *pos;
        int type = BW_ANY;

        if (readonly) {
            notice_lang(s_BotServ, u, BOT_BADWORDS_DISABLED);
            return MOD_CONT;
        }

        pos = strrchr(word, ' ');
        if (pos) {
            opt = pos + 1;
            if (*opt) {
                if (!stricmp(opt, "SINGLE"))
                    type = BW_SINGLE;
                else if (!stricmp(opt, "START"))
                    type = BW_START;
                else if (!stricmp(opt, "END"))
                    type = BW_END;
                if (type != BW_ANY)
                    *pos = 0;
            }
        }

        for (bw = ci->badwords, i = 0; i < ci->bwcount; bw++, i++) {
            if (bw->word && ((BSCaseSensitive && (!strcmp(bw->word, word)))
                             || (!BSCaseSensitive
                                 && (!stricmp(bw->word, word))))) {
                notice_lang(s_BotServ, u, BOT_BADWORDS_ALREADY_EXISTS,
                            bw->word, ci->name);
                return MOD_CONT;
            }
        }

        for (i = 0; i < ci->bwcount; i++) {
            if (!ci->badwords[i].in_use)
                break;
        }
        if (i == ci->bwcount) {
            if (i < BSBadWordsMax) {
                ci->bwcount++;
                ci->badwords =
                    srealloc(ci->badwords, sizeof(BadWord) * ci->bwcount);
            } else {
                notice_lang(s_BotServ, u, BOT_BADWORDS_REACHED_LIMIT,
                            BSBadWordsMax);
                return MOD_CONT;
            }
        }
        bw = &ci->badwords[i];
        bw->in_use = 1;
        bw->word = sstrdup(word);
        bw->type = type;
	
        alog("%s: %s!%s@%s added badword \"%s\" to %s",
             s_BotServ, u->nick, u->username, u->host, bw->word, ci->name);
        notice_lang(s_BotServ, u, BOT_BADWORDS_ADDED, bw->word, ci->name);

    } else if (stricmp(cmd, "DEL") == 0) {
        int deleted = 0, a, b;

        if (readonly) {
            notice_lang(s_BotServ, u, BOT_BADWORDS_DISABLED);
            return MOD_CONT;
        }

        /* Special case: is it a number/list?  Only do search if it isn't. */
        if (isdigit(*word) && strspn(word, "1234567890,-") == strlen(word)) {
            int count, last = -1;
            deleted =
                process_numlist(word, &count, badwords_del_callback, u, ci,
                                &last);
            if (!deleted) {
                if (count == 1) {
                    notice_lang(s_BotServ, u, BOT_BADWORDS_NO_SUCH_ENTRY,
                                last, ci->name);
                } else {
                    notice_lang(s_BotServ, u, BOT_BADWORDS_NO_MATCH,
                                ci->name);
                }
            } else if (deleted == 1) {
                alog("%s: %s!%s@%s deleted 1 badword from %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_BADWORDS_DELETED_ONE,
                            ci->name);
            } else {
                alog("%s: %s!%s@%s deleted %d badwords from %s",
                     s_BotServ, u->nick, u->username, u->host, deleted, ci->name);
                notice_lang(s_BotServ, u, BOT_BADWORDS_DELETED_SEVERAL,
                            deleted, ci->name);
            }
        } else {
            for (i = 0; i < ci->bwcount; i++) {
                if (ci->badwords[i].in_use
                    && !stricmp(ci->badwords[i].word, word))
                    break;
            }
            if (i == ci->bwcount) {
                notice_lang(s_BotServ, u, BOT_BADWORDS_NOT_FOUND, word,
                            chan);
                return MOD_CONT;
            }
            bw = &ci->badwords[i];
            alog("%s: %s!%s@%s deleted badword \"%s\" from %s",
                 s_BotServ, u->nick, u->username, u->host,  bw->word, ci->name);
            notice_lang(s_BotServ, u, BOT_BADWORDS_DELETED, bw->word,
                        ci->name);
            if (bw->word)
                free(bw->word);
            bw->word = NULL;
            bw->in_use = 0;
            deleted = 1;
        }

        if (deleted) {
            /* Reordering - DrStein */
            for (b = 0; b < ci->bwcount; b++) {
                if (ci->badwords[b].in_use) {
                    for (a = 0; a < ci->bwcount; a++) {
                        if (a > b)
                            break;
                        if (!(ci->badwords[a].in_use)) {
                            ci->badwords[a].in_use = ci->badwords[b].in_use;
                            ci->badwords[a].type = ci->badwords[b].type;
                            if (ci->badwords[b].word) {
                                ci->badwords[a].word = sstrdup(ci->badwords[b].word);
                                free(ci->badwords[b].word);
                            }
                            ci->badwords[b].word = NULL;
                            ci->badwords[b].in_use = 0;
                            break;
                        }
                    }
                }
            }
            /* After reordering only the entries at the end could still be empty.
             * We ll free the places no longer in use... - Viper */
            for (i = ci->bwcount - 1; i >= 0; i--) {
                if (ci->badwords[i].in_use)
                    break;
                ci->bwcount--;
            }
            ci->badwords =
                srealloc(ci->badwords,sizeof(BadWord) * ci->bwcount);
        }

    } else if (stricmp(cmd, "LIST") == 0) {
        int sent_header = 0;

        if (ci->bwcount == 0) {
            notice_lang(s_BotServ, u, BOT_BADWORDS_LIST_EMPTY, chan);
            return MOD_CONT;
        }
        if (word && strspn(word, "1234567890,-") == strlen(word)) {
            process_numlist(word, NULL, badwords_list_callback, u, ci,
                            &sent_header);
        } else {
            for (i = 0; i < ci->bwcount; i++) {
                if (!(ci->badwords[i].in_use))
                    continue;
                if (word && ci->badwords[i].word
                    && !match_wild_nocase(word, ci->badwords[i].word))
                    continue;
                badwords_list(u, i, ci, &sent_header);
            }
        }
        if (!sent_header)
            notice_lang(s_BotServ, u, BOT_BADWORDS_NO_MATCH, chan);

    } else if (stricmp(cmd, "CLEAR") == 0) {

        if (readonly) {
            notice_lang(s_BotServ, u, BOT_BADWORDS_DISABLED);
            return MOD_CONT;
        }

        for (i = 0; i < ci->bwcount; i++)
            if (ci->badwords[i].word)
                free(ci->badwords[i].word);

        free(ci->badwords);
        ci->badwords = NULL;
        ci->bwcount = 0;

        alog("%s: %s!%s@%s cleared badwords on %s",
             s_BotServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_BotServ, u, BOT_BADWORDS_CLEAR);

    } else {
        syntax_error(s_BotServ, u, "BADWORDS", BOT_BADWORDS_SYNTAX);
    }
    return MOD_CONT;
}

int badwords_del_callback(User * u, int num, va_list args)
{
    BadWord *bw;
    ChannelInfo *ci = va_arg(args, ChannelInfo *);
    int *last = va_arg(args, int *);

    *last = num;

    if (num < 1 || num > ci->bwcount)
        return 0;

    bw = &ci->badwords[num - 1];
    if (bw->word)
        free(bw->word);
    bw->word = NULL;
    bw->in_use = 0;

    return 1;
}

int badwords_list(User * u, int index, ChannelInfo * ci, int *sent_header)
{
    BadWord *bw = &ci->badwords[index];

    if (!bw->in_use)
        return 0;
    if (!*sent_header) {
        notice_lang(s_BotServ, u, BOT_BADWORDS_LIST_HEADER, ci->name);
        *sent_header = 1;
    }

    notice_lang(s_BotServ, u, BOT_BADWORDS_LIST_FORMAT, index + 1,
                bw->word,
                ((bw->type ==
                  BW_SINGLE) ? "(SINGLE)" : ((bw->type ==
                                              BW_START) ? "(START)"
                                             : ((bw->type ==
                                                 BW_END) ? "(END)" : "")))
        );
    return 1;
}

int badwords_list_callback(User * u, int num, va_list args)
{
    ChannelInfo *ci = va_arg(args, ChannelInfo *);
    int *sent_header = va_arg(args, int *);
    if (num < 1 || num > ci->bwcount)
        return 0;
    return badwords_list(u, num - 1, ci, sent_header);
}
