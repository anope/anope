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

enum NewsType
{
	NEWS_LOGON,
	NEWS_RANDOM,
	NEWS_OPER
};

struct NewsItem
	: Serializable
{
	NewsType type;
	Anope::string text;
	Anope::string who;
	time_t time;

	NewsItem() : Serializable("NewsItem") { }
};

class NewsService
	: public Service
{
public:
	NewsService(Module *m) : Service(m, "NewsService", "news") { }

	virtual NewsItem *CreateNewsItem() = 0;

	virtual void AddNewsItem(NewsItem *n) = 0;

	virtual void DelNewsItem(NewsItem *n) = 0;

	virtual std::vector<NewsItem *> &GetNewsList(NewsType t) = 0;
};

static ServiceReference<NewsService> news_service("NewsService", "news");
