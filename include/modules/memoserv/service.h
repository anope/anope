// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
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

#define MEMOSERV_SERVICE "MemoServ::Service"

namespace MemoServ
{
	class Service;

	/** Possible results when sending a memo. */
	enum MemoResult
	{
		MEMO_SUCCESS,
		MEMO_INVALID_TARGET,
		MEMO_TOO_FAST,
		MEMO_TARGET_FULL
	};

	ServiceReference<Service> service(MEMOSERV_SERVICE, MEMOSERV_SERVICE);
}

class MemoServ::Service
	: public ::Service
{
public:
	Service(Module *m)
		: ::Service(m, MEMOSERV_SERVICE, MEMOSERV_SERVICE)
	{
	}

	/** Sends a memo.
	 * @param source The source of the memo, can be anything.
	 * @param target The target of the memo, nick or channel.
	 * @param message Memo text
	 * @param force true to force the memo, restrictions/delays etc are not checked
	 */
	virtual MemoServ::MemoResult Send(const Anope::string &source, const Anope::string &target, const Anope::string &message, bool force = false) = 0;

	/** Check for new memos and notify the user if there are any
	 * @param u The user
	 */
	virtual void Check(User *u) = 0;
};
