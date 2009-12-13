/* cs_tban.c - Bans the user for a given length of time
 *
 * (C) 2003-2009 Anope Team
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
#define VERSION "$Id$"

void mySendResponse(User *u, const char *channel, char *mask, const char *time);

void addBan(Channel *c, time_t timeout, char *banmask);
int canBanUser(Channel *c, User *u, User *u2);

void mAddLanguages();

static Module *me = NULL;

#define LANG_NUM_STRINGS	4
#define TBAN_HELP		   0
#define TBAN_SYNTAX		 1
#define TBAN_HELP_DETAIL	2
#define TBAN_RESPONSE	   3

class CommandCSTBan : public Command
{
 public:
	CommandCSTBan() : Command("TBAN", 3, 3)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		char mask[BUFSIZE];
		Channel *c;
		User *u2 = NULL;

		const char *chan = params[0].c_str();
		const char *nick = params[1].c_str();
		const char *time = params[2].c_str();

		if (!(c = findchan(chan)))
			notice_lang(Config.s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
		else if (!(u2 = finduser(nick)))
			notice_lang(Config.s_ChanServ, u, NICK_X_NOT_IN_USE, nick);
		else
		{
			if (canBanUser(c, u, u2))
			{
				get_idealban(c->ci, u2, mask, sizeof(mask));
				addBan(c, dotime(time), mask);
				mySendResponse(u, chan, mask, time);
			}
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		this->OnSyntaxError(u, "");
		ircdproto->SendMessage(findbot(Config.s_ChanServ), u->nick, " ");
		me->NoticeLang(Config.s_ChanServ, u, TBAN_HELP_DETAIL);

		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		me->NoticeLang(Config.s_ChanServ, u, TBAN_SYNTAX);
	}
};

class CSTBan : public Module
{
 public:
	CSTBan(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		me = this;

		this->AddCommand(CHANSERV, new CommandCSTBan());

		this->SetAuthor(AUTHOR);
		this->SetVersion(VERSION);
		this->SetType(SUPPORTED);

		const char* langtable_en_us[] = {
			"    TBAN       Bans the user for a given length of time",
			"Syntax: TBAN channel nick time",
			"Bans the given user from a channel for a specified length of\n"
			"time. If the ban is removed before by hand, it will NOT be replaced.",
			"%s banned from %s, will auto-expire in %s"
		};

		const char* langtable_nl[] = {
			"    TBAN       Verban een gebruiker voor een bepaalde tijd",
			"Syntax: TBAN kanaal nick tijd",
			"Verbant de gegeven gebruiken van het gegeven kanaal voor de\n"
			"gegeven tijdsduur. Als de verbanning eerder wordt verwijderd,\n"
			"zal deze NIET worden vervangen.",
			"%s verbannen van %s, zal verlopen in %s"
		};

		const char* langtable_de[] = {
			"    TBAN       Bant ein User fьr eine bestimmte Zeit aus ein Channel",
			"Syntax: TBAN Channel Nickname Zeit",
			"Bant ein User fьr eine bestimmte Zeit aus ein Channel\n"
			"Wenn der Ban manuell entfernt wird, wird es NICHT ersetzt.",
			"%s gebannt von %s, wird auto-auslaufen in %s"
		};

		const char* langtable_pt[] = {
			"    TBAN       Bane o usuбrio por um determinado perнodo de tempo",
			"Sintaxe: TBAN canal nick tempo",
			"Bane de um canal o usuбrio especificado por um determinado perнodo de\n"
			"tempo. Se o ban for removido manualmente antes do tempo, ele nгo serб recolocado.",
			"%s foi banido do %s, irб auto-expirar em %s"
		};

		const char* langtable_ru[] = {
			"    TBAN       Банит пользователя на указанный промежуток времени",
			"Синтаксис: TBAN #канал ник время",
			"Банит пользователя на указанный промежуток времени в секундах\n"
			"Примечание: удаленный вручную (до своего истечения) бан НЕ БУДЕТ\n"
			"переустановлен сервисами автоматически!",
			"Установленный бан %s на канале %s истечет через %s секунд"
		};

		const char* langtable_it[] = {
			"    TBAN       Banna l'utente per un periodo di tempo specificato",
			"Sintassi: TBAN canale nick tempo",
			"Banna l'utente specificato da un canale per un periodo di tempo\n"
			"specificato. Se il ban viene rimosso a mano prima della scadenza, NON verrа rimpiazzato.",
			"%s bannato da %s, scadrа automaticamente tra %s"
		};

		this->InsertLanguage(LANG_EN_US, LANG_NUM_STRINGS, langtable_en_us);
		this->InsertLanguage(LANG_NL, LANG_NUM_STRINGS, langtable_nl);
		this->InsertLanguage(LANG_DE, LANG_NUM_STRINGS, langtable_de);
		this->InsertLanguage(LANG_PT, LANG_NUM_STRINGS, langtable_pt);
		this->InsertLanguage(LANG_RU, LANG_NUM_STRINGS, langtable_ru);
		this->InsertLanguage(LANG_IT, LANG_NUM_STRINGS, langtable_it);

		ModuleManager::Attach(I_OnChanServHelp, this);
	}
	void OnChanServHelp(User *u)
	{
		this->NoticeLang(Config.s_ChanServ, u, TBAN_HELP);
	}
};

void mySendResponse(User *u, const char *channel, char *mask, const char *time)
{
	me->NoticeLang(Config.s_ChanServ, u, TBAN_RESPONSE, mask, channel, time);
}

class TempBan : public Timer
{
	private:
		std::string chan;
		std::string mask;

	public:
		TempBan(time_t seconds, const std::string &channel, const std::string &banmask) : Timer(seconds), chan(channel), mask(banmask) { }

		void Tick(time_t ctime)
		{
			Channel *c;

			if ((c = findchan(chan.c_str())) && c->ci)
			{
				c->RemoveMode(NULL, CMODE_BAN, mask);
			}
		}
};

void addBan(Channel *c, time_t timeout, char *banmask)
{
	c->SetMode(NULL, CMODE_BAN, banmask);

	me->AddCallBack(new TempBan(timeout, c->name, banmask));
}

int canBanUser(Channel * c, User * u, User * u2)
{
	ChannelInfo *ci = c->ci;
	int ok = 0;
	if (!check_access(u, ci, CA_BAN))
		notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
	else if (is_excepted(ci, u2))
		notice_lang(Config.s_ChanServ, u, CHAN_EXCEPTED, u2->nick, ci->name);
	else if (is_protected(u2))
		notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
	else
		ok = 1;

	return ok;
}

MODULE_INIT(CSTBan)
