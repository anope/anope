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

#define OPERSERV_NEWS_ITEM_TYPE "NewsItem"
#define OPERSERV_NEWS_SERVICE "OperServ::NewsService"

namespace OperServ
{
	struct NewsItem;
	class NewsService;

	enum NewsType
	{
		NEWS_LOGON,
		NEWS_RANDOM,
		NEWS_OPER,
	};

	ServiceReference<NewsService> news_service(OPERSERV_NEWS_SERVICE, OPERSERV_NEWS_SERVICE);
}

struct OperServ::NewsItem
	: Serializable
{
	OperServ::NewsType type;
	Anope::string text;
	Anope::string who;
	time_t time;

	NewsItem()
		: Serializable(OPERSERV_NEWS_ITEM_TYPE)
	{
	}
};

class OperServ::NewsService
	: public Service
{
public:
	NewsService(Module *m)
		: Service(m, OPERSERV_NEWS_SERVICE, OPERSERV_NEWS_SERVICE)
	{
	}

	virtual OperServ::NewsItem *CreateNewsItem() = 0;

	virtual void AddNewsItem(OperServ::NewsItem *n) = 0;

	virtual void DelNewsItem(OperServ::NewsItem *n) = 0;

	virtual std::vector<OperServ::NewsItem *> &GetNewsList(OperServ::NewsType t) = 0;
};
