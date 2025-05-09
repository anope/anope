/*
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

#include <optional>

namespace Anope
{
	/** Attempts to convert a string to any type.
	 * @param in The value to convert.
	 * @param leftover If non-nullptr then the location to store leftover data.
	 */
	template<typename T>
	inline std::optional<T> TryConvert(const Anope::string &in, Anope::string *leftover = nullptr)
	{
		std::istringstream tmp(in.str());
		T out;
		if (!(tmp >> out))
			return std::nullopt;

		if (leftover)
		{
			leftover->clear();
			std::getline(tmp, leftover->str());
		}
		else
		{
			char extra;
			if (tmp >> extra)
				return std::nullopt;
		}
		return out;
	}

	/** Converts a string to any type.
	 * @param in The value to convert.
	 * @param def The default to use if the conversion failed.
	 * @param leftover If non-nullptr then the location to store leftover data.
	 */
	template<typename T>
	inline T Convert(const Anope::string &in, T def, Anope::string *leftover = nullptr)
	{
		return TryConvert<T>(in, leftover).value_or(def);
	}

	/** Attempts to convert any type to a string.
	 * @param in The value to convert.
	 */
	template <class T>
	inline std::optional<Anope::string> TryString(const T &in)
	{
		std::ostringstream tmp;
		if (!(tmp << in))
			return std::nullopt;
		return tmp.str();
	}

	/** No-op function that returns the string that was passed to it.
	 * @param in The string to return.
	 */
	inline const string &ToString(const string &in)
	{
		return in;
	}

	/** Converts a std::string to a string.
	 * @param in The value to convert.
	 */
	inline string ToString(const std::string &in)
	{
		return in;
	}

	/** Converts a char array to a string.
	 * @param in The value to convert.
	 */
	inline string ToString(const char *in)
	{
		return string(in);
	}

	/** Converts a char to a string.
	 * @param in The value to convert.
	 */
	inline string ToString(char in)
	{
		return string(1, static_cast<string::value_type>(in));
	}

	/** Converts an unsigned char to a string.
	 * @param in The value to convert.
	 */
	inline string ToString(unsigned char in)
	{
		return string(1, static_cast<string::value_type>(in));
	}

	/** Converts a bool to a string.
	 * @param in The value to convert.
	 */
	inline string ToString(bool in)
	{
		return (in ? "1" : "0");
	}

	/** Converts a type that std::to_string is implemented for to a string.
	 * @param in The value to convert.
	 */
	template<typename Stringable>
	inline std::enable_if_t<std::is_arithmetic_v<Stringable>, string> ToString(const Stringable &in)
	{
		return std::to_string(in);
	}

	/** Converts any type to a string.
	 * @param in The value to convert.
	 */
	template <class T>
	inline std::enable_if_t<!std::is_arithmetic_v<T>, string> ToString(const T &in)
	{
		return TryString(in).value_or(Anope::string());
	}
}
