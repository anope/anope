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

	Anope::string serialize_name() const { return "NewsItem"; }
	serialized_data serialize();
	static void unserialize(serialized_data &data);
};

class NewsService : public Service<Base>
{
 public:
	NewsService(Module *m) : Service<Base>(m, "news") { }
	
	virtual void AddNewsItem(NewsItem *n) = 0;
	
	virtual void DelNewsItem(NewsItem *n) = 0;
	
	virtual std::vector<NewsItem *> &GetNewsList(NewsType t) = 0;
};

static service_reference<NewsService, Base> news_service("news");

Serializable::serialized_data NewsItem::serialize()
{
	serialized_data data;
		
	data["type"] << this->type;
	data["text"] << this->text;
	data["who"] << this->who;
	data["time"] << this->time;

	return data;
}

void NewsItem::unserialize(serialized_data &data)
{
	if (!news_service)
		return;

	NewsItem *ni = new NewsItem();

	unsigned int t;
	data["type"] >> t;
	ni->type = static_cast<NewsType>(t);
	data["text"] >> ni->text;
	data["who"] >> ni->who;
	data["time"] >> ni->time;

	news_service->AddNewsItem(ni);
}

#endif // OS_NEWS

