// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

class AccessGroup;
class BotInfo;
namespace BotServ { class BadWord; }
class ChanAccess;
class Channel;
class ChannelInfo;
class ChannelStatus;
namespace ChanServ { class AutoKick; class ModeLock; }
class ClientSocket;
class Command;
class CommandSource;
namespace Configuration { class Conf; }
class ConnectionSocket;
namespace DNS { struct Query; }
class Entry;
class IdentifyRequest;
class InfoFormatter;
class IRCDProto;
class ListenSocket;
class Log;
struct Membership;
class Memo;
struct MemoInfo;
class MessageSource;
struct ModeData;
class Module;
class NickAlias;
class NickCore;
namespace NickServ { struct Cert; }
struct Oper;
namespace OperServ { struct Exception; }
class OperType;
class ReferenceBase;
class Regex;
namespace SASL { struct Message; }
class Serializable;
class Server;
class Socket;
class Thread;
class User;
class XLine;
class XLineManager;
