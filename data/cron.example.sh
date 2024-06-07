/*
 * Example configuration file for Anope. After making the appropriate
 * changes to this file, place it in the Anope conf directory (as
 * specified in the "Config" script, default /home/username/anope/conf)
 * under the name "anope.conf".
 *
 * The format of this file is fairly simple: three types of comments are supported:
 *  - All text after a '#' on a line is ignored, as in shell scripting
 *  - All text after '//' on a line is ignored, as in C++
 *  - A block of text like this one is ignored, as in C
 *
 * Outside of comments, there are three structures: blocks, keys, and values.
 *
 * A block is a named container, which contains a number of key to value pairs
 * - you may think of this as an array.
 *
 * A block is created like so:
 * foobar
 * {
 *    moo = "cow"
 *    foo = bar
 * }
 *
 * Note that nameless blocks are allowed and are often used with comments to allow
 * easily commenting an entire block, for example:
 * #foobar
 * {
 *    moo = "cow"
 *    foo = bar
 * }
 * is an entirely commented block.
 *
 * Keys are case insensitive. Values depend on what key - generally, information is
 * given in the key comment. The quoting of values (and most other syntax) is quite
 * flexible, however, please do not forget to quote your strings:
 *
 *   "This is a parameter string with spaces in it"
 *
 * If you need to include a double quote inside a quoted string, precede it
 * by a backslash:
 *
 *   "This string has \"double quotes\" in it"
 *
 * Time parameters can be specified either as an integer representing a
 * number of seconds (e.g. "3600" = 1 hour), or as an integer with a unit
 * specifier: "s" = seconds, "m" = minutes, "h" = hours, "d" = days.
 * Combinations (such as "1h30m") are not permitted. Examples (all of which
 * represent the same length of time, one day):
 *
 *   "86400", "86400s", "1440m", "24h", "1d"
 *
 * In the documentation for each directive, one of the following will be
 * included to indicate whether an option is required:
 *
 * [REQUIRED]
 *     Indicates a directive which must be given. Without it, Anope will
 *     not start.
 *
 * [RECOMMENDED]
 *     Indicates a directive which may be omitted, but omitting it may cause
 *     undesirable side effects.
 *
 * [OPTIONAL]
 *     Indicates a directive which is optional. If not given, the feature
 *     will typically be disabled. If this is not the case, more
 *     information will be given in the documentation.
 *
 * [DISCOURAGED]
 *     Indicates a directive which may cause undesirable side effects if
 *     specified.
 *
 * [DEPRECATED]
 *     Indicates a directive which will disappear in a future version of
 *     Anope, usually because its functionality has been either
 *     superseded by that of other directives or incorporated into the main
 *     program.
 */

/*
 * [OPTIONAL] Defines
 *
 * You can define values to other values, which can be used to easily change
 * many values in the configuration at once.
 */

/*
 * The services.host define is used in multiple different locations throughout the
 * configuration for services clients hostnames.
 */
define
{
	name = "services.host"
	value = "services.example.com"
}

/*
 * [OPTIONAL] Additional Includes
 *
 * You can include additional configuration files here.
 * You may also include executable files, which will be executed and
 * the output from it will be included into your configuration.
 */

#include
{
	type = "file"
	name = "some.conf"
}

#include
{
	type = "executable"
	name = "/usr/bin/wget -q -O - https://some.misconfigured.network.com/anope.conf"
}

/*
 * [REQUIRED] IRCd Config
 *
 * This section is used to set up Anope to connect to your IRC network.
 * This section can be included multiple times, and Anope will attempt to
 * connect to each server until it finally connects.
 *
 * Each uplink IRCd should have a corresponding configuration to allow Anope
 * to link to it.
 *
 * An example configuration for InspIRCd that is compatible with the below uplink
 * and serverinfo configuration would look like:
 *
 *     # This goes in inspircd.conf, *NOT* your Anope config!
 *     <module name="hidechans">
 *     <module name="services_account">
 *     <module name="spanningtree">
 *     <bind address="127.0.0.1" port="7000" type="servers">
 *     <link name="services.example.com"
 *           ipaddr="127.0.0.1"
 *           port="7000"
 *           sendpass="mypassword"
 *           recvpass="mypassword">
 *     <uline server="services.example.com" silent="yes">
 *
 * An example configuration for UnrealIRCd that is compatible with the below uplink
 * and serverinfo configuration would look like:
 *
 *     // This goes in unrealircd.conf, *NOT* your Anope config!
 *     listen {
 *         ip 127.0.0.1;
 *         port 7000;
 *         options {
 *             serversonly;
 *         };
 *     };
 *     link services.example.com {
 *         incoming {
 *             mask *@127.0.0.1;
 *         };
 *         password "mypassword";
 *         class servers;
 *     };
 *     ulines { services.example.com; };
 */
uplink
{
	/*
	 * The IP address, hostname, or UNIX socket path of the IRC server you wish
	 * to connect Anope to.
	 * Usually, you will want to connect over 127.0.0.1 (aka localhost).
	 *
	 * NOTE: On some shell providers, this will not be an option.
	 */
	host = "127.0.0.1"

	/*
	 * The protocol that Anope should use when connecting to the uplink. Can
	 * be set to "ipv4" (the default), "ipv6", or "unix".
	 */
	protocol = "ipv4"

	/*
	 * Enable if Anope should connect using SSL.
	 * You must have an SSL module loaded for this to work.
	 */
	ssl = no

	/*
	 * The port to connect to.
	 * The IRCd *MUST* be configured to listen on this port, and to accept
	 * server connections.
	 *
	 * Refer to your IRCd documentation for how this is to be done.
	 */
	port = 7000

	/*
	 * The password to send to the IRC server for authentication.
	 * This must match the link block on your IRCd.
	 *
	 * Refer to your IRCd documentation for more information on link blocks.
	 */
	password = "mypassword"
}

