/* cs_appendtopic.c - Add text to a channels topic
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Based on the original module by SGR <Alex_SGR@ntlworld.com>
 * Included in the Anope module pack since Anope 1.7.9
 * Anope Coder: GeniusDex <geniusdex@anope.org>
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

#define AUTHOR "SGR"
#define VERSION VERSION_STRING

  /* ------------------------------------------------------------
   * Name: cs_appendtopic
   * Author: SGR <Alex_SGR@ntlworld.com>
   * Date: 31/08/2003
   * ------------------------------------------------------------
   * 
   * This module has no configurable options. For information on
   * this module, load it and refer to /ChanServ APPENDTOPIC HELP
   * 
   * Thanks to dengel, Rob and Certus for all there support. 
   * Especially Rob, who always manages to show me where I have 
   * not allocated any memory. Even if it takes a few weeks of
   * pestering to get him to look at it.
   * 
   * ------------------------------------------------------------
   */

/* ---------------------------------------------------------------------- */
/* DO NOT EDIT BELOW THIS LINE UNLESS YOU KNOW WHAT YOU ARE DOING         */
/* ---------------------------------------------------------------------- */

#define LNG_NUM_STRINGS		3

#define LNG_CHAN_HELP				0
#define LNG_CHAN_HELP_APPENDTOPIC	1
#define LNG_APPENDTOPIC_SYNTAX		2

static int my_cs_appendtopic(User * u);
static void my_cs_help(User * u);
static int my_cs_help_appendtopic(User * u);
static void my_add_languages(void);

int AnopeInit(int argc, char **argv)
{
    Command *c;
    int status;

    moduleAddAuthor(AUTHOR);
    moduleAddVersion(VERSION);
    moduleSetType(SUPPORTED);

    c = createCommand("APPENDTOPIC", my_cs_appendtopic, NULL, -1, -1, -1,
                      -1, -1);
    if ((status = moduleAddCommand(CHANSERV, c, MOD_HEAD))) {
        alog("[cs_appendtopic] Unable to create APPENDTOPIC command: %d",
             status);
        return MOD_STOP;
    }
    moduleAddHelp(c, my_cs_help_appendtopic);
    moduleSetChanHelp(my_cs_help);

    my_add_languages();

    alog("[cs_appendtopic] Loaded successfully");

    return MOD_CONT;
}

void AnopeFini(void)
{
    alog("[cs_appendtopic] Unloaded successfully");
}

static void my_cs_help(User * u)
{
    moduleNoticeLang(s_ChanServ, u, LNG_CHAN_HELP);
}

static int my_cs_help_appendtopic(User * u)
{
    moduleNoticeLang(s_ChanServ, u, LNG_APPENDTOPIC_SYNTAX);
    notice(s_ChanServ, u->nick, " ");
    moduleNoticeLang(s_ChanServ, u, LNG_CHAN_HELP_APPENDTOPIC);
    return MOD_STOP;
}

static int my_cs_appendtopic(User * u)
{
    char *cur_buffer;
    char *chan;
    char *newtopic;
    char topic[1024];
    Channel *c;
    ChannelInfo *ci;

    cur_buffer = moduleGetLastBuffer();
    chan = myStrGetToken(cur_buffer, ' ', 0);
    newtopic = myStrGetTokenRemainder(cur_buffer, ' ', 1);

    if (!chan || !newtopic) {
        moduleNoticeLang(s_ChanServ, u, LNG_APPENDTOPIC_SYNTAX);
    } else if (!(c = findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, c->name);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, ci->name);
    } else if (!is_services_admin(u) && !check_access(u, ci, CA_TOPIC)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else {
        if (ci->last_topic) {
            snprintf(topic, sizeof(topic), "%s %s", ci->last_topic,
                     newtopic);
            free(ci->last_topic);
        } else {
            strscpy(topic, newtopic, sizeof(topic));
        }

        ci->last_topic = *topic ? sstrdup(topic) : NULL;
        strscpy(ci->last_topic_setter, u->nick, NICKMAX);
        ci->last_topic_time = time(NULL);

        if (c->topic)
            free(c->topic);
        c->topic = *topic ? sstrdup(topic) : NULL;
        strscpy(c->topic_setter, u->nick, NICKMAX);
        if (ircd->topictsbackward)
            c->topic_time = c->topic_time - 1;
        else
            c->topic_time = ci->last_topic_time;

        if (is_services_admin(u) && !check_access(u, ci, CA_TOPIC))
            alog("%s: %s!%s@%s changed topic of %s as services admin.",
                 s_ChanServ, u->nick, u->username, u->host, c->name);
        if (ircd->join2set) {
            if (whosends(ci) == s_ChanServ) {
                anope_cmd_join(s_ChanServ, c->name, c->creation_time);
                anope_cmd_mode(NULL, c->name, "+o %s", GET_BOT(s_ChanServ));
            }
        }
        anope_cmd_topic(whosends(ci), c->name, u->nick, topic, c->topic_time);
        if (ircd->join2set) {
            if (whosends(ci) == s_ChanServ) {
                anope_cmd_part(s_ChanServ, c->name, NULL);
            }
        }
    }
    Anope_Free(chan);
    Anope_Free(newtopic);
    return MOD_CONT;
}

