/*
 *
 * (C) 2011-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

#define GLOBAL_NO_MESSAGE     _("You do not have any messages queued and did not specify a message to send.")
#define GLOBAL_QUEUE_CONFLICT _("You can not send a single message while you have messages queued.")

class GlobalService
	: public Service
{
public:
	GlobalService(Module *m)
		: Service(m, "GlobalService", "Global")
	{
	}

	/** Retrieves the bot which sends global messages unless otherwise specified. */
	virtual Reference<BotInfo> GetDefaultSender() const = 0;

	/** Clears any queued messages for the specified account.
	 * @param nc The account to clear queued messages for.
	 */
	virtual void ClearQueue(NickCore *nc) = 0;

	/** Retrieves the size of the messages queue for the specified user.
	 * @param nc The account to count queued messages for.
	 */
	inline size_t CountQueue(NickCore* nc) const
	{
		auto *q = GetQueue(nc);
		return q ? q->size() : 0;
	}

	/** Retrieves the messages queue for the specified user.
	 * @param nc The account to retrieve queued messages for.
	 */
	virtual const std::vector<Anope::string> *GetQueue(NickCore* nc) const = 0;

	/** Queues a message to be sent later.
	 * @param nc The account to queue the message for.
	 * @param message The message to queue.
	 * @return The new number of messages in the queue.
	 */
	virtual size_t Queue(NickCore *nc, const Anope::string &message) = 0;

	/** Sends a single message to all users on the network.
	 * @param message The message to send.
	 * @param source If non-nullptr then the source of the message.
	 * @param sender If non-nullptr then the bot to send the message from.
	 * @param server If non-nullptr then the server to send messages to.
	 * @return If the message was sent then true; otherwise, false.
	 */
	virtual bool SendSingle(const Anope::string &message, CommandSource *source = nullptr, BotInfo *sender = nullptr, Server *server = nullptr) = 0;

	/** Sends a message queue to all users on the network.
	 * @param source The source of the message.
	 * @param sender If non-nullptr then the bot to send the message from.
	 * @param server If non-nullptr then the server to send messages to.
	 * @return If the message queue was sent then true; otherwise, false.
	 */
	virtual bool SendQueue(CommandSource &source, BotInfo *sender = nullptr, Server *server = nullptr) = 0;

	/** Unqueues a message from the message queue.
	 * @param nc The account to unqueue the message from.
	 * @param idx The index of the item to remove.
	 * @return Whether the message was removed from the queue.
	 */
	virtual bool Unqueue(NickCore *nc, size_t idx) = 0;
};
