/* hs_request.c - Add request and activate functionality to HostServ,
 *				along with adding +req as optional param to HostServ list.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Based on the original module by Rob <rob@anope.org>
 * Included in the Anope module pack since Anope 1.7.11
 * Anope Coder: GeniusDex <geniusdex@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Send bug reports to the Anope Coder instead of the module
 * author, because any changes since the inclusion into anope
 * are not supported by the original author.
 */

#include "module.h"

#define AUTHOR "Rob"
#define VERSION "$Id$"

/* Configuration variables */
int HSRequestMemoUser = 0;
int HSRequestMemoOper = 0;
int HSRequestMemoSetters = 0;

/* Language defines */
#define LNG_NUM_STRINGS 21

#define LNG_REQUEST_SYNTAX		0
#define LNG_REQUESTED			1
#define LNG_REQUEST_WAIT		2
#define LNG_REQUEST_MEMO		3
#define LNG_ACTIVATE_SYNTAX		4
#define LNG_ACTIVATED			5
#define LNG_ACTIVATE_MEMO		6
#define LNG_REJECT_SYNTAX		7
#define LNG_REJECTED			8
#define LNG_REJECT_MEMO			9
#define LNG_REJECT_MEMO_REASON	10
#define LNG_NO_REQUEST			11
#define LNG_HELP				12
#define LNG_HELP_SETTER			13
#define LNG_HELP_REQUEST		14
#define LNG_HELP_ACTIVATE		15
#define LNG_HELP_ACTIVATE_MEMO	16
#define LNG_HELP_REJECT			17
#define LNG_HELP_REJECT_MEMO	18
#define LNG_WAITING_SYNTAX		19
#define LNG_HELP_WAITING		20

void my_add_host_request(char *nick, char *vIdent, char *vhost, char *creator, time_t tmp_time);
int my_isvalidchar(const char c);
void my_memo_lang(User *u, const char *name, int z, int number, ...);
void req_send_memos(User *u, char *vIdent, char *vHost);

void my_load_config();
void my_add_languages();

struct HostRequest
{
	std::string ident;
	std::string host;
	time_t time;
};

std::map<std::string, HostRequest *> Requests;

static Module *me;

