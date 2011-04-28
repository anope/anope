/* ns_maxemail.c - Limit the amount of times an email address
 *                 can be used for a NickServ account.
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

void my_load_config(void);
void my_add_languages(void);
int my_ns_register(User * u);
int my_ns_set(User * u);
int my_event_reload(int argc, char **argv);
int my_event_addcommand(int argc, char **argv);
int my_event_delcommand(int argc, char **argv);

int NSEmailMax = 0;
int added_register = 0;

#define LNG_NUM_STRINGS		2
#define LNG_NSEMAILMAX_REACHED		0
#define LNG_NSEMAILMAX_REACHED_ONE	1

int AnopeInit(int argc, char **argv)
{
    Command *c;
    EvtHook *evt;
    int status;

    moduleAddAuthor(AUTHOR);
    moduleAddVersion(VERSION);
    moduleSetType(SUPPORTED);

    /* Only add the command if REGISTER is actually available.
     * If it s not available, hooking to it will suppress anopes
     * "Unknown Command" response.. ~ Viper */
    if (findCommand(NICKSERV, "REGISTER")) {
        c = createCommand("REGISTER", my_ns_register, NULL, -1, -1, -1, -1,
                          -1);
        if ((status = moduleAddCommand(NICKSERV, c, MOD_HEAD))) {
            alog("[ns_maxemail] Unable to create REGISTER command: %d",
                 status);
            return MOD_STOP;
        }
        added_register = 1;
    }

    c = createCommand("SET", my_ns_set, NULL, -1, -1, -1, -1, -1);
    if ((status = moduleAddCommand(NICKSERV, c, MOD_HEAD))) {
        alog("[ns_maxemail] Unable to create SET command: %d", status);
        return MOD_STOP;
    }

    evt = createEventHook(EVENT_RELOAD, my_event_reload);
    if ((status = moduleAddEventHook(evt))) {
        alog("[ns_maxemail] Unable to hook to EVENT_RELOAD: %d", status);
        return MOD_STOP;
    }

    /* If the REGISTER command is added after initial load, provide hooks.. */
    evt = createEventHook(EVENT_ADDCOMMAND, my_event_addcommand);
    if ((status = moduleAddEventHook(evt))) {
        alog("[ns_maxemail] Unable to hook to EVENT_ADDCOMMAND: %d", status);
        return MOD_STOP;
    }

    /* If the REGISTER command is deleted after initial load, remove hooks.. */
    evt = createEventHook(EVENT_DELCOMMAND, my_event_delcommand);
    if ((status = moduleAddEventHook(evt))) {
        alog("[ns_maxemail] Unable to hook to EVENT_DELCOMMAND: %d", status);
        return MOD_STOP;
    }

    my_load_config();
    my_add_languages();

    return MOD_CONT;
}

void AnopeFini(void)
{
    /* Nothing to do while unloading */
}

int count_email_in_use(char *email, User * u)
{
    NickCore *nc;
    int i;
    int count = 0;

    if (!email)
        return 0;

    for (i = 0; i < 1024; i++) {
        for (nc = nclists[i]; nc; nc = nc->next) {
            if (!(u->na && u->na->nc && (u->na->nc == nc)) && nc->email && (stricmp(nc->email, email) == 0))
                count++;
        }
    }

    return count;
}

int check_email_limit_reached(char *email, User * u)
{
    if ((NSEmailMax < 1) || !email || is_services_admin(u))
        return MOD_CONT;

    if (count_email_in_use(email, u) < NSEmailMax)
        return MOD_CONT;

    if (NSEmailMax == 1)
        moduleNoticeLang(s_NickServ, u, LNG_NSEMAILMAX_REACHED_ONE);
    else
        moduleNoticeLang(s_NickServ, u, LNG_NSEMAILMAX_REACHED,
                         NSEmailMax);

    return MOD_STOP;
}

int my_ns_register(User * u)
{
    char *cur_buffer;
    char *email;
    int ret;

    cur_buffer = moduleGetLastBuffer();
    email = myStrGetToken(cur_buffer, ' ', 1);
    if (!email)
        return MOD_CONT;

    ret = check_email_limit_reached(email, u);
    free(email);

    return ret;
}

