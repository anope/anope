#include "services.h"

static bool match_internal(const unsigned char *str, const unsigned char *mask, bool case_sensitive)
{
	unsigned char *cp = NULL, *mp = NULL;
	unsigned char *string = const_cast<unsigned char *>(str); // XXX: unsafe cast
	unsigned char *wild = const_cast<unsigned char *>(mask); // XXX: unsafe cast

	while (*string && *wild != '*')
	{
		if (case_sensitive)
		{
			if (*wild != *string && *wild != '?')
				return false;
		}
		else
		{
			if (tolower(*wild) != tolower(*string) && *wild != '?')
				return false;
		}

		wild++;
		string++;
	}

	while (*string)
	{
		if (*wild == '*')
		{
			if (!*++wild)
				return 1;

			mp = wild;
			cp = string + 1;
		}
		else
		{
			if (case_sensitive)
			{
				if (*wild == *string || *wild == '?')
				{
					++wild;
					++string;
				}
				else
				{
					wild = mp;
					string = cp++;
				}
			}
			else
			{
				if (tolower(*wild) == tolower(*string) || *wild == '?')
				{
					++wild;
					++string;
				}
				else
				{
					wild = mp;
					string = cp++;
				}
			}
		}

	}

	while (*wild == '*')
		++wild;

	return !*wild;
}

bool Anope::Match(const std::string &str, const std::string &mask, bool case_sensitive)
{
	return match_internal(reinterpret_cast<const unsigned char *>(str.c_str()), reinterpret_cast<const unsigned char *>(mask.c_str()), case_sensitive);
}
