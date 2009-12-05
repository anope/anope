/* Configuration file handling.
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */

#include "services.h"
#include "configreader.h"
#include "hashcomp.h" // If this gets added to services.h or someplace else later, remove it from here -- CyberBotX

/*************************************************************************/

ServerConfig Config;

static std::string Modules;
static std::string EncModules;
static std::string HostCoreModules;
static std::string MemoCoreModules;
static std::string BotCoreModules;
static std::string OperCoreModules;
static std::string NickCoreModules;
static std::string ChanCoreModules;
static std::string DefCon1;
static std::string DefCon2;
static std::string DefCon3;
static std::string DefCon4;
static char *UlineServers;
static std::string OSNotifications;
static std::string BSDefaults;
static std::string CSDefaults;
static char *temp_nsuserhost;
static std::string NSDefaults;

/*************************************************************************/

ServerConfig::ServerConfig() : include_stack(), errstr(""), newconfig(), config_data()
{
	this->ClearStack();
}

void ServerConfig::ClearStack()
{
	include_stack.clear();
}

bool ServerConfig::CheckOnce(const char *tag)
{
	int count = ConfValueEnum(config_data, tag);
	if (count > 1)
		throw ConfigException(std::string("You have more than one <") + tag + "> tag, this is not permitted.");
	if (count < 1)
		throw ConfigException(std::string("You have not defined a <") + tag + "> tag, this is required.");
	return true;
}

bool NoValidation(ServerConfig *, const char *, const char *, ValueItem &)
{
	return true;
}

bool DoneConfItem(ServerConfig *, const char *)
{
	return true;
}

void ServerConfig::ValidateNoSpaces(const char *p, const std::string &tag, const std::string &val)
{
	for (const char *ptr = p; *ptr; ++ptr)
		if (*ptr == ' ')
			throw ConfigException(std::string("The value of <") + tag + ":" + val + "> cannot contain spaces");
}

/* NOTE: Before anyone asks why we're not using inet_pton for this, it is because inet_pton and friends do not return so much detail,
 * even in strerror(errno). They just return 'yes' or 'no' to an address without such detail as to whats WRONG with the address.
 * Because ircd users arent as technical as they used to be (;)) we are going to give more of a useful error message.
 */
void ServerConfig::ValidateIP(const char *p, const std::string &tag, const std::string &val, bool wild)
{
	int num_dots = 0, num_seps = 0;
	bool not_numbers = false, not_hex = false;

	if (*p)
	{
		if (*p == '.')
			throw ConfigException(std::string("The value of <") + tag + ":" + val + "> is not an IP address");

		for (const char *ptr = p; *ptr; ++ptr)
		{
			if (wild && (*ptr == '*' || *ptr == '?' || *ptr == '/'))
				continue;

			if (*ptr != ':' && *ptr != '.')
			{
				if (*ptr < '0' || *ptr > '9')
				{
					not_numbers = true;
					if (toupper(*ptr) < 'A' || toupper(*ptr) > 'F')
						not_hex = true;
				}
			}
			switch (*ptr)
			{
				case ' ':
					throw ConfigException(std::string("The value of <") + tag + ":" + val + "> is not an IP address");
				case '.':
					++num_dots;
					break;
				case ':':
					++num_seps;
			}
		}
		if (num_dots > 3)
			throw ConfigException(std::string("The value of <") + tag + ":" + val + "> is an IPv4 address with too many fields!");

		if (num_seps > 8)
			throw ConfigException(std::string("The value of <") + tag + ":" + val + "> is an IPv6 address with too many fields!");

		if (!num_seps && num_dots < 3 && !wild)
			throw ConfigException(std::string("The value of <") + tag + ":" + val + "> looks to be a malformed IPv4 address");

		if (!num_seps && num_dots == 3 && not_numbers)
			throw ConfigException(std::string("The value of <") + tag + ":" + val + "> contains non-numeric characters in an IPv4 address");

		if (num_seps && not_hex)
			throw ConfigException(std::string("The value of <") + tag + ":" + val + "> contains non-hexdecimal characters in an IPv6 address");

		if (num_seps && num_dots != 3 && num_dots && !wild)
			throw ConfigException(std::string("The value of <") + tag + ":" + val + "> is a malformed IPv6 4in6 address");
	}
}

void ServerConfig::ValidateHostname(const char *p, const std::string &tag, const std::string &val)
{
	if (!strcasecmp(p, "localhost"))
		return;

	int num_dots = 0;
	if (*p)
	{
		if (*p == '.')
			throw ConfigException(std::string("The value of <") + tag + ":" + val + "> is not a valid hostname");
		for (const char *ptr = p; *ptr; ++ptr)
		{
			switch (*ptr)
			{
				case ' ':
					throw ConfigException(std::string("The value of <") + tag + ":" + val + "> is not a valid hostname");
				case '.':
					++num_dots;
			}
		}
		if (!num_dots)
			throw ConfigException(std::string("The value of <") + tag + ":" + val + "> is not a valid hostname");
	}
}

bool ValidateNotEmpty(ServerConfig *, const char *tag, const char *value, ValueItem &data)
{
	if (data.GetValue().empty())
		throw ConfigException(std::string("The value for <") + tag + ":" + value + "> cannot be empty!");
	return true;
}

bool ValidateNotZero(ServerConfig *, const char *tag, const char *value, ValueItem &data)
{
	if (!data.GetInteger())
		throw ConfigException(std::string("The value for <") + tag + ":" + value + "> must be non-zero!");
	return true;
}

bool ValidateEmailReg(ServerConfig *, const char *tag, const char *value, ValueItem &data)
{
	if (Config.NSEmailReg)
	{
		if (std::string(value) == "prenickdatabase")
		{
			if (data.GetValue().empty())
				throw ConfigException(std::string("The value for <") + tag + ":" + value + "> cannot be empty when e-mail registrations are enabled!");
		}
		else if (std::string(value) == "preregexpire")
		{
			if (!data.GetInteger())
				throw ConfigException(std::string("The value for <") + tag + ":" + value + "> must be non-zero when e-mail registration are enabled!");
		}
		else
		{
			if (!data.GetBool())
				throw ConfigException(std::string("The value for <") + tag + ":" + value + "> must be set to yes when e-mail registrations are enabled!");
		}
	}
	return true;
}

bool ValidatePort(ServerConfig *, const char *tag, const char *value, ValueItem &data)
{
	int port = data.GetInteger();
	if (!port)
		return true;
	if (port < 1 || port > 65535)
		throw ConfigException(std::string("The value for <") + tag + ":" + value + "> is not a value port, it must be between 1 and 65535!");
	return true;
}

bool ValidateLanguage(ServerConfig *, const char *, const char *, ValueItem &data)
{
	int language = data.GetInteger();
	char maxlang[3];
	snprintf(maxlang, 3, "%d", USED_LANGS);
	if (language < 1 || language > USED_LANGS)
		throw ConfigException(std::string("The value for <nickserv:defaultlanguage> must be between 1 and ") + maxlang + "!");
	data.Set(--language);
	return true;
}

bool ValidateGuestPrefix(ServerConfig *conf, const char *tag, const char *value, ValueItem &data)
{
	ValidateNotEmpty(conf, tag, value, data);
	if (data.GetValue().size() > 21)
		throw ConfigException("The value for <nickserv:guestnickprefix> cannot exceed 21 characters in length!");
	return true;
}

bool ValidateBantype(ServerConfig *, const char *, const char *, ValueItem &data)
{
	int bantype = data.GetInteger();
	if (bantype < 0 || bantype > 3)
		throw ConfigException("The value for <chanserv:defbantype> must be between 0 and 3!");
	return true;
}

bool ValidateBotServ(ServerConfig *, const char *tag, const char *value, ValueItem &data)
{
	if (Config.s_BotServ)
	{
		if (std::string(value) == "description" || std::string(value) == "database")
		{
			if (data.GetValue().empty())
				throw ConfigException(std::string("The value for <") + tag + ":" + value + "> cannot be empty when BotServ is enabled!");
		}
		else if (std::string(value) == "minusers" || std::string(value) == "badwordsmax" || std::string(value) == "keepdata")
		{
			if (!data.GetInteger())
				throw ConfigException(std::string("The value for <") + tag + ":" + value + "> must be non-zero when BotServ is enabled!");
		}
	}
	return true;
}

bool ValidateHostServ(ServerConfig *, const char *tag, const char *value, ValueItem &data)
{
	if (Config.s_HostServ)
	{
		if (std::string(value) == "description" || std::string(value) == "database")
		{
			if (data.GetValue().empty())
				throw ConfigException(std::string("The value for <") + tag + ":" + value + "> cannot be empty when HostServ is enabled!");
		}
	}
	return true;
}

bool ValidateLimitSessions(ServerConfig *, const char *tag, const char *value, ValueItem &data)
{
	if (Config.LimitSessions)
	{
		if (std::string(value) == "maxsessionlimit" || std::string(value) == "exceptionexpiry")
		{
			if (!data.GetInteger())
				throw ConfigException(std::string("The value for <") + tag + ":" + value + "> must be non-zero when session limiting is enabled!");
		}
	}
	return true;
}

bool ValidateDefCon(ServerConfig *, const char *tag, const char *value, ValueItem &data)
{
	if (std::string(value) == "defaultlevel")
	{
		int level = data.GetInteger();
		if (!level)
			return true;
		if (level > 5)
			throw ConfigException("The value for <defcon:defaultlevel> must be between 1 through 5 if you wish to use DefCon or 0 if you wish to disable it!");
	}
	else if (Config.DefConLevel)
	{
		if ((std::string(value).substr(0, 5) == "level" && isdigit(value[5])) || std::string(value) == "chanmodes" || std::string(value) == "akillreason")
		{
			if (data.GetValue().empty())
				throw ConfigException(std::string("The value for <") + tag + ":" + value + "> cannot be empty when DefCon is enabled!");
		}
		else if (std::string(value) == "message" && Config.GlobalOnDefconMore)
		{
			if (data.GetValue().empty())
				throw ConfigException("The value for <defcon:message> cannot be empty when globalondefconmore is enabled!");
		}
		else if (std::string(value) == "sessionlimit" || std::string(value) == "akillexpire")
		{
			if (!data.GetInteger())
				throw ConfigException(std::string("The value for <") + tag + ":" + value + "> must be non-zero when DefCon is enabled!");
		}
	}
	return true;
}

