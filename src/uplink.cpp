/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2016 Anope Team <team@anope.org>
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

#include "uplink.h"
#include "logger.h"
#include "config.h"
#include "protocol.h"
#include "servers.h"
#include "event.h"
#include "bots.h"
#include "timers.h"
#include "modules.h"

UplinkSocket *UplinkSock = NULL;

class ReconnectTimer : public Timer
{
 public:
	ReconnectTimer(int wait) : Timer(wait) { }

	void Tick(time_t) override
	{
		try
		{
			Uplink::Connect();
		}
		catch (const SocketException &ex)
		{
			Anope::Logger.Terminal(_("Unable to connect to uplink #{0} ({1}:{2}): {3}"), Anope::CurrentUplink + 1, Config->Uplinks[Anope::CurrentUplink].host, Config->Uplinks[Anope::CurrentUplink].port, ex.GetReason());
		}
	}
};

void Uplink::Connect()
{
	if (Config->Uplinks.empty())
	{
		Anope::Logger.Log(_("Warning: There are no configured uplinks."));
		return;
	}

	if (static_cast<unsigned>(++Anope::CurrentUplink) >= Config->Uplinks.size())
		Anope::CurrentUplink = 0;

	Configuration::Uplink &u = Config->Uplinks[Anope::CurrentUplink];

	new UplinkSocket();
	if (!Config->GetBlock("serverinfo")->Get<Anope::string>("localhost").empty())
		UplinkSock->Bind(Config->GetBlock("serverinfo")->Get<Anope::string>("localhost"));
	EventManager::Get()->Dispatch(&Event::PreServerConnect::OnPreServerConnect);
	Anope::string ip = Anope::Resolve(u.host, u.ipv6 ? AF_INET6 : AF_INET);
	Anope::Logger.Terminal(_("Attempting to connect to uplink #{0} {1} ({2}), port {3}"), Anope::CurrentUplink + 1, u.host, ip, u.port);
	UplinkSock->Connect(ip, u.port);
}

UplinkSocket::UplinkSocket() : Socket(-1, Config->Uplinks[Anope::CurrentUplink].ipv6), ConnectionSocket(), BufferedSocket()
{
	error = false;
	UplinkSock = this;
}

UplinkSocket::~UplinkSocket()
{
	if (!error && !Anope::Quitting)
	{
		this->OnError("");
		Module *protocol = ModuleManager::FindFirstOf(PROTOCOL);
		if (protocol && !protocol->name.find("inspircd"))
			Anope::Logger.Terminal(_("Check that you have loaded m_spanningtree.so on InspIRCd, and are not connecting Anope to an SSL enabled port without configuring SSL in Anope (or vice versa)"));
		else
			Anope::Logger.Terminal(_("Check that you are not connecting Anope to an SSL enabled port without configuring SSL in Anope (or vice versa)"));
	}

	if (IRCD && Servers::GetUplink() && Servers::GetUplink()->IsSynced())
	{
		EventManager::Get()->Dispatch(&Event::ServerDisconnect::OnServerDisconnect);

		for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
		{
			User *u = it->second;

			if (u->server == Me)
			{
				/* Don't use quitmsg here, it may contain information you don't want people to see */
				IRCD->SendQuit(u, "Shutting down");
				ServiceBot* bi = ServiceBot::Find(u->GetUID());
				if (bi != NULL)
					bi->introduced = false;
			}
		}

		IRCD->Send<messages::SQuit>(Me, Anope::QuitReason);

		this->ProcessWrite(); // Write out the last bit
	}

	for (unsigned i = Me->GetLinks().size(); i > 0; --i)
		if (!Me->GetLinks()[i - 1]->IsJuped())
			Me->GetLinks()[i - 1]->Delete(Me->GetName() + " " + Me->GetLinks()[i - 1]->GetName());

	UplinkSock = NULL;

	Me->Unsync();

	if (Anope::AtTerm())
	{
		if (static_cast<unsigned>(Anope::CurrentUplink + 1) == Config->Uplinks.size())
		{
			Anope::QuitReason = "Unable to connect to any uplink";
			Anope::Quitting = true;
			Anope::ReturnValue = -1;
		}
		else
		{
			new ReconnectTimer(1);
		}
	}
	else if (!Anope::Quitting)
	{
		time_t retry = Config->GetBlock("options")->Get<time_t>("retrywait");

		Anope::Logger.Log(_("Disconnected, retrying in {0} seconds"), retry);
		new ReconnectTimer(retry);
	}
}

bool UplinkSocket::ProcessRead()
{
	bool b = BufferedSocket::ProcessRead();
	for (Anope::string buf; (buf = this->GetLine()).empty() == false;)
	{
		Anope::Process(buf);
		User::QuitUsers();
		Channel::DeleteChannels();
	}
	return b;
}

void UplinkSocket::OnConnect()
{
	Anope::Logger.Terminal(_("Successfully connected to uplink #{0} {1}:{2}"), Anope::CurrentUplink + 1, Config->Uplinks[Anope::CurrentUplink].host, Config->Uplinks[Anope::CurrentUplink].port);
	IRCD->Handshake();
	EventManager::Get()->Dispatch(&Event::ServerConnect::OnServerConnect);
}

void UplinkSocket::OnError(const Anope::string &err)
{
	if (!this->flags[SF_CONNECTED])
		Anope::Logger.Terminal(_("Unable to connect to uplink #{0} ({1}:{2}): {3}{4}"), Anope::CurrentUplink + 1, Config->Uplinks[Anope::CurrentUplink].host, Config->Uplinks[Anope::CurrentUplink].port, !err.empty() ? (": " + err) : "");
	else
		Anope::Logger.Terminal(_("Lost connection from uplink #{0} ({1}:{2}): {3}{4}"), Anope::CurrentUplink + 1, Config->Uplinks[Anope::CurrentUplink].host, Config->Uplinks[Anope::CurrentUplink].port, !err.empty() ? (": " + err) : "");
	error |= !err.empty();
}

void Uplink::SendMessage(IRCMessage &message)
{
	const MessageSource &source = message.GetSource();
	Anope::string buffer = IRCD->Format(message);

	if (source.GetServer() != NULL)
	{
		const Server *s = source.GetServer();

		if (s != Me && !s->IsJuped())
		{
			Anope::Logger.Debug("Attempted to send \"{0}\" from {1} who is not from me?", buffer, s->GetName());
			return;
		}
	}
	else if (source.GetUser() != NULL)
	{
		const User *u = source.GetUser();

		if (u->server != Me && !u->server->IsJuped())
		{
			Anope::Logger.Debug("Attempted to send \"{0}\" from {1} who is not from me?", buffer, u->nick);
			return;
		}

		const ServiceBot *bi = source.GetBot();
		if (bi != NULL && bi->introduced == false)
		{
			Anope::Logger.Debug("Attempted to send \"{0}\" from {1} when not introduced", buffer, bi->nick);
			return;
		}
	}

	if (!UplinkSock)
	{
		Anope::Logger.Debug("Attempted to send \"{0}\" when not connected", buffer);
		return;
	}

	UplinkSock->Write(buffer);
	Anope::Logger.RawIO("Sent: {0}", buffer);
}