class CommandHSRequest : public Command
{
 public:
	CommandHSRequest() : Command("REQUEST", 1, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick;
		const char *rawhostmask = params[0].c_str();
		char *hostmask = new char[Config.HostLen];
		NickAlias *na;
		char *s;
		char *vIdent = NULL;
		time_t now = time(NULL);

		nick = u->nick.c_str();

		vIdent = myStrGetOnlyToken(rawhostmask, '@', 0); /* Get the first substring, @ as delimiter */
		if (vIdent)
		{
			rawhostmask = myStrGetTokenRemainder(rawhostmask, '@', 1); /* get the remaining string */
			if (!rawhostmask)
			{
				me->NoticeLang(Config.s_HostServ, u, LNG_REQUEST_SYNTAX);
				delete [] vIdent;
				delete [] hostmask;
				return MOD_CONT;
			}
			if (strlen(vIdent) > Config.UserLen)
			{
				notice_lang(Config.s_HostServ, u, HOST_SET_IDENTTOOLONG, Config.UserLen);
				delete [] vIdent;
				delete [] rawhostmask;
				delete [] hostmask;
				return MOD_CONT;
			}
			else
			{
				for (s = vIdent; *s; ++s)
				{
					if (!my_isvalidchar(*s))
					{
						notice_lang(Config.s_HostServ, u, HOST_SET_IDENT_ERROR);
						delete [] vIdent;
						delete [] rawhostmask;
						delete [] hostmask;
						return MOD_CONT;
					}
				}
			}
			if (!ircd->vident)
			{
				notice_lang(Config.s_HostServ, u, HOST_NO_VIDENT);
				delete [] vIdent;
				delete [] rawhostmask;
				delete [] hostmask;
				return MOD_CONT;
			}
		}
		if (strlen(rawhostmask) < Config.HostLen)
			snprintf(hostmask, Config.HostLen, "%s", rawhostmask);
		else
		{
			notice_lang(Config.s_HostServ, u, HOST_SET_TOOLONG, Config.HostLen);
			if (vIdent)
			{
				delete [] vIdent;
				delete [] rawhostmask;
			}
			delete [] hostmask;
			return MOD_CONT;
		}

		if (!isValidHost(hostmask, 3))
		{
			notice_lang(Config.s_HostServ, u, HOST_SET_ERROR);
			if (vIdent)
			{
				delete [] vIdent;
				delete [] rawhostmask;
			}
			delete [] hostmask;
			return MOD_CONT;
		}

		if ((na = findnick(nick)))
		{
			if (HSRequestMemoOper || HSRequestMemoSetters)
			{
				if (Config.MSSendDelay > 0 && u && u->lastmemosend + Config.MSSendDelay > now)
				{
					me->NoticeLang(Config.s_HostServ, u, LNG_REQUEST_WAIT, Config.MSSendDelay);
					u->lastmemosend = now;
					if (vIdent)
					{
						delete [] vIdent;
						delete [] rawhostmask;
					}
					delete [] hostmask;
					return MOD_CONT;
				}
			}
			my_add_host_request(const_cast<char *>(nick), vIdent, hostmask, const_cast<char *>(u->nick.c_str()), now);

			me->NoticeLang(Config.s_HostServ, u, LNG_REQUESTED);
			req_send_memos(u, vIdent, hostmask);
			Alog() << "New vHost Requested by " << nick;
		}
		else
			notice_lang(Config.s_HostServ, u, HOST_NOREG, nick);

		if (vIdent)
		{
			delete [] vIdent;
			delete [] rawhostmask;
		}
		delete [] hostmask;

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		me->NoticeLang(Config.s_HostServ, u, LNG_REQUEST_SYNTAX);
		u->SendMessage(Config.s_HostServ, " ");
		me->NoticeLang(Config.s_HostServ, u, LNG_HELP_REQUEST);

		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		me->NoticeLang(Config.s_HostServ, u, LNG_REQUEST_SYNTAX);
	}
};

class CommandHSActivate : public Command
{
 public:
	CommandHSActivate() : Command("ACTIVATE", 1, 1, "hostserv/set")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		NickAlias *na;

		if ((na = findnick(nick)))
		{
			std::map<std::string, HostRequest *>::iterator it = Requests.find(na->nick);
			if (it != Requests.end())
			{
				na->hostinfo.SetVhost(it->second->ident, it->second->host, u->nick,  it->second->time);
				Requests.erase(it);

				if (HSRequestMemoUser)
					my_memo_lang(u, na->nick, 2, LNG_ACTIVATE_MEMO);

				me->NoticeLang(Config.s_HostServ, u, LNG_ACTIVATED, nick);
				Alog() << "Host Request for " << nick << " activated by " << u->nick;
			}
			else
				me->NoticeLang(Config.s_HostServ, u, LNG_NO_REQUEST, nick);
		}
		else
			notice_lang(Config.s_HostServ, u, NICK_X_NOT_REGISTERED, nick);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		me->NoticeLang(Config.s_HostServ, u, LNG_ACTIVATE_SYNTAX);
		u->SendMessage(Config.s_HostServ, " ");
		me->NoticeLang(Config.s_HostServ, u, LNG_HELP_ACTIVATE);
		if (HSRequestMemoUser)
			me->NoticeLang(Config.s_HostServ, u, LNG_HELP_ACTIVATE_MEMO);

		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		me->NoticeLang(Config.s_HostServ, u, LNG_ACTIVATE_SYNTAX);
	}
};

class CommandHSReject : public Command
{
 public:
	CommandHSReject() : Command("REJECT", 1, 2, "hostserv/set")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		const char *reason = params.size() > 1 ? params[1].c_str() : NULL;

