
/* Gets the bit-th bit from key */
#define GET_BIT(key, bit) (key[bit / 8] & (1 << (bit & 7)))
/* Checks if the bit-th bit from key1 and key2 differ, returns 1 if they do */
#define GET_BIT_XOR(key1, key2, bit) ((key1[bit / 8] ^ key2[bit / 8]) & (1 << (bit & 7)))

template<typename Data> struct patricia_elem
{
	unsigned int bit;
	patricia_elem<Data> *up, *one, *zero;
	Anope::string key;
	Data data;
};

template<typename Data, typename char_traits = std::std_char_traits>
class patricia_tree
{
	typedef std::basic_string<char, char_traits, std::allocator<char> > String;

	patricia_elem<Data> *root;
	size_t count;

 public:
	patricia_tree()
	{
		this->root = NULL;
		this->count = 0;
	}

	virtual ~patricia_tree()
	{
		while (this->root)
			this->erase(this->root->key);
	}

	inline size_t size() const { return this->count; }
	inline bool empty() const { return this->count == 0; }

	Data find(const Anope::string &ukey)
	{
		Anope::string key;
		for (size_t i = 0, j = ukey.length(); i < j; ++i)
			key.push_back(char_traits::chartolower(ukey[i]));

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

		if (cur && String(cur->key.c_str()).compare(key.c_str()) == 0)
			return cur->data;

		return NULL;
	}

	void insert(const Anope::string &ukey, Data data)
	{
		Anope::string key;
		for (size_t i = 0, j = ukey.length(); i < j; ++i)
			key.push_back(char_traits::chartolower(ukey[i]));

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

		if (cur && String(cur->key.c_str()).compare(key.c_str()) == 0)
			return;

		patricia_elem<Data> *newelem = new patricia_elem<Data>();
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

		newelem->up = place;

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
			prev->up = newelem;

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

		++this->count;
	}

	Data erase(const Anope::string &ukey)
	{
		Anope::string key;
		for (size_t i = 0, j = ukey.length(); i < j; ++i)
			key.push_back(char_traits::chartolower(ukey[i]));

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

		if (!cur || String(cur->key.c_str()).compare(key.c_str()))
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

		Data data = cur->data;
		delete cur;

		--this->count;

		return data;
	}

	class iterator
	{
		enum IterationState
		{
			ITERATION_AT_CENTER,
			ITERATION_FROM_CENTER
		};

		patricia_elem<Data> *elem;
		IterationState from;

	 public:
		iterator(patricia_tree<Data, char_traits> &tree)
		{
			this->elem = tree.root;
			this->from = ITERATION_AT_CENTER;
		}

		bool next()
		{
			if (this->elem == NULL)
				;
			else if (this->from == ITERATION_AT_CENTER)
			{
				if (this->elem->zero != NULL && this->elem->zero->bit > this->elem->bit)
				{
					this->elem = this->elem->zero;
					return this->next();
				}

				this->from = ITERATION_FROM_CENTER;
				return true;
			}
			else if (this->from == ITERATION_FROM_CENTER)
			{
				if (this->elem->one != NULL && this->elem->one->bit > this->elem->bit)
				{
					this->elem = this->elem->one;
					this->from = ITERATION_AT_CENTER;
					return this->next();
				}

				while (this->elem->up != NULL && this->elem->up->one == this->elem)
					this->elem = this->elem->up;

				if (this->elem->up != NULL)
				{
					this->elem = this->elem->up;
					return true;
				}
			}

			return false;
		}

		inline Data operator*()
		{
			return this->elem->data;
		}
	};
};