/*
 * [REQUIRED] Server Information
 *
 * This section contains information about the services server.
 */
serverinfo
{
	/*
	 * The hostname that Anope will be seen as, it must have no conflicts with any
	 * other server names on the rest of your IRC network. Note that it does not have
	 * to be an existing hostname, just one that isn't on your network already.
	 */
	name = "services.example.com"

	/*
	 * The text which should appear as the server's information in /WHOIS and similar
	 * queries.
	 */
	description = "Anope IRC Services"

	/*
	 * The local address that Anope will bind to before connecting to the remote
	 * server. This may be useful for multihomed hosts. If omitted, Anope will let
	 * the Operating System choose the local address. This directive is optional.
	 *
	 * If you don't know what this means or don't need to use it, just leave this
	 * directive commented out.
	 */
	#localhost = "nowhere."

	/*
	 * What Server ID to use for this connection?
	 * Note: This should *ONLY* be used for TS6/P10 IRCds. Refer to your IRCd documentation
	 * to see if this is needed.
	 */
	#id = "00A"

	/*
	 * The filename containing the Anope process ID. The path is relative to the
	 * data directory.
	 */
	pid = "anope.pid"

	/*
	 * The filename containing the Message of the Day. The path is relative to the
	 * config directory.
	 */
	motd = "motd.txt"
}

/*
 * [REQUIRED] Protocol module
 *
 * This directive tells Anope which IRCd Protocol to speak when connecting.
 * You MUST modify this to match the IRCd you run.
 *
 * Supported:
 *  - bahamut
 *  - hybrid
 *  - inspircd
 *  - ngircd
 *  - plexus
 *  - ratbox
 *  - solanum
 *  - unrealircd
 */
module
{
	name = "inspircd"

	/*
	 * Some protocol modules can enforce mode locks server-side. This reduces the spam caused by
	 * services immediately reversing mode changes for locked modes.
	 *
	 * If the protocol module you have loaded does not support this, this setting will have no effect.
	 */
	use_server_side_mlock = yes

	/*
	 * Some protocol modules can enforce topic locks server-side. This reduces the spam caused by
	 * services immediately reversing topic changes.
	 *
	 * If the protocol module you have loaded does not support this, this setting will have no effect.
	 */
	use_server_side_topiclock = yes
}

/*
 * [REQUIRED] Network Information
 *
 * This section contains information about the IRC network that Anope will be
 * connecting to.
 */
networkinfo
{
	/*
	 * This is the name of the network that Anope will be running on.
	 */
	networkname = "LocalNet"

	/*
	 * Set this to the maximum allowed nick length on your network.
	 * Be sure to set this correctly, as setting this wrong can result in
	 * Anope being disconnected from the network. Defaults to 31.
	 */
	#nicklen = 31

	/* Set this to the maximum allowed ident length on your network.
	 * Be sure to set this correctly, as setting this wrong can result in
	 * Anope being disconnected from the network. Defaults to 10.
	 */
	#userlen = 10

	/* Set this to the maximum allowed hostname length on your network.
	 * Be sure to set this correctly, as setting this wrong can result in
	 * Anope being disconnected from the network. Defaults to 64.
	 */
	#hostlen = 64

	/* Set this to the maximum allowed channel length on your network.
	 * Defaults to 32.
	 */
	#chanlen = 32

	/* The maximum number of list modes settable on a channel (such as b, e, I).
	 * Set to 0 to disable. Defaults to 100.
	 */
	#modelistsize = 100

	/*
	 * Characters allowed in nicknames. This always includes the characters described
	 * in RFC1459, and so does not need to be set for normal behavior. Changing this to
	 * include characters your IRCd doesn't support will cause your IRCd and/or Anope
	 * to break. Multibyte characters are not supported, nor are escape sequences.
	 *
	 * It is recommended you DON'T change this.
	 */
	#nick_chars = ""

	/*
	 * The characters allowed in hostnames. This is used for validating hostnames given
	 * to services, such as BotServ bot hostnames and user vhosts. Changing this is not
	 * recommended unless you know for sure your IRCd supports whatever characters you are
	 * wanting to use. Telling services to set a vHost containing characters your IRCd
	 * disallows could potentially break the IRCd and/or Anope.
	 *
	 * It is recommended you DON'T change this.
	 */
	vhost_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-"

	/*
	 * If set to true, allows vHosts to not contain dots (.).
	 * Newer IRCds generally do not have a problem with this, but the same warning as
	 * vhost_chars applies.
	 *
	 * It is recommended you DON'T change this.
	 */
	allow_undotted_vhosts = false

	/*
	 * The characters that are not allowed to be at the very beginning or very ending
	 * of a vHost. The same warning as vhost_chars applies.
	 *
	 * It is recommended you DON'T change this.
	 */
	disallow_start_or_end = ".-"
}

