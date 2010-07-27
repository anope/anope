#include "services.h"

bool Anope::Match(const Anope::string &str, const Anope::string &mask, bool case_sensitive)
{
	size_t s = 0, m = 0, str_len = str.length(), mask_len = mask.length();

	while (s < str_len && m < mask_len && mask[m] != '*')
	{
		char string = str[s], wild = mask[m];
		if (case_sensitive)
		{
			if (wild != string && wild != '?')
				return false;
		}
		else
		{
			if (tolower(wild) != tolower(string) && wild != '?')
				return false;
		}

		++m;
		++s;
	}

	size_t sp = 0, mp = 0;
	while (s < str_len)
	{
		char string = str[s], wild = mask[m];
		if (wild == '*')
		{
			if (++m == mask_len)
				return 1;

			mp = m;
			sp = s + 1;
		}
		else
		{
			if (case_sensitive)
			{
				if (wild == string || wild == '?')
				{
					++m;
					++s;
				}
				else
				{
					m = mp;
					s = sp++;
				}
			}
			else
			{
				if (tolower(wild) == tolower(string) || wild == '?')
				{
					++m;
					++s;
				}
				else
				{
					m = mp;
					s = sp++;
				}
			}
		}
	}

	if (mask[m] == '*')
		++m;

	return m == mask_len;
}