bool ValidateNickLen(ServerConfig *, const char *, const char *, ValueItem &data)
{
	int nicklen = data.GetInteger();
	if (!nicklen)
	{
		alog("You have not defined the <networkinfo:nicklen> directive. It is strongly");
		alog("adviced that you do configure this correctly in your services.conf");
		data.Set(NICKMAX - 1);
	}
	else if (nicklen < 1 || nicklen >= NICKMAX)
	{
		alog("<networkinfo:nicklen> has an invalid value; setting to %d", NICKMAX - 1);
		data.Set(NICKMAX - 1);
	}
	return true;
}

bool ValidateMail(ServerConfig *, const char *tag, const char *value, ValueItem &data)
{
	if (Config.UseMail)
	{
		if (std::string(value) == "sendmailpath" || std::string(value) == "sendfrom")
		{
			if (data.GetValue().empty())
				throw ConfigException(std::string("The value for <") + tag + ":" + value + "> cannot be empty when e-mail is enabled!");
		}
	}
	return true;
}

bool ValidateGlobalOnCycle(ServerConfig *, const char *tag, const char *value, ValueItem &data)
{
	if (Config.GlobalOnCycle)
	{
		if (data.GetValue().empty())
		{
			alog("<%s:%s> was undefined, disabling <options:globaloncycle>", tag, value);
			Config.GlobalOnCycle = false;
		}
	}
	return true;
}

void ServerConfig::ReportConfigError(const std::string &errormessage, bool bail)
{
	alog("There were errors in your configuration file: %s", errormessage.c_str());
	if (bail)
	{
		// TODO -- Need a way to stop loading in a safe way -- CyberBotX
		//ServerInstance->Exit(EXIT_STATUS_CONFIG);
	}
}

bool InitUplinks(ServerConfig *, const char *, bool bail)
{
	// If bail is false, we were reloading, don't clear anything
	if (!bail)
		return true;
	if (!Config.Uplinks.empty())
	{
		std::list<Uplink *>::iterator curr_uplink = Config.Uplinks.begin(), end_uplink = Config.Uplinks.end();
		for (; curr_uplink != end_uplink; ++curr_uplink)
			delete *curr_uplink;
	}
	Config.Uplinks.clear();
	return true;
}

bool DoUplink(ServerConfig *conf, const char *, const char **, ValueList &values, int *, bool bail)
{
	// If bail is false, we were reloading, don't even try to add another uplink
	if (!bail)
		return true;
	// Validation variables
	const char *host = values[0].GetString(), *password = values[2].GetString();
	int port = values[1].GetInteger();
	ValueItem vi_host(host), vi_port(port), vi_password(password);
	// Validate the host to make sure it is not empty
	if (!ValidateNotEmpty(conf, "uplink", "host", vi_host))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	// Validate the port to make sure it is a valid port
	if (!ValidatePort(conf, "uplink", "port", vi_port))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	// Validate the password to make sure it is not empty
	if (!ValidateNotEmpty(conf, "uplink", "password", vi_password))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	// If we get here, all the values are valid, we'll add it to the Uplinks list
	Config.Uplinks.push_back(new Uplink(host, port, password));
	return true;
}

bool DoneUplinks(ServerConfig *, const char *, bool bail)
{
	// If bail is false, we were reloading, ignore this check
	if (!bail)
		return true;
	if (Config.Uplinks.empty())
		throw ConfigException("You must define at least one uplink block!");
	return true;
}

static bool InitOperTypes(ServerConfig *, const char *, bool)
{
	for (std::list<OperType *>::iterator it = Config.MyOperTypes.begin(); it != Config.MyOperTypes.end(); ++it)
		delete *it;

	Config.MyOperTypes.clear();
	return true;
}