/*
 * [REQUIRED] Anope Options
 *
 * This section contains various options which determine how Anope will operate.
 */
options
{
	/*
	 * On Linux/UNIX systems Anope can setuid and setgid to this user and group
	 * after starting up. This is useful if Anope has to bind to privileged ports.
	 */
	#user = "anope"
	#group = "anope"

	/*
	 * The case mapping used by services. This must be set to a valid locale name
	 * installed on your machine. Anope uses this case map to compare, with
	 * case insensitivity, things such as nick names, channel names, etc.
	 *
	 * We provide two special casemaps shipped with Anope, ascii and rfc1459.
	 *
	 * This value should be set to what your IRCd uses, which is probably rfc1459,
	 * however Anope has always used ascii for comparison, so the default is ascii.
	 *
	 * Changing this value once set is not recommended.
	 */
	casemap = "ascii"

	/*
	 * Sets the number of invalid password tries before services removes a user
	 * from the network. If a user enters a number of invalid passwords equal to
	 * the given amount for any services function or combination of functions
	 * during a single IRC session (subject to badpasstimeout, below), services
	 * will issues a /KILL for the user. If not given, services will ignore
	 * failed password attempts (though they will be logged in any case).
	 *
	 * This directive is optional, but recommended.
	 */
	badpasslimit = 5

	/*
	 * Sets the time after which invalid passwords are forgotten about. If a user
	 * does not enter any incorrect passwords in this amount of time, the incorrect
	 * password count will reset to zero. If not given, the timeout will be
	 * disabled, and the incorrect password count will never be reset until the user
	 * disconnects.
	 *
	 * This directive is optional.
	 */
	badpasstimeout = 1h

	/*
	 * Sets the delay between automatic database updates.
	 */
	updatetimeout = 5m

	/*
	 * Sets the delay between checks for expired nicknames and channels.
	 */
	expiretimeout = 30m

	/*
	 * Sets the timeout period for reading from the uplink.
	 */
	readtimeout = 5s

	/*
	 * Sets the interval between sending warning messages for program errors via
	 * WALLOPS/GLOBOPS.
	 */
	warningtimeout = 4h

	/*
	 * Sets the (maximum) frequency at which the timeout list is checked. This,
	 * combined with readtimeout above, determines how accurately timed events,
	 * such as nick kills, occur; it also determines how much CPU time services
	 * will use doing this. Higher values will cause less accurate timing but
	 * less CPU usage.
	 *
	 * Note that this value is not an absolute limit on the period between
	 * checks of the timeout list; the previous may be as great as readtimeout
	 * (above) during periods of inactivity.
	 *
	 * If this directive is not given, it will default to 0.
	 */
	timeoutcheck = 3s

	/*
	 * If set, this will allow users to let services send PRIVMSGs to them
	 * instead of NOTICEs. Also see the "msg" option of nickserv:defaults,
	 * which also toggles the default communication (PRIVMSG or NOTICE) to
	 * use for unregistered users.
	 *
	 * This is a feature that is against the IRC RFC and should be used ONLY
	 * if absolutely necessary.
	 *
	 * This directive is optional, and not recommended.
	 */
	#useprivmsg = yes

	/*
	 * If set, will force services to only respond to PRIVMSGs addresses to
	 * Nick@ServerName - e.g. NickServ@example.com. This should be used in
	 * conjunction with IRCd aliases. This directive is optional.
	 *
	 * This option will have no effect on some IRCds, such as TS6 IRCds.
	 */
	#usestrictprivmsg = yes

	/*
	 * If set, Anope will only show /stats o to IRC Operators. This directive
	 * is optional.
	 */
	#hidestatso = yes

	/*
	 * A space-separated list of U-lined servers on your network, it is assumed that
	 * the servers in this list are allowed to set channel modes and Anope will
	 * not attempt to reverse their mode changes.
	 *
	 * WARNING: Do NOT put your normal IRC user servers in this directive.
	 *
	 * This directive is optional.
	 */
	#ulineservers = "stats.your.network"

	/*
	 * How long to wait between connection retries with the uplink(s).
	 */
	retrywait = 60s

	/*
	 * If set, services will hide commands that users don't have the privilege to execute
	 * from HELP output.
	 */
	hideprivilegedcommands = yes

	/*
	 * If set, services will hide commands that users can't execute because they are not
	 * logged in from HELP output.
	 */
	hideregisteredcommands = yes

	/*
	 * If set, the maximum difference between an invalid and valid command name to allow
	 * as a suggestion. Defaults to 4.
	 */
	didyoumeandifference = 4

	/* The regex engine to use, as provided by the regex modules.
	 * Leave commented to disable regex matching.
	 *
	 * Note for this to work the regex module providing the regex engine must be loaded.
	 */
	#regexengine = "regex/stdlib"

	/*
	 * A list of languages to load on startup that will be available in /NICKSERV SET LANGUAGE.
	 * Useful if you translate Anope to your language. (Explained further in docs/LANGUAGE).
	 * Note that English should not be listed here because it is the base language.
	 *
	 * Removing .UTF-8 will instead use the default encoding for the language, e.g. iso-8859-1 for western European languages.
	 */
	languages = "ca_ES.UTF-8 de_DE.UTF-8 el_GR.UTF-8 es_ES.UTF-8 fr_FR.UTF-8 hu_HU.UTF-8 it_IT.UTF-8 nl_NL.UTF-8 pl_PL.UTF-8 pt_PT.UTF-8 ru_RU.UTF-8 tr_TR.UTF-8"

	/*
	 * Default language that non- and newly-registered nicks will receive messages in.
	 * Set to "en" to enable English. Defaults to the language the system uses.
	 */
	#defaultlanguage = "es_ES.UTF-8"
}

