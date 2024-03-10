/*
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

namespace Encryption
{
	/** Base class for encryption contexts. */
	class Context
	{
	public:
		virtual ~Context() = default;

		/** Updates the encryption context with the specified data.
		 * @param str The data to update the context with.
		 */
		inline void Update(const Anope::string &str)
		{
			Update(reinterpret_cast<const unsigned char *>(str.c_str()), str.length());
		}

		/** Updates the encryption context with the specified data.
		 * @param data The data to update the context with.
		 * @param len The length of the data.
		 */
		virtual void Update(const unsigned char *data, size_t len) = 0;

		/** Finalises the encryption context and returns the digest. */
		virtual Anope::string Finalize() = 0;
	};

	/** Provider of encryption contexts. */
	class Provider
		: public Service
	{
	public:
		/** The byte size of the block cipher. */
		const size_t block_size;

		/** The byte size of the resulting digest. */
		const size_t digest_size;

		/** Creates a provider of encryption contexts.
		 * @param creator The module that created this provider.
		 * @param algorithm The name of the encryption algorithm.
		 * @param bs The byte size of the block cipher.
		 * @param ds The byte size of the resulting digest.
		 */
		Provider(Module *creator, const Anope::string &algorithm, size_t bs, size_t ds)
			: Service(creator, "Encryption::Provider", algorithm)
			, block_size(bs)
			, digest_size(ds)
		{
		}

		virtual ~Provider() = default;

		/** Checks whether a plain text value matches a hash created by this provider. */
		virtual bool Compare(const Anope::string &hash, const Anope::string &plain)
		{
			return hash.equals_cs(plain);
		}

		/** Creates a new encryption context. */
		virtual std::unique_ptr<Context> CreateContext() = 0;

		template<typename... Args>
		Anope::string Encrypt(Args &&...args)
		{
			auto context = CreateContext();
			context->Update(std::forward<Args>(args)...);
			return context->Finalize();
		}

		inline Anope::string HMAC(const Anope::string &key, const Anope::string &data)
		{
			if (!block_size)
				return {};

			auto keybuf = key.length() > block_size ? Encrypt(key) : key;
			keybuf.resize(block_size);

			Anope::string hmac1;
			Anope::string hmac2;
			for (size_t i = 0; i < block_size; ++i)
			{
				hmac1.push_back(static_cast<char>(keybuf[i] ^ 0x5C));
				hmac2.push_back(static_cast<char>(keybuf[i] ^ 0x36));
			}
			hmac2.append(data);
			hmac1.append(Encrypt(hmac2));

			return Encrypt(hmac1);
		}
	};

	/** Helper template for creating simple providers of encryption contexts. */
	template <typename T>
	class SimpleProvider final
		: public Provider
	{
	public:
		/** Creates a simple provider of encryption contexts.
		 * @param creator The module that created this provider.
		 * @param algorithm The name of the encryption algorithm.
		 * @param bs The byte size of the block cipher.
		 * @param ds The byte size of the resulting digest.
		 */
		SimpleProvider(Module *creator, const Anope::string &algorithm, size_t bs, size_t ds)
			: Provider(creator, algorithm, bs, ds)
		{
		}

		/** @copydoc Encryption::Provider::CreateContext. */
		std::unique_ptr<Context> CreateContext() override
		{
			return std::make_unique<T>();
		}
	};
}
