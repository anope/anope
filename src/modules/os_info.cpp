/* os_info.c - Adds oper information lines to nicks/channels
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Based on the original module by Rob <rob@anope.org>
 * Included in the Anope module pack since Anope 1.7.9
 * Anope Coder: DrStein <drstein@anope.org>
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

/* Multi-language stuff */
#define LANG_NUM_STRINGS   10

#define OINFO_SYNTAX		0
#define OINFO_ADD_SUCCESS   1
#define OINFO_DEL_SUCCESS   2
#define OCINFO_SYNTAX	   3
#define OCINFO_ADD_SUCCESS  4
#define OCINFO_DEL_SUCCESS  5
#define OINFO_HELP		  6
#define OCINFO_HELP		 7
#define OINFO_HELP_CMD	  8
#define OCINFO_HELP_CMD	 9

/*************************************************************************/

static Module *me;

/*************************************************************************/

class CommandNSOInfo : public Command
{
 private:
	CommandReturn DoAdd(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[1].c_str();
		const char *info = params.size() > 2 ? params[2].c_str() : NULL;
		NickAlias *na = NULL;

		if (!info)
		{
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}

		if ((na = findnick(nick))) /* ok we've found the user */
		{
			na->nc->Shrink("os_info");
			/* Add the module data to the user */
			na->nc->Extend("os_info", new ExtensibleItemPointerArray<char>(sstrdup(info)));
			me->NoticeLang(Config.s_NickServ, u, OINFO_ADD_SUCCESS, nick);

		}
		else /* NickCore not found! */
			notice_lang(Config.s_NickServ, u, NICK_X_NOT_REGISTERED, nick);

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[1].c_str();
		NickAlias *na = NULL;

		if ((na = findnick(nick))) /* ok we've found the user */
		{
			na->nc->Shrink("os_info");

			me->NoticeLang(Config.s_NickServ, u, OINFO_DEL_SUCCESS, nick);

		}
		else /* NickCore not found! */
			notice_lang(Config.s_NickServ, u, NICK_X_NOT_REGISTERED, nick);

		return MOD_CONT;
	}
 public:
	CommandNSOInfo() : Command("OINFO", 2, 3, "nickserv/oinfo")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ci::string cmd = params[0];

		if (cmd == "ADD")
			return this->DoAdd(u, params);
		else if (cmd == "DEL")
			return this->DoDel(u, params);
		else
			this->OnSyntaxError(u, "");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		me->NoticeLang(Config.s_NickServ, u, OINFO_HELP);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		me->NoticeLang(Config.s_NickServ, u, OINFO_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		me->NoticeLang(Config.s_NickServ, u, OINFO_HELP_CMD);
	}
};

class CommandCSOInfo : public Command
{
 private:
	CommandReturn DoAdd(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *info = params.size() > 2 ? params[2].c_str() : NULL;
		ChannelInfo *ci = cs_findchan(chan);

		if (!info)
		{
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}

		ci->Shrink("os_info");
		/* Add the module data to the channel */
		ci->Extend("os_info", new ExtensibleItemPointerArray<char>(sstrdup(info)));
		me->NoticeLang(Config.s_ChanServ, u, OCINFO_ADD_SUCCESS, chan);

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		ChannelInfo *ci = cs_findchan(chan);

		/* Del the module data from the channel */
		ci->Shrink("os_info");
		me->NoticeLang(Config.s_ChanServ, u, OCINFO_DEL_SUCCESS, chan);

		return MOD_CONT;
	}
 public:
	CommandCSOInfo() : Command("OINFO", 2, 3, "chanserv/oinfo")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ci::string cmd = params[1];

		if (cmd == "ADD")
			return this->DoAdd(u, params);
		else if (cmd == "DEL")
			return this->DoDel(u, params);
		else
			this->OnSyntaxError(u, "");
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		me->NoticeLang(Config.s_ChanServ, u, OCINFO_HELP);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		me->NoticeLang(Config.s_ChanServ, u, OCINFO_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		me->NoticeLang(Config.s_ChanServ, u, OCINFO_HELP_CMD);
	}
};

