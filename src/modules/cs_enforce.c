/* cs_enforce - Add a /cs ENFORCE command to enforce various set
 *              options and channelmodes on a channel.
 * 
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 * 
 * Included in the Anope module pack since Anope 1.7.9
 * Anope Coder: GeniusDex <geniusdex@anope.org>
 * 
 * Please read COPYING and README for further details.
 *
 * Send any bug reports to the Anope Coder, as he will be able
 * to deal with it best.
 */

#include "module.h"

#define AUTHOR "Anope"
#define VERSION VERSION_STRING

int my_cs_enforce(User * u);
void my_cs_help(User * u);
int my_cs_help_enforce(User * u);
void my_add_languages(void);

#define LNG_NUM_STRINGS    6

#define LNG_CHAN_HELP                           0
#define LNG_ENFORCE_SYNTAX                      1
#define LNG_CHAN_HELP_ENFORCE                   2
#define LNG_CHAN_HELP_ENFORCE_R_ENABLED         3
#define LNG_CHAN_HELP_ENFORCE_R_DISABLED        4
#define LNG_CHAN_RESPONSE                       5

int AnopeInit(int argc, char **argv)
{
    Command *c;
    int status;

    moduleAddAuthor(AUTHOR);
    moduleAddVersion(VERSION);
    moduleSetType(SUPPORTED);

    c = createCommand("ENFORCE", my_cs_enforce, NULL, -1, -1, -1, -1, -1);
    if ((status = moduleAddCommand(CHANSERV, c, MOD_HEAD))) {
        alog("[cs_enforce] Unable to create ENFORCE command: %d", status);
        return MOD_STOP;
    }

    moduleAddHelp(c, my_cs_help_enforce);
    moduleSetChanHelp(my_cs_help);

    my_add_languages();

    return MOD_CONT;
}

void AnopeFini(void)
{
    /* Nothing to clean up */
}

/* Enforcing functions */
void do_enforce_secureops(Channel * c)
{
    struct c_userlist *user;
    struct c_userlist *next;
    ChannelInfo *ci;
    uint32 flags;

    if (!(ci = c->ci))
        return;

    if (debug)
        alog("debug: cs_enforce: Enforcing SECUREOPS on %s", c->name);

    /* Dirty hack to allow chan_set_correct_modes to work ok.
     * We pretend like SECUREOPS is on so it doesn't ignore that
     * part of the code. This way we can enforce SECUREOPS even
     * if it's off.
     */
    flags = ci->flags;
    ci->flags |= CI_SECUREOPS;

    user = c->users;
    do {
        next = user->next;
        chan_set_correct_modes(user->user, c, 0);
        user = next;
    } while (user);

    ci->flags = flags;
}

void do_enforce_restricted(Channel * c)
{
    struct c_userlist *user;
    struct c_userlist *next;
    ChannelInfo *ci;
    int16 old_nojoin_level;
    char mask[BUFSIZE];
    char *reason;
    char *av[3];
    User *u;

    if (!(ci = c->ci))
        return;

    if (debug)
        alog("debug: cs_enforce: Enforcing RESTRICTED on %s", c->name);

    old_nojoin_level = ci->levels[CA_NOJOIN];
    if (ci->levels[CA_NOJOIN] < 0)
        ci->levels[CA_NOJOIN] = 0;

    user = c->users;
    do {
        next = user->next;
        u = user->user;
        if (check_access(u, c->ci, CA_NOJOIN)) {
            get_idealban(ci, u, mask, sizeof(mask));
            reason = getstring(u->na, CHAN_NOT_ALLOWED_TO_JOIN);
            anope_cmd_mode(whosends(ci), ci->name, "+b %s %lu", mask,
                           time(NULL));
            anope_cmd_kick(whosends(ci), ci->name, u->nick, "%s", reason);
            av[0] = ci->name;
            av[1] = u->nick;
            av[2] = reason;
            do_kick(s_ChanServ, 3, av);
        }
        user = next;
    } while (user);

    ci->levels[CA_NOJOIN] = old_nojoin_level;
}