int my_ns_set(User * u)
{
    char *cur_buffer;
    char *set;
    char *email;
    int ret;

    cur_buffer = moduleGetLastBuffer();
    set = myStrGetToken(cur_buffer, ' ', 0);

    if (!set)
        return MOD_CONT;

    if (stricmp(set, "email") != 0) {
        free(set);
        return MOD_CONT;
    }

    free(set);
    email = myStrGetToken(cur_buffer, ' ', 1);
    if (!email)
        return MOD_CONT;

    ret = check_email_limit_reached(email, u);
    free(email);

    return ret;
}

int my_event_reload(int argc, char **argv)
{
    if ((argc > 0) && (stricmp(argv[0], EVENT_START) == 0))
        my_load_config();

    return MOD_CONT;
}

int my_event_addcommand(int argc, char **argv)
{
    Command *c;
    int status;

    if (argc == 2 && stricmp(argv[0], "ns_maxemail")
            && !stricmp(argv[1], "REGISTER") && !added_register) {
        c = createCommand("REGISTER", my_ns_register, NULL, -1, -1, -1, -1,
                          -1);
        if ((status = moduleAddCommand(NICKSERV, c, MOD_HEAD))) {
            alog("[ns_maxemail] Unable to create REGISTER command: %d",
                 status);
            return MOD_CONT;
        }
        added_register = 1;
    }

    return MOD_CONT;
}

int my_event_delcommand(int argc, char **argv)
{
    if (argc == 2 && stricmp(argv[0], "ns_maxemail")
            && !stricmp(argv[1], "REGISTER") && added_register) {
        moduleDelCommand(NICKSERV, "REGISTER");
        added_register = 0;
    }

    return MOD_CONT;
}

void my_load_config(void)
{
    Directive confvalues[] = {
        {"NSEmailMax", {{PARAM_INT, PARAM_RELOAD, &NSEmailMax}}}
    };

    moduleGetConfigDirective(confvalues);

    if (debug)
        alog("debug: [ns_maxemail] NSEmailMax set to %d", NSEmailMax);
}

void my_add_languages(void)
{
    char *langtable_en_us[] = {
        /* LNG_NSEMAILMAX_REACHED */
        "The given email address has reached its usage limit of %d users.",
        /* LNG_NSEMAILMAX_REACHED_ONE */
        "The given email address has reached its usage limit of 1 user."
    };

    char *langtable_nl[] = {
        /* LNG_NSEMAILMAX_REACHED */
        "Het gegeven email adres heeft de limiet van %d gebruikers bereikt.",
        /* LNG_NSEMAILMAX_REACHED_ONE */
        "Het gegeven email adres heeft de limiet van 1 gebruiker bereikt."
    };

   char *langtable_de[] = {
        /* LNG_NSEMAILMAX_REACHED */
        "Die angegebene eMail hat die limit Begrenzung von %d User erreicht.",
        /* LNG_NSEMAILMAX_REACHED_ONE */
        "Die angegebene eMail hat die limit Begrenzung von 1 User erreicht."
   };

    char *langtable_pt[] = {
        /* LNG_NSEMAILMAX_REACHED */
        "O endereзo de email fornecido alcanзou seu limite de uso de %d usuбrios.",
        /* LNG_NSEMAILMAX_REACHED_ONE */
        "O endereзo de email fornecido alcanзou seu limite de uso de 1 usuбrio."
    };

    char *langtable_ru[] = {
        /* LNG_NSEMAILMAX_REACHED */
        "Указанный вами email-адрес используется максимально допустимое кол-во раз: %d",
        /* LNG_NSEMAILMAX_REACHED_ONE */
        "Указанный вами email-адрес уже кем-то используется."
    };

    char *langtable_it[] = {
        /* LNG_NSEMAILMAX_REACHED */
        "L'indirizzo email specificato ha raggiunto il suo limite d'utilizzo di %d utenti.",
        /* LNG_NSEMAILMAX_REACHED_ONE */
        "L'indirizzo email specificato ha raggiunto il suo limite d'utilizzo di 1 utente."
    };

    moduleInsertLanguage(LANG_EN_US, LNG_NUM_STRINGS, langtable_en_us);
    moduleInsertLanguage(LANG_NL, LNG_NUM_STRINGS, langtable_nl);
    moduleInsertLanguage(LANG_DE, LNG_NUM_STRINGS, langtable_de);
    moduleInsertLanguage(LANG_PT, LNG_NUM_STRINGS, langtable_pt);
    moduleInsertLanguage(LANG_RU, LNG_NUM_STRINGS, langtable_ru);
    moduleInsertLanguage(LANG_IT, LNG_NUM_STRINGS, langtable_it);
}

/* EOF */
