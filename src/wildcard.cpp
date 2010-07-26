#include "services.h"

static bool match_internal(const unsigned char *str, const unsigned char *mask, bool case_sensitive)
{
	const unsigned char *cp = NULL, *mp = NULL;
	const unsigned char *string = str;
	const unsigned char *wild = mask;

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

		++wild;
		++string;
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

bool Anope::Match(const Anope::string &str, const Anope::string &mask, bool case_sensitive)
{
	return match_internal(reinterpret_cast<const unsigned char *>(str.c_str()), reinterpret_cast<const unsigned char *>(mask.c_str()), case_sensitive);
}