/*
 * [OPTIONAL] BotServ
 *
 * Includes botserv.example.conf, which is necessary for BotServ functionality.
 *
 * Remove this block to disable BotServ.
 */
include
{
	type = "file"
	name = "botserv.example.conf"
}

/*
 * [RECOMMENDED] ChanServ
 *
 * Includes chanserv.example.conf, which is necessary for ChanServ functionality.
 *
 * Remove this block to disable ChanServ.
 */
include
{
	type = "file"
	name = "chanserv.example.conf"
}

/*
 * [RECOMMENDED] Global
 *
 * Includes global.example.conf, which is necessary for Global functionality.
 *
 * Remove this block to disable Global.
 */
include
{
	type = "file"
	name = "global.example.conf"
}

/*
 * [OPTIONAL] HostServ
 *
 * Includes hostserv.example.conf, which is necessary for HostServ functionality.
 *
 * Remove this block to disable HostServ.
 */
include
{
	type = "file"
	name = "hostserv.example.conf"
}

/*
 * [OPTIONAL] MemoServ
 *
 * Includes memoserv.example.conf, which is necessary for MemoServ functionality.
 *
 * Remove this block to disable MemoServ.
 */
include
{
	type = "file"
	name = "memoserv.example.conf"
}

/*
 * [OPTIONAL] NickServ
 *
 * Includes nickserv.example.conf, which is necessary for NickServ functionality.
 *
 * Remove this block to disable NickServ.
 */
include
{
	type = "file"
	name = "nickserv.example.conf"
}

/*
 * [RECOMMENDED] OperServ
 *
 * Includes operserv.example.conf, which is necessary for OperServ functionality.
 *
 * Remove this block to disable OperServ.
 */
include
{
	type = "file"
	name = "operserv.example.conf"
}

/*
 * [RECOMMENDED] Logging Configuration
 *
 * This section is used for configuring what is logged and where it is logged to.
 * You may have multiple log blocks if you wish. Remember to properly secure any
 * channels you choose to have Anope log to!
 */
log
{
	/*
	 * Target(s) to log to, which may be one of the following:
	 *   - a channel name
	 *   - a filename
	 *   - globops
	 *
	 * If you specify a filename the current date in the format ".YYYYMMDD" will be appended to the path.
	 */
	target = "services.log"

	/* Log to both services.log and the channel #services
	 *
	 * Note that some older IRCds, such as Ratbox, require services to be in the
	 * log channel to be able to message it. To do this, configure service:channels to
	 * join your logging channel.
	 */
	#target = "services.log #services"

	/*
	 * The source(s) to only accept log messages from. Leave commented to allow all sources.
	 * This can be a users name, a channel name, one of our clients (e.g. OperServ), or a server name.
	 */
	#source = ""

	/*
	 * The bot used to log generic messages which have no predefined sender if the
	 * target directive is set to a channel or globops.
	 */
	bot = "Global"

	/*
	 * The number of days to keep log files, only useful if you are logging to a file.
	 * Set to 0 to never delete old log files.
	 *
	 * Note that Anope must run 24 hours a day for this feature to work correctly.
	 */
	logage = 7

	/*
	 * What types of log messages should be logged by this block. There are nine general categories:
	 *
	 *  admin      - Execution of admin commands (OperServ, etc).
	 *  override   - A services operator using their powers to execute a command they couldn't normally.
	 *  commands   - Execution of general commands.
	 *  servers    - Server actions, linking, squitting, etc.
	 *  channels   - Actions in channels such as joins, parts, kicks, etc.
	 *  users      - User actions such as connecting, disconnecting, changing name, etc.
	 *  other      - All other messages without a category.
	 *  rawio      - Logs raw input and output from services
	 *  debug      - Debug messages (log files can become VERY large from this).
	 *
	 * These options determine what messages from the categories should be logged. Wildcards are accepted, and
	 * you can also negate values with a ~. For example, "~operserv/akill operserv/*" would log all operserv
	 * messages except for operserv/akill. Note that processing stops at the first matching option, which
	 * means "* ~operserv/*" would log everything because * matches everything.
	 *
	 * Valid admin, override, and command options are:
	 *    pseudo-serv/commandname (e.g. operserv/akill, chanserv/set)
	 *
	 * Valid server options are:
	 *    connect, quit, sync, squit
	 *
	 * Valid channel options are:
	 *    create, destroy, join, part, kick, leave, mode
	 *
	 * Valid user options are:
	 *    connect, disconnect, quit, nick, ident, host, mode, maxusers, oper, away
	 *
	 * Rawio and debug are simple yes/no answers, there are no types for them.
	 *
	 * Note that modules may add their own values to these options.
	 */
	admin = "*"
	override = "chanserv/* nickserv/* memoserv/set ~botserv/set botserv/*"
	commands = "~operserv/* *"
	servers = "*"
	#channels = "~mode *"
	users = "connect disconnect nick"
	other = "*"
	rawio = no
	debug = no
}

/*
 * A log block to globops some useful things.
 */