		std::map<std::string, HostRequest *>::iterator it = Requests.find(nick);
		if (it != Requests.end())
		{
			if (HSRequestMemoUser)
			{
				if (reason)
					my_memo_lang(u, nick, 2, LNG_REJECT_MEMO_REASON, reason);
				else
					my_memo_lang(u, nick, 2, LNG_REJECT_MEMO);
			}

			me->NoticeLang(Config.s_HostServ, u, LNG_REJECTED, nick);
			Alog() << "Host Request for " << nick << " rejected by " << u->nick << " (" << (reason ? reason : "") << ")";
		}
		else
			me->NoticeLang(Config.s_HostServ, u, LNG_NO_REQUEST, nick);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		me->NoticeLang(Config.s_HostServ, u, LNG_REJECT_SYNTAX);
		u->SendMessage(Config.s_HostServ, " ");
		me->NoticeLang(Config.s_HostServ, u, LNG_HELP_REJECT);
		if (HSRequestMemoUser)
			me->NoticeLang(Config.s_HostServ, u, LNG_HELP_REJECT_MEMO);

		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		me->NoticeLang(Config.s_HostServ, u, LNG_REJECT_SYNTAX);
	}
};

class HSListBase : public Command
{
 protected:
	CommandReturn DoList(User *u)
	{
		char buf[BUFSIZE];
		int counter = 1;
		int from = 0, to = 0;
		unsigned display_counter = 0;
		tm *tm;

		for (std::map<std::string, HostRequest *>::iterator it = Requests.begin(); it != Requests.end(); ++it)
		{
			HostRequest *hr = it->second;
			if (((counter >= from && counter <= to) || (!from && !to)) && display_counter < Config.NSListMax)
			{
				++display_counter;
				tm = localtime(&hr->time);
				strftime(buf, sizeof(buf), getstring(u, STRFTIME_DATE_TIME_FORMAT), tm);
				if (!hr->ident.empty())
					notice_lang(Config.s_HostServ, u, HOST_IDENT_ENTRY, counter, it->first.c_str(), hr->ident.c_str(), hr->host.c_str(), it->first.c_str(), buf);
				else
					notice_lang(Config.s_HostServ, u, HOST_ENTRY, counter, it->first.c_str(), hr->host.c_str(), it->first.c_str(), buf);
			}
			++counter;
		}
		notice_lang(Config.s_HostServ, u, HOST_LIST_FOOTER, display_counter);

		return MOD_CONT;
	}
 public:
	HSListBase(const std::string &cmd, int min, int max) : Command(cmd, min, max, "hostserv/set")
	{
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		// no-op
	}
};

class CommandHSWaiting : public HSListBase
{
 public:
	CommandHSWaiting() : HSListBase("WAITING", 0, 0)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		return this->DoList(u);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		me->NoticeLang(Config.s_HostServ, u, LNG_WAITING_SYNTAX);
		u->SendMessage(Config.s_HostServ, " ");
		me->NoticeLang(Config.s_HostServ, u, LNG_HELP_WAITING);

		return true;
	}
};

