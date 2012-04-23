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

	const Anope::string serialize_name() const anope_override { return "NewsItem"; }
	Serialize::Data serialize() const anope_override;
	static Serializable* unserialize(Serializable *obj, Serialize::Data &data);
};

class NewsService : public Service
{
 public:
	NewsService(Module *m) : Service(m, "NewsService", "news") { }
	
	virtual void AddNewsItem(NewsItem *n) = 0;
	
	virtual void DelNewsItem(NewsItem *n) = 0;
	
	virtual std::vector<NewsItem *> &GetNewsList(NewsType t) = 0;
};

static service_reference<NewsService> news_service("NewsService", "news");

Serialize::Data NewsItem::serialize() const
{
	Serialize::Data data;
		
	data["type"] << this->type;
	data["text"] << this->text;
	data["who"] << this->who;
	data["time"] << this->time;

	return data;
}

Serializable* NewsItem::unserialize(Serializable *obj, Serialize::Data &data)
{
	if (!news_service)
		return NULL;

	NewsItem *ni;
	if (obj)
		ni = debug_cast<NewsItem *>(obj);
	else
		ni = new NewsItem();

	unsigned int t;
	data["type"] >> t;
	ni->type = static_cast<NewsType>(t);
	data["text"] >> ni->text;
	data["who"] >> ni->who;
	data["time"] >> ni->time;

	if (!obj)
		news_service->AddNewsItem(ni);
	return ni;
}

#endif // OS_NEWS

