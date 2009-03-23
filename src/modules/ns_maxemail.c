/* ns_maxemail.c - Limit the amount of times an email address
 *				 can be used for a NickServ account.
 *
 * (C) 2003-2009 Anope Team
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
#define VERSION "$Id$"

void my_load_config();
void my_add_languages();
CommandReturn check_email_limit_reached(const char *email, User * u);

int NSEmailMax = 0;

#define LNG_NUM_STRINGS		2
#define LNG_NSEMAILMAX_REACHED		0
#define LNG_NSEMAILMAX_REACHED_ONE	1

static Module *me;

// XXX: This should probably use an event one day.
class CommandNSRegister : public Command
{
 public:
	CommandNSRegister() : Command("REGISTER", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		return check_email_limit_reached(params.size() > 1 ? params[1].c_str() : NULL, u);
	}

	void OnSyntaxError(User *u)
	{
		// no-op
	}
};

class CommandNSSet : public Command
{
 public:
	CommandNSSet() : Command("SET", 2, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *set = params[0].c_str();
		const char *email = params[1].c_str();

		if (!stricmp(set, "email"))
			return MOD_CONT;

		return check_email_limit_reached(email, u);
	}

	void OnSyntaxError(User *u)
	{
		// no-op
	}
};

class NSMaxEmail : public Module
{
 public:
	NSMaxEmail(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		me = this;

		this->SetAuthor(AUTHOR);
		this->SetVersion(VERSION);
		this->SetType(SUPPORTED);

		this->AddCommand(NICKSERV, new CommandNSRegister(), MOD_HEAD);
		this->AddCommand(NICKSERV, new CommandNSSet(), MOD_HEAD);

		ModuleManager::Attach(I_OnReload,  this);

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
};


int count_email_in_use(const char *email, User * u)
{
	NickCore *nc;
	int i;
	int count = 0;

	if (!email)
		return 0;

	for (i = 0; i < 1024; ++i)
	{
		for (nc = nclists[i]; nc; nc = nc->next)
		{
			if (!(u->nc && u->nc == nc) && nc->email && !stricmp(nc->email, email))
				++count;
		}
	}

	return count;
}

CommandReturn check_email_limit_reached(const char *email, User * u)
{
	if (NSEmailMax < 1 || !email || is_services_admin(u))
		return MOD_CONT;

	if (count_email_in_use(email, u) < NSEmailMax)
		return MOD_CONT;

	if (NSEmailMax == 1)
		me->NoticeLang(s_NickServ, u, LNG_NSEMAILMAX_REACHED_ONE);
	else
		me->NoticeLang(s_NickServ, u, LNG_NSEMAILMAX_REACHED, NSEmailMax);

	return MOD_STOP;
}

void my_load_config()
{
	ConfigReader config;
	NSEmailMax = config.ReadInteger("ns_maxemail", "maxemails", "0", 0, false);

	if (debug)
		alog("debug: [ns_maxemail] NSEmailMax set to %d", NSEmailMax);
}

MODULE_INIT("ns_maxemail", NSMaxEmail)