class HSRequest : public Module
{
 public:
	HSRequest(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		me = this;

		this->AddCommand(HOSTSERV, new CommandHSRequest());
		this->AddCommand(HOSTSERV, new CommandHSActivate());
		this->AddCommand(HOSTSERV, new CommandHSReject());
		this->AddCommand(HOSTSERV, new CommandHSWaiting());

		this->SetAuthor(AUTHOR);
		this->SetVersion(VERSION);
		this->SetType(SUPPORTED);

		my_load_config();

		const char* langtable_en_us[] = {
			/* LNG_REQUEST_SYNTAX */
			"Syntax: \002REQUEST \037vhost\037\002",
			/* LNG_REQUESTED */
			"Your vHost has been requested",
			/* LNG_REQUEST_WAIT */
			"Please wait %d seconds before requesting a new vHost",
			/* LNG_REQUEST_MEMO */
			"[auto memo] vHost \002%s\002 has been requested.",
			/* LNG_ACTIVATE_SYNTAX */
			"Syntax: \002ACTIVATE \037nick\037\002",
			/* LNG_ACTIVATED */
			"vHost for %s has been activated",
			/* LNG_ACTIVATE_MEMO */
			"[auto memo] Your requested vHost has been approved.",
			/* LNG_REJECT_SYNTAX */
			"Syntax: \002REJECT \037nick\037\002",
			/* LNG_REJECTED */
			"vHost for %s has been rejected",
			/* LNG_REJECT_MEMO */
			"[auto memo] Your requested vHost has been rejected.",
			/* LNG_REJECT_MEMO_REASON */
			"[auto memo] Your requested vHost has been rejected. Reason: %s",
			/* LNG_NO_REQUEST */
			"No request for nick %s found.",
			/* LNG_HELP */
			"    REQUEST     Request a vHost for your nick",
			/* LNG_HELP_SETTER */
			"    ACTIVATE    Approve the requested vHost of a user\n"
			"    REJECT      Reject the requested vHost of a user\n"
			"    WAITING     Convenience command for LIST +req",
			/* LNG_HELP_REQUEST */
			"Request the given vHost to be actived for your nick by the\n"
			"network administrators. Please be patient while your request\n"
			"is being considered.",
			/* LNG_HELP_ACTIVATE */
			"Activate the requested vHost for the given nick.",
			/* LNG_HELP_ACTIVATE_MEMO */
			"A memo informing the user will also be sent.",
			/* LNG_HELP_REJECT */
			"Reject the requested vHost for the given nick.",
			/* LNG_HELP_REJECT_MEMO */
			"A memo informing the user will also be sent.",
			/* LNG_WAITING_SYNTAX */
			"Syntax: \002WAITING\002",
			/* LNG_HELP_WAITING */
			"This command is provided for convenience. It is essentially\n"
			"the same as performing a LIST +req ."
		};

		const char* langtable_nl[] = {
			/* LNG_REQUEST_SYNTAX */
			"Gebruik: \002REQUEST \037vhost\037\002",
			/* LNG_REQUESTED */
			"Je vHost is aangevraagd",
			/* LNG_REQUEST_WAIT */
			"Wacht %d seconden voor je een nieuwe vHost aanvraagt",
			/* LNG_REQUEST_MEMO */
			"[auto memo] vHost \002%s\002 is aangevraagd.",
			/* LNG_ACTIVATE_SYNTAX */
			"Gebruik: \002ACTIVATE \037nick\037\002",
			/* LNG_ACTIVATED */
			"vHost voor %s is geactiveerd",
			/* LNG_ACTIVATE_MEMO */
			"[auto memo] Je aangevraagde vHost is geaccepteerd.",
			/* LNG_REJECT_SYNTAX */
			"Gebruik: \002REJECT \037nick\037\002",
			/* LNG_REJECTED */
			"vHost voor %s is afgekeurd",
			/* LNG_REJECT_MEMO */
			"[auto memo] Je aangevraagde vHost is afgekeurd.",
			/* LNG_REJECT_MEMO_REASON */
			"[auto memo] Je aangevraagde vHost is afgekeurd. Reden: %s",
			/* LNG_NO_REQUEST */
			"Geen aanvraag voor nick %s gevonden.",
			/* LNG_HELP */
			"    REQUEST     Vraag een vHost aan voor je nick",
			/* LNG_HELP_SETTER */
			"    ACTIVATE    Activeer de aangevraagde vHost voor een gebruiker\n"
			"    REJECT      Keur de aangevraagde vHost voor een gebruiker af\n"
			"    WAITING     Snelkoppeling naar LIST +req",
			/* LNG_HELP_REQUEST */
			"Verzoek de gegeven vHost te activeren voor jouw nick bij de\n"
			"netwerk beheerders. Het kan even duren voordat je aanvraag\n"
			"afgehandeld wordt.",
			/* LNG_HELP_ACTIVATE */
			"Activeer de aangevraagde vHost voor de gegeven nick.",
			/* LNG_HELP_ACTIVATE_MEMO */
			"Een memo die de gebruiker op de hoogste stelt zal ook worden verstuurd.",
			/* LNG_HELP_REJECT */
			"Keur de aangevraagde vHost voor de gegeven nick af.",
			/* LNG_HELP_REJECT_MEMO */
			"Een memo die de gebruiker op de hoogste stelt zal ook worden verstuurd.",
			/* LNG_WAITING_SYNTAX */
			"Gebruik: \002WAITING\002",
			/* LNG_HELP_WAITING */
			"Dit commando is beschikbaar als handigheid. Het is simpelweg\n"
			"hetzelfde als LIST +req ."
		};

		const char* langtable_pt[] = {
			/* LNG_REQUEST_SYNTAX */
			"Sintaxe: \002REQUEST \037vhost\037\002",
			/* LNG_REQUESTED */
			"Seu pedido de vHost foi encaminhado",
			/* LNG_REQUEST_WAIT */
			"Por favor, espere %d segundos antes de fazer um novo pedido de vHost",
			/* LNG_REQUEST_MEMO */
			"[Auto Memo] O vHost \002%s\002 foi solicitado.",
			/* LNG_ACTIVATE_SYNTAX */
			"Sintaxe: \002ACTIVATE \037nick\037\002",
			/* LNG_ACTIVATED */
			"O vHost para %s foi ativado",
			/* LNG_ACTIVATE_MEMO */
			"[Auto Memo] Seu pedido de vHost foi aprovado.",
			/* LNG_REJECT_SYNTAX */
			"Sintaxe: \002REJECT \037nick\037\002",
			/* LNG_REJECTED */
			"O vHost de %s foi recusado",
			/* LNG_REJECT_MEMO */
			"[Auto Memo] Seu pedido de vHost foi recusado.",
			/* LNG_REJECT_MEMO_REASON */
			"[Auto Memo] Seu pedido de vHost foi recusado. Motivo: %s",
			/* LNG_NO_REQUEST */
			"Nenhum pedido encontrado para o nick %s.",
			/* LNG_HELP */
			"    REQUEST     Request a vHost for your nick",
			/* LNG_HELP_SETTER */
			"    ACTIVATE    Aprova o pedido de vHost de um usuбrio\n"
			"    REJECT      Recusa o pedido de vHost de um usuбrio\n"
			"    WAITING     Comando para LISTAR +req",
			/* LNG_HELP_REQUEST */
			"Solicita a ativaзгo do vHost fornecido em seu nick pelos\n"
			"administradores da rede. Por favor, tenha paciкncia\n"
			"enquanto seu pedido й analisado.",
			/* LNG_HELP_ACTIVATE */
			"Ativa o vHost solicitado para o nick fornecido.",
			/* LNG_HELP_ACTIVATE_MEMO */
			"Um memo informando o usuбrio tambйm serб enviado.",
			/* LNG_HELP_REJECT */
			"Recusa o pedido de vHost para o nick fornecido.",
			/* LNG_HELP_REJECT_MEMO */
			"Um memo informando o usuбrio tambйm serб enviado.",
			/* LNG_WAITING_SYNTAX */
			"Sintaxe: \002WAITING\002",
			/* LNG_HELP_WAITING */
			"Este comando й usado por conveniкncia. Й essencialmente\n"
			"o mesmo que fazer um LIST +req"
		};

		const char* langtable_ru[] = {
			/* LNG_REQUEST_SYNTAX */
			"Синтаксис: \002REQUEST \037vHost\037\002",
			/* LNG_REQUESTED */
			"Ваш запрос на vHost отправлен.",
			/* LNG_REQUEST_WAIT */
			"Пожалуйста, подождите %d секунд, прежде чем запрашивать новый vHost",
			/* LNG_REQUEST_MEMO */
			"[авто-сообщение] Был запрошен vHost \002%s\002",
			/* LNG_ACTIVATE_SYNTAX */
			"Синтаксис: \002ACTIVATE \037ник\037\002",
			/* LNG_ACTIVATED */
			"vHost для %s успешно активирован",
			/* LNG_ACTIVATE_MEMO */
			"[авто-сообщение] Запрашиваемый вами vHost утвержден и активирован.",
			/* LNG_REJECT_SYNTAX */
			"Синтаксис: \002REJECT \037ник\037\002",
			/* LNG_REJECTED */
			"vHost для %s отклонен.",
			/* LNG_REJECT_MEMO */
			"[авто-сообщение] Запрашиваемый вами vHost отклонен.",
			/* LNG_REJECT_MEMO_REASON */
			"[авто-сообщение] Запрашиваемый вами vHost отклонен. Причина: %s",
			/* LNG_NO_REQUEST */
			"Запрос на vHost для ника %s не найден.",
			/* LNG_HELP */
			"    REQUEST     Запрос на vHost для вашего текущего ника",
			/* LNG_HELP_SETTER */
			"    ACTIVATE    Утвердить запрашиваемый пользователем  vHost\n"
			"    REJECT      Отклонить запрашиваемый пользователем  vHost\n"
			"    WAITING     Список запросов ожидающих обработки (аналог LIST +req)",
			/* LNG_HELP_REQUEST */
			"Отправляет запрос на активацию vHost, который будет рассмотрен одним из\n"
			"администраторов сети. Просьба проявить терпение, пока запрос\n"
			"рассматривается администрацией.",
			/* LNG_HELP_ACTIVATE */
			"Утвердить запрашиваемый vHost для указанного ника.",
			/* LNG_HELP_ACTIVATE_MEMO */
			"Пользователю будет послано авто-уведомление об активации его запроса.",
			/* LNG_HELP_REJECT */
			"Отклонить запрашиваемый vHost для указанного ника.",
			/* LNG_HELP_REJECT_MEMO */
			"Пользователю будет послано авто-уведомление об отклонении его запроса.",
			/* LNG_WAITING_SYNTAX */
			"Синтаксис: \002WAITING\002",
			/* LNG_HELP_WAITING */
			"Данная команда создана для удобства использования и выводит список запросов,\n"
			"ожидающих обработки. Аналогичная команда: LIST +req ."
		};

		const char* langtable_it[] = {
			/* LNG_REQUEST_SYNTAX */
			"Sintassi: \002REQUEST \037vhost\037\002",
			/* LNG_REQUESTED */
			"Il tuo vHost и stato richiesto",
			/* LNG_REQUEST_WAIT */
			"Prego attendere %d secondi prima di richiedere un nuovo vHost",
			/* LNG_REQUEST_MEMO */
			"[auto memo] и stato richiesto il vHost \002%s\002.",
			/* LNG_ACTIVATE_SYNTAX */
			"Sintassi: \002ACTIVATE \037nick\037\002",
			/* LNG_ACTIVATED */
			"Il vHost per %s и stato attivato",
			/* LNG_ACTIVATE_MEMO */
			"[auto memo] Il vHost da te richiesto и stato approvato.",
			/* LNG_REJECT_SYNTAX */
			"Sintassi: \002REJECT \037nick\037\002",
			/* LNG_REJECTED */
			"Il vHost per %s и stato rifiutato",
			/* LNG_REJECT_MEMO */
			"[auto memo] Il vHost da te richiesto и stato rifiutato.",
			/* LNG_REJECT_MEMO_REASON */
			"[auto memo] Il vHost da te richiesto и stato rifiutato. Motivo: %s",
			/* LNG_NO_REQUEST */
			"Nessuna richiesta trovata per il nick %s.",
			/* LNG_HELP */
			"    REQUEST     Richiede un vHost per il tuo nick",
			/* LNG_HELP_SETTER */
			"    ACTIVATE    Approva il vHost richiesto di un utente\n"
			"    REJECT      Rifiuta il vHost richiesto di un utente\n"
			"    WAITING     Comando per LIST +req",
			/* LNG_HELP_REQUEST */
			"Richiede l'attivazione del vHost specificato per il tuo nick da parte\n"
			"degli amministratori di rete. Sei pregato di pazientare finchи la tua\n"
			"richiesta viene elaborata.",
			/* LNG_HELP_ACTIVATE */
			"Attiva il vHost richiesto per il nick specificato.",
			/* LNG_HELP_ACTIVATE_MEMO */
			"Viene inviato un memo per informare l'utente.",
			/* LNG_HELP_REJECT */
			"Rifiuta il vHost richiesto per il nick specificato.",
			/* LNG_HELP_REJECT_MEMO */
			"Viene inviato un memo per informare l'utente.",
			/* LNG_WAITING_SYNTAX */
			"Sintassi: \002WAITING\002",
			/* LNG_HELP_WAITING */
			"Questo comando и per comoditа. Praticamente и la stessa cosa che\n"
			"eseguire un LIST +req ."
		};

		this->InsertLanguage(LANG_EN_US, LNG_NUM_STRINGS, langtable_en_us);
		this->InsertLanguage(LANG_NL, LNG_NUM_STRINGS, langtable_nl);
		this->InsertLanguage(LANG_PT, LNG_NUM_STRINGS, langtable_pt);
		this->InsertLanguage(LANG_RU, LNG_NUM_STRINGS, langtable_ru);
		this->InsertLanguage(LANG_IT, LNG_NUM_STRINGS, langtable_it);

		Implementation i[] = { I_OnHostServHelp, I_OnPreCommand, I_OnDatabaseRead, I_OnDatabaseWrite };
		ModuleManager::Attach(i, this, 4);
	}

