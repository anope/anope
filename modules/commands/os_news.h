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

struct NewsItem
{
	NewsType type;
	Anope::string text;
	Anope::string who;
	time_t time;
};

class NewsService : public Service<NewsService>
{
 public:
	NewsService(Module *m) : Service<NewsService>(m, "news") { }
	
	virtual void AddNewsItem(NewsItem *n) = 0;
	
	virtual void DelNewsItem(NewsItem *n) = 0;
	
	virtual std::vector<NewsItem *> &GetNewsList(NewsType t) = 0;
};

#endif // OS_NEWS