class OSInfo : public Module
{
 public:
	OSInfo(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		me = this;

		this->SetAuthor(AUTHOR);
		this->SetType(SUPPORTED);

		this->AddCommand(NickServ, new CommandNSOInfo());
		this->AddCommand(ChanServ, new CommandCSOInfo());

		const char* langtable_en_us[] = {
			/* OINFO_SYNTAX */
			"Syntax: OINFO [ADD|DEL] nick <info>",
			/* OINFO_ADD_SUCCESS */
			"OperInfo line has been added to nick %s",
			/* OINFO_DEL_SUCCESS */
			"OperInfo line has been removed from nick %s",
			/* OCINFO_SYNTAX */
			"Syntax: OINFO #chan [ADD|DEL] <info>",
			/* OCINFO_ADD_SUCCESS */
			"OperInfo line has been added to channel %s",
			/* OCINFO_DEL_SUCCESS */
			"OperInfo line has been removed from channel %s",
			/* OINFO_HELP */
			"Syntax: OINFO [ADD|DEL] nick <info>\n"
			"Add or Delete Oper information for the given nick\n"
			"This will show up when any oper /ns info nick's the user.\n"
			"and can be used for 'tagging' users etc....",
			/* OCINFO_HELP */
			"Syntax: OINFO #chan [ADD|DEL] <info>\n"
			"Add or Delete Oper information for the given channel\n"
			"This will show up when any oper /cs info's the channel.\n"
			"and can be used for 'tagging' channels etc....",
			/* OINFO_HELP_CMD */
			"    OINFO      Add / Del an OperInfo line to a nick",
			/* OCINFO_HELP_CMD */
			"    OINFO      Add / Del an OperInfo line to a channel"
		};

		const char* langtable_es[] = {
			/* OINFO_SYNTAX */
			"Sintaxis: OINFO [ADD|DEL] nick <info>",
			/* OINFO_ADD_SUCCESS */
			"Una linea OperInfo ha sido agregada al nick %s",
			/* OINFO_DEL_SUCCESS */
			"La linea OperInfo ha sido removida del nick %s",
			/* OCINFO_SYNTAX */
			"Sintaxis: OINFO [ADD|DEL] chan <info>",
			/* OCINFO_ADD_SUCCESS */
			"Linea OperInfo ha sido agregada al canal %s",
			/* OCINFO_DEL_SUCCESS */
			"La linea OperInfo ha sido removida del canal %s",
			/* OINFO_HELP */
			"Sintaxis: OINFO [ADD|DEL] nick <info>\n"
			"Agrega o elimina informacion para Operadores al nick dado\n"
			"Esto se mostrara cuando cualquier operador haga /ns info nick\n"
			"y puede ser usado para 'marcado' de usuarios, etc....",
			/* OCINFO_HELP */
			"Sintaxis: OINFO [ADD|DEL] chan <info>\n"
			"Agrega o elimina informacion para Operadores al canal dado\n"
			"Esto se mostrara cuando cualquier operador haga /cs info canal\n"
			"y puede ser usado para 'marcado' de canales, etc....",
			/* OINFO_HELP_CMD */
			"    OINFO      Agrega / Elimina una linea OperInfo al nick",
			/* OCINFO_HELP_CMD */
			"    OINFO      Agrega / Elimina una linea OperInfo al canal"
		};

		const char* langtable_nl[] = {
			/* OINFO_SYNTAX */
			"Gebruik: OINFO [ADD|DEL] nick <info>",
			/* OINFO_ADD_SUCCESS */
			"OperInfo regel is toegevoegd aan nick %s",
			/* OINFO_DEL_SUCCESS */
			"OperInfo regel is weggehaald van nick %s",
			/* OCINFO_SYNTAX */
			"Gebruik: OINFO [ADD|DEL] kanaal <info>",
			/* OCINFO_ADD_SUCCESS */
			"OperInfo regel is toegevoegd aan kanaal %s",
			/* OCINFO_DEL_SUCCESS */
			"OperInfo regel is weggehaald van kanaal %s",
			/* OINFO_HELP */
			"Gebruik: OINFO [ADD|DEL] nick <info>\n"
			"Voeg een Oper informatie regel toe aan de gegeven nick, of\n"
			"verwijder deze. Deze regel zal worden weergegeven wanneer\n"
			"een oper /ns info nick doet voor deze gebruiker, en kan worden\n"
			"gebruikt om een gebruiker te 'markeren' etc...",
			/* OCINFO_HELP */
			"Gebruik: OINFO [ADD|DEL] kanaal <info>\n"
			"Voeg een Oper informatie regel toe aan de gegeven kanaal, of\n"
			"verwijder deze. Deze regel zal worden weergegeven wanneer\n"
			"een oper /cs info kanaal doet voor dit kanaal, en kan worden\n"
			"gebruikt om een kanaal te 'markeren' etc...",
			/* OINFO_HELP_CMD */
			"    OINFO      Voeg een OperInfo regel toe aan een nick of verwijder deze",
			/* OCINFO_HELP_CMD */
			"    OINFO         Voeg een OperInfo regel toe aan een kanaal of verwijder deze"
		};

		const char* langtable_de[] = {
			/* OINFO_SYNTAX */
			"Syntax: OINFO [ADD|DEL] Nickname <Information>",
			/* OINFO_ADD_SUCCESS */
			"Eine OperInfo Linie wurde zu den Nicknamen %s hinzugefьgt",
			/* OINFO_DEL_SUCCESS */
			"Die OperInfo Linie wurde von den Nicknamen %s enfernt",
			/* OCINFO_SYNTAX */
			"Syntax: OINFO [ADD|DEL] Channel <Information>",
			/* OCINFO_ADD_SUCCESS */
			"Eine OperInfo Linie wurde zu den Channel %s hinzugefьgt",
			/* OCINFO_DEL_SUCCESS */
			"Die OperInfo Linie wurde von den Channel %s enfernt",
			/* OINFO_HELP */
			"Syntax: OINFO [ADD|DEL] Nickname <Information>\n"
			"Addiert oder lцscht eine OperInfo Linie zu den angegebenen\n"
			"Nicknamen.Sie wird angezeigt wenn ein Oper mit /ns info sich\n"
			"ьber den Nicknamen informiert.",
			/* OCINFO_HELP */
			"Syntax: OINFO [ADD|DEL] chan <info>\n"
			"Addiert oder lцscht eine OperInfo Linie zu den angegebenen\n"
			"Channel.Sie wird angezeigt wenn ein Oper mit /cs info sich\n"
			"ьber den Channel informiert.",
			/* OINFO_HELP_CMD */
			"    OINFO      Addiert / Lцscht eine OperInfo Linie zu / von einen Nicknamen",
			/* OCINFO_HELP_CMD */
			"    OINFO      Addiert / Lцscht eine OperInfo Linie zu / von einen Channel"
		};

		const char* langtable_pt[] = {
			/* OINFO_SYNTAX */
			"Sintaxe: OINFO [ADD|DEL] nick <informaзгo>",
			/* OINFO_ADD_SUCCESS */
			"A linha OperInfo foi adicionada ao nick %s",
			/* OINFO_DEL_SUCCESS */
			"A linha OperInfo foi removida do nick %s",
			/* OCINFO_SYNTAX */
			"Sintaxe: OINFO [ADD|DEL] canal <informaзгo>",
			/* OCINFO_ADD_SUCCESS */
			"A linha OperInfo foi adicionada ao canal %s",
			/* OCINFO_DEL_SUCCESS */
			"A linha OperInfo foi removida do canal %s",
			/* OINFO_HELP */
			"Sintaxe: OINFO [ADD|DEL] nick <informaзгo>\n"
			"Adiciona ou apaga informaзгo para Operadores ao nick fornecido\n"
			"Isto serб mostrado quando qualquer Operador usar /ns info nick\n"
			"e pode ser usado para 'etiquetar' usuбrios etc...",
			/* OCINFO_HELP */
			"Sintaxe: OINFO [ADD|DEL] canal <informaзгo>\n"
			"Adiciona ou apaga informaзгo para Operadores ao canal fornecido\n"
			"Isto serб mostrado quando qualquer Operador usar /cs info canal\n"
			"e pode ser usado para 'etiquetar' canais etc...",
			/* OINFO_HELP_CMD */
			"    OINFO      Adiciona ou Apaga a linha OperInfo para um nick",
			/* OCINFO_HELP_CMD */
			"    OINFO      Adiciona ou Apaga a linha OperInfo para um canal"
		};

		const char* langtable_ru[] = {
			/* OINFO_SYNTAX */
			"Синтаксис: OINFO ADD|DEL ник тест",
			/* OINFO_ADD_SUCCESS */
			"Опер-Информация для ника %s добавлена",
			/* OINFO_DEL_SUCCESS */
			"Опер-Информация для ника %s была удалена",
			/* OCINFO_SYNTAX */
			"Синтаксис: OINFO ADD|DEL #канал текст",
			/* OCINFO_ADD_SUCCESS */
			"Опер-Информация для канала %s успешно установлена",
			/* OCINFO_DEL_SUCCESS */
			"Опер-Информация для канала %s была удалена",
			/* OINFO_HELP */
			"Синтаксис: OINFO ADD|DEL ник текст\n"
			"Устанавливает или удаляет Опер-Информацию для указанного ника,\n"
			"которая будет показана любому оператору, запрашивающему INFO ника.\n"
			"Может быть использована для 'пометки' пользователей и т. д...",
			/* OCINFO_HELP */
			"Синтаксис: OINFO ADD|DEL #канал текст\n"
			"Устанавливает или удаляет Опер-Информацию для указанного канала,\n"
			"которая будет показана любому оператору, запрашивающему INFO канала.\n"
			"Может быть использована для 'пометки' каналов и т. д...",
			/* OINFO_HELP_CMD */
			"    OINFO      Добавляет/Удаляет опер-инфо для ника",
			/* OCINFO_HELP_CMD */
			"    OINFO      Добавляет/Удаляет опер-инфо для канала"
		};

		const char* langtable_it[] = {
			/* OINFO_SYNTAX */
			"Sintassi: OINFO [ADD|DEL] nick <info>",
			/* OINFO_ADD_SUCCESS */
			"Linea OperInfo aggiunta al nick %s",
			/* OINFO_DEL_SUCCESS */
			"Linea OperInfo rimossa dal nick %s",
			/* OCINFO_SYNTAX */
			"Sintassi: OINFO [ADD|DEL] chan <info>",
			/* OCINFO_ADD_SUCCESS */
			"Linea OperInfo aggiunta al canale %s",
			/* OCINFO_DEL_SUCCESS */
			"Linea OperInfo rimossa dal canale %s",
			/* OINFO_HELP */
			"Sintassi: OINFO [ADD|DEL] nick <info>\n"
			"Aggiunge o rimuove informazioni Oper per il nick specificato\n"
			"Queste vengono mostrate quando un oper esegue il comando /ns info <nick>\n"
			"e possono essere utilizzate per 'marchiare' gli utenti ecc...",
			/* OCINFO_HELP */
			"Sintassi: OINFO [ADD|DEL] chan <info>\n"
			"Aggiunge o rimuove informazioni Oper per il canale specificato\n"
			"Queste vengono mostrate quando un oper esegue il comando /cs info <canale>\n"
			"e possono essere utilizzate per 'marchiare' i canali ecc...",
			/* OINFO_HELP_CMD */
			"    OINFO      Aggiunge/Rimuove una linea OperInfo ad/da un nick",
			/* OCINFO_HELP_CMD */
			"    OINFO      Aggiunge/Rimuove una linea OperInfo ad/da un canale"
		};

		this->InsertLanguage(LANG_EN_US, LANG_NUM_STRINGS, langtable_en_us);
		this->InsertLanguage(LANG_ES, LANG_NUM_STRINGS, langtable_es);
		this->InsertLanguage(LANG_NL, LANG_NUM_STRINGS, langtable_nl);
		this->InsertLanguage(LANG_DE, LANG_NUM_STRINGS, langtable_de);
		this->InsertLanguage(LANG_PT, LANG_NUM_STRINGS, langtable_pt);
		this->InsertLanguage(LANG_RU, LANG_NUM_STRINGS, langtable_ru);
		this->InsertLanguage(LANG_IT, LANG_NUM_STRINGS, langtable_it);

		Implementation i[] = { I_OnNickInfo, I_OnChanInfo, I_OnDatabaseReadMetadata, I_OnDatabaseWriteMetadata };
		ModuleManager::Attach(i, this, 4);
	}