log
{
	bot = "Global"
	target = "globops"
	admin = "global/* operserv/chankill operserv/mode operserv/kick operserv/akill operserv/s*line operserv/noop operserv/jupe operserv/set operserv/svsnick operserv/svsjoin operserv/svspart nickserv/getpass */drop"
	servers = "squit"
	users = "oper"
	other = "expire/* bados akill/*"
}

/*
 * [RECOMMENDED] Oper Access Config
 *
 * This section is used to set up staff access to restricted oper only commands.
 * You may define groups of commands and privileges, as well as who may use them.
 *
 * This block is recommended, as without it you will be unable to access most oper commands.
 *
 * The command names below are defaults and are configured in the *serv.conf's. If you configure
 * additional commands with permissions, such as commands from third party modules, the permissions
 * must be included in the opertype block before the command can be used.
 *
 * Available privileges:
 *  botserv/administration        - Can view and assign private BotServ bots
 *  botserv/fantasy               - Can use fantasy commands without the FANTASIA privilege
 *  chanserv/administration       - Can modify the settings of any channel (including changing of the owner!)
 *  chanserv/access/list          - Can view channel access and akick lists, but not modify them
 *  chanserv/access/modify        - Can modify channel access and akick lists, and use /chanserv enforce
 *  chanserv/auspex               - Can see any information with /CHANSERV INFO
 *  chanserv/no-register-limit    - May register an unlimited number of channels and nicknames
 *  chanserv/kick                 - Can kick and ban users from channels through ChanServ
 *  chanserv/drop/override        - Allows dropping channels without using a confirmation code
 *  memoserv/info                 - Can see any information with /MEMOSERV INFO
 *  memoserv/set-limit            - Can set the limit of max stored memos on any user and channel
 *  memoserv/no-limit             - Can send memos through limits and throttles
 *  nickserv/alist                - Can see the channel access list of other users
 *  nickserv/auspex               - Can see any information with /NICKSERV INFO
 *  nickserv/cert                 - Can modify other users certificate lists
 *  nickserv/confirm              - Can confirm other users nicknames
 *  nickserv/drop                 - Can drop other users nicks
 *  nickserv/drop/override        - Allows dropping nicks without using a confirmation code
 *  nickserv/recover              - Can recover other users nicks
 *  operserv/config               - Can modify services's configuration
 *  operserv/oper/modify          - Can add and remove operators with at most the same privileges
 *  protected                     - Can not be kicked from channels by services
 *
 * Available commands:
 *   botserv/bot/del          botserv/bot/add               botserv/bot/change        botserv/set/private
 *   botserv/set/nobot
 *
 *   chanserv/drop            chanserv/getkey               chanserv/invite
 *   chanserv/list            chanserv/suspend              chanserv/topic
 *
 *   chanserv/saset/noexpire
 *
 *   memoserv/sendall        memoserv/staff
 *
 *   nickserv/getemail         nickserv/suspend       nickserv/ajoin           nickserv/list
 *
 *   nickserv/saset/autoop   nickserv/saset/display    nickserv/saset/email    nickserv/saset/greet
 *   nickserv/saset/kill     nickserv/saset/keepmodes  nickserv/saset/language  nickserv/saset/message
 *   nickserv/saset/neverop  nickserv/saset/noexpire   nickserv/saset/password  nickserv/saset/private
 *   nickserv/saset/url
 *
 *   hostserv/set            hostserv/del           hostserv/list
 *
 *   global/global         global/queue          global/server
 *
 *   operserv/news         operserv/stats        operserv/kick       operserv/exception    operserv/seen
 *   operserv/mode         operserv/session      operserv/modinfo    operserv/ignore       operserv/chanlist
 *   operserv/chankill     operserv/akill        operserv/sqline     operserv/snline       operserv/userlist
 *   operserv/oper         operserv/config       operserv/umode      operserv/logsearch
 *   operserv/modload      operserv/jupe         operserv/set        operserv/noop
 *   operserv/quit         operserv/update       operserv/reload     operserv/restart
 *   operserv/shutdown     operserv/svs          operserv/kill
 *
 * Firstly, we define 'opertypes' which are named whatever we want ('Network Administrator', etc).
 * These can contain commands for oper-only strings (see above) which grants access to that specific command,
 * and privileges (which grant access to more general permissions for the named area).
 * Wildcard entries are permitted for both, e.g. 'commands = "operserv/*"' for all OperServ commands.
 * You can also negate values with a ~. For example, "~operserv/akill operserv/*" would allow all OperServ
 * commands except for operserv/akill. Note that processing stops at the first matching option, which
 * means "* ~operserv/*" would allow everything because * matches everything.
 *
 * Below are some default example types, but this is by no means exhaustive,
 * and it is recommended that you configure them to your needs.
 */

opertype
{
	/* The name of this opertype */
	name = "Helper"

	/* What commands (see above) this opertype has */
	commands = "hostserv/*"
}

