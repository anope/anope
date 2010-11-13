
/* Gets the bit-th bit from key */
#define GET_BIT(key, bit) (key[bit / 8] & (1 << (bit & 7)))
/* Checks if the bit-th bit from key1 and key2 differ, returns 1 if they do */
#define GET_BIT_XOR(key1, key2, bit) ((key1[bit / 8] ^ key2[bit / 8]) & (1 << (bit & 7)))

template<typename Data> struct patricia_elem
{
	unsigned int bit;
	patricia_elem<Data> *up, *one, *zero;
	typename std::list<Data *>::iterator node;
	Anope::string key;
	Data *data;
};

template<typename Data, typename Compare = std::equal_to<Anope::string> >
class patricia_tree
{
	Compare comp;

	patricia_elem<Data> *root;
	std::list<Data *> list;

 public:

	patricia_tree()
	{
		this->root = NULL;
	}

	virtual ~patricia_tree()
	{
		while (this->root)
			this->erase(this->root->key);
	}

	typedef typename std::list<Data *>::iterator iterator;
	typedef typename std::list<Data *>::const_iterator const_iterator;

	inline iterator begin() { return this->list.begin(); }
	inline iterator end() { return this->list.end(); }

	inline const const_iterator begin() const { return this->list.begin(); }
	inline const const_iterator end() const { return this->list.end(); }

	inline Data *front() { return this->list.front(); }
	inline Data *back() { return this->list.back(); }

	inline size_t size() const { return this->list.size(); }
	inline bool empty() const { return this->list.empty(); }

	Data *find(const Anope::string &key)
	{
		size_t keylen = key.length();
		patricia_elem<Data> *prev = NULL, *cur = this->root;
		bool bitval;
		while (cur && (!prev || prev->bit < cur->bit))
		{
			prev = cur;

			if (cur->bit / 8 < keylen)
				bitval = GET_BIT(key, cur->bit);
			else
				bitval = false;

			cur = bitval ? cur->one : cur->zero;
		}

		if (cur && comp(cur->key, key))
			return cur->data;

		return NULL;
	}

	void insert(const Anope::string &key, Data *data)
	{
		if (key.empty() || data == NULL)
			throw CoreExport;

		size_t keylen = key.length();
		patricia_elem<Data> *prev = NULL, *cur = this->root;
		bool bitval;
		while (cur && (!prev || prev->bit < cur->bit))
		{
			prev = cur;

			if (cur->bit / 8 < keylen)
				bitval = GET_BIT(key, cur->bit);
			else
				bitval = false;

			cur = bitval ? cur->one : cur->zero;
		}

		if (cur && comp(cur->key, key))
			return;

		patricia_elem<Data> *newelem = new patricia_elem<Data>();
		newelem->up = prev;
		newelem->key = key;
		newelem->data = data;

		if (!cur)
			newelem->bit = prev ? prev->bit + 1 : 0;
		else
			for (newelem->bit = 0; GET_BIT_XOR(key, cur->key, newelem->bit) == 0; ++newelem->bit);

		patricia_elem<Data> *place = prev;
		while (place && newelem->bit < place->bit)
		{
			prev = place;
			place = place->up;
		}

		if (GET_BIT(key, newelem->bit))
		{
			newelem->one = newelem;
			newelem->zero = place == prev ? cur : prev;
		}
		else
		{
			newelem->zero = newelem;
			newelem->one = place == prev ? cur : prev;
		}

		if (place != prev)
		{
			prev->up = newelem;
			newelem->up = place;
		}

		if (place)
		{
			bitval = GET_BIT(key, place->bit);
			if (bitval)
				place->one = newelem;
			else
				place->zero = newelem;
		}
		else
			this->root = newelem;

		this->list.push_front(data);
		newelem->node = this->list.begin();
	}

	Data *erase(const Anope::string &key)
	{
		size_t keylen = key.length();
		patricia_elem<Data> *prev = NULL, *cur = this->root;
		bool bitval;
		while (cur && (!prev || prev->bit < cur->bit))
		{
			prev = cur;

			if (cur->bit / 8 < keylen)
				bitval = GET_BIT(key, cur->bit);
			else
				bitval = false;

			cur = bitval ? cur->one : cur->zero;
		}

		if (!cur || comp(cur->key, key) == false)
			return NULL;

		patricia_elem<Data> *other = (bitval ? prev->zero : prev->one);

		if (!prev->up)
			this->root = other;
		else if (prev->up->zero == prev)
			prev->up->zero = other;
		else
			prev->up->one = other;

		if (prev->zero && prev->zero->up == prev)
			prev->zero->up = prev->up;
		if (prev->one && prev->one->up == prev)
			prev->one->up = prev->up;

		if (cur != prev)
		{
			if (!cur->up)
				this->root = prev;
			else if (cur->up->zero == cur)
				cur->up->zero = prev;
			else
				cur->up->one = prev;

			if (cur->zero && cur->zero->up == cur)
				cur->zero->up = prev;
			if (cur->one && cur->one->up == cur)
				cur->one->up = prev;

			prev->one = cur->one;
			prev->zero = cur->zero;
			prev->up = cur->up;
			prev->bit = cur->bit;
		}

		this->list.erase(cur->node);

		Data *data = cur->data;
		delete cur;

		return data;
	}
};


