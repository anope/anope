#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>

#ifndef _WIN32
static const std::string C_LBLUE = "\033[1;34m";
static const std::string C_NONE = "\033[m";
#else
static const std::string C_LBLUE = "";
static const std::string C_NONE = "";
#endif

/** sepstream allows for splitting token seperated lists.
 * Each successive call to sepstream::GetToken() returns
 * the next token, until none remain, at which point the method returns
 * an empty string.
 */
class sepstream
{
 private:
	/** Original string.
	 */
	std::string tokens;
	/** Last position of a seperator token
	 */
	std::string::iterator last_starting_position;
	/** Current string position
	 */
	std::string::iterator n;
	/** Seperator value
	 */
	char sep;
 public:
	/** Create a sepstream and fill it with the provided data
	 */
	sepstream(const std::string &source, char seperator);
	virtual ~sepstream() { }

	/** Fetch the next token from the stream
	 * @param token The next token from the stream is placed here
	 * @return True if tokens still remain, false if there are none left
	 */
	virtual bool GetToken(std::string &token);

	/** Fetch the entire remaining stream, without tokenizing
	 * @return The remaining part of the stream
	 */
	virtual const std::string GetRemaining();

	/** Returns true if the end of the stream has been reached
	 * @return True if the end of the stream has been reached, otherwise false
	 */
	virtual bool StreamEnd();
};

sepstream::sepstream(const std::string &source, char seperator) : tokens(source), sep(seperator)
{
	last_starting_position = n = tokens.begin();
}

bool sepstream::GetToken(std::string &token)
{
	std::string::iterator lsp = last_starting_position;

	while (n != tokens.end())
	{
		if (*n == sep || n + 1 == tokens.end())
		{
			last_starting_position = n + 1;
			token = std::string(lsp, n + 1 == tokens.end() ? n + 1 : n);

			while (token.length() && token.rfind(sep) == token.length() - 1)
				token.erase(token.end() - 1);

			++n;

			return true;
		}

		++n;
	}

	token.clear();
	return false;
}

const std::string sepstream::GetRemaining()
{
	return std::string(n, tokens.end());
}

bool sepstream::StreamEnd()
{
	return n == tokens.end();
}

std::vector<std::string> BuildStringVector(const std::string &src, char delim = ' ')
{
	sepstream tokens(src, delim);
	std::string token;
	std::vector<std::string> Ret;

	while (tokens.GetToken(token))
		Ret.push_back(token);

	return Ret;
}

std::string Hex(const char *data, size_t len)
{
	const char hextable[] = "0123456789abcdef";

	std::string rv;
	for (size_t i = 0; i < len; ++i)
	{
		unsigned char c = data[i];
		rv += hextable[c >> 4];
		rv += hextable[c & 0xF];
	}
	return rv;
}

static const std::string Base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char Pad64 = '=';

unsigned b64_decode(const std::string &src, char *target)
{
	unsigned state = 0, tarindex = 0;
	std::string::const_iterator ch = src.begin(), end = src.end();
	for (; ch != end; ++ch)
	{
		if (isspace(*ch)) /* Skip whitespace anywhere */
			continue;

		if (*ch == Pad64)
			break;

		size_t pos = Base64.find(*ch);
		if (pos == std::string::npos) /* A non-base64 character */
			return 0;

		switch (state)
		{
			case 0:
				target[tarindex] = pos << 2;
				state = 1;
				break;
			case 1:
				target[tarindex++] |= pos >> 4;
				target[tarindex] = (pos & 0x0f) << 4;
				state = 2;
				break;
			case 2:
				target[tarindex++] |= pos >> 2;
				target[tarindex] = (pos & 0x03) << 6;
				state = 3;
				break;
			case 3:
				target[tarindex++] |= pos;
				state = 0;
		}
	}
	return tarindex;
}

int main(int argc, char *argv[])
{
	std::cout << C_LBLUE << "Anope 1.9.2 -> 1.9.3 database upgrader" << C_NONE << std::endl << std::endl;

	std::ifstream in("anope.db.old");
	if (!in.is_open())
	{
		std::cout << C_LBLUE << "Could not open anope.db.old for reading" << C_NONE << std::endl << std::endl;
		return 1;
	}

	std::ofstream out("anope.db");
	if (!out.is_open())
	{
		std::cout << C_LBLUE << "Could not open anope.db for writing" << C_NONE << std::endl << std::endl;
		in.close();
		return 1;
	}

	std::string line;
	while (getline(in, line))
	{
		if (line.substr(0, 2) == "NC")
		{
			std::vector<std::string> parts = BuildStringVector(line);
			std::string password = parts[2];
			size_t colon = password.find(':');
			if (colon != std::string::npos && password.substr(0, colon) != "plain")
			{
				std::string hash = password.substr(colon + 1), iv, hashm = password.substr(0, colon);
				unsigned len;
				if (hashm == "sha256")
				{
					colon = hash.find(':');
					iv = hash.substr(colon + 1);
					hash = hash.substr(0, colon);

					char *iv_decoded = new char[iv.length() * 3 / 4 + 1];
					memset(iv_decoded, 0, iv.length() * 3 / 4 + 1);
					len = b64_decode(iv, iv_decoded);
					if (len)
						iv = Hex(iv_decoded, len);
					else
						iv.clear();
					delete [] iv_decoded;
				}
				char *hash_decoded = new char[hash.length() * 3 / 4 + 1];
				memset(hash_decoded, 0, hash.length() * 3 / 4 + 1);
				len = b64_decode(hash, hash_decoded);
				if (len)
					hash = Hex(hash_decoded, len);
				else
					hash.clear();
				delete [] hash_decoded;
				password = hashm + ":";
				if (!hash.empty())
					password += hash;
				if (!iv.empty())
					password += ":" + iv;
				parts[2] = password;
			}
			line.clear();
			for (unsigned part = 0, end = parts.size(); part < end; ++part)
			{
				if (part)
					line += ' ';
				line += parts[part];
			}
		}
		out << line << std::endl;
	}

	std::cout << "Upgrade complete!" << std::endl;

	in.close();
	out.close();

	return 0;
}
