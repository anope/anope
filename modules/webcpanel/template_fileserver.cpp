/*
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "webcpanel.h"
#include <fstream>
#include <stack>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct ForLoop
{
	static std::vector<ForLoop> Stack;

	size_t start;       /* Index of start of this loop */
	std::vector<Anope::string> vars; /* User defined variables */
	typedef std::pair<TemplateFileServer::Replacements::iterator, TemplateFileServer::Replacements::iterator> range;
	std::vector<range> ranges; /* iterator ranges for each variable */

	ForLoop(size_t s, TemplateFileServer::Replacements &r, const std::vector<Anope::string> &v, const std::vector<Anope::string> &r_names) : start(s), vars(v)
	{
		for (unsigned i = 0; i < r_names.size(); ++i)
			ranges.push_back(r.equal_range(r_names[i]));
	}

	void increment(const TemplateFileServer::Replacements &r)
	{
		for (unsigned i = 0; i < ranges.size(); ++i)
		{
			range &ra = ranges[i];

			if (ra.first != r.end() && ra.first != ra.second)
				++ra.first;
		}
	}

	bool finished(const TemplateFileServer::Replacements &r) const
	{
		for (unsigned i = 0; i < ranges.size(); ++i)
		{
			const range &ra = ranges[i];

			if (ra.first != r.end() && ra.first != ra.second)
				return false;
		}

		return true;
	}
};
std::vector<ForLoop> ForLoop::Stack;

std::stack<bool> IfStack;

static Anope::string FindReplacement(const TemplateFileServer::Replacements &r, const Anope::string &key)
{
	/* Search first through for loop stack then global replacements */
	for (unsigned i = ForLoop::Stack.size(); i > 0; --i)
	{
		ForLoop &fl = ForLoop::Stack[i - 1];

		for (unsigned j = 0; j < fl.vars.size(); ++j)
		{
			const Anope::string &var_name = fl.vars[j];

			if (key == var_name)
			{
				const ForLoop::range &range = fl.ranges[j];

				if (range.first != r.end() && range.first != range.second)
				{
					return range.first->second;
				}
			}
		}
	}
	
	TemplateFileServer::Replacements::const_iterator it = r.find(key);
	if (it != r.end())
		return it->second;
	return "";
}

TemplateFileServer::TemplateFileServer(const Anope::string &f_n) : file_name(f_n)
{
}

void TemplateFileServer::Serve(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, Replacements &r)
{
	int fd = open((template_base + "/" + this->file_name).c_str(), O_RDONLY);
	if (fd < 0)
	{
		Log(LOG_NORMAL, "httpd") << "Error serving file " << page_name << " (" << (template_base + "/" + this->file_name) << "): " << strerror(errno);

		client->SendError(HTTP_PAGE_NOT_FOUND, "Page not found");
		return;
	}

	Anope::string buf;

	int i;
	char buffer[BUFSIZE];
	while ((i = read(fd, buffer, sizeof(buffer) - 1)) > 0)
	{
		buffer[i] = 0;
		buf += buffer;
	}
	
	close(fd);

	Anope::string finished;

	bool escaped = false;
	for (unsigned j = 0; j < buf.length(); ++j)
	{
		if (buf[j] == '\\' && j + 1 < buf.length() && (buf[j + 1] == '{' || buf[j + 1] == '}'))
			escaped = true;
		else if (buf[j] == '{' && !escaped)
		{
			size_t f = buf.substr(j).find('}');
			if (f == Anope::string::npos)
				break;
			const Anope::string &content = buf.substr(j + 1, f - 1);

			if (content.find("IF ") == 0)
			{
				std::vector<Anope::string> tokens;
				spacesepstream(content).GetTokens(tokens);

				if (tokens.size() == 4 && tokens[1] == "EQ")
				{
					Anope::string first = FindReplacement(r, tokens[2]), second = FindReplacement(r, tokens[3]);
					if (first.empty())
						first = tokens[2];
					if (second.empty())
						second = tokens[3];

					bool stackok = IfStack.empty() || IfStack.top();
					IfStack.push(stackok && first == second);
				}
				else if (tokens.size() == 3 && tokens[1] == "EXISTS")
				{
					bool stackok = IfStack.empty() || IfStack.top();
					IfStack.push(stackok && r.count(tokens[2]) > 0);
				}
				else
					Log() << "Invalid IF in web template " << this->file_name;
			}
			else if (content == "ELSE")
			{
				if (IfStack.empty())
					Log() << "Invalid ELSE with no stack in web template" << this->file_name;
				else
				{
					bool old = IfStack.top();
					IfStack.pop(); // Pop off previous if()
					bool stackok = IfStack.empty() || IfStack.top();
					IfStack.push(stackok && !old); // Push back the opposite of what was popped
				}
			}
			else if (content == "END IF")
			{
				if (IfStack.empty())
					Log() << "END IF with empty stack?";
				else
					IfStack.pop();
			}
			else if (content.find("FOR ") == 0)
			{
				std::vector<Anope::string> tokens;
				spacesepstream(content).GetTokens(tokens);

				if (tokens.size() != 4 || tokens[2] != "IN")
					Log() << "Invalid FOR in web template " << this->file_name;
				else
				{
					std::vector<Anope::string> temp_variables, real_variables;
					commasepstream(tokens[1]).GetTokens(temp_variables);
					commasepstream(tokens[3]).GetTokens(real_variables);

					if (temp_variables.size() != real_variables.size())
						Log() << "Invalid FOR in web template " << this->file_name << " variable mismatch";
					else
						ForLoop::Stack.push_back(ForLoop(j + f, r, temp_variables, real_variables));
				}
			}
			else if (content == "END FOR")
			{
				if (ForLoop::Stack.empty())
					Log() << "END FOR with empty stack?";
				else
				{
					ForLoop &fl = ForLoop::Stack.back();
					if (fl.finished(r))
						ForLoop::Stack.pop_back();
					else
					{
						fl.increment(r);
						if (fl.finished(r))
							ForLoop::Stack.pop_back();
						else
						{
							j = fl.start; // Move pointer back to start of the loop
							continue; // To prevent skipping over this block which doesn't exist anymore
						}
					}
				}
			}
			else if (content.find("INCLUDE ") == 0)
			{
				std::vector<Anope::string> tokens;
				spacesepstream(content).GetTokens(tokens);

				if (tokens.size() != 2)
					Log() << "Invalid INCLUDE in web template " << this->file_name;
				else
				{
					if (!finished.empty())
					{
						reply.Write(finished); // Write out what we have currently so we insert this files contents here
						finished.clear();
					}

					TemplateFileServer tfs(tokens[1]);
					tfs.Serve(server, page_name, client, message, reply, r);
				}
			}
			else
			{
				// If the if stack is empty or we are in a true statement
				bool ifok = IfStack.empty() || IfStack.top();
				bool forok = ForLoop::Stack.empty() || !ForLoop::Stack.back().finished(r);
			
				if (ifok && forok)
				{
					const Anope::string &replacement = FindReplacement(r, content.substr(0, f - 1));
					finished += replacement;
				}
			}

			j += f; // Skip over this whole block
		}
		else
		{
			escaped = false;

			// If the if stack is empty or we are in a true statement
			bool ifok = IfStack.empty() || IfStack.top();
			bool forok = ForLoop::Stack.empty() || !ForLoop::Stack.back().finished(r);
			
			if (ifok && forok)
				finished += buf[j];
		}
	}

	if (!finished.empty())
		reply.Write(finished);
}

