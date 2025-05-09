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

class CoreExport HelpWrapper final
{
private:
	std::vector<std::pair<Anope::string, Anope::string>> entries;
	size_t longest = 0;

public:
	void AddEntry(const Anope::string &name, const Anope::string &desc);
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