opertype
{
	/* The name of this opertype */
	name = "Services Operator"

	/* What opertype(s) this inherits from. Separate with a comma. */
	inherits = "Helper, Another Helper"

	/* What commands (see above) this opertype may use */
	commands = "chanserv/list chanserv/suspend chanserv/topic memoserv/staff nickserv/list nickserv/suspend operserv/mode operserv/chankill operserv/akill operserv/session operserv/modinfo operserv/sqline operserv/oper operserv/kick operserv/ignore operserv/snline"

	/* What privs (see above) this opertype has */
	privs = "chanserv/auspex chanserv/no-register-limit memoserv/* nickserv/auspex nickserv/confirm"

	/*
	 * Modes to be set on users when they identify to accounts linked to this opertype.
	 *
	 * This can be used to automatically oper users who identify for services operator accounts, and is
	 * useful for setting modes such as Plexus's user mode +N.
	 *
	 * Note that some IRCds, such as InspIRCd, do not allow directly setting +o, and this will not work.
	 */
	#modes = "+o"
}

opertype
{
	name = "Services Administrator"

	inherits = "Services Operator"

	commands = "botserv/* chanserv/access/list chanserv/drop chanserv/getkey chanserv/saset/noexpire memoserv/sendall nickserv/saset/* nickserv/getemail operserv/news operserv/jupe operserv/svs operserv/stats operserv/noop operserv/forbid global/*"

	privs = "*"
}

opertype
{
	name = "Services Root"

	commands = "*"

	privs = "*"
}

/*
 * After defining different types of operators in the above opertype section, we now define who is in these groups
 * through 'oper' blocks, similar to ircd access.
 *
 * The default is to comment these out (so NOBODY will have access).
 * You probably want to add yourself and a few other people at minimum.
 *
 * As with all permissions, make sure to only give trustworthy people access.
 */

#oper
{
	/* The nickname of this services oper */
	#name = "nick1"

	/* The opertype this person will have */
	type = "Services Root"

	/* If set, the user must be an oper on the IRCd to gain their
	 * oper privileges.
	 */
	require_oper = yes

	/* An optional password. If defined, the user must login using "/OPERSERV LOGIN" first */
	#password = "secret"

	/* An optional SSL fingerprint. If defined, it's required to be able to use this opertype. */
	#certfp = "ed3383b3f7d74e89433ddaa4a6e5b2d7"

	/* An optional list of user@host masks. If defined the user must be connected from one of them */
	#host = "*@*.anope.org ident@*"

	/* An optional vHost to set on users who identify for this oper block.
	 * This will override HostServ vHosts, and may not be available on all IRCds
	 */
	#vhost = "oper.mynet"
}

#oper
{
	name = "nick2"
	type = "Services Administrator"
}

#oper
{
	name = "nick3"
	type = "Helper"
}

/*
 * [OPTIONAL] Mail Config
 *
 * This section contains settings related to the use of email from services.
 * If the usemail directive is set to yes, unless specified otherwise, all other
 * directives are required.
 *
 * NOTE: Users can find the IP of the machine services is running on by examining
 * mail headers. If you do not want your IP known, you should set up a mail relay
 * to strip the relevant headers.
 */
mail
{
	/*
	 * If set, this option enables the mail commands in Anope. You may choose
	 * to disable it if you have no Sendmail-compatible mailer installed. Whilst
	 * this directive (and entire block) is optional, it is required if
	 * nickserv:registration is set to yes.
	 */
	usemail = yes

	/*
	 * This is the command-line that will be used to call the mailer to send an
	 * email. It must be called with all the parameters needed to make it
	 * scan the mail input to find the mail recipient; consult your mailer
	 * documentation.
	 *
	 * Postfix users must use the compatible sendmail utility provided with
	 * it. This one usually needs no parameters on the command-line. Most
	 * sendmail applications (or replacements of it) require the -t option
	 * to be used.
	 *
	 * If you are running on Windows you should use a Windows sendmail port
	 * like https://www.glob.com.au/sendmail/ for sending emails.
	 */
	#sendmailpath = "/usr/sbin/sendmail -it"

	/*
	 * This is the email address from which all the emails are to be sent from.
	 * It should really exist.
	 */
	sendfrom = "services@example.com"

	/*
	 * This controls the minimum amount of time a user must wait before sending
	 * another email after they have sent one. It also controls the minimum time
	 * a user must wait before they can receive another email.
	 *
	 * This feature prevents users from being mail bombed using services and
	 * it is highly recommended that it be used.
	 *
	 * This directive is optional, but highly recommended.
	 */
	delay = 5m

	/*
	 * If set, Anope will not put quotes around the TO: fields
	 * in emails.
	 *
	 * This directive is optional, and as far as we know, it's only needed
	 * if you are using ESMTP or QMail to send out emails.
	 */
	#dontquoteaddresses = yes

	/*
	 * The content type to use when sending emails.
	 *
	 * This directive is optional, and is generally only needed if you want to
	 * use HTML or non UTF-8 text in your services emails.
	 */
	#content_type = "text/plain; charset=UTF-8"

	/*
	 * The subject and message of emails sent to users when they register accounts.
	 *
	 * Available tokens for this template are:
	 *  %n - Gets replaced with the nickname
	 *  %N - Gets replaced with the network name
	 *  %c - Gets replaced with the confirmation code
	 */
	registration_subject = "Nickname registration for %n"
	registration_message = "Hi,

				You have requested to register the nickname %n on %N.
				Please type \" /msg NickServ CONFIRM %c \" to complete registration.

				If you don't know why this mail was sent to you, please ignore it silently.

				%N administrators."

	/*
	 * The subject and message of emails sent to users when they request a new password.
	 *
	 * Available tokens for this template are:
	 *  %n - Gets replaced with the nickname
	 *  %N - Gets replaced with the network name
	 *  %c - Gets replaced with the confirmation code
	 */
	reset_subject = "Reset password request for %n"
	reset_message = "Hi,

			You have requested to have the password for %n reset.
			To reset your password, type \" /msg NickServ CONFIRM %n %c \"

			If you don't know why this mail was sent to you, please ignore it silently.

			%N administrators."

	/*
	 * The subject and message of emails sent to users when they request a new email address.
	 *
	 * Available tokens for this template are:
	 *  %e - Gets replaced with the old email address
	 *  %E - Gets replaced with the new email address
	 *  %n - Gets replaced with the nickname
	 *  %N - Gets replaced with the network name
	 *  %c - Gets replaced with the confirmation code
	 */
	emailchange_subject = "Email confirmation"
	emailchange_message = "Hi,

			You have requested to change your email address from %e to %E.
			Please type \" /msg NickServ CONFIRM %c \" to confirm this change.

			If you don't know why this mail was sent to you, please ignore it silently.

			%N administrators."

	/*
	 * The subject and message of emails sent to users when they receive a new memo.
	 *
	 * Available tokens for this template are:
	 *  %n - Gets replaced with the nickname
	 *  %s - Gets replaced with the sender's nickname
	 *  %d - Gets replaced with the memo number
	 *  %t - Gets replaced with the memo text
	 *  %N - Gets replaced with the network name
	 */
	memo_subject = "New memo"
	memo_message = "Hi %n,

			You've just received a new memo from %s. This is memo number %d.

			Memo text:

			%t"
}

