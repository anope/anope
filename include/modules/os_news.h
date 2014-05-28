#ifndef OS_NEWS
#define OS_NEWS

enum NewsType
{
	NEWS_LOGON,
	NEWS_RANDOM,
	NEWS_OPER
};

struct NewsMessages
{
	NewsType type;
	Anope::string name;
	const char *msgs[10];
};

struct NewsItem : Serializable
{
	NewsType type;
	Anope::string text;
	Anope::string who;
	time_t time;

	NewsItem() : Serializable("NewsItem") { }
};

class NewsService : public Service
{
 public:
	NewsService(Module *m) : Service(m, "NewsService", "news") { }

	virtual NewsItem *CreateNewsItem() anope_abstract;

	virtual void AddNewsItem(NewsItem *n) anope_abstract;

	virtual void DelNewsItem(NewsItem *n) anope_abstract;

	virtual std::vector<NewsItem *> &GetNewsList(NewsType t) anope_abstract;
};

static ServiceReference<NewsService> news_service("NewsService", "news");

#endif // OS_NEWS