void do_enforce_cmode_R(Channel * c)
{
    struct c_userlist *user;
    struct c_userlist *next;
    ChannelInfo *ci;
    char mask[BUFSIZE];
    char *reason;
    char *av[3];
    User *u;
    CBMode *cbm;

    if (!(ci = c->ci))
        return;

    if (debug)
        alog("debug: cs_enforce: Enforcing mode +R on %s", c->name);

    user = c->users;
    do {
        next = user->next;
        u = user->user;
        if (!nick_identified(u)) {
            get_idealban(ci, u, mask, sizeof(mask));
            reason = getstring(u->na, CHAN_NOT_ALLOWED_TO_JOIN);
            if (((cbm = &cbmodes['R'])->flag == 0)
                || !(c->mode & cbm->flag))
                anope_cmd_mode(whosends(ci), ci->name, "+b %s %lu", mask,
                               time(NULL));
            anope_cmd_kick(whosends(ci), ci->name, u->nick, "%s", reason);
            av[0] = ci->name;
            av[1] = u->nick;
            av[2] = reason;
            do_kick(s_ChanServ, 3, av);
        }
        user = next;
    } while (user);
}

/* Enforcing Group Functions */
void do_enforce_set(Channel * c)
{
    ChannelInfo *ci;

    if (!(ci = c->ci))
        return;

    if (ci->flags & CI_SECUREOPS)
        do_enforce_secureops(c);
    if (ci->flags & CI_RESTRICTED)
        do_enforce_restricted(c);
}

void do_enforce_modes(Channel * c)
{
    CBMode *cbm;

    if (((cbm = &cbmodes['R'])->flag != 0) && (c->mode & cbm->flag))
        do_enforce_cmode_R(c);
}

/* End of enforcing functions */