/*
 * [REQUIRED] Database configuration.
 *
 * This section is used to configure databases used by Anope.
 * You should at least load one database method, otherwise any data you
 * have will not be stored!
 */

/*
 * [DEPRECATED] db_old
 *
 * This is the old binary database format from late Anope 1.7.x, Anope 1.8.x, and
 * early Anope 1.9.x. This module only loads these databases, and will NOT save them.
 * You should only use this to upgrade old databases to a newer database format by loading
 * other database modules in addition to this one, which will be used when saving databases.
 */
#module
{
	name = "db_old"

	/*
	 * This is the encryption type used by the databases. This must be set correctly or
	 * your passwords will not work. Valid options are: md5, oldmd5, sha1, and plain.
	 * You must also be sure to load the correct encryption module below in the Encryption
	 * Modules section so that your passwords work.
	 */
	#hash = "md5"
}

/*
 * db_atheme
 *
 * This allows importing databases from Atheme. You should load another database module as
 * well as this as it can only read Atheme databases not write them.
 */
#module
{
	name = "db_atheme"

	/*
	 * The database name db_atheme should use.
	 */
	database = "atheme.db"
}

/*
 * [RECOMMENDED] db_flatfile
 *
 * This is the default flatfile database format.
 */
module
{
	name = "db_flatfile"

	/*
	 * The database name db_flatfile should use
	 */
	database = "anope.db"

	/*
	 * Sets the number of days backups of databases are kept. If you don't give it,
	 * or if you set it to 0, Anope won't backup the databases.
	 *
	 * NOTE: Anope must run 24 hours a day for this feature to work.
	 *
	 * This directive is optional, but recommended.
	 */
	keepbackups = 3

	/*
	 * Allows Anope to continue file write operations (i.e. database saving)
	 * even if the original file cannot be backed up. Enabling this option may
	 * allow Anope to continue operation under conditions where it might
	 * otherwise fail, such as a nearly-full disk.
	 *
	 * NOTE: Enabling this option can cause irrecoverable data loss under some
	 * conditions, so make CERTAIN you know what you're doing when you enable it!
	 *
	 * This directive is optional, and you are discouraged against enabling it.
	 */
	#nobackupokay = yes

	/*
	 * If enabled, services will fork a child process to save databases.
	 *
	 * This is only useful with very large databases, with hundreds
	 * of thousands of objects, that have a noticeable delay from
	 * writing databases.
	 *
	 * If your database is large enough cause a noticeable delay when
	 * saving you should consider a more powerful alternative such
	 * as db_sql or db_redis, which incrementally update their
	 * databases asynchronously in real time.
	 */
	fork = no
}

/*
 * db_sql and db_sql_live
 *
 * db_sql module allows saving and loading databases using one of the SQL engines.
 * This module loads the databases once on startup, then incrementally updates
 * objects in the database as they are changed within Anope in real time. Changes
 * to the SQL tables not done by Anope will have no effect and will be overwritten.
 *
 * db_sql_live module allows saving and loading databases using one of the SQL engines.
 * This module reads and writes to SQL in real time. Changes to the SQL tables
 * will be immediately reflected into Anope. This module should not be loaded
 * in conjunction with db_sql.
 *
 */
#module
{
	name = "db_sql"
	#name = "db_sql_live"

	/*
	 * The SQL service db_sql(_live) should use, these are configured in modules.conf.
	 * For MySQL, this should probably be mysql/main.
	 */
	engine = "sqlite/main"

	/*
	 * An optional prefix to prepended to the name of each created table.
	 * Do not use the same prefix for other programs.
	 */
	#prefix = "anope_db_"

	/* Whether or not to import data from another database module in to SQL on startup.
	 * If you enable this, be sure that the database services is configured to use is
	 * empty and that another database module to import from is loaded before db_sql.
	 * After you enable this and do a database import you should disable it for
	 * subsequent restarts.
	 *
	 * Note that you can not import databases using db_sql_live. If you want to import
	 * databases and use db_sql_live you should import them using db_sql, then shut down
	 * and start services with db_sql_live.
	 */
	import = false
}

