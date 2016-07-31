/*
 * Anope IRC Services
 *
 * Copyright (C) 2004-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
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