int my_cs_enforce(User * u)
{
    char *cur_buffer;
    char *chan=NULL;
    char *what=NULL;
    Channel *c;
    ChannelInfo *ci;

    cur_buffer = moduleGetLastBuffer();
    chan = myStrGetToken(cur_buffer, ' ', 0);

    if (!chan) {
        moduleNoticeLang(s_ChanServ, u, LNG_ENFORCE_SYNTAX);
    } else if (!(c = findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, ci->name);
    } else if (!is_services_admin(u) && !check_access(u, ci, CA_AKICK)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else {
        what = myStrGetToken(cur_buffer, ' ', 1);
        if (!what || (stricmp(what, "SET") == 0)) {
            do_enforce_set(c);
            moduleNoticeLang(s_ChanServ,u,LNG_CHAN_RESPONSE, what ? what : "SET");
        } else if (stricmp(what, "MODES") == 0) {
            do_enforce_modes(c);
            moduleNoticeLang(s_ChanServ,u,LNG_CHAN_RESPONSE,what);
        } else if (stricmp(what, "SECUREOPS") == 0) {
            do_enforce_secureops(c);
            moduleNoticeLang(s_ChanServ,u,LNG_CHAN_RESPONSE,what);
        } else if (stricmp(what, "RESTRICTED") == 0) {
            do_enforce_restricted(c);
            moduleNoticeLang(s_ChanServ,u,LNG_CHAN_RESPONSE,what);
        } else if (stricmp(what, "+R") == 0) {
            do_enforce_cmode_R(c);
            moduleNoticeLang(s_ChanServ,u,LNG_CHAN_RESPONSE,what);
        } else {
            moduleNoticeLang(s_ChanServ, u, LNG_ENFORCE_SYNTAX);
        }
    }

    if(chan) free(chan);
    if(what) free(what);

    return MOD_CONT;
}

/* Language and response stuff */
void my_cs_help(User * u)
{
    moduleNoticeLang(s_ChanServ, u, LNG_CHAN_HELP);
}

int my_cs_help_enforce(User * u)
{
    moduleNoticeLang(s_ChanServ, u, LNG_ENFORCE_SYNTAX);
    notice(s_ChanServ, u->nick, " ");
    moduleNoticeLang(s_ChanServ, u, LNG_CHAN_HELP_ENFORCE);
    notice(s_ChanServ, u->nick, " ");
    if (cbmodes['R'].flag != 0)
        moduleNoticeLang(s_ChanServ, u, LNG_CHAN_HELP_ENFORCE_R_ENABLED);
    else
        moduleNoticeLang(s_ChanServ, u, LNG_CHAN_HELP_ENFORCE_R_DISABLED);

    return MOD_STOP;
}

void my_add_languages(void)
{
    /* English (US) */
    char *langtable_en_us[] = {
        /* LNG_CHAN_HELP */
        "    ENFORCE    Enforce various channel modes and set options",
        /* LNG_ENFORCE_SYNTAX */
        "Syntax: \002ENFORCE \037channel\037 [\037what\037]\002",
        /* LNG_CHAN_HELP_ENFORCE */
        "Enforce various channel modes and set options. The \037channel\037\n"
            "option indicates what channel to enforce the modes and options\n"
            "on. The \037what\037 option indicates what modes and options to\n"
            "enforce, and can be any of SET, SECUREOPS, RESTRICTED, MODES,\n"
            "or +R. When left out, it defaults to SET.\n"
            " \n"
            "If \037what\037 is SET, it will enforce SECUREOPS and RESTRICTED\n"
            "on the users currently in the channel, if they are set. Give\n"
            "SECUREOPS to enforce the SECUREOPS option, even if it is not\n"
            "enabled. Use RESTRICTED to enfore the RESTRICTED option, also\n"
            "if it's not enabled.",
        /* LNG_CHAN_HELP_ENFORCE_R_ENABLED */
        "If \037what\037 is MODES, it will enforce channelmode +R if it is\n"
            "set. If +R is specified for \037what\037, the +R channelmode will\n"
            "also be enforced, but even if it is not set. If it is not set,\n"
            "users will be banned to ensure they don't just rejoin.",
        /* LNG_CHAN_HELP_ENFORCE_R_DISABLED */
        "If \037what\037 is MODES, nothing will be enforced, since it would\n"
            "enforce modes that the current ircd does not support. If +R is\n"
            "specified for \037what\037, an equalivant of channelmode +R on\n"
            "other ircds will be enforced. All users that are in the channel\n"
            "but have not identified for their nickname will be kicked and\n"
            "banned from the channel.",
        /* LNG_CHAN_RESPONSE */
        "Enforced %s"
    };

    /* Dutch (NL) */
    char *langtable_nl[] = {
        /* LNG_CHAN_HELP */
        "    ENFORCE    Forceer enkele kanaalmodes en set-opties",
        /* LNG_ENFORCE_SYNTAX */
        "Syntax: \002ENFORCE \037kanaal\037 [\037wat\037]\002",
        /* LNG_CHAN_HELP_ENFORCE */
        "Forceer enkele kannalmodes en set-opties. De \037kanaal\037 optie\n"
            "geeft aan op welk kanaal de modes en opties geforceerd moeten\n"
            "worden. De \037wat\037 optie geeft aan welke modes en opties\n"
            "geforceerd moeten worden; dit kan SET, SECUREOPS, RESTRICTED,\n"
            "MODES, of +R zijn. Indien weggelaten is dit standaard SET.\n"
            " \n"
            "Als er voor \037wat\037 SET wordt ingevuld, zullen SECUREOPS en\n"
            "RESTRICTED geforceerd worden op de gebruikers in het kanaal,\n"
            "maar alleen als die opties aangezet zijn voor het kanaal. Als\n"
            "SECUREOPS of RESTRICTED wordt gegeven voor \037wat\037 zal die optie\n"
            "altijd geforceerd worden, ook als die niet is aangezet.",
        /* LNG_CHAN_HELP_ENFORCE_R_ENABLED */
        "Als er voor \037wat\037 MODES wordt ingevuld, zal kanaalmode +R worden\n"
            "geforceerd, als die op het kanaal aan staat. Als +R wordt ingevuld,\n"
            "zal kanaalmode +R worden geforceerd, maar ook als die niet aan"
            "staat voor het kanaal. Als +R niet aan staat, zullen alle ook\n"
            "gebanned worden om te zorgen dat ze niet opnieuw het kanaal binnen\n"
            "kunnen komen.",
        /* LNG_CHAN_HELP_ENFORCE_R_DISABLED */
        "Als er voor \037wat\037 MODES wordt ingevuld, zal er niks gebeuren.\n"
            "Normaal gesproken wordt er een kanaalmode geforceerd die op deze\n"
            "server niet ondersteund wordt. Als +R wordt ingevuld voor \037wat\037\n"
            "zullen alle gebruikers die in het kanaal zitten maar zich niet\n"
            "hebben geidentificeerd voor hun nick uit het kanaal gekicked en\n"
            "verbannen worden.",
        /* LNG_CHAN_RESPONSE */
        "Enforced %s"
    };

   /* German (DE) */
   char *langtable_de[] = {
        /* LNG_CHAN_HELP */
        "    ENFORCE   Erzwingt verschieden Modes und SET Optionen",
        /* LNG_ENFORCE_SYNTAX */
        "Syntax: \002ENFORCE \037Channel\037 [\037was\037]\002",
        /* LNG_CHAN_HELP_ENFORCE */
        "Erzwingt verschieden Modes und SET Optionen. Die \037Channel\037\n"
            "Option zeigt dir den Channel an, indem Modes und Optionen\n"
            "zu erzwingen sind. Die \037was\037 Option zeigt dir welche Modes\n"
            "und Optionen zu erzwingen sind. Die kцnnen nur SET, SECUREOPS,\n"
            "RESTRICTED, MODES oder +R sein.Default ist SET.\n"
            " \n"
            "Wenn \037was\037 SET ist, wird SECUREOPS und RESTRICTED\n"
            "auf die User die z.Z.in Channel sind erzwungen, wenn sie AN sind.\n"
            "Benutze SECUREOPS oder RESTRICTED , um die Optionen einzeln\n"
            "zu erzwingen, also wenn sie nicht eingeschaltet sind.",
        /* LNG_CHAN_HELP_ENFORCE_R_ENABLED */
        "Wenn \037was\037 MODES ist, wird das ChannelMode +R erzwungen\n"
            "falls an. Wenn \037was\037 +R ist, wird +R erzwungen aber eben\n"
            "wenn noch nicht als Channel-Mode ist. Wenn +R noch nicht als\n"
            "Channel-Mode war werden alle User aus den Channel gebannt um\n"
            "sicher zu sein das sie nicht rejoinen.",
        /* LNG_CHAN_HELP_ENFORCE_R_DISABLED */
        "Wenn \037was\037 MODES ist, wird nichts erzwungen weil es MODES seine\n"
            "kцnnen die dein IRCD nicht unterstьtzt. Wenn \037was\037 +R ist\n"
            "oder ein Modes was auf ein anderen IRCD gleich +R ist, wird es\n"
            "erzwungen. Alle User die nicht fьr deren Nicknamen identifiziert\n"
            "sind werden aus den Channel gekickt und gebannt.",
        /* LNG_CHAN_RESPONSE */
        "Erzwungen %s"
    };

    /* Portuguese (PT) */
    char *langtable_pt[] = {
        /* LNG_CHAN_HELP */
        "    ENFORCE    Verifica o cumprimento de vбrios modos de canal e opзхes ajustadas",
        /* LNG_ENFORCE_SYNTAX */
        "Sintaxe: \002ENFORCE \037canal\037 [\037opзгo\037]\002",
        /* LNG_CHAN_HELP_ENFORCE */
        "Verifica o cumprimento de vбrios modos de canal e opзхes ajustadas.\n"
            "O campo \037canal\037 indica qual canal deve ter os modos e opзхes verificadas\n"
            "O campo \037opзгo\037 indica quais modos e opзхes devem ser verificadas,\n"
            "e pode ser: SET, SECUREOPS, RESTRICTED, MODES ou +R\n"
            "Quando deixado em branco, o padrгo й SET.\n"
            " \n"
            "Se \037opзгo\037 for SET, serгo verificadas as opзхes SECUREOPS e RESTRICTED\n"
            "para usuбrios que estiverem no canal, caso elas estejam ativadas. Use\n"
            "SECUREOPS para verificar a opзгo SECUREOPS, mesmo que ela nгo esteja ativada\n"
            "Use RESTRICTED para verificar a opзгo RESTRICTED, mesmo que ela nгo esteja\n"
            "ativada.",
        /* LNG_CHAN_HELP_ENFORCE_R_ENABLED */
        "Se \037opзгo\037 for MODES, serб verificado o modo de canal +R caso ele\n"
            "esteja ativado. Se +R for especificado para \037opзгo\037, o modo de canal\n"
            "+R tambйm serб verificado, mesmo que ele nгo esteja ativado. Se ele nгo\n"
            "estiver ativado, os usuбrios serгo banidos para evitar que reentrem no canal.",
        /* LNG_CHAN_HELP_ENFORCE_R_DISABLED */
        "Se \037opзгo\037 for MODES, nada serб verificado, visto que isto poderia\n"
            "verificar modos que o IRCd atual nгo suporta. Se +R for especificado\n"
            "para \037opзгo\037, um equivalente ao modo de canal +R em outros IRCds\n"
            "serб verificado. Todos os usuбrios que estгo no canal, mas nгo estejam\n"
            "identificados para seus nicks serгo kickados e banidos do canal.",
        /* LNG_CHAN_RESPONSE */
        "Verificado %s"
    };

    /* Russian (RU) */
    char *langtable_ru[] = {
        /* LNG_CHAN_HELP */
        "    ENFORCE    Перепроверка и установка различных режимов и опций канала",
        /* LNG_ENFORCE_SYNTAX */
        "Синтаксис: \002ENFORCE \037#канал\037 \037параметр\037\002",
        /* LNG_CHAN_HELP_ENFORCE */
        "Перепроверка и установка различных режимов и опций канала.\n"
            "\037Параметр\037 указывает какие опции или режимы канала должны быть\n"
            "перепроверены. В качестве параметра могут быть указаны: SET, SECUREOPS,\n"
            "RESTRICTED, MODES, или +R. Если параметр не указан, по-умолчанию будет SET.\n"
            " \n"
            "Если в качестве \037параметра\037 указано SET, будут перепроверены опции\n"
            "SECUREOPS и RESTRICTED относительно пользователей на указанном канале\n"
            "(при условии, что опции включены). Отдельно указанный параметр SECUREOPS\n"
            "применит опцию SECUREOPS (даже если она \037НЕ\037 установлена). Параметр\n"
            "RESTRICTED применит опцию RESTRICTED (даже если она \037НЕ\037 установлена)",
        /* LNG_CHAN_HELP_ENFORCE_R_ENABLED */
        "Если в качестве \037параметра\037 указано MODES, будет перепроверен режим +R\n"
            "(если он установлен). Отдельно указанный параметр \037+R\037 применит\n"
            "канальный режим +R, даже если он не установлен, и забанит всех пользователей,\n"
            "которые не идентифицировались к своему нику или не имеют зарегистрированного ника.",
        /* LNG_CHAN_HELP_ENFORCE_R_DISABLED */
        "Если в качестве \037параметра\037 указано MODES, перепроверка осуществлена\n"
            "НЕ БУДЕТ, так как текущий IRCD не поддерживает необходимые режимы.\n"
            "Отдельно указанный параметр \037+R\037 применит канальный режим, эквивалентный\n"
            "режиму +R и забанит всех пользователей, которые не идентифицировались к своему\n"
            "нику или не имеют зарегистрированного ника.",
        /* LNG_CHAN_RESPONSE */
        "Перепроверено: %s"
    };

    /* Italian (IT) */
    char *langtable_it[] = {
        /* LNG_CHAN_HELP */
        "    ENFORCE    Forza diversi modi di canale ed opzioni SET",
        /* LNG_ENFORCE_SYNTAX */
        "Sintassi: \002ENFORCE \037canale\037 [\037cosa\037]\002",
        /* LNG_CHAN_HELP_ENFORCE */
        "Forza diversi modi di canale ed opzioni SET. Il parametro \037canale\037\n"
            "indica il canale sul quale forzare i modi e le opzioni. Il parametro\n"
            "\037cosa\037 indica i modi e le opzioni da forzare, e possono essere\n"
            "qualsiasi delle opzioni SET, SECUREOPS, RESTRICTED, MODES, o +R.\n"
            "Se non specificato, viene sottointeso SET.\n"
            " \n"
            "Se \037cosa\037 и SET, forzerа SECUREOPS e RESTRICTED sugli utenti\n"
            "attualmente nel canale, se sono impostati. Specifica SECUREOPS per\n"
            "forzare l'opzione SECUREOPS, anche se non и attivata. Specifica\n"
            "RESTRICTED per forzare l'opzione RESTRICTED, anche se non и\n"
            "attivata.",
        /* LNG_CHAN_HELP_ENFORCE_R_ENABLED */
        "Se \037cosa\037 и MODES, forzerа il modo del canale +R se и impostato.\n"
            "Se +R и specificato per \037cosa\037, il modo del canale +R verrа\n"
            "forzato, anche se non и impostato. Se non и impostato, gli utenti\n"
            "verranno bannati per assicurare che non rientrino semplicemente.",
        /* LNG_CHAN_HELP_ENFORCE_R_DISABLED */
        "Se \037cosa\037 и MODES, niente verrа forzato, siccome forzerebbe\n"
            "dei modi che l'ircd in uso non supporterebbe. Se +R и specificato\n"
            "per \037cosa\037, un modo equivalente a +R sui altri ircd verrа\n"
            "forzato. Tutti gli utenti presenti nel canale ma non identificati\n"
            "per il loro nickname verranno bannati ed espulsi dal canale.\n",
        /* LNG_CHAN_RESPONSE */
        "Forzato %s"
    };

    moduleInsertLanguage(LANG_EN_US, LNG_NUM_STRINGS, langtable_en_us);
    moduleInsertLanguage(LANG_NL, LNG_NUM_STRINGS, langtable_nl);
    moduleInsertLanguage(LANG_DE, LNG_NUM_STRINGS, langtable_de);
    moduleInsertLanguage(LANG_PT, LNG_NUM_STRINGS, langtable_pt);
    moduleInsertLanguage(LANG_RU, LNG_NUM_STRINGS, langtable_ru);
	moduleInsertLanguage(LANG_IT, LNG_NUM_STRINGS, langtable_it);
}

/* EOF */