/*
 * db_redis.
 *
 * This module allows using Redis (https://redis.io/) as a database backend.
 * This module requires that redis is loaded and configured properly.
 *
 * Redis 2.8 supports keyspace notifications which allows Redis to push notifications
 * to Anope about outside modifications to the database. This module supports this and
 * will internally reflect any changes made to the database immediately once notified.
 * See docs/REDIS for more information regarding this.
 */
#module
{
	name = "db_redis"

	/*
	 * Redis database to use. This must be configured with redis.
	 */
	engine = "redis/main"
}

/*
 * [RECOMMENDED] Encryption modules.
 *
 * The encryption modules are used when dealing with passwords. This determines
 * how the passwords are stored in the databases.
 *
 * The first encryption module loaded is the primary encryption module. All new
 * passwords are encrypted by this module. Old passwords encrypted with another
 * encryption method are automatically re-encrypted with the primary encryption
 * module the next time the user identifies.
 */

/*
 * enc_sha2
 *
 * Provides support for encrypting passwords using the HMAC-SHA-2 algorithm. See
 * https://en.wikipedia.org/wiki/SHA-2 and https://en.wikipedia.org/wiki/HMAC
 * for more information.
 */
module
{
	name = "enc_sha2"

	/** The sub-algorithm to use. Can be set to sha224 for SHA-224, sha256 for
	 * SHA-256, sha284 for SHA-384 or sha512 to SHA-512. Defaults to sha256.
	 */
	#algorithm = "sha256"
}

/*
 * [EXTRA] enc_argon2
 *
 * Provides support for encrypting passwords using the Argon2 algorithm. See
 * https://en.wikipedia.org/wiki/Argon2 for more information.
 */
#module
{
	name = "enc_argon2"

	/** The sub-algorithm to use. Can be set to argon2d for Argon2d, argon2i for
	 * Argon2i, or argon2id for Argon2id. Defaults to argon2id.
	 */
	#algorithm = "argon2id"

	/** The memory hardness in kibibytes of the Argon2 algorithm. Defaults to
	 * 128 mebibytes.
	 */
	#memory_cost = 121072

	/** The time hardness (iterations) of the Argon2 algorithm. Defaults to 3.
	 */
	#time_cost = 3

	/** The amount of parallel threads to use when encrypting passwords.
	 * Defaults to 1.
	 */
	#parallelism = 1

	/** The length in bytes of an Argon2 hash. Defaults to 32. */
	#hash_length = 32

	/** The length in bytes of an Argon2 salt. Defaults to 32. */
	#salt_length = 32
}

/*
 * enc_bcrypt
 *
 * Provides support for encrypting passwords using the Bcrypt algorithm. See
 * https://en.wikipedia.org/wiki/Bcrypt for more information.
 */
#module
{
	name = "enc_bcrypt"

	/** The number of Bcrypt rounds to perform on passwords. Can be set to any
	 * number between 10 and 32 but higher numbers are more CPU intensive and
	 * may impact performance.
	 */
	#rounds = 10
}

/*
 * [EXTRA] enc_posix
 *
 * Provides verify-only support for passwords encrypted using the POSIX crypt()
 * function. Load this if you are migratign from another services packages such
 * as Atheme. See https://en.wikipedia.org/wiki/Crypt_(C) for more information.
 *
 * You must load another encryption method before this to re-encrypt passwords
 * with when a user logs in.
 */
#module { name = "enc_posix" }

/*
 * [DEPRECATED] enc_md5, enc_none, enc_old, enc_sha1, enc_sha256
 *
 * Provides verify-only support for passwords encrypted using encryption methods
 * from older versions of Anope. These methods are no longer considered secure
 * and will be removed in a future version of Anope. Only load them if you are
 * upgrading from a previous version of Anope that used them.
 *
 * enc_md5:    Verifies passwords encrypted with the MD5 algorithm
 * enc_none:   Verifies passwords that are not encrypted
 * enc_sha1:   Verifies passwords encrypted with the SHA1 algorithm
 * enc_old:    Verifies passwords encrypted with the broken MD5 algorithm used
 *             before 1.7.17.
 * enc_sha256: Verifies passwords encrypted with the SHA256 algorithm using a
 *             custom initialisation vector as a salt.
 *
 * You must load another encryption method before this to re-encrypt passwords
 * with when a user logs in.
 */
#module { name = "enc_md5" }
#module { name = "enc_none" }
#module { name = "enc_old" }
#module { name = "enc_sha1" }
#module { name = "enc_sha256" }

/* Extra (optional) modules. */
include
{
	type = "file"
	name = "modules.example.conf"
}

/*
 * Chanstats module.
 * Requires a MySQL Database.
 */
#include
{
	type = "file"
	name = "chanstats.example.conf"
}

/*
 * IRC2SQL Gateway
 * This module collects data about users, channels and servers. It doesn't build stats
 * itself, however, it gives you the database, it's up to you how you use it.
 *
 * Requires a MySQL Database and MySQL version 5.5 or higher
 */
#include
{
	type = "file"
	name = "irc2sql.example.conf"
}