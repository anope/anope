/* cs_tban.c - Bans the user for a given length of time
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Based on the original module by Rob <rob@anope.org>
 * Included in the Anope module pack since Anope 1.7.8
 * Anope Coder: Rob <rob@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Send bug reports to the Anope Coder instead of the module
 * author, because any changes since the inclusion into anope
 * are not supported by the original author.
 *
 */
/*************************************************************************/

#include "module.h"

#define AUTHOR "Rob"
#define VERSION VERSION_STRING

void myHelp(User * u);
void myFullHelpSyntax(User * u);
int myFullHelp(User * u);
void mySendResponse(User * u, char *channel, char *mask, char *time);

int do_tban(User * u);
void addBan(Channel * c, time_t timeout, char *banmask);
int delBan(int argc, char **argv);
int canBanUser(Channel * c, User * u, User * u2);

void mAddLanguages(void);

#define LANG_NUM_STRINGS    4
#define TBAN_HELP           0
#define TBAN_SYNTAX         1
#define TBAN_HELP_DETAIL    2
#define TBAN_RESPONSE       3

int AnopeInit(int argc, char **argv)
{
    Command *c;
    int status = 0;

    moduleSetChanHelp(myHelp);
    c = createCommand("TBAN", do_tban, NULL, -1, -1, -1, -1, -1);
    moduleAddHelp(c, myFullHelp);
    status = moduleAddCommand(CHANSERV, c, MOD_HEAD);

    mAddLanguages();

    moduleAddAuthor(AUTHOR);
    moduleAddVersion(VERSION);
    moduleSetType(SUPPORTED);

    if (status != MOD_ERR_OK) {
        return MOD_STOP;
    }
    return MOD_CONT;
}

void AnopeFini(void)
{
    /* module is unloading */
}

void myHelp(User * u)
{
    moduleNoticeLang(s_ChanServ, u, TBAN_HELP);
}

void myFullHelpSyntax(User * u)
{
    moduleNoticeLang(s_ChanServ, u, TBAN_SYNTAX);
}

int myFullHelp(User * u)
{
    myFullHelpSyntax(u);
    notice(s_ChanServ, u->nick, "");
    moduleNoticeLang(s_ChanServ, u, TBAN_HELP_DETAIL);
    return MOD_CONT;
}

void mySendResponse(User * u, char *channel, char *mask, char *time)
{
    moduleNoticeLang(s_ChanServ, u, TBAN_RESPONSE, mask, channel, time);
}

int do_tban(User * u)
{
    char mask[BUFSIZE];
    Channel *c;
    User *u2 = NULL;

    char *buffer = moduleGetLastBuffer();
    char *chan;
    char *nick;
    char *time;

    chan = myStrGetToken(buffer, ' ', 0);
    nick = myStrGetToken(buffer, ' ', 1);
    time = myStrGetToken(buffer, ' ', 2);

    if (time && chan && nick) {

        if (!(c = findchan(chan))) {
            notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
        } else if (!(u2 = finduser(nick))) {
            notice_lang(s_ChanServ, u, NICK_X_NOT_IN_USE, nick);
        } else {
            if (canBanUser(c, u, u2)) {
                get_idealban(c->ci, u2, mask, sizeof(mask));
                addBan(c, dotime(time), mask);
                mySendResponse(u, chan, mask, time);
            }
        }
    } else {
        myFullHelpSyntax(u);
    }
    if (time)
        free(time);
    if (nick)
        free(nick);
    if (chan)
        free(chan);

    return MOD_CONT;
}

void addBan(Channel * c, time_t timeout, char *banmask)
{
    char *av[3];
    char *cb[2];

    cb[0] = c->name;
    cb[1] = banmask;

    av[0] = sstrdup("+b");
    av[1] = banmask;

    anope_cmd_mode(whosends(c->ci), c->name, "+b %s", av[1]);
    chan_set_modes(s_ChanServ, c, 2, av, 1);

    free(av[0]);
    moduleAddCallback("tban", time(NULL) + timeout, delBan, 2, cb);
}