	~HSRequest()
	{
		/* Clean up all open host requests */
		while (!Requests.empty())
		{
			delete Requests.begin()->second;
			Requests.erase(Requests.begin());
		}
	}

	EventReturn OnPreCommand(User *u, const std::string &service, const ci::string &command, const std::vector<ci::string> &params)
	{
		if (Config.s_HostServ && service == Config.s_HostServ)
		{
			if (command == "LIST")
			{
				ci::string key = params.size() ? params[0] : "";

				if (!key.empty() && key == "+req")
				{
					std::vector<ci::string> emptyParams;
					Command *c = findCommand(HOSTSERV, "WAITING");
					c->Execute(u, emptyParams);
					return EVENT_STOP;
				}
			}
		}
		else if (service == Config.s_NickServ)
		{
			if (command == "DROP")
			{
				NickAlias *na = findnick(u->nick);

				if (na)
				{
					std::map<std::string, HostRequest *>::iterator it = Requests.find(na->nick);

					if (it != Requests.end())
					{
						delete it->second;
						Requests.erase(it);
					}
				}
			}
		}

		return EVENT_CONTINUE;
	}

	void OnHostServHelp(User *u)
	{
		this->NoticeLang(Config.s_HostServ, u, LNG_HELP);
		this->NoticeLang(Config.s_HostServ, u, LNG_HELP_SETTER);
	}

