/* Configuration file handling.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "config.h"
#include "hashcomp.h" // If this gets added to services.h or someplace else later, remove it from here -- CyberBotX

/*************************************************************************/

Anope::string services_conf = "services.conf"; // Services configuration file name
ServerConfig *Config = NULL;

static Anope::string Modules;
static Anope::string EncModules;
static Anope::string DBModules;
static Anope::string HostCoreModules;
static Anope::string MemoCoreModules;
static Anope::string BotCoreModules;
static Anope::string OperCoreModules;
static Anope::string NickCoreModules;
static Anope::string ChanCoreModules;
static Anope::string DefCon1;
static Anope::string DefCon2;
static Anope::string DefCon3;
static Anope::string DefCon4;
static Anope::string UlineServers;
static Anope::string OSNotifications;
static Anope::string BSDefaults;
static Anope::string CSDefaults;
static Anope::string temp_nsuserhost;
static Anope::string NSDefaults;

/*************************************************************************/

ServerConfig::ServerConfig() : errstr(""), config_data()
{
	this->Read();

	if (!temp_nsuserhost.empty())
	{
		size_t at = temp_nsuserhost.find('@');
		if (at == Anope::string::npos)
		{
			this->NSEnforcerUser = temp_nsuserhost;
			this->NSEnforcerHost = this->ServiceHost;
		}
		else
		{
			this->NSEnforcerUser = temp_nsuserhost.substr(0, at);
			this->NSEnforcerHost = temp_nsuserhost.substr(at + 1);
		}
	}

	if (NSDefaults.empty())
	{
		this->NSDefFlags.SetFlag(NI_SECURE);
		this->NSDefFlags.SetFlag(NI_MEMO_SIGNON);
		this->NSDefFlags.SetFlag(NI_MEMO_RECEIVE);
	}
	else if (!NSDefaults.equals_ci("none"))
	{
		spacesepstream options(NSDefaults);
		Anope::string option;
		while (options.GetToken(option))
		{
			if (option.equals_ci("kill"))
				this->NSDefFlags.SetFlag(NI_KILLPROTECT);
			else if (option.equals_ci("killquick"))
				this->NSDefFlags.SetFlag(NI_KILL_QUICK);
			else if (option.equals_ci("secure"))
				this->NSDefFlags.SetFlag(NI_SECURE);
			else if (option.equals_ci("private"))
				this->NSDefFlags.SetFlag(NI_PRIVATE);
			else if (option.equals_ci("msg"))
			{
				if (!this->UsePrivmsg)
					Log() << "msg in <nickserv:defaults> can only be used when UsePrivmsg is set";
				else
					this->NSDefFlags.SetFlag(NI_MSG);
			}
			else if (option.equals_ci("hideemail"))
				this->NSDefFlags.SetFlag(NI_HIDE_EMAIL);
			else if (option.equals_ci("hideusermask"))
				this->NSDefFlags.SetFlag(NI_HIDE_MASK);
			else if (option.equals_ci("hidequit"))
				this->NSDefFlags.SetFlag(NI_HIDE_QUIT);
			else if (option.equals_ci("memosignon"))
				this->NSDefFlags.SetFlag(NI_MEMO_SIGNON);
			else if (option.equals_ci("memoreceive"))
				this->NSDefFlags.SetFlag(NI_MEMO_RECEIVE);
			else if (option.equals_ci("autoop"))
				this->NSDefFlags.SetFlag(NI_AUTOOP);
		}
	}

	if ((this->NSDefLanguage = langlist[this->NSDefLanguage]) < 0)
		this->NSDefLanguage = DEF_LANGUAGE;

	if (this->CSDefBantype < 0 || this->CSDefBantype > 3)
	{
		throw ConfigException("Value of CSDefBantype must be between 0 and 3 included");
	}

	if (CSDefaults.empty())
	{
		this->CSDefFlags.SetFlag(CI_KEEPTOPIC);
		this->CSDefFlags.SetFlag(CI_SECURE);
		this->CSDefFlags.SetFlag(CI_SECUREFOUNDER);
		this->CSDefFlags.SetFlag(CI_SIGNKICK);
	}
	else if (!CSDefaults.equals_ci("none"))
	{
		spacesepstream options(CSDefaults);
		Anope::string option;
		while (options.GetToken(option))
		{
			if (option.equals_ci("keeptopic"))
				this->CSDefFlags.SetFlag(CI_KEEPTOPIC);
			else if (option.equals_ci("topiclock"))
				this->CSDefFlags.SetFlag(CI_TOPICLOCK);
			else if (option.equals_ci("private"))
				this->CSDefFlags.SetFlag(CI_PRIVATE);
			else if (option.equals_ci("restricted"))
				this->CSDefFlags.SetFlag(CI_RESTRICTED);
			else if (option.equals_ci("secure"))
				this->CSDefFlags.SetFlag(CI_SECURE);
			else if (option.equals_ci("secureops"))
				this->CSDefFlags.SetFlag(CI_SECUREOPS);
			else if (option.equals_ci("securefounder"))
				this->CSDefFlags.SetFlag(CI_SECUREFOUNDER);
			else if (option.equals_ci("signkick"))
				this->CSDefFlags.SetFlag(CI_SIGNKICK);
			else if (option.equals_ci("signkicklevel"))
				this->CSDefFlags.SetFlag(CI_SIGNKICK_LEVEL);
			else if (option.equals_ci("opnotice"))
				this->CSDefFlags.SetFlag(CI_OPNOTICE);
			else if (option.equals_ci("xop"))
				this->CSDefFlags.SetFlag(CI_XOP);
			else if (option.equals_ci("peace"))
				this->CSDefFlags.SetFlag(CI_PEACE);
			else if (option.equals_ci("persist"))
				this->CSDefFlags.SetFlag(CI_PERSIST);
		}
	}

	if (!BSDefaults.empty())
	{
		spacesepstream options(BSDefaults);
		Anope::string option;
		while (options.GetToken(option))
		{
			if (option.equals_ci("dontkickops"))
				this->BSDefFlags.SetFlag(BS_DONTKICKOPS);
			else if (option.equals_ci("dontkickvoices"))
				this->BSDefFlags.SetFlag(BS_DONTKICKVOICES);
			else if (option.equals_ci("greet"))
				this->BSDefFlags.SetFlag(BS_GREET);
			else if (option.equals_ci("fantasy"))
				this->BSDefFlags.SetFlag(BS_FANTASY);
			else if (option.equals_ci("symbiosis"))
				this->BSDefFlags.SetFlag(BS_SYMBIOSIS);
		}
	}

	this->WallOper = this->WallBadOS = this->WallOSGlobal = this->WallOSMode = this->WallOSClearmodes = this->WallOSKick = this->WallOSAkill = this->WallOSSNLine = this->WallOSSQLine =
		this->WallOSSZLine = this->WallOSNoOp = this->WallOSJupe = this->WallAkillExpire = this->WallSNLineExpire = this->WallSQLineExpire = this->WallSZLineExpire = this->WallExceptionExpire =
		this->WallGetpass = this->WallSetpass = this->WallForbid = this->WallDrop = false;
	if (!OSNotifications.empty())
	{
		spacesepstream notifications(OSNotifications);
		Anope::string notice;
		while (notifications.GetToken(notice))
		{
			if (notice.equals_ci("oper"))
				this->WallOper = true;
			else if (notice.equals_ci("bados"))
				this->WallBadOS = true;
			else if (notice.equals_ci("osglobal"))
				this->WallOSGlobal = true;
			else if (notice.equals_ci("osmode"))
				this->WallOSMode = true;
			else if (notice.equals_ci("osclearmodes"))
				this->WallOSClearmodes = true;
			else if (notice.equals_ci("oskick"))
				this->WallOSKick = true;
			else if (notice.equals_ci("osakill"))
				this->WallOSAkill = true;
			else if (notice.equals_ci("ossnline"))
				this->WallOSSNLine = true;
			else if (notice.equals_ci("ossqline"))
				this->WallOSSQLine = true;
			else if (notice.equals_ci("osszline"))
				this->WallOSSZLine = true;
			else if (notice.equals_ci("osnoop"))
				this->WallOSNoOp = true;
			else if (notice.equals_ci("osjupe"))
				this->WallOSJupe = true;
			else if (notice.equals_ci("akillexpire"))
				this->WallAkillExpire = true;
			else if (notice.equals_ci("snlineexpire"))
				this->WallSNLineExpire = true;
			else if (notice.equals_ci("sqlineexpire"))
				this->WallSQLineExpire = true;
			else if (notice.equals_ci("szlineexpire"))
				this->WallSZLineExpire = true;
			else if (notice.equals_ci("exceptionexpire"))
				this->WallExceptionExpire = true;
			else if (notice.equals_ci("getpass"))
				this->WallGetpass = true;
			else if (notice.equals_ci("setpass"))
				this->WallSetpass = true;
			else if (notice.equals_ci("forbid"))
				this->WallForbid = true;
			else if (notice.equals_ci("drop"))
				this->WallDrop = true;
		}
	}

	/* Ulines */
	if (!UlineServers.empty())
	{
		this->Ulines.clear();

		spacesepstream ulines(UlineServers);
		Anope::string uline;
		while (ulines.GetToken(uline))
			this->Ulines.push_back(uline);
	}

	/* Modules Autoload building... :P */
	this->ModulesAutoLoad = BuildStringList(Modules);
	this->EncModuleList = BuildStringList(EncModules);
	this->DBModuleList = BuildStringList(DBModules);
	this->HostServCoreModules = BuildStringList(HostCoreModules);
	this->MemoServCoreModules = BuildStringList(MemoCoreModules);
	this->BotServCoreModules = BuildStringList(BotCoreModules);
	this->OperServCoreModules = BuildStringList(OperCoreModules);
	this->ChanServCoreModules = BuildStringList(ChanCoreModules);
	this->NickServCoreModules = BuildStringList(NickCoreModules);

	if (this->LimitSessions)
	{
		if (this->MaxSessionKill && !this->SessionAutoKillExpiry)
			this->SessionAutoKillExpiry = 1800; /* 30 minutes */
	}

	if (!this->s_BotServ.empty())
	{
		if (this->BSFantasyCharacter.empty())
			this->BSFantasyCharacter = "!";
		if (this->BSFantasyCharacter.length() > 1)
			printf("*** this->BSFantasyCharacter is more than 1 character long. Only the first\n*** character ('%c') will be used. The others will be ignored.\n", this->BSFantasyCharacter[0]);
	}

	/* Check the user keys */
	if (this->UserKey1 == this->UserKey2 || this->UserKey1 == this->UserKey3 || this->UserKey3 == this->UserKey2)
		Log() << "Every UserKey must be different. It's for YOUR safety! Remember that!";

	/**
	 * Check all DEFCON dependiencies...
	 **/
	if (this->DefConLevel)
	{
		/* Build DefCon's */
		DefCon.resize(6);
		DefCon[5].reset();
		for (unsigned int level = 1; level < 5; ++level)
		{
			DefCon[level] = 0;
			Anope::string *levelDefinition = NULL;
			switch (level)
			{
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
			Anope::string operation;
			while (operations.GetToken(operation))
			{
				if (operation.equals_ci("nonewchannels"))
					AddDefCon(level, DEFCON_NO_NEW_CHANNELS);
				else if (operation.equals_ci("nonewnicks"))
					AddDefCon(level, DEFCON_NO_NEW_NICKS);
				else if (operation.equals_ci("nomlockchanges"))
					AddDefCon(level, DEFCON_NO_MLOCK_CHANGE);
				else if (operation.equals_ci("forcechanmodes"))
					AddDefCon(level, DEFCON_FORCE_CHAN_MODES);
				else if (operation.equals_ci("reducedsessions"))
					AddDefCon(level, DEFCON_REDUCE_SESSION);
				else if (operation.equals_ci("nonewclients"))
					AddDefCon(level, DEFCON_NO_NEW_CLIENTS);
				else if (operation.equals_ci("operonly"))
					AddDefCon(level, DEFCON_OPER_ONLY);
				else if (operation.equals_ci("silentoperonly"))
					AddDefCon(level, DEFCON_SILENT_OPER_ONLY);
				else if (operation.equals_ci("akillnewclients"))
					AddDefCon(level, DEFCON_AKILL_NEW_CLIENTS);
				else if (operation.equals_ci("nonewmemos"))
					AddDefCon(level, DEFCON_NO_NEW_MEMOS);
			}
		}

		/* Check any defcon needed settings */
		for (int defconCount = 1; defconCount <= 5; ++defconCount)
		{
			if (CheckDefCon(defconCount, DEFCON_REDUCE_SESSION))
			{
				if (!this->DefConSessionLimit)
				{
					throw ConfigException("this->DefConSessionLimit missing");
				}
			}
			if (CheckDefCon(defconCount, DEFCON_AKILL_NEW_CLIENTS))
			{
				if (!this->DefConAKILL)
				{
					throw ConfigException("this->DefConAKILL missing");
				}
				if (this->DefConAkillReason.empty())
				{
					throw ConfigException("this->DefConAkillReason missing");
				}
			}
			if (CheckDefCon(defconCount, DEFCON_FORCE_CHAN_MODES))
			{
				if (this->DefConChanModes.empty())
				{
					throw ConfigException("this->DefConChanModes missing");
				}
			}
		}
	}

	SetDefaultMLock(this);

	if (IsFile(this->NameServer))
	{
		std::ifstream f(this->NameServer.c_str());
		Anope::string server;
		bool success = false;

		while (f.is_open() && getline(f, server.str()))
		{
			if (server.find("nameserver ") == 0)
			{
				if (server.substr(11).is_pos_number_only())
				{
					this->NameServer = server.substr(11);
					Log(LOG_DEBUG) << "Nameserver set to " << this->NameServer;
					success = true;
					break;
				}
			}
		}

		if (f.is_open())
			f.close();

		if (!success)
		{
			Log() << "Unable to find nameserver, defaulting to 127.0.0.1";
			this->NameServer = "127.0.0.1";
		}
	}
}

bool ServerConfig::CheckOnce(const Anope::string &tag)
{
	int count = ConfValueEnum(config_data, tag);
	if (count > 1)
		throw ConfigException("You have more than one <" + tag + "> tag, this is not permitted.");
	if (count < 1)
		throw ConfigException("You have not defined a <" + tag + "> tag, this is required.");
	return true;
}

bool NoValidation(ServerConfig *, const Anope::string &, const Anope::string &, ValueItem &)
{
	return true;
}

void ServerConfig::ValidateNoSpaces(const Anope::string &p, const Anope::string &tag, const Anope::string &val) const
{
	for (Anope::string::const_iterator ptr = p.begin(), end = p.end(); ptr != end; ++ptr)
		if (*ptr == ' ')
			throw ConfigException("The value of <" + tag + ":" + val + "> cannot contain spaces");
}

/* NOTE: Before anyone asks why we're not using inet_pton for this, it is because inet_pton and friends do not return so much detail,
 * even in strerror(errno). They just return 'yes' or 'no' to an address without such detail as to whats WRONG with the address.
 * Because ircd users arent as technical as they used to be (;)) we are going to give more of a useful error message.
 */
void ServerConfig::ValidateIP(const Anope::string &p, const Anope::string &tag, const Anope::string &val, bool wild) const
{
	int num_dots = 0, num_seps = 0;
	bool not_numbers = false, not_hex = false;

	if (!p.empty())
	{
		if (p[0] == '.')
			throw ConfigException("The value of <" + tag + ":" + val + "> is not an IP address");

		for (Anope::string::const_iterator ptr = p.begin(), end = p.end(); ptr != end; ++ptr)
		{
			if (wild && (*ptr == '*' || *ptr == '?' || *ptr == '/'))
				continue;

			if (*ptr != ':' && *ptr != '.' && (*ptr < '0' || *ptr > '9'))
			{
				not_numbers = true;
				if (toupper(*ptr) < 'A' || toupper(*ptr) > 'F')
					not_hex = true;
			}
			switch (*ptr)
			{
				case ' ':
					throw ConfigException("The value of <" + tag + ":" + val + "> is not an IP address");
				case '.':
					++num_dots;
					break;
				case ':':
					++num_seps;
			}
		}
		if (num_dots > 3)
			throw ConfigException("The value of <" + tag + ":" + val + "> is an IPv4 address with too many fields!");

		if (num_seps > 8)
			throw ConfigException("The value of <" + tag + ":" + val + "> is an IPv6 address with too many fields!");

		if (!num_seps && num_dots < 3 && !wild)
			throw ConfigException("The value of <" + tag + ":" + val + "> looks to be a malformed IPv4 address");

		if (!num_seps && num_dots == 3 && not_numbers)
			throw ConfigException("The value of <" + tag + ":" + val + "> contains non-numeric characters in an IPv4 address");

		if (num_seps && not_hex)
			throw ConfigException("The value of <" + tag + ":" + val + "> contains non-hexdecimal characters in an IPv6 address");

		if (num_seps && num_dots != 3 && num_dots && !wild)
			throw ConfigException("The value of <" + tag + ":" + val + "> is a malformed IPv6 4in6 address");
	}
}

void ServerConfig::ValidateHostname(const Anope::string &p, const Anope::string &tag, const Anope::string &val) const
{
	if (p.equals_ci("localhost"))
		return;

	int num_dots = 0, num_seps = 0;
	if (!p.empty())
	{
		if (p[0] == '.')
			throw ConfigException("The value of <" + tag + ":" + val + "> is not a valid hostname");
		for (unsigned i = 0, end = p.length(); i < end; ++i)
		{
			switch (p[i])
			{
				case ' ':
					throw ConfigException("The value of <" + tag + ":" + val + "> is not a valid hostname");
				case '.':
					++num_dots;
					break;
				case ':':
					++num_seps;
					break;
			}
		}
		if (!num_dots && !num_seps)
			throw ConfigException("The value of <" + tag + ":" + val + "> is not a valid hostname");
	}
}

bool ValidateNotEmpty(ServerConfig *, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (data.GetValue().empty())
		throw ConfigException("The value for <" + tag + ":" + value + "> cannot be empty!");
	return true;
}

bool ValidateNotZero(ServerConfig *, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (!data.GetInteger() && !dotime(data.GetValue()))
		throw ConfigException("The value for <" + tag + ":" + value + "> must be non-zero!");
	return true;
}

bool ValidateEmailReg(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (config->NSEmailReg)
	{
		if (value.equals_ci("preregexpire"))
		{
			if (!data.GetInteger())
				throw ConfigException("The value for <" + tag + ":" + value + "> must be non-zero when e-mail registration are enabled!");
		}
		else
		{
			if (!data.GetBool())
				throw ConfigException("The value for <" + tag + ":" + value + "> must be set to yes when e-mail registrations are enabled!");
		}
	}
	return true;
}

bool ValidatePort(ServerConfig *, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	int port = data.GetInteger();
	if (!port)
		return true;
	if (port < 1 || port > 65535)
		throw ConfigException("The value for <" + tag + ":" + value + "> is not a value port, it must be between 1 and 65535!");
	return true;
}

bool ValidateLanguage(ServerConfig *, const Anope::string &, const Anope::string &, ValueItem &data)
{
	int language = data.GetInteger();
	if (language < 1 || language > USED_LANGS)
		throw ConfigException("The value for <nickserv:defaultlanguage> must be between 1 and " + stringify(USED_LANGS) + "!");
	data.Set(--language);
	return true;
}

bool ValidateGuestPrefix(ServerConfig *conf, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	ValidateNotEmpty(conf, tag, value, data);
	if (data.GetValue().length() > 21)
		throw ConfigException("The value for <nickserv:guestnickprefix> cannot exceed 21 characters in length!");
	return true;
}

bool ValidateBantype(ServerConfig *, const Anope::string &, const Anope::string &, ValueItem &data)
{
	int bantype = data.GetInteger();
	if (bantype < 0 || bantype > 3)
		throw ConfigException("The value for <chanserv:defbantype> must be between 0 and 3!");
	return true;
}

bool ValidateBotServ(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (!config->s_BotServ.empty())
	{
		if (value.equals_ci("description"))
		{
			if (data.GetValue().empty())
				throw ConfigException("The value for <" + tag + ":" + value + "> cannot be empty when BotServ is enabled!");
		}
		else if (value.equals_ci("badwordsmax") || value.equals_ci("keepdata"))
		{
			if (!data.GetInteger() && !dotime(data.GetValue()))
				throw ConfigException("The value for <" + tag + ":" + value + "> must be non-zero when BotServ is enabled!");
		}
		else if (value.equals_ci("minusers"))
		{
			if (data.GetInteger() < 0)
				throw ConfigException("The value for <" + tag + ":" + value + "> must be greater than or equal to zero!");
		}
	}
	return true;
}

bool ValidateHostServ(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (!config->s_HostServ.empty())
	{
		if (value.equals_ci("description"))
		{
			if (data.GetValue().empty())
				throw ConfigException("The value for <" + tag + ":" + value + "> cannot be empty when HostServ is enabled!");
		}
	}
	return true;
}

bool ValidateLimitSessions(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (config->LimitSessions)
	{
		if (value.equals_ci("maxsessionlimit") || value.equals_ci("exceptionexpiry"))
		{
			if (!data.GetInteger() && !dotime(data.GetValue()))
				throw ConfigException("The value for <" + tag + ":" + value + "> must be non-zero when session limiting is enabled!");
		}
	}
	return true;
}

bool ValidateDefCon(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (value.equals_ci("defaultlevel"))
	{
		int level = data.GetInteger();
		if (!level)
			return true;
		if (level > 5)
			throw ConfigException("The value for <defcon:defaultlevel> must be between 1 through 5 if you wish to use DefCon or 0 if you wish to disable it!");
	}
	else if (config->DefConLevel)
	{
		if ((value.substr(0, 5).equals_ci("level") && isdigit(value[5])) || value.equals_ci("chanmodes") || value.equals_ci("akillreason"))
		{
			if (data.GetValue().empty())
				throw ConfigException("The value for <" + tag + ":" + value + "> cannot be empty when DefCon is enabled!");
		}
		else if (value.equals_ci("message") && config->GlobalOnDefconMore)
		{
			if (data.GetValue().empty())
				throw ConfigException("The value for <defcon:message> cannot be empty when globalondefconmore is enabled!");
		}
		else if (value.equals_ci("sessionlimit") || value.equals_ci("akillexpire"))
		{
			if (!data.GetInteger() && !dotime(data.GetValue()))
				throw ConfigException("The value for <" + tag + ":" + value + "> must be non-zero when DefCon is enabled!");
		}
	}
	return true;
}

bool ValidateNickLen(ServerConfig *, const Anope::string &, const Anope::string &, ValueItem &data)
{
	int nicklen = data.GetInteger();
	if (!nicklen)
	{
		Log() << "You have not defined the <networkinfo:nicklen> directive. It is strongly";
		Log() << "adviced that you do configure this correctly in your services.conf";
		data.Set(31);
	}
	else if (nicklen < 1)
	{
		Log() << "<networkinfo:nicklen> has an invalid value; setting to 31";
		data.Set(31);
	}
	return true;
}

bool ValidateMail(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (config->UseMail)
	{
		if (value.equals_ci("sendmailpath") || value.equals_ci("sendfrom"))
		{
			if (data.GetValue().empty())
				throw ConfigException("The value for <" + tag + ":" + value + "> cannot be empty when e-mail is enabled!");
		}
	}
	return true;
}

bool ValidateGlobalOnCycle(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (config->GlobalOnCycle)
	{
		if (data.GetValue().empty())
		{
			Log() << "<" << tag << ":" << value << "> was undefined, disabling <options:globaloncycle>";
			config->GlobalOnCycle = false;
		}
	}
	return true;
}

bool InitUplinks(ServerConfig *config, const Anope::string &)
{
	if (!config->Uplinks.empty())
	{
		std::list<Uplink *>::iterator curr_uplink = config->Uplinks.begin(), end_uplink = config->Uplinks.end();
		for (; curr_uplink != end_uplink; ++curr_uplink)
			delete *curr_uplink;
	}
	config->Uplinks.clear();
	return true;
}

bool DoUplink(ServerConfig *config, const Anope::string &, const Anope::string *, ValueList &values, int *)
{
	// Validation variables
	Anope::string host = values[0].GetValue(), password = values[3].GetValue();
	int port = values[2].GetInteger();
	bool ipv6 = values[1].GetBool();
	ValueItem vi_host(host), vi_port(port), vi_password(password);
	// Validate the host to make sure it is not empty
	if (!ValidateNotEmpty(config, "uplink", "host", vi_host))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	// Validate the port to make sure it is a valid port
	if (!ValidatePort(config, "uplink", "port", vi_port))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	// Validate the password to make sure it is not empty
	if (!ValidateNotEmpty(config, "uplink", "password", vi_password))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	// If we get here, all the values are valid, we'll add it to the Uplinks list
	config->Uplinks.push_back(new Uplink(host, port, password, ipv6));
	return true;
}

bool DoneUplinks(ServerConfig *config, const Anope::string &)
{
	if (config->Uplinks.empty())
		throw ConfigException("You must define at least one uplink block!");
	return true;
}

static bool InitOperTypes(ServerConfig *config, const Anope::string &)
{
	for (std::list<OperType *>::iterator it = config->MyOperTypes.begin(), it_end = config->MyOperTypes.end(); it != it_end; ++it)
		delete *it;

	config->MyOperTypes.clear();
	return true;
}

static bool DoOperType(ServerConfig *config, const Anope::string &, const Anope::string *, ValueList &values, int *)
{
	Anope::string name = values[0].GetValue();
	Anope::string inherits = values[1].GetValue();
	Anope::string commands = values[2].GetValue();
	Anope::string privs = values[3].GetValue();

	ValueItem vi(name);
	if (!ValidateNotEmpty(config, "opertype", "name", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");

	OperType *ot = new OperType(name);

	Anope::string tok;
	spacesepstream cmdstr(commands);
	while (cmdstr.GetToken(tok))
		ot->AddCommand(tok);

	spacesepstream privstr(privs);
	while (privstr.GetToken(tok))
		ot->AddPriv(tok);

	commasepstream inheritstr(inherits);
	while (inheritstr.GetToken(tok))
	{
		/* Strip leading ' ' after , */
		if (tok.length() > 1 && tok[0] == ' ')
			tok.erase(tok.begin());
		for (std::list<OperType *>::iterator it = config->MyOperTypes.begin(), it_end = config->MyOperTypes.end(); it != it_end; ++it)
		{
			if ((*it)->GetName().equals_ci(tok))
			{
				Log() << "Inheriting commands and privs from " << (*it)->GetName() << " to " << ot->GetName();
				ot->Inherits(*it);
				break;
			}
		}
	}

	config->MyOperTypes.push_back(ot);
	return true;
}

static bool DoneOperTypes(ServerConfig *, const Anope::string &)
{
	return true;
}

/*************************************************************************/

static bool InitOpers(ServerConfig *config, const Anope::string &)
{
	for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
		it->second->ot = NULL;

	config->Opers.clear();

	return true;
}

static bool DoOper(ServerConfig *config, const Anope::string &, const Anope::string *, ValueList &values, int *)
{
	Anope::string name = values[0].GetValue();
	Anope::string type = values[1].GetValue();

	ValueItem vi(name);
	if (!ValidateNotEmpty(config, "oper", "name", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");

	ValueItem vi2(type);
	if (!ValidateNotEmpty(config, "oper", "type", vi2))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");

	config->Opers.push_back(std::make_pair(name, type));
	return true;
}

static bool DoneOpers(ServerConfig *config, const Anope::string &)
{
	for (std::list<std::pair<Anope::string, Anope::string> >::iterator it = config->Opers.begin(), it_end = config->Opers.end(); it != it_end; ++it)
	{
		Anope::string nick = it->first, type = it->second;

		NickAlias *na = findnick(nick);
		if (!na)
			// Nonexistant nick
			continue;

		if (!na->nc)
			throw CoreException("Nick with no core?");

		for (std::list<OperType *>::iterator tit = config->MyOperTypes.begin(), tit_end = config->MyOperTypes.end(); tit != tit_end; ++tit)
		{
			OperType *ot = *tit;
			if (ot->GetName().equals_ci(type))
			{
				Log() << "Tied oper " << na->nc->display << " to type " << type;
				na->nc->ot = ot;
			}
		}
	}
	return true;
}

/*************************************************************************/

bool InitModules(ServerConfig *, const Anope::string &)
{
	Modules.clear();
	return true;
}

bool DoModule(ServerConfig *conf, const Anope::string &, const Anope::string *, ValueList &values, int *)
{
	// First we validate that there was a name in the module block
	Anope::string module = values[0].GetValue();
	ValueItem vi(module);
	if (!ValidateNotEmpty(conf, "module", "name", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	// If the string isn't empty, add a space before we add the module name
	if (!Modules.empty())
		Modules += " ";
	// Add the module name to the string
	Modules += values[0].GetValue();
	return true;
}

bool DoneModules(ServerConfig *, const Anope::string &)
{
	return true;
}

bool InitLogs(ServerConfig *config, const Anope::string &)
{
	for (unsigned i = 0; i < config->LogInfos.size(); ++i)
	{
		LogInfo *l = config->LogInfos[i];

		for (std::list<Anope::string>::const_iterator sit = l->Targets.begin(), sit_end = l->Targets.end(); sit != sit_end; ++sit)
		{
			const Anope::string &target = *sit;

			if (target[0] == '#')
			{
				Channel *c = findchan(target);
				if (c && c->HasFlag(CH_LOGCHAN))
				{
					for (CUserList::const_iterator cit = c->users.begin(), cit_end = c->users.end(); cit != cit_end; ++cit)
					{
						UserContainer *uc = *cit;
						BotInfo *bi = findbot(uc->user->nick);

						if (bi && bi->HasFlag(BI_CORE))
						{
							bi->Part(c, "Reloading configuration");
						}
					}

					c->UnsetFlag(CH_PERSIST);
					c->UnsetFlag(CH_LOGCHAN);
					if (c->users.empty())
						delete c;
				}
			}
		}

		delete config->LogInfos[i];
	}
	config->LogInfos.clear();

	return true;
}

bool DoLogs(ServerConfig *config, const Anope::string &, const Anope::string *, ValueList &values, int *)
{
	//{"target", "source", "logage", "inhabit", "admin", "override", "commands", "servers", "channels", "users", "normal", "rawio", "debug"},
	Anope::string targets = values[0].GetValue();
	ValueItem vi(targets);
	if (!ValidateNotEmpty(config, "log", "target", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	
	Anope::string source = values[1].GetValue();
	int logage = values[2].GetInteger();
	bool inhabit = values[3].GetBool();
	Anope::string admin = values[4].GetValue();
	Anope::string override = values[5].GetValue();
	Anope::string commands = values[6].GetValue();
	Anope::string servers = values[7].GetValue();
	Anope::string channels = values[8].GetValue();
	Anope::string users = values[9].GetValue();
	bool normal = values[10].GetBool();
	bool rawio = values[11].GetBool();
	bool ldebug = values[12].GetBool();

	LogInfo *l = new LogInfo(logage, inhabit, normal, rawio, ldebug);
	l->Targets = BuildStringList(targets);
	l->Sources = BuildStringList(source);
	l->Admin = BuildStringList(admin);
	l->Override = BuildStringList(override);
	l->Commands = BuildStringList(commands);
	l->Servers = BuildStringList(servers);
	l->Channels = BuildStringList(channels);
	l->Users = BuildStringList(users);

	config->LogInfos.push_back(l);

	return true;
}

bool DoneLogs(ServerConfig *config, const Anope::string &)
{
	if (ircd)
		InitLogChannels(config);

	Log() << "Loaded " << config->LogInfos.size() << " log blocks";

	return true;
}

void ServerConfig::Read()
{
	errstr.clear();
	// These tags MUST occur and must ONLY occur once in the config file
	static const Anope::string Once[] = {"serverinfo", "networkinfo", "options", "nickserv", "chanserv", "memoserv", "operserv", ""};
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
		 * If you want to create a directive using a case-insensitive string:
		 * std::string blarg;
		 * {"tag", "value", "", new ValueContainerCIString(&blarg), DT_CISTRING, <validation>},
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
		 * DT_ALLOW_NEWLINE - Allows new line characters in DT_CHARPTR, DT_STRING, and DT_CISTRING
		 *
		 * We may need to add some other validation functions to handle certain things, we can handle that later.
		 * Any questions about these, w00t, feel free to ask. */
		{"serverinfo", "name", "", new ValueContainerString(&this->ServerName), DT_HOSTNAME | DT_NORELOAD, ValidateNotEmpty},
		{"serverinfo", "description", "", new ValueContainerString(&this->ServerDesc), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"serverinfo", "localhost", "", new ValueContainerString(&this->LocalHost), DT_HOSTNAME | DT_NORELOAD, NoValidation},
		{"serverinfo", "type", "", new ValueContainerString(&this->IRCDModule), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"serverinfo", "id", "", new ValueContainerString(&this->Numeric), DT_NOSPACES | DT_NORELOAD, NoValidation},
		{"serverinfo", "ident", "", new ValueContainerString(&this->ServiceUser), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"serverinfo", "hostname", "", new ValueContainerString(&this->ServiceHost), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"serverinfo", "pid", "services.pid", new ValueContainerString(&this->PIDFilename), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"serverinfo", "motd", "services.motd", new ValueContainerString(&this->MOTDFilename), DT_STRING, ValidateNotEmpty},
		{"networkinfo", "networkname", "", new ValueContainerString(&this->NetworkName), DT_STRING, ValidateNotEmpty},
		{"networkinfo", "nicklen", "0", new ValueContainerUInt(&this->NickLen), DT_UINTEGER | DT_NORELOAD, ValidateNickLen},
		{"networkinfo", "userlen", "10", new ValueContainerUInt(&this->UserLen), DT_UINTEGER | DT_NORELOAD, NoValidation},
		{"networkinfo", "hostlen", "64", new ValueContainerUInt(&this->HostLen), DT_UINTEGER | DT_NORELOAD, NoValidation},
		{"options", "encryption", "", new ValueContainerString(&EncModules), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"options", "passlen", "32", new ValueContainerUInt(&this->PassLen), DT_UINTEGER | DT_NORELOAD, NoValidation},
		{"options", "database", "", new ValueContainerString(&DBModules), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"options", "socketengine", "", new ValueContainerString(&this->SocketEngine), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"options", "userkey1", "0", new ValueContainerLUInt(&this->UserKey1), DT_LUINTEGER, NoValidation},
		{"options", "userkey2", "0", new ValueContainerLUInt(&this->UserKey2), DT_LUINTEGER, NoValidation},
		{"options", "userkey3", "0", new ValueContainerLUInt(&this->UserKey3), DT_LUINTEGER, NoValidation},
		{"options", "nobackupokay", "no", new ValueContainerBool(&this->NoBackupOkay), DT_BOOLEAN, NoValidation},
		{"options", "strictpasswords", "no", new ValueContainerBool(&this->StrictPasswords), DT_BOOLEAN, NoValidation},
		{"options", "badpasslimit", "0", new ValueContainerUInt(&this->BadPassLimit), DT_UINTEGER, NoValidation},
		{"options", "badpasstimeout", "0", new ValueContainerTime(&this->BadPassTimeout), DT_TIME, NoValidation},
		{"options", "updatetimeout", "0", new ValueContainerTime(&this->UpdateTimeout), DT_TIME, ValidateNotZero},
		{"options", "expiretimeout", "0", new ValueContainerTime(&this->ExpireTimeout), DT_TIME, ValidateNotZero},
		{"options", "readtimeout", "0", new ValueContainerTime(&this->ReadTimeout), DT_TIME, ValidateNotZero},
		{"options", "warningtimeout", "0", new ValueContainerTime(&this->WarningTimeout), DT_TIME, ValidateNotZero},
		{"options", "timeoutcheck", "0", new ValueContainerTime(&this->TimeoutCheck), DT_TIME, NoValidation},
		{"options", "keepbackups", "0", new ValueContainerInt(&this->KeepBackups), DT_INTEGER, NoValidation},
		{"options", "forceforbidreason", "no", new ValueContainerBool(&this->ForceForbidReason), DT_BOOLEAN, NoValidation},
		{"options", "useprivmsg", "no", new ValueContainerBool(&this->UsePrivmsg), DT_BOOLEAN, NoValidation},
		{"options", "usestrictprivmsg", "no", new ValueContainerBool(&this->UseStrictPrivMsg), DT_BOOLEAN, NoValidation},
		{"options", "hidestatso", "no", new ValueContainerBool(&this->HideStatsO), DT_BOOLEAN, NoValidation},
		{"options", "globaloncycle", "no", new ValueContainerBool(&this->GlobalOnCycle), DT_BOOLEAN, NoValidation},
		{"options", "globaloncycledown", "", new ValueContainerString(&this->GlobalOnCycleMessage), DT_STRING, ValidateGlobalOnCycle},
		{"options", "globaloncycleup", "", new ValueContainerString(&this->GlobalOnCycleUP), DT_STRING, ValidateGlobalOnCycle},
		{"options", "anonymousglobal", "no", new ValueContainerBool(&this->AnonymousGlobal), DT_BOOLEAN, NoValidation},
		{"options", "nickregdelay", "0", new ValueContainerUInt(&this->NickRegDelay), DT_UINTEGER, NoValidation},
		{"options", "restrictopernicks", "no", new ValueContainerBool(&this->RestrictOperNicks), DT_BOOLEAN, NoValidation},
		{"options", "newscount", "3", new ValueContainerUInt(&this->NewsCount), DT_UINTEGER, NoValidation},
		{"options", "ulineservers", "", new ValueContainerString(&UlineServers), DT_STRING, NoValidation},
		{"options", "mlock", "+nrt", new ValueContainerString(&this->MLock), DT_STRING, NoValidation},
		{"options", "botmodes", "", new ValueContainerString(&this->BotModes), DT_STRING, NoValidation},
		{"options", "maxretries", "10", new ValueContainerUInt(&this->MaxRetries), DT_UINTEGER, NoValidation},
		{"options", "retrywait", "60", new ValueContainerInt(&this->RetryWait), DT_INTEGER, ValidateNotZero},
		{"options", "hideprivilegedcommands", "no", new ValueContainerBool(&this->HidePrivilegedCommands), DT_BOOLEAN, NoValidation},
		{"nickserv", "nick", "NickServ", new ValueContainerString(&this->s_NickServ), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"nickserv", "description", "Nickname Registration Service", new ValueContainerString(&this->desc_NickServ), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"nickserv", "emailregistration", "no", new ValueContainerBool(&this->NSEmailReg), DT_BOOLEAN, NoValidation},
		{"nickserv", "modules", "", new ValueContainerString(&NickCoreModules), DT_STRING, NoValidation},
		{"nickserv", "forceemail", "no", new ValueContainerBool(&this->NSForceEmail), DT_BOOLEAN, ValidateEmailReg},
		{"nickserv", "defaults", "secure memosignon memoreceive", new ValueContainerString(&NSDefaults), DT_STRING, NoValidation},
		{"nickserv", "defaultlanguage", "0", new ValueContainerUInt(&this->NSDefLanguage), DT_UINTEGER, ValidateLanguage},
		{"nickserv", "regdelay", "0", new ValueContainerTime(&this->NSRegDelay), DT_TIME, NoValidation},
		{"nickserv", "resenddelay", "0", new ValueContainerTime(&this->NSResendDelay), DT_TIME, NoValidation},
		{"nickserv", "expire", "21d", new ValueContainerTime(&this->NSExpire), DT_TIME, NoValidation},
		{"nickserv", "preregexpire", "0", new ValueContainerTime(&this->NSRExpire), DT_TIME, ValidateEmailReg},
		{"nickserv", "maxaliases", "0", new ValueContainerInt(&this->NSMaxAliases), DT_INTEGER, NoValidation},
		{"nickserv", "accessmax", "0", new ValueContainerUInt(&this->NSAccessMax), DT_UINTEGER, ValidateNotZero},
		{"nickserv", "enforceruser", "", new ValueContainerString(&temp_nsuserhost), DT_STRING, ValidateNotEmpty},
		{"nickserv", "releasetimeout", "0", new ValueContainerTime(&this->NSReleaseTimeout), DT_TIME, ValidateNotZero},
		{"nickserv", "allowkillimmed", "no", new ValueContainerBool(&this->NSAllowKillImmed), DT_BOOLEAN | DT_NORELOAD, NoValidation},
		{"nickserv", "nogroupchange", "no", new ValueContainerBool(&this->NSNoGroupChange), DT_BOOLEAN, NoValidation},
		{"nickserv", "listopersonly", "no", new ValueContainerBool(&this->NSListOpersOnly), DT_BOOLEAN, NoValidation},
		{"nickserv", "listmax", "0", new ValueContainerUInt(&this->NSListMax), DT_UINTEGER, ValidateNotZero},
		{"nickserv", "guestnickprefix", "", new ValueContainerString(&this->NSGuestNickPrefix), DT_STRING, ValidateGuestPrefix},
		{"nickserv", "secureadmins", "no", new ValueContainerBool(&this->NSSecureAdmins), DT_BOOLEAN, NoValidation},
		{"nickserv", "strictprivileges", "no", new ValueContainerBool(&this->NSStrictPrivileges), DT_BOOLEAN, NoValidation},
		{"nickserv", "modeonid", "no", new ValueContainerBool(&this->NSModeOnID), DT_BOOLEAN, NoValidation},
		{"nickserv", "addaccessonreg", "no", new ValueContainerBool(&this->NSAddAccessOnReg), DT_BOOLEAN, NoValidation},
		{"mail", "usemail", "no", new ValueContainerBool(&this->UseMail), DT_BOOLEAN, ValidateEmailReg},
		{"mail", "sendmailpath", "", new ValueContainerString(&this->SendMailPath), DT_STRING, ValidateMail},
		{"mail", "sendfrom", "", new ValueContainerString(&this->SendFrom), DT_STRING, ValidateMail},
		{"mail", "restrict", "no", new ValueContainerBool(&this->RestrictMail), DT_BOOLEAN, NoValidation},
		{"mail", "delay", "0", new ValueContainerTime(&this->MailDelay), DT_TIME, NoValidation},
		{"mail", "dontquoteaddresses", "no", new ValueContainerBool(&this->DontQuoteAddresses), DT_BOOLEAN, NoValidation},
		{"dns", "nameserver", "127.0.0.1", new ValueContainerString(&this->NameServer), DT_STRING, NoValidation},
		{"dns", "timeout", "5", new ValueContainerTime(&this->DNSTimeout), DT_TIME, NoValidation},
		{"chanserv", "nick", "ChanServ", new ValueContainerString(&this->s_ChanServ), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"chanserv", "description", "Channel Registration Service", new ValueContainerString(&this->desc_ChanServ), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"chanserv", "modules", "", new ValueContainerString(&ChanCoreModules), DT_STRING, NoValidation},
		{"chanserv", "defaults", "keeptopic secure securefounder signkick", new ValueContainerString(&CSDefaults), DT_STRING, NoValidation},
		{"chanserv", "maxregistered", "0", new ValueContainerUInt(&this->CSMaxReg), DT_UINTEGER, NoValidation},
		{"chanserv", "expire", "14d", new ValueContainerTime(&this->CSExpire), DT_TIME, NoValidation},
		{"chanserv", "defbantype", "2", new ValueContainerInt(&this->CSDefBantype), DT_INTEGER, ValidateBantype},
		{"chanserv", "accessmax", "0", new ValueContainerUInt(&this->CSAccessMax), DT_UINTEGER, ValidateNotZero},
		{"chanserv", "autokickmax", "0", new ValueContainerUInt(&this->CSAutokickMax), DT_UINTEGER, ValidateNotZero},
		{"chanserv", "autokickreason", "User has been banned from the channel", new ValueContainerString(&this->CSAutokickReason), DT_STRING, ValidateNotEmpty},
		{"chanserv", "inhabit", "0", new ValueContainerTime(&this->CSInhabit), DT_TIME, ValidateNotZero},
		{"chanserv", "listopersonly", "no", new ValueContainerBool(&this->CSListOpersOnly), DT_BOOLEAN, NoValidation},
		{"chanserv", "listmax", "0", new ValueContainerUInt(&this->CSListMax), DT_UINTEGER, ValidateNotZero},
		{"chanserv", "opersonly", "no", new ValueContainerBool(&this->CSOpersOnly), DT_BOOLEAN, NoValidation},
		{"memoserv", "nick", "MemoServ", new ValueContainerString(&this->s_MemoServ), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"memoserv", "description", "Memo Service", new ValueContainerString(&this->desc_MemoServ), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"memoserv", "modules", "", new ValueContainerString(&MemoCoreModules), DT_STRING, NoValidation},
		{"memoserv", "maxmemos", "0", new ValueContainerUInt(&this->MSMaxMemos), DT_UINTEGER, NoValidation},
		{"memoserv", "senddelay", "0", new ValueContainerTime(&this->MSSendDelay), DT_TIME, NoValidation},
		{"memoserv", "notifyall", "no", new ValueContainerBool(&this->MSNotifyAll), DT_BOOLEAN, NoValidation},
		{"memoserv", "memoreceipt", "0", new ValueContainerUInt(&this->MSMemoReceipt), DT_UINTEGER, NoValidation},
		{"botserv", "nick", "", new ValueContainerString(&this->s_BotServ), DT_STRING | DT_NORELOAD, NoValidation},
		{"botserv", "description", "Bot Service", new ValueContainerString(&this->desc_BotServ), DT_STRING | DT_NORELOAD, ValidateBotServ},
		{"botserv", "modules", "", new ValueContainerString(&BotCoreModules), DT_STRING, NoValidation},
		{"botserv", "defaults", "", new ValueContainerString(&BSDefaults), DT_STRING, NoValidation},
		{"botserv", "minusers", "0", new ValueContainerUInt(&this->BSMinUsers), DT_UINTEGER, ValidateBotServ},
		{"botserv", "badwordsmax", "0", new ValueContainerUInt(&this->BSBadWordsMax), DT_UINTEGER, ValidateBotServ},
		{"botserv", "keepdata", "0", new ValueContainerTime(&this->BSKeepData), DT_TIME, ValidateBotServ},
		{"botserv", "smartjoin", "no", new ValueContainerBool(&this->BSSmartJoin), DT_BOOLEAN, NoValidation},
		{"botserv", "gentlebadwordreason", "no", new ValueContainerBool(&this->BSGentleBWReason), DT_BOOLEAN, NoValidation},
		{"botserv", "casesensitive", "no", new ValueContainerBool(&this->BSCaseSensitive), DT_BOOLEAN, NoValidation},
		{"botserv", "fantasycharacter", "!", new ValueContainerString(&this->BSFantasyCharacter), DT_STRING, NoValidation},
		{"hostserv", "nick", "", new ValueContainerString(&this->s_HostServ), DT_STRING | DT_NORELOAD, NoValidation},
		{"hostserv", "description", "vHost Service", new ValueContainerString(&this->desc_HostServ), DT_STRING | DT_NORELOAD, ValidateHostServ},
		{"hostserv", "modules", "", new ValueContainerString(&HostCoreModules), DT_STRING, NoValidation},
		{"operserv", "nick", "OperServ", new ValueContainerString(&this->s_OperServ), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"operserv", "description", "Operator Service", new ValueContainerString(&this->desc_OperServ), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"operserv", "globalnick", "Global", new ValueContainerString(&this->s_GlobalNoticer), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"operserv", "globaldescription", "Global Noticer", new ValueContainerString(&this->desc_GlobalNoticer), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"operserv", "modules", "", new ValueContainerString(&OperCoreModules), DT_STRING, NoValidation},
		{"operserv", "superadmin", "no", new ValueContainerBool(&this->SuperAdmin), DT_BOOLEAN, NoValidation},
		{"operserv", "autokillexpiry", "0", new ValueContainerTime(&this->AutokillExpiry), DT_TIME, ValidateNotZero},
		{"operserv", "chankillexpiry", "0", new ValueContainerTime(&this->ChankillExpiry), DT_TIME, ValidateNotZero},
		{"operserv", "snlineexpiry", "0", new ValueContainerTime(&this->SNLineExpiry), DT_TIME, ValidateNotZero},
		{"operserv", "sqlineexpiry", "0", new ValueContainerTime(&this->SQLineExpiry), DT_TIME, ValidateNotZero},
		{"operserv", "szlineexpiry", "0", new ValueContainerTime(&this->SZLineExpiry), DT_TIME, ValidateNotZero},
		{"operserv", "akillonadd", "no", new ValueContainerBool(&this->AkillOnAdd), DT_BOOLEAN, NoValidation},
		{"operserv", "killonsnline", "no", new ValueContainerBool(&this->KillonSNline), DT_BOOLEAN, NoValidation},
		{"operserv", "killonsqline", "no", new ValueContainerBool(&this->KillonSQline), DT_BOOLEAN, NoValidation},
		{"operserv", "notifications", "", new ValueContainerString(&OSNotifications), DT_STRING, NoValidation},
		{"operserv", "limitsessions", "no", new ValueContainerBool(&this->LimitSessions), DT_BOOLEAN, NoValidation},
		{"operserv", "defaultsessionlimit", "0", new ValueContainerUInt(&this->DefSessionLimit), DT_UINTEGER, NoValidation},
		{"operserv", "maxsessionlimit", "0", new ValueContainerUInt(&this->MaxSessionLimit), DT_UINTEGER, ValidateLimitSessions},
		{"operserv", "exceptionexpiry", "0", new ValueContainerTime(&this->ExceptionExpiry), DT_TIME, ValidateLimitSessions},
		{"operserv", "sessionlimitexceeded", "", new ValueContainerString(&this->SessionLimitExceeded), DT_STRING, NoValidation},
		{"operserv", "sessionlimitdetailsloc", "", new ValueContainerString(&this->SessionLimitDetailsLoc), DT_STRING, NoValidation},
		{"operserv", "maxsessionkill", "0", new ValueContainerInt(&this->MaxSessionKill), DT_INTEGER, NoValidation},
		{"operserv", "sessionautokillexpiry", "0", new ValueContainerTime(&this->SessionAutoKillExpiry), DT_TIME, NoValidation},
		{"operserv", "addakiller", "no", new ValueContainerBool(&this->AddAkiller), DT_BOOLEAN, NoValidation},
		{"operserv", "opersonly", "no", new ValueContainerBool(&this->OSOpersOnly), DT_BOOLEAN, NoValidation},
		{"defcon", "defaultlevel", "0", new ValueContainerInt(&DefConLevel), DT_INTEGER, ValidateDefCon},
		{"defcon", "level4", "", new ValueContainerString(&DefCon4), DT_STRING, ValidateDefCon},
		{"defcon", "level3", "", new ValueContainerString(&DefCon3), DT_STRING, ValidateDefCon},
		{"defcon", "level2", "", new ValueContainerString(&DefCon2), DT_STRING, ValidateDefCon},
		{"defcon", "level1", "", new ValueContainerString(&DefCon1), DT_STRING, ValidateDefCon},
		{"defcon", "sessionlimit", "0", new ValueContainerInt(&this->DefConSessionLimit), DT_INTEGER, ValidateDefCon},
		{"defcon", "akillexpire", "0", new ValueContainerTime(&this->DefConAKILL), DT_TIME, ValidateDefCon},
		{"defcon", "chanmodes", "", new ValueContainerString(&this->DefConChanModes), DT_STRING, ValidateDefCon},
		{"defcon", "timeout", "0", new ValueContainerTime(&this->DefConTimeOut), DT_TIME, NoValidation},
		{"defcon", "globalondefcon", "no", new ValueContainerBool(&this->GlobalOnDefcon), DT_BOOLEAN, NoValidation},
		{"defcon", "globalondefconmore", "no", new ValueContainerBool(&this->GlobalOnDefconMore), DT_BOOLEAN, NoValidation},
		{"defcon", "message", "", new ValueContainerString(&this->DefconMessage), DT_STRING, ValidateDefCon},
		{"defcon", "offmessage", "", new ValueContainerString(&this->DefConOffMessage), DT_STRING, NoValidation},
		{"defcon", "akillreason", "", new ValueContainerString(&this->DefConAkillReason), DT_STRING, ValidateDefCon},
		{"", "", "", NULL, DT_NOTHING, NoValidation}
	};

	/* These tags can occur multiple times, and therefore they have special code to read them
	 * which is different to the code for reading the singular tags listed above. */
	MultiConfig MultiValues[] = {
		{"uplink",
			{"host", "ipv6", "port", "password", ""},
			{"", "no", "0", "", ""},
			{DT_HOSTNAME | DT_NORELOAD, DT_BOOLEAN | DT_NORELOAD, DT_UINTEGER | DT_NORELOAD, DT_NOSPACES | DT_NORELOAD},
			InitUplinks, DoUplink, DoneUplinks},
		{"module",
			{"name", ""},
			{"", ""},
			{DT_CHARPTR},
			InitModules, DoModule, DoneModules},
		{"log",
			{"target", "source", "logage", "inhabitlogchannel", "admin", "override", "commands", "servers", "channels", "users", "normal", "rawio", "debug", ""},
			{"", "", "7", "yes", "", "", "", "", "", "", "no", "no", ""},
			{DT_STRING, DT_STRING, DT_INTEGER, DT_BOOLEAN, DT_STRING, DT_STRING, DT_STRING, DT_STRING, DT_STRING, DT_STRING, DT_BOOLEAN, DT_BOOLEAN, DT_BOOLEAN},
			InitLogs, DoLogs, DoneLogs},
		{"opertype",
			{"name", "inherits", "commands", "privs", ""},
			{"", "", "", "", ""},
			{DT_CHARPTR, DT_CHARPTR, DT_CHARPTR, DT_CHARPTR},
			InitOperTypes, DoOperType, DoneOperTypes},
		{"oper",
			{"name", "type", ""},
			{"", "", ""},
			{DT_CHARPTR, DT_CHARPTR},
			InitOpers, DoOper, DoneOpers},
		{"",
			{""},
			{""},
			{0},
			NULL, NULL, NULL}
	};

	if (!LoadConf(this->config_data, services_conf))
		throw ConfigException(this->errstr.str());
	/* This boolean is set to true when the Values array is completely iterated through, to avoid needing
	 * to do so inside the catch block to clean up the new'd values from the array. */
	bool CheckedAllValues = false;
	// The stuff in here may throw ConfigException, be sure we're in a position to catch it.
	try
	{
		// Read the values of all the tags which occur once or not at all, and call their callbacks.
		for (int Index = 0; !Values[Index].tag.empty(); ++Index)
		{
			Anope::string item;
			int dt = Values[Index].datatype;
			bool allow_newlines = dt & DT_ALLOW_NEWLINE, allow_wild = dt & DT_ALLOW_WILD, noreload = dt & DT_NORELOAD;
			dt &= ~DT_ALLOW_NEWLINE;
			dt &= ~DT_ALLOW_WILD;
			dt &= ~DT_NORELOAD;

			ConfigDataHash &hash = (noreload && Config ? Config->config_data : this->config_data);
			ConfValue(hash, Values[Index].tag, Values[Index].value, Values[Index].default_value, 0, item, allow_newlines);
			ValueItem vi(item);

			if (!Values[Index].validation_function(this, Values[Index].tag, Values[Index].value, vi))
				throw ConfigException("One or more values in your configuration file failed to validate. Please see your logfiles for more information.");

			switch (dt)
			{
				case DT_NOSPACES:
					{
						ValueContainerString *vcs = debug_cast<ValueContainerString *>(Values[Index].val);
						ValidateNoSpaces(vi.GetValue(), Values[Index].tag, Values[Index].value);
						vcs->Set(vi.GetValue());
					}
					break;
				case DT_HOSTNAME:
					{
						ValueContainerString *vcs = debug_cast<ValueContainerString *>(Values[Index].val);
						ValidateHostname(vi.GetValue(), Values[Index].tag, Values[Index].value);
						vcs->Set(vi.GetValue());
					}
					break;
				case DT_IPADDRESS:
					{
						ValueContainerString *vcs = debug_cast<ValueContainerString *>(Values[Index].val);
						ValidateIP(vi.GetValue(), Values[Index].tag, Values[Index].value, allow_wild);
						vcs->Set(vi.GetValue());
					}
					break;
				case DT_CHARPTR:
					{
						ValueContainerChar *vcc = debug_cast<ValueContainerChar *>(Values[Index].val);
						// Make sure we also copy the null terminator
						vcc->Set(vi.GetString(), strlen(vi.GetString()) + 1);
					}
					break;
				case DT_CSSTRING:
					{
						ValueContainerCSString *vcs = debug_cast<ValueContainerCSString *>(Values[Index].val);
						vcs->Set(vi.GetCSValue());
					}
					break;
				case DT_CISTRING:
					{
						ValueContainerCIString *vcs = debug_cast<ValueContainerCIString *>(Values[Index].val);
						vcs->Set(vi.GetCIValue());
					}
					break;
				case DT_STRING:
					{
						ValueContainerString *vcs = debug_cast<ValueContainerString *>(Values[Index].val);
						vcs->Set(vi.GetValue());
					}
					break;
				case DT_INTEGER:
					{
						int val = vi.GetInteger();
						ValueContainerInt *vci = debug_cast<ValueContainerInt *>(Values[Index].val);
						vci->Set(&val, sizeof(int));
					}
					break;
				case DT_UINTEGER:
					{
						unsigned val = vi.GetInteger();
						ValueContainerUInt *vci = debug_cast<ValueContainerUInt *>(Values[Index].val);
						vci->Set(&val, sizeof(unsigned));
					}
					break;
				case DT_LUINTEGER:
					{
						unsigned long val = vi.GetInteger();
						ValueContainerLUInt *vci = debug_cast<ValueContainerLUInt *>(Values[Index].val);
						vci->Set(&val, sizeof(unsigned long));
					}
					break;
				case DT_TIME:
					{
						time_t time = dotime(vi.GetValue());
						ValueContainerTime *vci = debug_cast<ValueContainerTime *>(Values[Index].val);
						vci->Set(&time, sizeof(time_t));
					}
					break;
				case DT_BOOLEAN:
					{
						bool val = vi.GetBool();
						ValueContainerBool *vcb = debug_cast<ValueContainerBool *>(Values[Index].val);
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
		for (int Index = 0; !MultiValues[Index].tag.empty(); ++Index)
		{
			MultiValues[Index].init_function(this, MultiValues[Index].tag);
			int number_of_tags = ConfValueEnum(config_data, MultiValues[Index].tag);
			for (int tagnum = 0; tagnum < number_of_tags; ++tagnum)
			{
				ValueList vl;
				vl.clear();
				for (int valuenum = 0; !MultiValues[Index].items[valuenum].empty(); ++valuenum)
				{
					int dt = MultiValues[Index].datatype[valuenum];
					bool allow_newlines =  dt & DT_ALLOW_NEWLINE, allow_wild = dt & DT_ALLOW_WILD, noreload = dt & DT_NORELOAD;
					dt &= ~DT_ALLOW_NEWLINE;
					dt &= ~DT_ALLOW_WILD;
					dt &= ~DT_NORELOAD;

					ConfigDataHash &hash = (noreload && Config ? Config->config_data : this->config_data);
					switch (dt)
					{
						case DT_NOSPACES:
							{
								Anope::string item;
								if (ConfValue(hash, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum, item, allow_newlines))
									vl.push_back(ValueItem(item));
								else
									vl.push_back(ValueItem(""));
								ValidateNoSpaces(vl[vl.size() - 1].GetValue(), MultiValues[Index].tag, MultiValues[Index].items[valuenum]);
							}
							break;
						case DT_HOSTNAME:
							{
								Anope::string item;
								if (ConfValue(hash, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum, item, allow_newlines))
									vl.push_back(ValueItem(item));
								else
									vl.push_back(ValueItem(""));
								ValidateHostname(vl[vl.size() - 1].GetValue(), MultiValues[Index].tag, MultiValues[Index].items[valuenum]);
							}
							break;
						case DT_IPADDRESS:
							{
								Anope::string item;
								if (ConfValue(hash, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum, item, allow_newlines))
									vl.push_back(ValueItem(item));
								else
									vl.push_back(ValueItem(""));
								ValidateIP(vl[vl.size() - 1].GetValue(), MultiValues[Index].tag, MultiValues[Index].items[valuenum], allow_wild);
							}
							break;
						case DT_CHARPTR:
							{
								Anope::string item;
								if (ConfValue(hash, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum, item, allow_newlines))
									vl.push_back(ValueItem(item.c_str()));
								else
									vl.push_back(ValueItem(""));
							}
							break;
						case DT_CSSTRING:
							{
								Anope::string item;
								if (ConfValue(hash, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum, item, allow_newlines))
									vl.push_back(ValueItem(item));
								else
									vl.push_back(ValueItem(""));
							}
							break;
						case DT_CISTRING:
							{
								Anope::string item;
								if (ConfValue(hash, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum, item, allow_newlines))
									vl.push_back(ValueItem(item));
								else
									vl.push_back(ValueItem(""));
							}
							break;
						case DT_STRING:
							{
								Anope::string item;
								if (ConfValue(hash, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum, item, allow_newlines))
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
								if (ConfValueInteger(hash, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum, item))
									vl.push_back(ValueItem(item));
								else
									vl.push_back(ValueItem(0));
							}
							break;
						case DT_TIME:
							{
								Anope::string item;
								if (ConfValue(hash, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum, item, allow_newlines))
								{
#ifdef _WIN32
									long time = static_cast<long>(dotime(item));
#else
									time_t time = dotime(item);
#endif
									vl.push_back(ValueItem(time));
								}
								else
									vl.push_back(ValueItem(0));
							}
							break;
						case DT_BOOLEAN:
							{
								bool item = ConfValueBool(hash, MultiValues[Index].tag, MultiValues[Index].items[valuenum], MultiValues[Index].items_default[valuenum], tagnum);
								vl.push_back(ValueItem(item));
							}
					}
				}
				MultiValues[Index].validation_function(this, MultiValues[Index].tag, MultiValues[Index].items, vl, MultiValues[Index].datatype);
			}
			MultiValues[Index].finish_function(this, MultiValues[Index].tag);
		}
	}
	catch (const ConfigException &ce)
	{
		if (!CheckedAllValues)
		{
			for (int Index = 0; !Values[Index].tag.empty(); ++Index)
			{
				if (Values[Index].val)
					delete Values[Index].val;
			}
		}
		throw ConfigException(ce);
	}
	Log(LOG_DEBUG) << "End config";
	for (int Index = 0; !Once[Index].empty(); ++Index)
		CheckOnce(Once[Index]);
	Log() << "Done reading configuration file.";
}

bool ServerConfig::LoadConf(ConfigDataHash &target, const Anope::string &filename)
{
	Anope::string line;
	Anope::string section, wordbuffer, itemname;
	std::ifstream conf(filename.c_str());
	int linenumber = 0;
	bool in_word = false, in_quote = false, in_ml_comment = false;
	KeyValList sectiondata;
	if (conf.fail())
	{
		errstr << "File " << filename << " could not be opened." << std::endl;
		return false;
	}
	Log(LOG_DEBUG) << "Start to read conf " << filename;
	// Start reading characters...
	while (getline(conf, line.str()))
	{
		++linenumber;
		unsigned c = 0, len = line.length();
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
					errstr << "Unexpected quoted string: " << filename << ":" << linenumber << std::endl;
					return false;
				}
				if (in_word || !wordbuffer.empty())
				{
					errstr << "Unexpected quoted string (prior unhandled words): " << filename << ":" << linenumber << std::endl;
					return false;
				}
				in_quote = in_word = true;
				continue;
			}
			else if (ch == '=')
			{
				if (section.empty())
				{
					errstr << "Config item outside of section (or stray '='): " << filename << ":" << linenumber << std::endl;
					return false;
				}
				if (!itemname.empty())
				{
					errstr << "Stray '=' sign or item without value: " << filename << ":" << linenumber << std::endl;
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
					errstr << "Section inside another section: " << filename << ":" << linenumber << std::endl;
					return false;
				}
				if (wordbuffer.empty())
				{
					errstr << "Section without a name or unexpected '{': " << filename << ":" << linenumber << std::endl;
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
					errstr << "Stray '}': " << filename << ":" << linenumber << std::endl;
					return false;
				}
				if (!wordbuffer.empty() || !itemname.empty())
				{
					// this will allow for the construct:  section { key = value }
					// but will not allow for anything else, such as:  section { key = value; key = value }
					if (!sectiondata.empty())
					{
						errstr << "Unexpected end of section: " << filename << ":" << linenumber << std::endl;
						return false;
					}
					// this is the same as the below section for testing if itemname is non-empty after the loop, but done inside it to allow the above construct
					Log(LOG_DEBUG) << "ln "<< linenumber << " EOL: s='" << section << "' '" << itemname << "' set to '" << wordbuffer << "'";
					sectiondata.push_back(KeyVal(itemname, wordbuffer));
					wordbuffer.clear();
					itemname.clear();
				}
				target.insert(std::pair<Anope::string, KeyValList>(section, sectiondata));
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
					errstr << "Unexpected word: " << filename << ":" << linenumber << std::endl;
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
				errstr << "Item without value: " << filename << ":" << linenumber << std::endl;
				return false;
			}
			Log(LOG_DEBUG) << "ln " << linenumber << " EOL: s='" << section << "' '" << itemname << "' set to '" << wordbuffer << "'";
			sectiondata.push_back(KeyVal(itemname, wordbuffer));
			wordbuffer.clear();
			itemname.clear();
		}
	}
	if (in_ml_comment)
	{
		errstr << "Unterminated multiline comment at end of file: " << filename << std::endl;
		return false;
	}
	if (in_quote)
	{
		errstr << "Unterminated quote at end of file: " << filename << std::endl;
		return false;
	}
	if (!itemname.empty() || !wordbuffer.empty())
	{
		errstr << "Unexpected garbage at end of file: " << filename << std::endl;
		return false;
	}
	if (!section.empty())
	{
		errstr << "Unterminated section at end of file: " << filename << std::endl;
		return false;
	}
	return true;
}

bool ServerConfig::ConfValue(ConfigDataHash &target, const Anope::string &tag, const Anope::string &var, int index, Anope::string &result, bool allow_linefeeds)
{
	return ConfValue(target, tag, var, "", index, result, allow_linefeeds);
}

bool ServerConfig::ConfValue(ConfigDataHash &target, const Anope::string &tag, const Anope::string &var, const Anope::string &default_value, int index, Anope::string &result, bool allow_linefeeds)
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
			if (j->first.equals_ci(var))
			{
				if (!allow_linefeeds && j->second.find('\n') != Anope::string::npos)
				{
					Log(LOG_DEBUG) << "Value of <" << tag << ":" << var << "> contains a linefeed, and linefeeds in this value are not permitted -- stripped to spaces.";
					j->second.replace_all_cs("\n", " ");
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

bool ServerConfig::ConfValueInteger(ConfigDataHash &target, const Anope::string &tag, const Anope::string &var, int index, int &result)
{
	return ConfValueInteger(target, tag, var, "", index, result);
}

bool ServerConfig::ConfValueInteger(ConfigDataHash &target, const Anope::string &tag, const Anope::string &var, const Anope::string &default_value, int index, int &result)
{
	Anope::string value;
	std::istringstream stream;
	bool r = ConfValue(target, tag, var, default_value, index, value);
	stream.str(value.str());
	if (!(stream >> result))
		return false;
	else
	{
		if (!value.empty())
		{
			if (value.substr(0, 2).equals_ci("0x"))
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

bool ServerConfig::ConfValueBool(ConfigDataHash &target, const Anope::string &tag, const Anope::string &var, int index)
{
	return ConfValueBool(target, tag, var, "", index);
}

bool ServerConfig::ConfValueBool(ConfigDataHash &target, const Anope::string &tag, const Anope::string &var, const Anope::string &default_value, int index)
{
	Anope::string result;
	if (!ConfValue(target, tag, var, default_value, index, result))
		return false;

	return result.equals_ci("yes") || result.equals_ci("true") || result.equals_ci("1");
}

int ServerConfig::ConfValueEnum(const ConfigDataHash &target, const Anope::string &tag)
{
	return target.count(tag);
}

int ServerConfig::ConfVarEnum(ConfigDataHash &target, const Anope::string &tag, int index)
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

ValueItem::ValueItem(const ci::string &value) : v(value) { }

ValueItem::ValueItem(const Anope::string &value) : v(value) { }

void ValueItem::Set(const char *value)
{
	v = value;
}

void ValueItem::Set(const std::string &value)
{
	v = value;
}

void ValueItem::Set(const ci::string &value)
{
	v = value;
}

void ValueItem::Set(const Anope::string &value)
{
	v = value;
}

void ValueItem::Set(int value)
{
	std::stringstream n;
	n << value;
	v = n.str();
}

int ValueItem::GetInteger() const
{
	if (v.empty() || !v.is_number_only())
		return 0;
	return convertTo<int>(v);
}

const char *ValueItem::GetString() const
{
	return v.c_str();
}

bool ValueItem::GetBool() const
{
	return GetInteger() || v == "yes" || v == "true";
}