static bool DoOperType(ServerConfig *conf, const char *, const char **, ValueList &values, int *, bool)
{
	const char *name = values[0].GetString();
	const char *commands = values[1].GetString();
	const char *privs = values[2].GetString();

	ValueItem vi(name);
	if (!ValidateNotEmpty(conf, "opertype", "name", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");

	OperType *ot = new OperType(name);

	std::string tok;
	spacesepstream cmdstr(commands);
	while (cmdstr.GetToken(tok))
		ot->AddCommand(tok);

	spacesepstream privstr(privs);
	while (privstr.GetToken(tok))
		ot->AddPriv(tok);

	Config.MyOperTypes.push_back(ot);
	return true;
}

static bool DoneOperTypes(ServerConfig *, const char *, bool)
{
	return true;
}

/*************************************************************************/

static bool InitOpers(ServerConfig *, const char *, bool)
{
	int i;
	NickCore *nc;

	for (i = 0; i < 1024; ++i)
		for (nc = nclists[i]; nc; nc = nc->next)
			nc->ot = NULL;
	
	Config.Opers.clear();

	return true;
}

static bool DoOper(ServerConfig *conf, const char *, const char **, ValueList &values, int *, bool)
{
	const char *name = values[0].GetString();
	const char *type = values[1].GetString();

	ValueItem vi(name);
	if (!ValidateNotEmpty(conf, "oper", "name", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");

	ValueItem vi2(type);
	if (!ValidateNotEmpty(conf, "oper", "type", vi2))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");

	Config.Opers.push_back(std::make_pair(name, type));
	return true;
}

static bool DoneOpers(ServerConfig *, const char *, bool)
{
	// XXX: this is duplicated in config.c
	for (std::list<std::pair<std::string, std::string> >::iterator it = Config.Opers.begin(); it != Config.Opers.end(); ++it)
	{
		std::string nick = it->first, type = it->second;

		NickAlias *na = findnick(nick);
		if (!na)
			// Nonexistant nick
			continue;

		if (!na->nc)
			// Nick with no core (wtf?)
			abort();

		for (std::list<OperType *>::iterator tit = Config.MyOperTypes.begin(); tit != Config.MyOperTypes.end(); ++tit)
		{
			OperType *ot = *tit;
			if (ot->GetName() == type)
			{
				alog("Tied oper %s to type %s", na->nc->display, type.c_str());
				na->nc->ot = ot;
			}
		}
	}
	return true;
}

/*************************************************************************/

bool InitModules(ServerConfig *, const char *, bool)
{
	Modules.clear();
	return true;
}

bool DoModule(ServerConfig *conf, const char *, const char **, ValueList &values, int *, bool)
{
	// First we validate that there was a name in the module block
	const char *module = values[0].GetString();
	ValueItem vi(module);
	if (!ValidateNotEmpty(conf, "module", "name", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	// If the string isn't empty, add a space before we add the module name
	if (!Modules.empty())
		Modules += " ";
	// Add the module name to the string
	Modules += values[0].GetString();
	return true;
}

bool DoneModules(ServerConfig *, const char *, bool)
{
	return true;
}

int ServerConfig::Read(bool bail)
{
	errstr.clear();
	// These tags MUST occur and must ONLY occur once in the config file
	static const char *Once[] = {"serverinfo", "networkinfo", "options", "nickserv", "chanserv", "memoserv", "operserv", NULL};
	// These tags can occur ONCE or not at all
	InitialConfig Values[] = {
		/* The following comments are from CyberBotX to w00t as examples to use:
		 *
		 * The last argument, for validation, must use one of the functions with the following signature:
		 * bool <function>(ServerConfig *, const char *, const char *, ValueItem &)
		 * Examples are: NoValidation, ValidateNotEmpty, etc.
		 *
		 * If you want to create a directive using an integer:
		 * int blarg;
		 * {"tag", "value", "0", new ValueContainerInt(&blarg), DT_INTEGER, <validation>},
		 *
		 * If you want to create a directive using an unsigned integer:
		 * unsigned blarg;
		 * {"tag", "value", "0", new ValueContainerUInt(&blarg), DT_UINTEGER, <validation>},
		 *
		 * If you want to create a directive using a character pointer without additional validation (see below for hostnames, fields with no spaces, and IP addresses):
		 * char *blarg;
		 * {"tag", "value", "", new ValueContainerChar(&blarg), DT_CHARPTR, <validation>},
		 *
		 * If you want to create a directive using a string:
		 * std::string blarg;
		 * {"tag", "value", "", new ValueContainerString(&blarg), DT_STRING, <validation>},
		 *
		 * If you want to create a directive using a boolean:
		 * bool blarg;
		 * {"tag", "value", "no", new ValueContainerBool(&blarg), DT_BOOLEAN, <validation>},
		 *
		 * If you want to create a directive using a character pointer specifically to hold a hostname (this will call ValidateHostname automatically):
		 * char *blarg;
		 * {"tag", "value", "", new ValueContainerChar(&blarg), DT_HOSTNAME, <validation>},
		 *
		 * If you want to create a directive using a character pointer that specifically can not have spaces in it (this will call ValidateNoSpaces automatically):
		 * char *blarg;
		 * {"tag", "value", "", new ValueContainerChar(&blarg), DT_NOSPACES, <validation>},
		 *
		 * If you want to create a directive using a character pointer specifically to hold an IP address (this will call ValidateIP automatically):
		 * char *blarg;
		 * {"tag", "value", "", new ValueContainerChar(&blarg), DT_IPADDRESS, <validation>},
		 *
		 * If you want to create a directive using a time (a time_t variable converted from a string):
		 * time_t blarg;
		 * {"tag", "value", "", new ValueContainterTime(&blarg), DT_TIME, <validation>},
		 *
		 * For the second-to-last argument, you can or (|) in the following values:
		 * DT_NORELOAD - The variable can't be changed on a reload of the configuration
		 * DT_ALLOW_WILD - Allows wildcards/CIDR in DT_IPADDRESS
		 * DT_ALLOW_NEWLINE - Allows new line characters in DT_CHARPTR and DT_STRING
		 *
		 * We may need to add some other validation functions to handle certain things, we can handle that later.
		 * Any questions about these, w00t, feel free to ask. */
		{"serverinfo", "name", "", new ValueContainerChar(&Config.ServerName), DT_HOSTNAME | DT_NORELOAD, ValidateNotEmpty},
		{"serverinfo", "description", "", new ValueContainerChar(&Config.ServerDesc), DT_CHARPTR | DT_NORELOAD, ValidateNotEmpty},
		{"serverinfo", "localhost", "", new ValueContainerChar(&Config.LocalHost), DT_HOSTNAME | DT_NORELOAD, NoValidation},
		{"serverinfo", "localport", "0", new ValueContainerUInt(&Config.LocalPort), DT_UINTEGER | DT_NORELOAD, ValidatePort},
		{"serverinfo", "type", "", new ValueContainerChar(&Config.IRCDModule), DT_CHARPTR | DT_NORELOAD, ValidateNotEmpty},
		{"serverinfo", "id", "", new ValueContainerChar(&Config.Numeric), DT_NOSPACES | DT_NORELOAD, NoValidation},
		{"serverinfo", "ident", "", new ValueContainerChar(&Config.ServiceUser), DT_CHARPTR | DT_NORELOAD, ValidateNotEmpty},
		{"serverinfo", "hostname", "", new ValueContainerChar(&Config.ServiceHost), DT_CHARPTR | DT_NORELOAD, ValidateNotEmpty},
		{"serverinfo", "pid", "services.pid", new ValueContainerChar(&Config.PIDFilename), DT_CHARPTR | DT_NORELOAD, ValidateNotEmpty},
		{"serverinfo", "motd", "services.motd", new ValueContainerChar(&Config.MOTDFilename), DT_CHARPTR, ValidateNotEmpty},
		{"networkinfo", "helpchannel", "", new ValueContainerChar(&Config.HelpChannel), DT_CHARPTR, NoValidation},
		{"networkinfo", "logchannel", "", new ValueContainerChar(&Config.LogChannel), DT_CHARPTR, NoValidation},
		{"networkinfo", "logbot", "no", new ValueContainerBool(&Config.LogBot), DT_BOOLEAN, NoValidation},
		{"networkinfo", "networkname", "", new ValueContainerChar(&Config.NetworkName), DT_CHARPTR, ValidateNotEmpty},
		{"networkinfo", "nicklen", "0", new ValueContainerUInt(&Config.NickLen), DT_UINTEGER | DT_NORELOAD, ValidateNickLen},
		{"options", "encryption", "", new ValueContainerString(&EncModules), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"options", "userkey1", "0", new ValueContainerLUInt(&Config.UserKey1), DT_LUINTEGER, NoValidation},
		{"options", "userkey2", "0", new ValueContainerLUInt(&Config.UserKey2), DT_LUINTEGER, NoValidation},
		{"options", "userkey3", "0", new ValueContainerLUInt(&Config.UserKey3), DT_LUINTEGER, NoValidation},
		{"options", "nobackupokay", "no", new ValueContainerBool(&Config.NoBackupOkay), DT_BOOLEAN, NoValidation},
		{"options", "strictpasswords", "no", new ValueContainerBool(&Config.StrictPasswords), DT_BOOLEAN, NoValidation},
		{"options", "badpasslimit", "0", new ValueContainerUInt(&Config.BadPassLimit), DT_UINTEGER, NoValidation},
		{"options", "badpasstimeout", "0", new ValueContainerTime(&Config.BadPassTimeout), DT_TIME, NoValidation},
		{"options", "updatetimeout", "0", new ValueContainerTime(&Config.UpdateTimeout), DT_TIME, ValidateNotZero},
		{"options", "expiretimeout", "0", new ValueContainerTime(&Config.ExpireTimeout), DT_TIME, ValidateNotZero},
		{"options", "readtimeout", "0", new ValueContainerTime(&Config.ReadTimeout), DT_TIME, ValidateNotZero},
		{"options", "warningtimeout", "0", new ValueContainerTime(&Config.WarningTimeout), DT_TIME, ValidateNotZero},
		{"options", "timeoutcheck", "0", new ValueContainerTime(&Config.TimeoutCheck), DT_TIME, NoValidation},
		{"options", "keeplogs", "0", new ValueContainerInt(&Config.KeepLogs), DT_INTEGER, NoValidation},
		{"options", "keepbackups", "0", new ValueContainerInt(&Config.KeepBackups), DT_INTEGER, NoValidation},
		{"options", "forceforbidreason", "no", new ValueContainerBool(&Config.ForceForbidReason), DT_BOOLEAN, NoValidation},
		{"options", "useprivmsg", "no", new ValueContainerBool(&Config.UsePrivmsg), DT_BOOLEAN, NoValidation},
		{"options", "usestrictprivmsg", "no", new ValueContainerBool(&Config.UseStrictPrivMsg), DT_BOOLEAN, NoValidation},
		{"options", "dumpcore", "yes", new ValueContainerBool(&Config.DumpCore), DT_BOOLEAN | DT_NORELOAD, NoValidation},
		{"options", "logusers", "no", new ValueContainerBool(&Config.LogUsers), DT_BOOLEAN, NoValidation},
		{"options", "hidestatso", "no", new ValueContainerBool(&Config.HideStatsO), DT_BOOLEAN, NoValidation},
		{"options", "globaloncycle", "no", new ValueContainerBool(&Config.GlobalOnCycle), DT_BOOLEAN, NoValidation},
		{"options", "globaloncycledown", "", new ValueContainerChar(&Config.GlobalOnCycleMessage), DT_CHARPTR, ValidateGlobalOnCycle},
		{"options", "globaloncycleup", "", new ValueContainerChar(&Config.GlobalOnCycleUP), DT_CHARPTR, ValidateGlobalOnCycle},
		{"options", "anonymousglobal", "no", new ValueContainerBool(&Config.AnonymousGlobal), DT_BOOLEAN, NoValidation},
		{"options", "nickregdelay", "0", new ValueContainerUInt(&Config.NickRegDelay), DT_UINTEGER, NoValidation},
		{"options", "restrictopernicks", "no", new ValueContainerBool(&Config.RestrictOperNicks), DT_BOOLEAN, NoValidation},
		{"options", "newscount", "3", new ValueContainerUInt(&Config.NewsCount), DT_UINTEGER, NoValidation},
		{"options", "ulineservers", "", new ValueContainerChar(&UlineServers), DT_CHARPTR, NoValidation},
		{"options", "enablelogchannel", "no", new ValueContainerBool(&LogChan), DT_BOOLEAN, NoValidation},
		{"nickserv", "nick", "NickServ", new ValueContainerChar(&Config.s_NickServ), DT_CHARPTR | DT_NORELOAD, ValidateNotEmpty},
		{"nickserv", "description", "Nickname Registration Service", new ValueContainerChar(&Config.desc_NickServ), DT_CHARPTR | DT_NORELOAD, ValidateNotEmpty},
		{"nickserv", "database", "nick.db", new ValueContainerChar(&Config.NickDBName), DT_CHARPTR, ValidateNotEmpty},
		{"nickserv", "emailregistration", "no", new ValueContainerBool(&Config.NSEmailReg), DT_BOOLEAN, NoValidation},
		{"nickserv", "prenickdatabase", "", new ValueContainerChar(&Config.PreNickDBName), DT_CHARPTR, ValidateEmailReg},
		{"nickserv", "modules", "", new ValueContainerString(&NickCoreModules), DT_STRING, NoValidation},
		{"nickserv", "forceemail", "no", new ValueContainerBool(&Config.NSForceEmail), DT_BOOLEAN, ValidateEmailReg},
		{"nickserv", "defaults", "secure memosignon memoreceive", new ValueContainerString(&NSDefaults), DT_STRING, NoValidation},
		{"nickserv", "defaultlanguage", "0", new ValueContainerUInt(&Config.NSDefLanguage), DT_UINTEGER, ValidateLanguage},
		{"nickserv", "regdelay", "0", new ValueContainerTime(&Config.NSRegDelay), DT_TIME, NoValidation},
		{"nickserv", "resenddelay", "0", new ValueContainerTime(&Config.NSResendDelay), DT_TIME, NoValidation},
		{"nickserv", "expire", "21d", new ValueContainerTime(&Config.NSExpire), DT_TIME, NoValidation},
		{"nickserv", "preregexpire", "0", new ValueContainerTime(&Config.NSRExpire), DT_TIME, ValidateEmailReg},
		{"nickserv", "maxaliases", "0", new ValueContainerInt(&Config.NSMaxAliases), DT_INTEGER, NoValidation},
		{"nickserv", "accessmax", "0", new ValueContainerUInt(&Config.NSAccessMax), DT_UINTEGER, ValidateNotZero},
		{"nickserv", "enforceruser", "", new ValueContainerChar(&temp_nsuserhost), DT_CHARPTR, ValidateNotEmpty},
		{"nickserv", "releasetimeout", "0", new ValueContainerTime(&Config.NSReleaseTimeout), DT_TIME, ValidateNotZero},
		{"nickserv", "allowkillimmed", "no", new ValueContainerBool(&Config.NSAllowKillImmed), DT_BOOLEAN | DT_NORELOAD, NoValidation},
		{"nickserv", "nogroupchange", "no", new ValueContainerBool(&Config.NSNoGroupChange), DT_BOOLEAN, NoValidation},
		{"nickserv", "listopersonly", "no", new ValueContainerBool(&Config.NSListOpersOnly), DT_BOOLEAN, NoValidation},
		{"nickserv", "listmax", "0", new ValueContainerUInt(&Config.NSListMax), DT_UINTEGER, ValidateNotZero},
		{"nickserv", "guestnickprefix", "", new ValueContainerChar(&Config.NSGuestNickPrefix), DT_CHARPTR, ValidateGuestPrefix},
		{"nickserv", "secureadmins", "no", new ValueContainerBool(&Config.NSSecureAdmins), DT_BOOLEAN, NoValidation},
		{"nickserv", "strictprivileges", "no", new ValueContainerBool(&Config.NSStrictPrivileges), DT_BOOLEAN, NoValidation},
		{"nickserv", "modeonid", "no", new ValueContainerBool(&Config.NSModeOnID), DT_BOOLEAN, NoValidation},
		{"nickserv", "addaccessonreg", "no", new ValueContainerBool(&Config.NSAddAccessOnReg), DT_BOOLEAN, NoValidation},
		{"mail", "usemail", "no", new ValueContainerBool(&Config.UseMail), DT_BOOLEAN, ValidateEmailReg},
		{"mail", "sendmailpath", "", new ValueContainerChar(&Config.SendMailPath), DT_CHARPTR, ValidateMail},
		{"mail", "sendfrom", "", new ValueContainerChar(&Config.SendFrom), DT_CHARPTR, ValidateMail},
		{"mail", "restrict", "no", new ValueContainerBool(&Config.RestrictMail), DT_BOOLEAN, NoValidation},
		{"mail", "delay", "0", new ValueContainerTime(&Config.MailDelay), DT_TIME, NoValidation},
		{"mail", "dontquoteaddresses", "no", new ValueContainerBool(&Config.DontQuoteAddresses), DT_BOOLEAN, NoValidation},
		{"chanserv", "nick", "ChanServ", new ValueContainerChar(&Config.s_ChanServ), DT_CHARPTR | DT_NORELOAD, ValidateNotEmpty},
		{"chanserv", "description", "Channel Registration Service", new ValueContainerChar(&Config.desc_ChanServ), DT_CHARPTR | DT_NORELOAD, ValidateNotEmpty},
		{"chanserv", "database", "chan.db", new ValueContainerChar(&Config.ChanDBName), DT_CHARPTR, ValidateNotEmpty},
		{"chanserv", "modules", "", new ValueContainerString(&ChanCoreModules), DT_STRING, NoValidation},
		{"chanserv", "defaults", "keeptopic secure securefounder signkick", new ValueContainerString(&CSDefaults), DT_STRING, NoValidation},
		{"chanserv", "maxregistered", "0", new ValueContainerUInt(&Config.CSMaxReg), DT_UINTEGER, NoValidation},
		{"chanserv", "expire", "14d", new ValueContainerTime(&Config.CSExpire), DT_TIME, NoValidation},
		{"chanserv", "defbantype", "2", new ValueContainerInt(&Config.CSDefBantype), DT_INTEGER, ValidateBantype},
		{"chanserv", "accessmax", "0", new ValueContainerUInt(&Config.CSAccessMax), DT_UINTEGER, ValidateNotZero},
		{"chanserv", "autokickmax", "0", new ValueContainerUInt(&Config.CSAutokickMax), DT_UINTEGER, ValidateNotZero},
		{"chanserv", "autokickreason", "User has been banned from the channel", new ValueContainerChar(&Config.CSAutokickReason), DT_CHARPTR, ValidateNotEmpty},
		{"chanserv", "inhabit", "0", new ValueContainerTime(&Config.CSInhabit), DT_TIME, ValidateNotZero},
		{"chanserv", "listopersonly", "no", new ValueContainerBool(&Config.CSListOpersOnly), DT_BOOLEAN, NoValidation},
		{"chanserv", "listmax", "0", new ValueContainerUInt(&Config.CSListMax), DT_UINTEGER, ValidateNotZero},
		{"chanserv", "opersonly", "no", new ValueContainerBool(&Config.CSOpersOnly), DT_BOOLEAN, NoValidation},
		{"memoserv", "nick", "MemoServ", new ValueContainerChar(&Config.s_MemoServ), DT_CHARPTR | DT_NORELOAD, ValidateNotEmpty},
		{"memoserv", "description", "Memo Service", new ValueContainerChar(&Config.desc_MemoServ), DT_CHARPTR | DT_NORELOAD, ValidateNotEmpty},
		{"memoserv", "modules", "", new ValueContainerString(&MemoCoreModules), DT_STRING, NoValidation},
		{"memoserv", "maxmemos", "0", new ValueContainerUInt(&Config.MSMaxMemos), DT_UINTEGER, NoValidation},
		{"memoserv", "senddelay", "0", new ValueContainerTime(&Config.MSSendDelay), DT_TIME, NoValidation},
		{"memoserv", "notifyall", "no", new ValueContainerBool(&Config.MSNotifyAll), DT_BOOLEAN, NoValidation},
		{"memoserv", "memoreceipt", "0", new ValueContainerUInt(&Config.MSMemoReceipt), DT_UINTEGER, NoValidation},
		{"botserv", "nick", "", new ValueContainerChar(&Config.s_BotServ), DT_CHARPTR | DT_NORELOAD, NoValidation},
		{"botserv", "description", "Bot Service", new ValueContainerChar(&Config.desc_BotServ), DT_CHARPTR | DT_NORELOAD, ValidateBotServ},
		{"botserv", "database", "bot.db", new ValueContainerChar(&Config.BotDBName), DT_CHARPTR, ValidateBotServ},
		{"botserv", "modules", "", new ValueContainerString(&BotCoreModules), DT_STRING, NoValidation},
		{"botserv", "defaults", "", new ValueContainerString(&BSDefaults), DT_STRING, NoValidation},
		{"botserv", "minusers", "0", new ValueContainerUInt(&Config.BSMinUsers), DT_UINTEGER, ValidateBotServ},
		{"botserv", "badwordsmax", "0", new ValueContainerUInt(&Config.BSBadWordsMax), DT_UINTEGER, ValidateBotServ},
		{"botserv", "keepdata", "0", new ValueContainerTime(&Config.BSKeepData), DT_TIME, ValidateBotServ},
		{"botserv", "smartjoin", "no", new ValueContainerBool(&Config.BSSmartJoin), DT_BOOLEAN, NoValidation},
		{"botserv", "gentlebadwordreason", "no", new ValueContainerBool(&Config.BSGentleBWReason), DT_BOOLEAN, NoValidation},
		{"botserv", "casesensitive", "no", new ValueContainerBool(&Config.BSCaseSensitive), DT_BOOLEAN, NoValidation},
		{"botserv", "fantasycharacter", "!", new ValueContainerChar(&Config.BSFantasyCharacter), DT_CHARPTR, NoValidation},
		{"hostserv", "nick", "", new ValueContainerChar(&Config.s_HostServ), DT_CHARPTR | DT_NORELOAD, NoValidation},
		{"hostserv", "description", "vHost Service", new ValueContainerChar(&Config.desc_HostServ), DT_CHARPTR | DT_NORELOAD, ValidateHostServ},
		{"hostserv", "database", "hosts.db", new ValueContainerChar(&Config.HostDBName), DT_CHARPTR, ValidateHostServ},
		{"hostserv", "modules", "", new ValueContainerString(&HostCoreModules), DT_STRING, NoValidation},
		{"operserv", "nick", "OperServ", new ValueContainerChar(&Config.s_OperServ), DT_CHARPTR | DT_NORELOAD, ValidateNotEmpty},
		{"operserv", "description", "Operator Service", new ValueContainerChar(&Config.desc_OperServ), DT_CHARPTR | DT_NORELOAD, ValidateNotEmpty},
		{"operserv", "globalnick", "Global", new ValueContainerChar(&Config.s_GlobalNoticer), DT_CHARPTR | DT_NORELOAD, ValidateNotEmpty},
		{"operserv", "globaldescription", "Global Noticer", new ValueContainerChar(&Config.desc_GlobalNoticer), DT_CHARPTR | DT_NORELOAD, ValidateNotEmpty},
		{"operserv", "database", "oper.db", new ValueContainerChar(&Config.OperDBName), DT_CHARPTR, ValidateNotEmpty},
		{"operserv", "newsdatabase", "news.db", new ValueContainerChar(&Config.NewsDBName), DT_CHARPTR, ValidateNotEmpty},
		{"operserv", "exceptiondatabase", "exception.db", new ValueContainerChar(&Config.ExceptionDBName), DT_CHARPTR, ValidateNotEmpty},
		{"operserv", "modules", "", new ValueContainerString(&OperCoreModules), DT_STRING, NoValidation},
		{"operserv", "superadmin", "no", new ValueContainerBool(&Config.SuperAdmin), DT_BOOLEAN, NoValidation},
		{"operserv", "logmaxusers", "no", new ValueContainerBool(&Config.LogMaxUsers), DT_BOOLEAN, NoValidation},
		{"operserv", "autokillexpiry", "0", new ValueContainerTime(&Config.AutokillExpiry), DT_TIME, ValidateNotZero},
		{"operserv", "chankillexpiry", "0", new ValueContainerTime(&Config.ChankillExpiry), DT_TIME, ValidateNotZero},
		{"operserv", "sglineexpiry", "0", new ValueContainerTime(&Config.SGLineExpiry), DT_TIME, ValidateNotZero},
		{"operserv", "sqlineexpiry", "0", new ValueContainerTime(&Config.SQLineExpiry), DT_TIME, ValidateNotZero},
		{"operserv", "szlineexpiry", "0", new ValueContainerTime(&Config.SZLineExpiry), DT_TIME, ValidateNotZero},
		{"operserv", "akillonadd", "no", new ValueContainerBool(&Config.AkillOnAdd), DT_BOOLEAN, NoValidation},
		{"operserv", "killonsgline", "no", new ValueContainerBool(&Config.KillonSGline), DT_BOOLEAN, NoValidation},
		{"operserv", "killonsqline", "no", new ValueContainerBool(&Config.KillonSQline), DT_BOOLEAN, NoValidation},
		{"operserv", "notifications", "", new ValueContainerString(&OSNotifications), DT_STRING, NoValidation},
		{"operserv", "limitsessions", "no", new ValueContainerBool(&Config.LimitSessions), DT_BOOLEAN, NoValidation},
		{"operserv", "defaultsessionlimit", "0", new ValueContainerUInt(&Config.DefSessionLimit), DT_UINTEGER, NoValidation},
		{"operserv", "maxsessionlimit", "0", new ValueContainerUInt(&Config.MaxSessionLimit), DT_UINTEGER, ValidateLimitSessions},
		{"operserv", "exceptionexpiry", "0", new ValueContainerTime(&Config.ExceptionExpiry), DT_TIME, ValidateLimitSessions},
		{"operserv", "sessionlimitexceeded", "", new ValueContainerChar(&Config.SessionLimitExceeded), DT_CHARPTR, NoValidation},
		{"operserv", "sessionlimitdetailsloc", "", new ValueContainerChar(&Config.SessionLimitDetailsLoc), DT_CHARPTR, NoValidation},
		{"operserv", "maxsessionkill", "0", new ValueContainerInt(&Config.MaxSessionKill), DT_INTEGER, NoValidation},
		{"operserv", "sessionautokillexpiry", "0", new ValueContainerTime(&Config.SessionAutoKillExpiry), DT_TIME, NoValidation},
		{"operserv", "addakiller", "no", new ValueContainerBool(&Config.AddAkiller), DT_BOOLEAN, NoValidation},
		{"operserv", "opersonly", "no", new ValueContainerBool(&Config.OSOpersOnly), DT_BOOLEAN, NoValidation},
		{"defcon", "defaultlevel", "0", new ValueContainerInt(&DefConLevel), DT_INTEGER, ValidateDefCon},
		{"defcon", "level4", "", new ValueContainerString(&DefCon4), DT_STRING, ValidateDefCon},
		{"defcon", "level3", "", new ValueContainerString(&DefCon3), DT_STRING, ValidateDefCon},
		{"defcon", "level2", "", new ValueContainerString(&DefCon2), DT_STRING, ValidateDefCon},
		{"defcon", "level1", "", new ValueContainerString(&DefCon1), DT_STRING, ValidateDefCon},
		{"defcon", "sessionlimit", "0", new ValueContainerInt(&Config.DefConSessionLimit), DT_INTEGER, ValidateDefCon},
		{"defcon", "akillexpire", "0", new ValueContainerTime(&Config.DefConAKILL), DT_TIME, ValidateDefCon},
		{"defcon", "chanmodes", "", new ValueContainerChar(&Config.DefConChanModes), DT_CHARPTR, ValidateDefCon},
		{"defcon", "timeout", "0", new ValueContainerTime(&Config.DefConTimeOut), DT_TIME, NoValidation},
		{"defcon", "globalondefcon", "no", new ValueContainerBool(&Config.GlobalOnDefcon), DT_BOOLEAN, NoValidation},
		{"defcon", "globalondefconmore", "no", new ValueContainerBool(&Config.GlobalOnDefconMore), DT_BOOLEAN, NoValidation},
		{"defcon", "message", "", new ValueContainerChar(&Config.DefconMessage), DT_CHARPTR, ValidateDefCon},
		{"defcon", "offmessage", "", new ValueContainerChar(&Config.DefConOffMessage), DT_CHARPTR, NoValidation},
		{"defcon", "akillreason", "", new ValueContainerChar(&Config.DefConAkillReason), DT_CHARPTR, ValidateDefCon},
		{NULL, NULL, NULL, NULL, DT_NOTHING, NoValidation}
	};

	/* These tags can occur multiple times, and therefore they have special code to read them
	 * which is different to the code for reading the singular tags listed above. */
	MultiConfig MultiValues[] = {
		{"uplink",
			{"host", "port", "password", NULL},
			{"", "0", "", NULL},
			{DT_HOSTNAME | DT_NORELOAD, DT_UINTEGER | DT_NORELOAD, DT_NOSPACES | DT_NORELOAD},
			InitUplinks, DoUplink, DoneUplinks},
		{"module",
			{"name", NULL},
			{"", NULL},
			{DT_CHARPTR},
			InitModules, DoModule, DoneModules},
		{"opertype",
			{"name", "commands", "privs", NULL},
			{"", "", "", NULL},
			{DT_CHARPTR, DT_CHARPTR, DT_CHARPTR},
			InitOperTypes, DoOperType, DoneOperTypes},
		{"oper",
			{"name", "type", NULL},
			{"", "", NULL},
			{DT_CHARPTR, DT_CHARPTR},
			InitOpers, DoOper, DoneOpers},
		{NULL,
			{NULL},
			{NULL},
			{0},
			NULL, NULL, NULL}
	};

	// Load and parse the config file, if there are any errors then explode
	// Make a copy here so if it fails then we can carry on running with an unaffected config
	newconfig.clear();
	if (LoadConf(newconfig, SERVICES_CONF, errstr))
		// If we succeeded, set the config to the new one
		config_data = newconfig;
	else
	{
		ReportConfigError(errstr.str(), bail);
		return 0;
	}
	/* This boolean is set to true when the Values array is completely iterated through, to avoid needing
	 * to do so inside the catch block to clean up the new'd values from the array. */
	bool CheckedAllValues = false;
	// The stuff in here may throw ConfigException, be sure we're in a position to catch it.
	try
	{
		// Read the values of all the tags which occur once or not at all, and call their callbacks.
		for (int Index = 0; Values[Index].tag; ++Index)
		{
			char item[BUFSIZE];
			int dt = Values[Index].datatype;
			bool allow_newlines = dt & DT_ALLOW_NEWLINE, allow_wild = dt & DT_ALLOW_WILD, noreload = dt & DT_NORELOAD;
			dt &= ~DT_ALLOW_NEWLINE;
			dt &= ~DT_ALLOW_WILD;
			dt &= ~DT_NORELOAD;
			// If the value is set to not allow reloading and we are reloading (bail will be false), skip the item
			if (noreload && !bail)
			{
				delete Values[Index].val;
				Values[Index].val = NULL;
				continue;
			}

			ConfValue(config_data, Values[Index].tag, Values[Index].value, Values[Index].default_value, 0, item, BUFSIZE, allow_newlines);
			ValueItem vi(item);

			if (!Values[Index].validation_function(this, Values[Index].tag, Values[Index].value, vi))
				throw ConfigException("One or more values in your configuration file failed to validate. Please see your ircd.log for more information.");

			switch (dt)
			{
				case DT_NOSPACES:
					{
						ValueContainerChar *vcc = dynamic_cast<ValueContainerChar *>(Values[Index].val);
						ValidateNoSpaces(vi.GetString(), Values[Index].tag, Values[Index].value);
						vcc->Set(vi.GetString(), strlen(vi.GetString()) + 1);
					}
					break;
				case DT_HOSTNAME:
					{
						ValueContainerChar *vcc = dynamic_cast<ValueContainerChar *>(Values[Index].val);
						ValidateHostname(vi.GetString(), Values[Index].tag, Values[Index].value);
						vcc->Set(vi.GetString(), strlen(vi.GetString()) + 1);
					}
					break;
				case DT_IPADDRESS:
					{
						ValueContainerChar *vcc = dynamic_cast<ValueContainerChar *>(Values[Index].val);
						ValidateIP(vi.GetString(), Values[Index].tag, Values[Index].value, allow_wild);
						vcc->Set(vi.GetString(), strlen(vi.GetString()) + 1);
					}
					break;
				case DT_CHARPTR:
					{
						ValueContainerChar *vcc = dynamic_cast<ValueContainerChar *>(Values[Index].val);
						// Make sure we also copy the null terminator
						vcc->Set(vi.GetString(), strlen(vi.GetString()) + 1);
					}
					break;
				case DT_STRING:
					{
						ValueContainerString *vcs = dynamic_cast<ValueContainerString *>(Values[Index].val);
						vcs->Set(vi.GetValue());
					}
					break;
				case DT_INTEGER:
					{
						int val = vi.GetInteger();
						ValueContainerInt *vci = dynamic_cast<ValueContainerInt *>(Values[Index].val);
						vci->Set(&val, sizeof(int));
					}
					break;
				case DT_UINTEGER:
					{
						unsigned val = vi.GetInteger();
						ValueContainerUInt *vci = dynamic_cast<ValueContainerUInt *>(Values[Index].val);
						vci->Set(&val, sizeof(unsigned));
					}
					break;
				case DT_LUINTEGER:
					{
						unsigned long val = vi.GetInteger();
						ValueContainerLUInt *vci = dynamic_cast<ValueContainerLUInt *>(Values[Index].val);
						vci->Set(&val, sizeof(unsigned long));
					}
					break;
				case DT_TIME:
					{
						time_t time = dotime(vi.GetString());
						ValueContainerTime *vci = dynamic_cast<ValueContainerTime *>(Values[Index].val);
						vci->Set(&time, sizeof(time_t));
					}
					break;
				case DT_BOOLEAN:
					{
						bool val = vi.GetBool();
						ValueContainerBool *vcb = dynamic_cast<ValueContainerBool *>(Values[Index].val);
						vcb->Set(&val, sizeof(bool));
					}
					break;
				default:
					break;
			}
			// We're done with this now
			delete Values[Index].val;
			Values[Index].val = NULL;
		}
		CheckedAllValues = true;
		/* Read the multiple-tag items (class tags, connect tags, etc)
		 * and call the callbacks associated with them. We have three
		 * callbacks for these, a 'start', 'item' and 'end' callback. */
		for (int Index = 0; MultiValues[Index].tag; ++Index)
		{
			MultiValues[Index].init_function(this, MultiValues[Index].tag, bail);
			int number_of_tags = ConfValueEnum(config_data, MultiValues[Index].tag);
			for (int tagnum = 0; tagnum < number_of_tags; ++tagnum)
			{
				ValueList vl;
				vl.clear();
				for (int valuenum = 0; MultiValues[Index].items[valuenum]; ++valuenum)
				{
					int dt = MultiValues[Index].datatype[valuenum];
					bool allow_newlines =  dt & DT_ALLOW_NEWLINE, allow_wild = dt & DT_ALLOW_WILD, noreload = dt & DT_NORELOAD;
					dt &= ~DT_ALLOW_NEWLINE;
					dt &= ~DT_ALLOW_WILD;
					dt &= ~DT_NORELOAD;
					// If the value is set to not allow reloading and we are reloading (bail will be false), skip the item
					if (noreload && !bail)
						continue;
					switch (dt)
					{
						case DT_NOSPACES:
							{
								char item[BUFSIZE];
								if (ConfValue(config_data, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum, item, BUFSIZE, allow_newlines))
									vl.push_back(ValueItem(item));
								else
									vl.push_back(ValueItem(""));
								ValidateNoSpaces(vl[vl.size() - 1].GetString(), MultiValues[Index].tag, MultiValues[Index].items[valuenum]);
							}
							break;
						case DT_HOSTNAME:
							{
								char item[BUFSIZE];
								if (ConfValue(config_data, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum, item, BUFSIZE, allow_newlines))
									vl.push_back(ValueItem(item));
								else
									vl.push_back(ValueItem(""));
								ValidateHostname(vl[vl.size() - 1].GetString(), MultiValues[Index].tag, MultiValues[Index].items[valuenum]);
							}
							break;
						case DT_IPADDRESS:
							{
								char item[BUFSIZE];
								if (ConfValue(config_data, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum, item, BUFSIZE, allow_newlines))
									vl.push_back(ValueItem(item));
								else
									vl.push_back(ValueItem(""));
								ValidateIP(vl[vl.size() - 1].GetString(), MultiValues[Index].tag, MultiValues[Index].items[valuenum], allow_wild);
							}
							break;
						case DT_CHARPTR:
							{
								char item[BUFSIZE];
								if (ConfValue(config_data, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum, item, BUFSIZE, allow_newlines))
									vl.push_back(ValueItem(item));
								else
									vl.push_back(ValueItem(""));
							}
							break;
						case DT_STRING:
							{
								std::string item;
								if (ConfValue(config_data, std::string(MultiValues[Index].tag), std::string(MultiValues[Index].items[valuenum]), std::string(MultiValues[Index].items_default[valuenum]), tagnum, item, allow_newlines))
									vl.push_back(ValueItem(item));
								else
									vl.push_back(ValueItem(""));
							}
							break;
						case DT_INTEGER:
						case DT_UINTEGER:
						case DT_LUINTEGER:
							{
								int item = 0;
								if (ConfValueInteger(config_data, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum, item))
									vl.push_back(ValueItem(item));
								else
									vl.push_back(ValueItem(0));
							}
							break;
						case DT_TIME:
							{
								std::string item;
								if (ConfValue(config_data, std::string(MultiValues[Index].tag), std::string(MultiValues[Index].items[valuenum]), std::string(MultiValues[Index].items_default[valuenum]), tagnum, item, allow_newlines))
								{
#ifdef _WIN32
									long time = static_cast<long>(dotime(item.c_str()));
#else
									time_t time = dotime(item.c_str());
#endif
									vl.push_back(ValueItem(time));
								}
								else
									vl.push_back(ValueItem(0));
							}
							break;
						case DT_BOOLEAN:
							{
								bool item = ConfValueBool(config_data, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum);
								vl.push_back(ValueItem(item));
							}
					}
				}
				MultiValues[Index].validation_function(this, MultiValues[Index].tag, static_cast<const char **>(MultiValues[Index].items), vl, MultiValues[Index].datatype, bail);
			}
			MultiValues[Index].finish_function(this, MultiValues[Index].tag, bail);
		}
	}
	catch (ConfigException &ce)
	{
		ReportConfigError(ce.GetReason(), bail);
		if (!CheckedAllValues)
		{
			for (int Index = 0; Values[Index].tag; ++Index)
			{
				if (Values[Index].val)
					delete Values[Index].val;
			}
		}
		return 0;
	}
	if (debug)
		alog("End config");
	for (int Index = 0; Once[Index]; ++Index)
		if (!CheckOnce(Once[Index]))
			return 0;
	alog("Done reading configuration file.");
	return 1;
}

bool ServerConfig::LoadConf(ConfigDataHash &target, const char *filename, std::ostringstream &errorstream)
{
	std::string line, wordbuffer, section, itemname;
	std::ifstream conf(filename);
	int linenumber = 0;
	bool in_word = false, in_quote = false, in_ml_comment = false;
	KeyValList sectiondata;
	if (conf.fail())
	{
		errorstream << "File " << filename << " could not be opened." << std::endl;
		return false;
	}
	if (debug)
		alog("Start to read conf %s", filename);
	// Start reading characters...
	while (getline(conf, line))
	{
		++linenumber;
		unsigned c = 0, len = line.size();
		for (; c < len; ++c)
		{
			char ch = line[c];
			if (in_quote)
			{
				if (ch == '"')
				{
					in_quote = in_word = false;
					continue;
				}
				wordbuffer += ch;
				continue;
			}
			if (in_ml_comment)
			{
				if (ch == '*' && c + 1 < len && line[c + 1] == '/')
				{
					in_ml_comment = false;
					++c;
				}
				continue;
			}
			if (ch == '#' || (ch == '/' && c + 1 < len && line[c + 1] == '/'))
				break; // Line comment, ignore the rest of the line (much like this one!)
			else if (ch == '/' && c + 1 < len && line[c + 1] == '*')
			{
				// Multiline (or less than one line) comment
				in_ml_comment = true;
				++c;
				continue;
			}
			else if (ch == '"')
			{
				// Quotes are valid only in the value position
				if (section.empty() || itemname.empty())
				{
					errorstream << "Unexpected quoted string: " << filename << ":" << linenumber << std::endl;
					return false;
				}
				if (in_word || !wordbuffer.empty())
				{
					errorstream << "Unexpected quoted string (prior unhandled words): " << filename << ":" << linenumber << std::endl;
					return false;
				}
				in_quote = in_word = true;
				continue;
			}
			else if (ch == '=')
			{
				if (section.empty())
				{
					errorstream << "Config item outside of section (or stray '='): " << filename << ":" << linenumber << std::endl;
					return false;
				}
				if (!itemname.empty())
				{
					errorstream << "Stray '=' sign or item without value: " << filename << ":" << linenumber << std::endl;
					return false;
				}
				if (in_word)
					in_word = false;
				itemname = wordbuffer;
				wordbuffer.clear();
			}
			else if (ch == '{')
			{
				if (!section.empty())
				{
					errorstream << "Section inside another section: " << filename << ":" << linenumber << std::endl;
					return false;
				}
				if (wordbuffer.empty())
				{
					errorstream << "Section without a name or unexpected '{': " << filename << ":" << linenumber << std::endl;
					return false;
				}
				if (in_word)
					in_word = false;
				section = wordbuffer;
				wordbuffer.clear();
			}
			else if (ch == '}')
			{
				if (section.empty())
				{
					errorstream << "Stray '}': " << filename << ":" << linenumber << std::endl;
					return false;
				}
				if (!wordbuffer.empty() || !itemname.empty())
				{
					// this will allow for the construct:  section { key = value }
					// but will not allow for anything else, such as:  section { key = value; key = value }
					if (!sectiondata.empty())
					{
						errorstream << "Unexpected end of section: " << filename << ":" << linenumber << std::endl;
						return false;
					}
					// this is the same as the below section for testing if itemname is non-empty after the loop, but done inside it to allow the above construct
					if (debug)
						alog("ln %d EOL: s='%s' '%s' set to '%s'", linenumber, section.c_str(), itemname.c_str(), wordbuffer.c_str());
					sectiondata.push_back(KeyVal(itemname, wordbuffer));
					wordbuffer.clear();
					itemname.clear();
				}
				target.insert(std::pair<std::string, KeyValList>(section, sectiondata));
				section.clear();
				sectiondata.clear();
			}
			else if (ch == ';' || ch == '\r')
				continue; // Ignore
			else if (ch == ' ' || ch == '\t')
			{
				// Terminate word
				if (in_word)
					in_word = false;
			}
			else
			{
				if (!in_word && !wordbuffer.empty())
				{
					errorstream << "Unexpected word: " << filename << ":" << linenumber << std::endl;
					return false;
				}
				wordbuffer += ch;
				in_word = true;
			}
		}
		if (in_quote)
		{
			// Quotes can span multiple lines; all we need to do is go to the next line without clearing things
			wordbuffer += "\n";
			continue;
		}
		in_word = false;
		if (!itemname.empty())
		{
			if (wordbuffer.empty())
			{
				errorstream << "Item without value: " << filename << ":" << linenumber << std::endl;
				return false;
			}
			if (debug)
				alog("ln %d EOL: s='%s' '%s' set to '%s'", linenumber, section.c_str(), itemname.c_str(), wordbuffer.c_str());
			sectiondata.push_back(KeyVal(itemname, wordbuffer));
			wordbuffer.clear();
			itemname.clear();
		}
	}
	if (in_ml_comment)
	{
		errorstream << "Unterminated multiline comment at end of file: " << filename << std::endl;
		return false;
	}
	if (in_quote)
	{
		errorstream << "Unterminated quote at end of file: " << filename << std::endl;
		return false;
	}
	if (!itemname.empty() || !wordbuffer.empty())
	{
		errorstream << "Unexpected garbage at end of file: " << filename << std::endl;
		return false;
	}
	if (!section.empty())
	{
		errorstream << "Unterminated section at end of file: " << filename << std::endl;
		return false;
	}
	return true;
}

bool ServerConfig::LoadConf(ConfigDataHash &target, const std::string &filename, std::ostringstream &errorstream)
{
	return LoadConf(target, filename.c_str(), errorstream);
}

bool ServerConfig::ConfValue(ConfigDataHash &target, const char *tag, const char *var, int index, char *result, int length, bool allow_linefeeds)
{
	return ConfValue(target, tag, var, "", index, result, length, allow_linefeeds);
}

bool ServerConfig::ConfValue(ConfigDataHash &target, const char *tag, const char *var, const char *default_value, int index, char *result, int length, bool allow_linefeeds)
{
	std::string value;
	bool r = ConfValue(target, std::string(tag), std::string(var), std::string(default_value), index, value, allow_linefeeds);
	strlcpy(result, value.c_str(), length);
	return r;
}

bool ServerConfig::ConfValue(ConfigDataHash &target, const std::string &tag, const std::string &var, int index, std::string &result, bool allow_linefeeds)
{
	return ConfValue(target, tag, var, "", index, result, allow_linefeeds);
}

bool ServerConfig::ConfValue(ConfigDataHash &target, const std::string &tag, const std::string &var, const std::string &default_value, int index, std::string &result, bool allow_linefeeds)
{
	ConfigDataHash::size_type pos = index;
	if (pos < target.count(tag))
	{
		ConfigDataHash::iterator iter = target.find(tag);

		for (int i = 0; i < index; ++i)
			++iter;

		KeyValList::iterator j = iter->second.begin(), jend = iter->second.end();
		for (; j != jend; ++j)
		{
			if (j->first == var)
			{
				if (!allow_linefeeds && j->second.find('\n') != std::string::npos)
				{
					alog("Value of <%s:%s> contains a linefeed, and linefeeds in this value are not permitted -- stripped to spaces.", tag.c_str(), var.c_str());
					std::string::iterator n = j->second.begin(), nend = j->second.end();
					for (; n != nend; ++n)
						if (*n == '\n')
							*n = ' ';
				}
				else
				{
					result = j->second;
					return true;
				}
			}
		}
		if (!default_value.empty())
		{
			result = default_value;
			return true;
		}
	}
	else if (!pos)
	{
		if (!default_value.empty())
		{
			result = default_value;
			return true;
		}
	}
	return false;
}

bool ServerConfig::ConfValueInteger(ConfigDataHash &target, const char *tag, const char *var, int index, int &result)
{
	return ConfValueInteger(target, std::string(tag), std::string(var), "", index, result);
}

bool ServerConfig::ConfValueInteger(ConfigDataHash &target, const char *tag, const char *var, const char *default_value, int index, int &result)
{
	return ConfValueInteger(target, std::string(tag), std::string(var), std::string(default_value), index, result);
}

bool ServerConfig::ConfValueInteger(ConfigDataHash &target, const std::string &tag, const std::string &var, int index, int &result)
{
	return ConfValueInteger(target, tag, var, "", index, result);
}

bool ServerConfig::ConfValueInteger(ConfigDataHash &target, const std::string &tag, const std::string &var, const std::string &default_value, int index, int &result)
{
	std::string value;
	std::istringstream stream;
	bool r = ConfValue(target, tag, var, default_value, index, value);
	stream.str(value);
	if (!(stream >> result))
		return false;
	else
	{
		if (!value.empty())
		{
			if (value.substr(0, 2) == "0x")
			{
				char *endptr;

				value.erase(0, 2);
				result = strtol(value.c_str(), &endptr, 16);

				/* No digits found */
				if (endptr == value.c_str())
					return false;
			}
			else
			{
				char denominator = *(value.end() - 1);
				switch (toupper(denominator))
				{
					case 'K':
						// Kilobytes -> bytes
						result = result * 1024;
						break;
					case 'M':
						// Megabytes -> bytes
						result = result * 1048576;
						break;
					case 'G':
						// Gigabytes -> bytes
						result = result * 1073741824;
				}
			}
		}
	}
	return r;
}

bool ServerConfig::ConfValueBool(ConfigDataHash &target, const char *tag, const char *var, int index)
{
	return ConfValueBool(target, std::string(tag), std::string(var), "", index);
}

bool ServerConfig::ConfValueBool(ConfigDataHash &target, const char *tag, const char *var, const char *default_value, int index)
{
	return ConfValueBool(target, std::string(tag), std::string(var), std::string(default_value), index);
}

bool ServerConfig::ConfValueBool(ConfigDataHash &target, const std::string &tag, const std::string &var, int index)
{
	return ConfValueBool(target, tag, var, "", index);
}

bool ServerConfig::ConfValueBool(ConfigDataHash &target, const std::string &tag, const std::string &var, const std::string &default_value, int index)
{
	std::string result;
	if (!ConfValue(target, tag, var, default_value, index, result))
		return false;

	return result == "yes" || result == "true" || result == "1";
}

int ServerConfig::ConfValueEnum(ConfigDataHash &target, const char *tag)
{
	return target.count(tag);
}

int ServerConfig::ConfValueEnum(ConfigDataHash &target, const std::string &tag)
{
	return target.count(tag);
}

int ServerConfig::ConfVarEnum(ConfigDataHash &target, const char *tag, int index)
{
	return ConfVarEnum(target, std::string(tag), index);
}

int ServerConfig::ConfVarEnum(ConfigDataHash &target, const std::string &tag, int index)
{
	ConfigDataHash::size_type pos = index;

	if (pos < target.count(tag))
	{
		ConfigDataHash::const_iterator iter = target.find(tag);

		for (int i = 0; i < index; ++i)
			++iter;

		return iter->second.size();
	}

	return 0;
}

ValueItem::ValueItem(int value) : v("")
{
	std::stringstream n;
	n << value;
	v = n.str();
}

ValueItem::ValueItem(long value) : v("")
{
	std::stringstream n;
	n << value;
	v = n.str();
}

ValueItem::ValueItem(bool value) : v("")
{
	std::stringstream n;
	n << value;
	v = n.str();
}

ValueItem::ValueItem(const char *value) : v(value) { }

ValueItem::ValueItem(const std::string &value) : v(value) { }

void ValueItem::Set(const char *value)
{
	v = value;
}

void ValueItem::Set(const std::string &value)
{
	v = value;
}

void ValueItem::Set(int value)
{
	std::stringstream n;
	n << value;
	v = n.str();
}

int ValueItem::GetInteger()
{
	if (v.empty()) return 0;
	return atoi(v.c_str());
}

const char *ValueItem::GetString() const
{
	return v.c_str();
}

bool ValueItem::GetBool()
{
	return GetInteger() || v == "yes" || v == "true";
}

/*************************************************************************/

/* Print an error message to the log (and the console, if open). */

void error(int linenum, const char *message, ...)
{
	char buf[4096];
	va_list args;

	va_start(args, message);
	vsnprintf(buf, sizeof(buf), message, args);
	va_end(args);

	if (linenum)
		alog("%s:%d: %s", SERVICES_CONF, linenum, buf);
	else
		alog("%s: %s", SERVICES_CONF, buf);

	if (!nofork && isatty(2)) {
		if (linenum)
			fprintf(stderr, "%s:%d: %s\n", SERVICES_CONF, linenum, buf);
		else
			fprintf(stderr, "%s: %s\n", SERVICES_CONF, buf);
	}
}

/*************************************************************************/

#define CHECK(v) do {			\
	if (!v) {				\
	error(0, #v " missing");	\
	retval = 0;			\
	}					\
} while (0)

#define CHEK2(v,n) do {			\
	if (!v) {				\
	error(0, #n " missing");	\
	retval = 0;			\
	}					\
} while (0)

/* Read the entire configuration file.  If an error occurs while reading
 * the file or a required directive is not found, print and log an
 * appropriate error message and return 0; otherwise, return 1.
 *
 * If reload is 1, will reload the configuration file.
 *		--lara
 *
 */

int read_config(int reload)
{
	int retval = 1;
	char *s;
	int defconCount = 0;

	retval = Config.Read(reload ? false : true);
	if (!retval) return 0; // Temporary until most of the below is modified to use the new parser -- CyberBotX

	if (!reload) {
		if (Config.LocalHost) {
			std::list<Uplink *>::iterator curr_uplink = Config.Uplinks.begin(), end_uplink = Config.Uplinks.end();
			for (; curr_uplink != end_uplink; ++curr_uplink) {
				Uplink *this_uplink = *curr_uplink;
				if (!stricmp(Config.LocalHost, this_uplink->host) && Config.LocalPort == this_uplink->port) {
					printf("\n<serverinfo:localhost> matches an <uplink:host> entry (%s)\nand <serverinfo:localport> matches an <uplink:port> entry (%d).\nThis will fail, you must make sure they are different.\n", this_uplink->host, this_uplink->port);
					retval = 0;
				}
			}
		}
		// Just in case someone put something in for <serverinfo:localport> without defining <serverinfo:localhost> too
		else Config.LocalPort = 0;
	}

	if (temp_nsuserhost) {
		if (!(s = strchr(temp_nsuserhost, '@'))) {
			Config.NSEnforcerUser = temp_nsuserhost;
			Config.NSEnforcerHost = Config.ServiceHost;
		} else {
			*s++ = 0;
			Config.NSEnforcerUser = temp_nsuserhost;
			Config.NSEnforcerHost = s;
		}
	}

	if (NSDefaults.empty())
	{
		Config.NSDefFlags.SetFlag(NI_SECURE);
		Config.NSDefFlags.SetFlag(NI_MEMO_SIGNON);
		Config.NSDefFlags.SetFlag(NI_MEMO_RECEIVE);
	}
	else if (NSDefaults != "none") {
		bool hadAutoop = false;
		spacesepstream options(NSDefaults);
		std::string option;
		while (options.GetToken(option))
		{
			if (option == "kill") Config.NSDefFlags.SetFlag(NI_KILLPROTECT);
			else if (option == "killquick") Config.NSDefFlags.SetFlag(NI_KILL_QUICK);
			else if (option == "secure") Config.NSDefFlags.SetFlag(NI_SECURE);
			else if (option == "private") Config.NSDefFlags.SetFlag(NI_PRIVATE);
			else if (option == "msg") {
				if (!Config.UsePrivmsg) alog("msg in <nickserv:defaults> can only be used when UsePrivmsg is set");
				else Config.NSDefFlags.SetFlag(NI_MSG);
			}
			else if (option == "hideemail") Config.NSDefFlags.SetFlag(NI_HIDE_EMAIL);
			else if (option == "hideusermask") Config.NSDefFlags.SetFlag(NI_HIDE_MASK);
			else if (option == "hidequit") Config.NSDefFlags.SetFlag(NI_HIDE_QUIT);
			else if (option == "memosignon") Config.NSDefFlags.SetFlag(NI_MEMO_SIGNON);
			else if (option == "memoreceive") Config.NSDefFlags.SetFlag(NI_MEMO_RECEIVE);
			else if (option == "autoop") hadAutoop = true;
		}
		if (!hadAutoop) Config.NSDefFlags.SetFlag(NI_AUTOOP);
	}

	if (reload) {
		if ((Config.NSDefLanguage = langlist[Config.NSDefLanguage]) < 0)
			Config.NSDefLanguage = DEF_LANGUAGE;
	}

	if (Config.CSDefBantype < 0 || Config.CSDefBantype > 3) {
		error(0, "Value of CSDefBantype must be between 0 and 3 included");
		retval = 0;
	}

	if (CSDefaults.empty())
	{
		Config.CSDefFlags.SetFlag(CI_KEEPTOPIC);
		Config.CSDefFlags.SetFlag(CI_SECURE);
		Config.CSDefFlags.SetFlag(CI_SECUREFOUNDER);
		Config.CSDefFlags.SetFlag(CI_SIGNKICK);
	}
	else if (CSDefaults != "none") {
		spacesepstream options(CSDefaults);
		std::string option;
		while (options.GetToken(option)) {
			if (option == "keeptopic") Config.CSDefFlags.SetFlag(CI_KEEPTOPIC);
			else if (option == "topiclock") Config.CSDefFlags.SetFlag(CI_TOPICLOCK);
			else if (option == "private") Config.CSDefFlags.SetFlag(CI_PRIVATE);
			else if (option == "restricted") Config.CSDefFlags.SetFlag(CI_RESTRICTED);
			else if (option == "secure") Config.CSDefFlags.SetFlag(CI_SECURE);
			else if (option == "secureops") Config.CSDefFlags.SetFlag(CI_SECUREOPS);
			else if (option == "securefounder") Config.CSDefFlags.SetFlag(CI_SECUREFOUNDER);
			else if (option == "signkick") Config.CSDefFlags.SetFlag(CI_SIGNKICK);
			else if (option == "signkicklevel") Config.CSDefFlags.SetFlag(CI_SIGNKICK_LEVEL);
			else if (option == "opnotice") Config.CSDefFlags.SetFlag(CI_OPNOTICE);
			else if (option == "xop") Config.CSDefFlags.SetFlag(CI_XOP);
			else if (option == "peace") Config.CSDefFlags.SetFlag(CI_PEACE);
			else if (option == "persist") Config.CSDefFlags.SetFlag(CI_PERSIST);
		}
	}

	if (!BSDefaults.empty()) {
		spacesepstream options(BSDefaults);
		std::string option;
		while (options.GetToken(option)) {
			if (option == "dontkickops") Config.BSDefFlags.SetFlag(BS_DONTKICKOPS);
			else if (option == "dontkickvoices") Config.BSDefFlags.SetFlag(BS_DONTKICKVOICES);
			else if (option == "greet") Config.BSDefFlags.SetFlag(BS_GREET);
			else if (option == "fantasy") Config.BSDefFlags.SetFlag(BS_FANTASY);
			else if (option == "symbiosis") Config.BSDefFlags.SetFlag(BS_SYMBIOSIS);
		}
	}

	Config.WallOper = Config.WallBadOS = Config.WallOSGlobal = Config.WallOSMode = Config.WallOSClearmodes = Config.WallOSKick = Config.WallOSAkill = Config.WallOSSGLine = Config.WallOSSQLine =
	Config.WallOSSZLine = Config.WallOSNoOp = Config.WallOSJupe = Config.WallAkillExpire = Config.WallSGLineExpire = Config.WallSQLineExpire = Config.WallSZLineExpire = Config.WallExceptionExpire = Config.WallGetpass = Config.WallSetpass = Config.WallForbid =
	Config.WallDrop = false;
	if (!OSNotifications.empty()) {
		spacesepstream notifications(OSNotifications);
		std::string notice;
		while (notifications.GetToken(notice)) {
			if (notice == "oper") Config.WallOper = true;
			else if (notice == "bados") Config.WallBadOS = true;
			else if (notice == "osglobal") Config.WallOSGlobal = true;
			else if (notice == "osmode") Config.WallOSMode = true;
			else if (notice == "osclearmodes") Config.WallOSClearmodes = true;
			else if (notice == "oskick") Config.WallOSKick = true;
			else if (notice == "osakill") Config.WallOSAkill = true;
			else if (notice == "ossgline") Config.WallOSSGLine = true;
			else if (notice == "ossqline") Config.WallOSSQLine = true;
			else if (notice == "osszline") Config.WallOSSZLine = true;
			else if (notice == "osnoop") Config.WallOSNoOp = true;
			else if (notice == "osjupe") Config.WallOSJupe = true;
			else if (notice == "akillexpire") Config.WallAkillExpire = true;
			else if (notice == "sglineexpire") Config.WallSGLineExpire = true;
			else if (notice == "sqlineexpire") Config.WallSQLineExpire = true;
			else if (notice == "szlineexpire") Config.WallSZLineExpire = true;
			else if (notice == "exceptionexpire") Config.WallExceptionExpire = true;
			else if (notice == "getpass") Config.WallGetpass = true;
			else if (notice == "setpass") Config.WallSetpass = true;
			else if (notice == "forbid") Config.WallForbid = true;
			else if (notice == "drop") Config.WallDrop = true;
		}
	}

	/* Ulines */

	if (UlineServers) {
		Config.NumUlines = 0;

		s = strtok(UlineServers, " ");
		do {
			if (s) {
				Config.NumUlines++;
				Config.Ulines = static_cast<char **>(realloc(Config.Ulines, sizeof(char *) * Config.NumUlines));
				Config.Ulines[Config.NumUlines - 1] = sstrdup(s);
			}
		} while ((s = strtok(NULL, " ")));
	}

	/* Modules Autoload building... :P */
	Config.ModulesAutoLoad = BuildStringList(!Modules.empty() ? Modules : "");
	Config.EncModuleList = BuildStringList(!EncModules.empty() ? EncModules : "");
	Config.HostServCoreModules = BuildStringList(!HostCoreModules.empty() ? HostCoreModules : "");
	Config.MemoServCoreModules = BuildStringList(!MemoCoreModules.empty() ? MemoCoreModules : "");
	Config.BotServCoreModules = BuildStringList(!BotCoreModules.empty() ? BotCoreModules : "");
	Config.OperServCoreModules = BuildStringList(!OperCoreModules.empty() ? OperCoreModules : "");
	Config.ChanServCoreModules = BuildStringList(!ChanCoreModules.empty() ? ChanCoreModules : "");
	Config.NickServCoreModules = BuildStringList(!NickCoreModules.empty() ? NickCoreModules : "");

	if (Config.LimitSessions) {
		if (Config.MaxSessionKill && !Config.SessionAutoKillExpiry)
			Config.SessionAutoKillExpiry = 1800;	/* 30 minutes */
	}

	if (Config.s_BotServ) {
		if (!Config.BSFantasyCharacter || !*Config.BSFantasyCharacter)
			Config.BSFantasyCharacter = sstrdup("!");
		if (*Config.BSFantasyCharacter && (strlen(Config.BSFantasyCharacter) > 1)) {
			printf
				("*** Config.BSFantasyCharacter is more than 1 character long. Only the first\n"
				 "*** character ('%c') will be used. The others will be ignored.\n",
				 *Config.BSFantasyCharacter);
		}
	}

	/* Check the user keys */
	if ((Config.UserKey1 == Config.UserKey2) || (Config.UserKey1 == Config.UserKey3)
		|| (Config.UserKey3 == Config.UserKey2))
		alog("Every UserKey must be different. It's for YOUR safety! Remember that!");

	/**
	 * Check all DEFCON dependiencies...
	 **/
	if (Config.DefConLevel) {
		/* Build DefCon's */
		DefCon.reserve(6);
		DefCon[5].reset();
		for (unsigned int level = 1; level < 5; ++level) {
			DefCon[level] = 0;
			std::string *levelDefinition = NULL;
			switch (level) {
				case 1:
					levelDefinition = &DefCon1;
					break;
				case 2:
					levelDefinition = &DefCon2;
					break;
				case 3:
					levelDefinition = &DefCon3;
					break;
				case 4:
					levelDefinition = &DefCon4;
			}
			spacesepstream operations(*levelDefinition);
			std::string operation;
			while (operations.GetToken(operation)) {
				if (operation == "nonewchannels") AddDefCon(level, DEFCON_NO_NEW_CHANNELS);
				else if (operation == "nonewnicks") AddDefCon(level, DEFCON_NO_NEW_NICKS);
				else if (operation == "nomlockchanges") AddDefCon(level, DEFCON_NO_MLOCK_CHANGE); 
				else if (operation == "forcechanmodes") AddDefCon(level, DEFCON_FORCE_CHAN_MODES);
				else if (operation == "reducedsessions") AddDefCon(level, DEFCON_REDUCE_SESSION);
				else if (operation == "nonewclients") AddDefCon(level, DEFCON_NO_NEW_CLIENTS);
				else if (operation == "operonly") AddDefCon(level, DEFCON_OPER_ONLY);
				else if (operation == "silentoperonly") AddDefCon(level, DEFCON_SILENT_OPER_ONLY);
				else if (operation == "akillnewclients") AddDefCon(level, DEFCON_AKILL_NEW_CLIENTS);
				else if (operation == "nonewmemos") AddDefCon(level, DEFCON_NO_NEW_MEMOS);
			}
		}

		/* Check any defcon needed settings */
		for (defconCount = 1; defconCount <= 5; defconCount++)
		{
			if (CheckDefCon(defconCount, DEFCON_REDUCE_SESSION))
			{
				CHECK(Config.DefConSessionLimit);
			}
			if (CheckDefCon(defconCount, DEFCON_AKILL_NEW_CLIENTS))
			{
				CHECK(Config.DefConAKILL);
				CHECK(Config.DefConAkillReason);
			}
			if (CheckDefCon(defconCount, DEFCON_FORCE_CHAN_MODES))
			{
				CHECK(Config.DefConChanModes);
			}
		}
	}

	/**
	 * If they try to enable any email registration option,
	 * make sure they have everything else they need too...
	 *
	 * rob
	 **/
	if (!Config.NSEmailReg) {
		delete [] Config.PreNickDBName;
		Config.PreNickDBName = NULL;
		Config.NSRExpire = 0;
	}

	if (!retval) {
		printf
			("\n*** Support resources: Read through the services.conf self-contained \n*** documentation. Read the documentation files found in the 'docs' \n*** folder. Visit our portal located at http://www.anope.org/. Join \n*** our support channel on /server irc.anope.org channel #anope.\n\n");
	}

	return retval;
}


/*************************************************************************/
