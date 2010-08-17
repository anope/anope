/* ns_maxemail.c - Limit the amount of times an email address
 *                 can be used for a NickServ account.
 *
 * (C) 2003-2010 Anope Team
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

void my_load_config();
void my_add_languages();
bool check_email_limit_reached(const Anope::string &email, User *u);

int NSEmailMax = 0;

enum
{
	LNG_NSEMAILMAX_REACHED,
	LNG_NSEMAILMAX_REACHED_ONE,
	LNG_NUM_STRINGS
};

static Module *me;

class NSMaxEmail : public Module
{
 public:
	NSMaxEmail(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		me = this;

		this->SetAuthor(AUTHOR);
		this->SetType(SUPPORTED);

		ModuleManager::Attach(I_OnReload, this);
		ModuleManager::Attach(I_OnPreCommand, this);

		my_load_config();

		const char *langtable_en_us[] = {
			/* LNG_NSEMAILMAX_REACHED */
			"The given email address has reached it's usage limit of %d users.",
			/* LNG_NSEMAILMAX_REACHED_ONE */
			"The given email address has reached it's usage limit of 1 user."
		};

		const char *langtable_nl[] = {
			/* LNG_NSEMAILMAX_REACHED */
			"Het gegeven email adres heeft de limiet van %d gebruikers bereikt.",
			/* LNG_NSEMAILMAX_REACHED_ONE */
			"Het gegeven email adres heeft de limiet van 1 gebruiker bereikt."
		};

		const char *langtable_de[] = {
			/* LNG_NSEMAILMAX_REACHED */
			"Die angegebene eMail hat die limit Begrenzung von %d User erreicht.",
			/* LNG_NSEMAILMAX_REACHED_ONE */
			"Die angegebene eMail hat die limit Begrenzung von 1 User erreicht."
		};

		const char *langtable_pt[] = {
			/* LNG_NSEMAILMAX_REACHED */
			"O endereço de email fornecido alcançou seu limite de uso de %d usuários.",
			/* LNG_NSEMAILMAX_REACHED_ONE */
			"O endereço de email fornecido alcançou seu limite de uso de 1 usuário."
		};

		const char *langtable_ru[] = {
			/* LNG_NSEMAILMAX_REACHED */
			"Óêàçàííûé âàìè email-àäðåñ èñïîëüçóåòñÿ ìàêñèìàëüíî äîïóñòèìîå êîë-âî ðàç: %d",
			/* LNG_NSEMAILMAX_REACHED_ONE */
			"Óêàçàííûé âàìè email-àäðåñ óæå êåì-òî èñïîëüçóåòñÿ."
		};

		const char *langtable_it[] = {
			/* LNG_NSEMAILMAX_REACHED */
			"L'indirizzo email specificato ha raggiunto il suo limite d'utilizzo di %d utenti.",
			/* LNG_NSEMAILMAX_REACHED_ONE */
			"L'indirizzo email specificato ha raggiunto il suo limite d'utilizzo di 1 utente."
		};

		this->InsertLanguage(LANG_EN_US, LNG_NUM_STRINGS, langtable_en_us);
		this->InsertLanguage(LANG_NL, LNG_NUM_STRINGS, langtable_nl);
		this->InsertLanguage(LANG_DE, LNG_NUM_STRINGS, langtable_de);
		this->InsertLanguage(LANG_PT, LNG_NUM_STRINGS, langtable_pt);
		this->InsertLanguage(LANG_RU, LNG_NUM_STRINGS, langtable_ru);
		this->InsertLanguage(LANG_IT, LNG_NUM_STRINGS, langtable_it);
	}

	void OnReload(bool started)
	{
		my_load_config();
	}

	EventReturn OnPreCommand(User *u, BotInfo *service, const Anope::string &command, const std::vector<Anope::string> &params)
	{
		if (service == findbot(Config->s_NickServ))
		{
			if (command.equals_ci("REGISTER"))
			{
				if (check_email_limit_reached(params.size() > 1 ? params[1] : "", u))
					return EVENT_STOP;
			}
			else if (command.equals_ci("SET"))
			{
				Anope::string set = params[0];
				Anope::string email = params.size() > 1 ? params[1] : "";

				if (set.equals_ci("email") && check_email_limit_reached(email, u))
					return EVENT_STOP;
			}
		}

		return EVENT_CONTINUE;
	}
};

int count_email_in_use(const Anope::string &email, User *u)
{
	int count = 0;

	if (email.empty())
		return 0;

	for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
	{
		NickCore *nc = it->second;

		if (!(u->Account() && u->Account() == nc) && !nc->email.empty() && nc->email.equals_ci(email))
			++count;
	}

	return count;
}

bool check_email_limit_reached(const Anope::string &email, User *u)
{
	if (NSEmailMax < 1 || email.empty())
		return false;

	if (count_email_in_use(email, u) < NSEmailMax)
		return false;

	if (NSEmailMax == 1)
		me->NoticeLang(Config->s_NickServ, u, LNG_NSEMAILMAX_REACHED_ONE);
	else
		me->NoticeLang(Config->s_NickServ, u, LNG_NSEMAILMAX_REACHED, NSEmailMax);

	return true;
}

void my_load_config()
{
	ConfigReader config;
	NSEmailMax = config.ReadInteger("ns_maxemail", "maxemails", "0", 0, false);
	Alog(LOG_DEBUG) << "[ns_maxemail] NSEmailMax set to " << NSEmailMax;
}

MODULE_INIT(NSMaxEmail)
