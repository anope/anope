#include "protocol.h"

/* Common IRCD messages.
 * Protocol modules may chose to include some, none, or all of these handlers
 * as they see fit.
 */

struct CoreExport CoreIRCDMessageAway : IRCDMessage
{
	CoreIRCDMessageAway(const Anope::string &mname = "AWAY") : IRCDMessage(mname, 0) { SetFlag(IRCDMESSAGE_REQUIRE_USER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessageCapab : IRCDMessage
{
	CoreIRCDMessageCapab(const Anope::string &mname = "CAPAB") : IRCDMessage(mname, 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessageError : IRCDMessage
{
	CoreIRCDMessageError(const Anope::string &mname = "ERROR") : IRCDMessage(mname, 1) { }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessageJoin : IRCDMessage
{
	CoreIRCDMessageJoin(const Anope::string &mname = "JOIN") : IRCDMessage(mname, 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessageKick : IRCDMessage
{
	CoreIRCDMessageKick(const Anope::string &mname = "KICK") : IRCDMessage(mname, 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessageKill : IRCDMessage
{
	CoreIRCDMessageKill(const Anope::string &mname = "KILL") : IRCDMessage(mname, 2) { }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessageMOTD : IRCDMessage
{
	CoreIRCDMessageMOTD(const Anope::string &mname = "MOTD") : IRCDMessage(mname, 1) { }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessagePart : IRCDMessage
{
	CoreIRCDMessagePart(const Anope::string &mname = "PART") : IRCDMessage(mname, 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessagePing : IRCDMessage
{
	CoreIRCDMessagePing(const Anope::string &mname = "PING") : IRCDMessage(mname, 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessagePrivmsg : IRCDMessage
{
	CoreIRCDMessagePrivmsg(const Anope::string &mname = "PRIVMSG") : IRCDMessage(mname, 2) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessageQuit : IRCDMessage
{
	CoreIRCDMessageQuit(const Anope::string &mname = "QUIT") : IRCDMessage(mname, 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessageSQuit : IRCDMessage
{
	CoreIRCDMessageSQuit(const Anope::string &mname = "SQUIT") : IRCDMessage(mname, 2) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessageStats : IRCDMessage
{
	CoreIRCDMessageStats(const Anope::string &mname = "STATS") : IRCDMessage(mname, 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessageTime : IRCDMessage
{
	CoreIRCDMessageTime(const Anope::string &mname = "TIME") : IRCDMessage(mname, 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessageTopic : IRCDMessage
{
	CoreIRCDMessageTopic(const Anope::string &mname = "TOPIC") : IRCDMessage(mname, 2) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessageVersion : IRCDMessage
{
	CoreIRCDMessageVersion(const Anope::string &mname = "VERSION") : IRCDMessage(mname, 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

struct CoreExport CoreIRCDMessageWhois : IRCDMessage
{
	CoreIRCDMessageWhois(const Anope::string &mname = "WHOIS") : IRCDMessage(mname, 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override;
};