int delBan(int argc, char **argv)
{
    char *av[3];
    Channel *c;

    av[0] = sstrdup("-b");
    av[1] = argv[1];

    if ((c = findchan(argv[0])) && c->ci) {
        anope_cmd_mode(whosends(c->ci), c->name, "-b %s", av[1]);
        chan_set_modes(s_ChanServ, c, 2, av, 1);
    }

    free(av[0]);

    return MOD_CONT;
}

int canBanUser(Channel * c, User * u, User * u2)
{
    ChannelInfo *ci;
    int ok = 0;
    if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, c->name);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, c->name);
    } else if (!check_access(u, ci, CA_BAN)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else if (ircd->except && is_excepted(ci, u2)) {
        notice_lang(s_ChanServ, u, CHAN_EXCEPTED, u2->nick, ci->name);
    } else if (ircd->protectedumode && is_protected(u2)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else {
        ok = 1;
    }

    return ok;
}


void mAddLanguages(void)
{
    char *langtable_en_us[] = {
        "    TBAN       Bans the user for a given length of time",
        "Syntax: TBAN channel nick time",
        "Bans the given user from a channel for a specified length of\n"
            "time. If the ban is removed before by hand, it will NOT be replaced.",
        "%s banned from %s, will auto-expire in %s"
    };

    char *langtable_nl[] = {
        "    TBAN       Verban een gebruiker voor een bepaalde tijd",
        "Syntax: TBAN kanaal nick tijd",
        "Verbant de gegeven gebruiken van het gegeven kanaal voor de\n"
            "gegeven tijdsduur. Als de verbanning eerder wordt verwijderd,\n"
            "zal deze NIET worden vervangen.",
        "%s verbannen van %s, zal verlopen in %s"
    };

    char *langtable_de[] = {
        "    TBAN       Bant ein User fьr eine bestimmte Zeit aus ein Channel",
        "Syntax: TBAN Channel Nickname Zeit",
        "Bant ein User fьr eine bestimmte Zeit aus ein Channel\n"
            "Wenn der Ban manuell entfernt wird, wird es NICHT ersetzt.",
        "%s gebannt von %s, wird auto-auslaufen in %s"
    };

    char *langtable_pt[] = {
        "    TBAN       Bane o usuбrio por um determinado perнodo de tempo",
        "Sintaxe: TBAN canal nick tempo",
        "Bane de um canal o usuбrio especificado por um determinado perнodo de\n"
            "tempo. Se o ban for removido manualmente antes do tempo, ele nгo serб recolocado.",
        "%s foi banido do %s, irб auto-expirar em %s"
    };

    char *langtable_ru[] = {
        "    TBAN       Банит пользователя на указанный промежуток времени",
        "Синтаксис: TBAN #канал ник время",
        "Банит пользователя на указанный промежуток времени в секундах\n"
            "Примечание: удаленный вручную (до своего истечения) бан НЕ БУДЕТ\n"
            "переустановлен сервисами автоматически!",
        "Установленный бан %s на канале %s истечет через %s секунд"
    };

	char *langtable_it[] = {
        "    TBAN       Banna l'utente per un periodo di tempo specificato",
        "Sintassi: TBAN canale nick tempo",
        "Banna l'utente specificato da un canale per un periodo di tempo\n"
            "specificato. Se il ban viene rimosso a mano prima della scadenza, NON verrа rimpiazzato.",
        "%s bannato da %s, scadrа automaticamente tra %s"
    };

    moduleInsertLanguage(LANG_EN_US, LANG_NUM_STRINGS, langtable_en_us);
    moduleInsertLanguage(LANG_NL, LANG_NUM_STRINGS, langtable_nl);
    moduleInsertLanguage(LANG_DE, LANG_NUM_STRINGS, langtable_de);
    moduleInsertLanguage(LANG_PT, LANG_NUM_STRINGS, langtable_pt);
    moduleInsertLanguage(LANG_RU, LANG_NUM_STRINGS, langtable_ru);
	moduleInsertLanguage(LANG_IT, LANG_NUM_STRINGS, langtable_it);

}


/* EOF */
