/*
 *
 * (C) 2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 * 
 */

#pragma once

enum NewsType
{
	NEWS_LOGON,
	NEWS_RANDOM,
	NEWS_OPER
};

class NewsItem : public Serialize::Object
{
 public:
	static constexpr const char *const NAME = "newsitem";
	 
	using Serialize::Object::Object;

	virtual NewsType GetNewsType() anope_abstract;
	virtual void SetNewsType(const NewsType &) anope_abstract;

	virtual Anope::string GetText() anope_abstract;
	virtual void SetText(const Anope::string &) anope_abstract;

	virtual Anope::string GetWho() anope_abstract;
	virtual void SetWho(const Anope::string &) anope_abstract;

	virtual time_t GetTime() anope_abstract;
	virtual void SetTime(const time_t &) anope_abstract;
};
