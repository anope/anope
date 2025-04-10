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

class TextSplitter final
{
private:
	Anope::string text;
	std::vector<Anope::string> formatting;

public:
	TextSplitter(const Anope::string &t);
	bool GetLine(Anope::string &out);
};

