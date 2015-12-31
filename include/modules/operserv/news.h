#ifndef OS_NEWS
#define OS_NEWS

enum NewsType
{
	NEWS_LOGON,
	NEWS_RANDOM,
	NEWS_OPER
};

class NewsItem : public Serialize::Object
{
 public:
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

static Serialize::TypeReference<NewsItem> newsitem("NewsItem");

#endif // OS_NEWS