	EventReturn OnDatabaseRead(const std::vector<std::string> &params)
	{
		if (params[0] == "HS_REQUEST" && params.size() >= 5)
		{
			char *vident = params[2] == "(null)" ? NULL : const_cast<char *>(params[2].c_str());
			my_add_host_request(const_cast<char *>(params[1].c_str()), vident, const_cast<char *>(params[3].c_str()), const_cast<char *>(params[1].c_str()), strtol(params[4].c_str(), NULL, 10));

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnDatabaseWrite(void (*Write)(const std::string &))
	{
		for (std::map<std::string, HostRequest *>::iterator it = Requests.begin(); it != Requests.end(); ++it)
		{
			HostRequest *hr = it->second;
			std::stringstream buf;
			buf << "HS_REQUEST " << it->first << " " << (hr->ident.empty() ? "(null)" : hr->ident) << " " << hr->host << " " << hr->time;
			Write(buf.str());
		}
	}
};

void my_memo_lang(User *u, const char *name, int z, int number, ...)
{
	va_list va;
	char buffer[4096], outbuf[4096];
	char *fmt = NULL;
	int lang = LANG_EN_US;
	char *s, *t, *buf;
	User *u2;

	u2 = finduser(name);
	/* Find the users lang, and use it if we cant */
	if (u2 && u2->Account())
		lang = u2->Account()->language;

	/* If the users lang isnt supported, drop back to enlgish */
	if (!me->lang[lang].argc)
		lang = LANG_EN_US;

	/* If the requested lang string exists for the language */
	if (me->lang[lang].argc > number)
	{
		fmt = me->lang[lang].argv[number];

		buf = sstrdup(fmt);
		s = buf;
		while (*s)
		{
			t = s;
			s += strcspn(s, "\n");
			if (*s)
				*s++ = '\0';
			strscpy(outbuf, t, sizeof(outbuf));

			va_start(va, number);
			vsnprintf(buffer, 4095, outbuf, va);
			va_end(va);
			memo_send(u, name, buffer, z);
		}
		delete [] buf;
	}
	else
		Alog() << me->name << ": INVALID language string call, language: [" << lang << "], String [" << number << "]";
}

void req_send_memos(User *u, char *vIdent, char *vHost)
{
	int z = 2;
	char host[BUFSIZE];
	std::list<std::pair<std::string, std::string> >::iterator it;

	if (vIdent)
		snprintf(host, sizeof(host), "%s@%s", vIdent, vHost);
	else
		snprintf(host, sizeof(host), "%s", vHost);

	if (HSRequestMemoOper == 1)
	{
		for (it = Config.Opers.begin(); it != Config.Opers.end(); ++it)
		{
			std::string nick = it->first;
			my_memo_lang(u, nick.c_str(), z, LNG_REQUEST_MEMO, host);
		}
	}
	if (HSRequestMemoSetters == 1)
	{
		/* Needs to be rethought because of removal of HostSetters in favor of opertype priv -- CyberBotX
		for (i = 0; i < HostNumber; ++i)
			my_memo_lang(u, HostSetters[i], z, LNG_REQUEST_MEMO, host);*/
	}
}

void my_add_host_request(char *nick, char *vIdent, char *vhost, char *creator, time_t tmp_time)
{
	HostRequest *hr = new HostRequest;
	hr->ident = vIdent ? vIdent : "";
	hr->host = vhost;
	hr->time = tmp_time;
	std::map<std::string, HostRequest *>::iterator it = Requests.find(nick);
	if (it != Requests.end())
		Requests.erase(it);
	Requests.insert(std::make_pair(nick, hr));
}

int my_isvalidchar(const char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '-')
		return 1;
	else
		return 0;
}

void my_load_config()
{
	ConfigReader config;
	HSRequestMemoUser = config.ReadFlag("hs_request", "memouser", "no", 0);
	HSRequestMemoOper = config.ReadFlag("hs_request", "memooper", "no", 0);
	HSRequestMemoSetters = config.ReadFlag("hs_request", "memosetters", "no", 0);

	Alog(LOG_DEBUG) << "[hs_request] Set config vars: MemoUser=" << HSRequestMemoUser << " MemoOper=" <<  HSRequestMemoOper << " MemoSetters=" << HSRequestMemoSetters;
}

MODULE_INIT(HSRequest)