static void my_add_languages(void)
{
    /* English (US) */
    char *langtable_en_us[] = {
        /* LNG_CHAN_HELP */
        "    APPENDTOPIC   Add text to a channels topic",
        /* LNG_CHAN_HELP_APPENDTOPIC */
        "This command allows users to append text to a currently set\n"
            "channel topic. When TOPICLOCK is on, the topic is updated and\n"
            "the new, updated topic is locked.",
        /* LNG_APPENDTOPIC_SYNTAX */
        "Syntax: APPENDTOPIC channel text\n"
    };

    /* Dutch (NL) */
    char *langtable_nl[] = {
        /* LNG_CHAN_HELP */
        "    APPENDTOPIC   Voeg tekst aan een kanaal onderwerp toe",
        /* LNG_CHAN_HELP_APPENDTOPIC */
        "Dit command stelt gebruikers in staat om text toe te voegen\n"
            "achter het huidige onderwerp van een kanaal. Als TOPICLOCK aan\n"
            "staat, zal het onderwerp worden bijgewerkt en zal het nieuwe,\n"
            "bijgewerkte topic worden geforceerd.",
        /* LNG_APPENDTOPIC_SYNTAX */
        "Gebruik: APPENDTOPIC kanaal tekst\n"
    };

	/* German (DE) */
    char *langtable_de[] = {
        /* LNG_CHAN_HELP */
        "    APPENDTOPIC   F�gt einen Text zu einem Channel-Topic hinzu.",
        /* LNG_CHAN_HELP_APPENDTOPIC */
        "Dieser Befehl erlaubt Benutzern, einen Text zu dem vorhandenen Channel-Topic\n"
            "hinzuzuf�gen. Wenn TOPICLOCK gesetzt ist, wird das Topic aktualisiert\n"
            "und das neue, aktualisierte Topic wird gesperrt.",
        /* LNG_APPENDTOPIC_SYNTAX */
        "Syntax: APPENDTOPIC Channel Text\n"
    };

    /* Portuguese (PT) */
    char *langtable_pt[] = {
        /* LNG_CHAN_HELP */
        "    APPENDTOPIC   Adiciona texto ao t�pico de um canal",
        /* LNG_CHAN_HELP_APPENDTOPIC */
        "Este comando permite aos usu�rios anexar texto a um t�pico de canal\n"
            "j� definido. Quando TOPICLOCK est� ativado, o t�pico � atualizado e\n"
            "o novo t�pico � travado.",
        /* LNG_APPENDTOPIC_SYNTAX */
        "Sintaxe: APPENDTOPIC canal texto\n"
    };

    /* Russian (RU) */
    char *langtable_ru[] = {
        /* LNG_CHAN_HELP */
        "    APPENDTOPIC   ��������� ����� � ������ ������",
        /* LNG_CHAN_HELP_APPENDTOPIC */
        "������ ������� ��������� �������� ����� � ������, ������� ���������� �� ���������\n"
            "������. ���� ����������� ����� TOPICLOCK, ����� ����� �������� � ������������.\n"
            "����������: ����� ����� �������� � ������, �� ���� ������ ����� ������ �� �����.\n",
        /* LNG_APPENDTOPIC_SYNTAX */
        "���������: APPENDTOPIC #����� �����\n"
    };

    /* Italian (IT) */
    char *langtable_it[] = {
        /* LNG_CHAN_HELP */
        "    APPENDTOPIC   Aggiunge del testo al topic di un canale",
        /* LNG_CHAN_HELP_APPENDTOPIC */
        "Questo comando permette agli utenti di aggiungere del testo ad un topic di un canale\n"
            "gi� impostato. Se TOPICLOCK � attivato, il topic viene aggiornato e il nuovo topic\n"
            "viene bloccato.",
        /* LNG_APPENDTOPIC_SYNTAX */
        "Sintassi: APPENDTOPIC canale testo\n"
    };

    /* French (US) */
    char *langtable_fr[] = {
        /* LNG_CHAN_HELP */
        "    APPENDTOPIC   Ajoute du texte dans le sujet d'un salon",
        /* LNG_CHAN_HELP_APPENDTOPIC */
        "Cette commande permet aux utilisateurs d'ajouter du texte � un sujet\n"
            "du salon. Quand TOPICLOCK est actif, le sujet est mis � jour et\n"
            "le nouveau sujet modifi� est v�rrouill�.",
        /* LNG_APPENDTOPIC_SYNTAX */
        "Syntaxe: \002APPENDTOPIC \037canal\037 \037texte\037\002\n"
    };

    moduleInsertLanguage(LANG_EN_US, LNG_NUM_STRINGS, langtable_en_us);
    moduleInsertLanguage(LANG_NL, LNG_NUM_STRINGS, langtable_nl);
    moduleInsertLanguage(LANG_DE, LNG_NUM_STRINGS, langtable_de);
    moduleInsertLanguage(LANG_PT, LNG_NUM_STRINGS, langtable_pt);
    moduleInsertLanguage(LANG_RU, LNG_NUM_STRINGS, langtable_ru);
	moduleInsertLanguage(LANG_IT, LNG_NUM_STRINGS, langtable_it);
	moduleInsertLanguage(LANG_FR, LNG_NUM_STRINGS, langtable_fr);
}

/* EOF */
