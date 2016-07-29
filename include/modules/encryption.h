/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

namespace Encryption
{
	typedef std::pair<const unsigned char *, size_t> Hash;
	typedef std::pair<const uint32_t *, size_t> IV;

	class Context
	{
	 public:
	 	virtual ~Context() { }
		virtual void Update(const unsigned char *data, size_t len) anope_abstract;
		virtual void Finalize() anope_abstract;
		virtual Hash GetFinalizedHash() anope_abstract;
	};

	class Provider : public Service
	{
	 public:
		static constexpr const char *NAME = "hash";
		
		Provider(Module *creator, const Anope::string &sname) : Service(creator, NAME, sname) { }
		virtual ~Provider() { }

		virtual Context *CreateContext(IV * = NULL) anope_abstract;
		virtual IV GetDefaultIV() anope_abstract;
	};
}