	~OSInfo()
	{
		OnSaveDatabase();

		for (nickcore_map::const_iterator it = NickCoreList.begin(); it != NickCoreList.end(); ++it)
		{
			NickCore *nc = it->second;

			nc->Shrink("os_info");
		}

		for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(); it != RegisteredChannelList.end(); ++it)
		{
			ChannelInfo *ci = it->second;

			ci->Shrink("os_info");
		}
	}

	void OnNickInfo(User *u, NickAlias *na, bool)
	{
		if (is_oper(u))
		{
			char *c;
			if (na->nc->GetExtArray("os_info", c))
				u->SendMessage(Config.s_NickServ, " OperInfo: %s", c);
		}
	}

	void OnChanInfo(User *u, ChannelInfo *ci, bool)
	{
		if (is_oper(u))
		{
			char *c;
			if (ci->GetExtArray("os_info", c))
				u->SendMessage(Config.s_ChanServ, " OperInfo: %s", c);
		}
	}

	void OnDatabaseWriteMetadata(void (*WriteMetadata)(const std::string &, const std::string &), NickCore *nc)
	{
		char *c;

		if (nc->GetExtArray("os_info", c))
		{
			std::string buf = ":";
			buf += c;
			WriteMetadata("OS_INFO", buf.c_str());
		}
	}

	void OnDatabaseWriteMetadata(void (*WriteMetadata)(const std::string &, const std::string &), ChannelInfo *ci)
	{
		char *c;

		if (ci->GetExtArray("os_info", c))
		{
			std::string buf = ":";
			buf += c;
			WriteMetadata("OS_INFO", buf.c_str());
		}
	}

	EventReturn OnDatabaseReadMetadata(NickCore *nc, const std::string &key, const std::vector<std::string> &params)
	{
		if (key == "OS_INFO")
		{
			nc->Shrink("os_info");
			nc->Extend("os_info", new ExtensibleItemPointerArray<char>(sstrdup(params[0].c_str()))); /// We really should use std::string here...

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnDatabaseReadMetadata(ChannelInfo *ci, const std::string &key, const std::vector<std::string> &params)
	{
		if (key == "OS_INFO")
		{
			ci->Shrink("os_info");
			ci->Extend("os_info", new ExtensibleItemPointerArray<char>(sstrdup(params[0].c_str())));
			
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(OSInfo)
