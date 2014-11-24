/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */


class AutoKick;
class BotInfo;
class Channel;
class ChannelMode;
class ChannelStatus;
namespace ChanServ
{
	class AccessGroup;
	class ChanAccess;
	class Channel;
}
struct ChanUserContainer;
class ClientSocket;
class Command;
class CommandSource;
namespace Configuration { struct Conf; }
class ConnectionSocket;
class Entry;
class ExtensibleBase;
class InfoFormatter;
class IRCDProto;
class ListenSocket;
class Log;
class LogInfo;
namespace NickServ
{
	class Account;
	class Nick;
	class IdentifyRequest;
}
namespace MemoServ
{
	class Memo;
	class MemoInfo;
}
class MessageSource;
class Module;
class OperType;
class ReferenceBase;
class Regex;
class ServiceBot;
namespace Serialize
{
	using ID = uint64_t;
	struct Edge;
	class FieldBase;
	class TypeBase;
	class Object;
}
class Server;
class Socket;
class Thread;
class User;
class XLine;
class XLineManager;
class Oper;
namespace SASL { struct Message; }
class UserMode;

