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

#define ANOPE_FORMAT(LAST, FORMAT, BUFFER) \
	do { \
		va_list _valist; \
		va_start(_valist, LAST); \
		(BUFFER) = Anope::Format(_valist, (FORMAT)); \
		va_end(_valist); \
	} while (false);

namespace Anope
{
	/** Encode a string as base-64.
	 * @param str The string to encode as base-64 .
	 */
	extern CoreExport Anope::string B64Encode(const Anope::string &str);

	/** Decode a string from base-64.
	 * @param str The string to decode from base-64.
	 */
	extern CoreExport Anope::string B64Decode(const Anope::string &str);

	/** Calculates the levenshtein distance between two strings.
	 * @param s1 The first string.
	 * @param s2 The second string.
	 */
	extern CoreExport size_t Distance(const Anope::string &s1, const Anope::string &s2);

	/** Expands a path fragment that is relative to the base directory.
	 * @param base The base directory that it is relative to.
	 * @param fragment The fragment to expand.
	 */
	extern CoreExport Anope::string Expand(const Anope::string &base, const Anope::string &fragment);

	/** Expands a config path. */
	inline auto ExpandConfig(const Anope::string &path) { return Expand(ConfigDir, path); }

	/** Expands a data path. */
	inline auto ExpandData(const Anope::string &path) { return Expand(DataDir, path); }

	/** Expands a locale path. */
	inline auto ExpandLocale(const Anope::string &path) { return Expand(LocaleDir, path); }

	/** Expands a log path. */
	inline auto ExpandLog(const Anope::string &path) { return Expand(LogDir, path); }

	/** Expands a module path. */
	inline auto ExpandModule(const Anope::string &path) { return Expand(ModuleDir, path); }

	/** Formats a string using one or more values. This uses snprintf internally but with a flexible buffer.
	 * @param fmt The message to format.
	 */
	extern CoreExport Anope::string Format(const char *fmt, ...) ATTR_FORMAT(1, 2);

	/** Formats a string using a list of values. This uses snprintf internally but with a flexible buffer.
	 * @param valist A list of values to format the message with.
	 * @param fmt The message to format.
	 */
	extern CoreExport Anope::string Format(va_list &valist, const char *fmt) ATTR_FORMAT(2, 0);

	/** Formats a CTCP message for sending to a client.
	 * @param name The name of the CTCP.
	 * @param body If present then the body of the CTCP.
	 * @return A formatted CTCP ready to send to a client.
	 */
	extern CoreExport Anope::string FormatCTCP(const Anope::string &name, const Anope::string &body = "");

	/** Parses a CTCP message received from a client.
	 * @param text The raw message to parse.
	 * @param name The location to store the name of the CTCP.
	 * @param body The location to store body of the CTCP if one is present.
	 * @return True if the message was a well formed CTCP; otherwise, false.
	 */
	extern CoreExport bool ParseCTCP(const Anope::string &text, Anope::string &name, Anope::string &body);

	/** Remove all formatting characters from a string.
	 * @param text A string containing formatting characters.
	 * @return A copy of \p text with all formatting characters removed.
	 */
	extern CoreExport Anope::string RemoveFormatting(const Anope::string &text);

	/** Replaces template variables within a string with values from a map.
	 * @param str The string to template from.
	 * @param vars The variables to replace within the string.
	 * @return The specified string with all variables replaced within it.
	 */
	extern CoreExport Anope::string Template(const Anope::string &str, const Anope::map<Anope::string> &vars);
}

class CoreExport HelpWrapper final
{
private:
	std::vector<std::pair<Anope::string, Anope::string>> entries;
	size_t longest = 0;

public:
	void AddEntry(const Anope::string &name, const Anope::string &desc);
	void SendTo(CommandSource &source);
};

class CoreExport InfoFormatter final
{
private:
	size_t longest = 0;
	NickCore *nc;
	std::vector<std::pair<Anope::string, Anope::string> > replies;

public:
	InfoFormatter(NickCore *nc);
	Anope::string &operator[](const Anope::string &key);
	void AddOption(const Anope::string &opt);
	void SendTo(CommandSource &source);
};

class CoreExport LineWrapper final
{
private:
	std::vector<Anope::string> formatting;
	const size_t max_length;
	Anope::string text;

public:
	LineWrapper(const Anope::string &t, size_t ml = 0);
	bool GetLine(Anope::string &out);
};

class CoreExport ListFormatter final
{
public:
	using ListEntry = std::map<Anope::string, Anope::string>;
	using FlexibleFormatFn = std::function<Anope::string(ListEntry &)>;

private:
	std::vector<Anope::string> columns;
	std::vector<ListEntry> entries;
	FlexibleFormatFn flexiblerow;
	NickCore *nc;
	void SendFixed(CommandSource &source);
	void SendFlexible(CommandSource &source);

public:
	ListFormatter(NickCore *nc);
	ListFormatter &AddColumn(const Anope::string &name);
	void AddEntry(const ListEntry &entry);
	bool IsEmpty() const;
	void SendTo(CommandSource &source);
	void SetFlexible(const Anope::string &format);
	void SetFlexible(const FlexibleFormatFn &formatter);
};
