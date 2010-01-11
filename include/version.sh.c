/* version file handler for win32.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * Written by Dominick Meglio <codemastr@unrealircd.com>
 *
 */

/* Needed due to Windows lack of a decent interpreter */

#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <cctype>

long version_major, version_minor, version_patch, version_build, build;
char *version_extra = NULL;
char version[1024];
char version_dotted[1024];


void load_ctrl(FILE *);
long get_value(char *);
char *get_value_str(char *);
char *strip(char *);
void parse_version(FILE *);
void write_version(FILE *, const char *);
void parse_line(FILE *, char *);

int main(int argc, char *argv[])
{
	if (argc < 4) {
		fprintf(stderr, "Syntax: %s <version.log> <version.sh> <version.h>\n", argv[0]);
		exit(1);
	}
	FILE *fd = fopen(argv[1], "r");


	if (!fd) {
		fprintf(stderr, "Error: Unable to find control file: %s\n", argv[1]);
		exit(0);
	}

	load_ctrl(fd);
	fclose(fd);

	_snprintf(version, 1024, "%ld.%ld.%ld%s (%ld)", version_major, version_minor,
			  version_patch, (version_extra ? version_extra : ""), version_build);

	_snprintf(version_dotted, 1024, "%ld.%ld.%ld%s.%ld", version_major, version_minor,
			  version_patch, (version_extra ? version_extra : ""), version_build);

	fd = fopen(argv[3], "r");

	if (fd) {
		parse_version(fd);
		fclose(fd);
	} else
		build = 1;


	fd = fopen(argv[3], "w");
	write_version(fd, argv[2]);
	fclose(fd);

	if (version_extra)
		free(version_extra);
}

void load_ctrl(FILE * fd)
{
	char buf[512];
	while (fgets(buf, 511, fd)) {
		char *var;

		strip(buf);

		var = strtok(buf, "=");
		if (!var)
			continue;
		if (!strcmp(var, "VERSION_MAJOR"))
			version_major = get_value(strtok(NULL, ""));
		else if (!strcmp(var, "VERSION_MINOR"))
			version_minor = get_value(strtok(NULL, ""));
		else if (!strcmp(var, "VERSION_PATCH"))
			version_patch = get_value(strtok(NULL, ""));
		else if (!strcmp(var, "VERSION_BUILD"))
			version_build = get_value(strtok(NULL, ""));
		else if (!strcmp(var, "VERSION_EXTRA"))
			version_extra = get_value_str(strtok(NULL, ""));

	}
}

char *strip(char *str)
{
	char *c;
	if ((c = strchr(str, '\n')))
		*c = 0;
	if ((c = strchr(str, '\r')))
		*c = 0;
	return str;
}

long get_value(char *string)
{
	// XXX : if the fields in version.log are empty strtok returns a double quote, dont try to atol it then
	if (string[1] != '"')
		return atol(get_value_str(string));
	else
		return 0;
}

char *get_value_str(char *string)
{
	int len;
	char *newstr;

	if (*string == '"')
		string++;

	len = strlen(string);

	if (string[len - 1] == '"')
		string[len - 1] = 0;
	if (!*string)
		return NULL;
	newstr = (char *)malloc(len + 1);
	strcpy(newstr, string);
	return newstr;
	//return strdup(string);
}

void parse_version(FILE * fd)
{
	char buf[1024];

	while (fgets(buf, 1023, fd)) {
		char *para1;

		strip(buf);
		para1 = strtok(buf, " \t");

		if (!para1)
			continue;

		if (!strcmp(para1, "#define")) {
			char *para2 = strtok(NULL, " \t");

			if (!para2)
				continue;

			if (!strcmp(para2, "BUILD")) {
				char *value = strtok(NULL, "");
				build = get_value(value);
				build++;
				return;
			}
		}
	}
	build = 1;
}

void write_version(FILE * fd, const char *input)
{
	FILE *fdin = fopen(input, "r");
	char buf[1024];
	short until_eof = 0;

	while (fgets(buf, 1023, fdin)) {
		strip(buf);

		if (until_eof)
		{
			if (!strcmp(buf, "EOF"))
				break;
			else
				parse_line(fd, buf);
		}

		if (!strcmp(buf, "cat >$VERSIONH <<EOF"))
			until_eof = 1;
	}

}

void parse_line(FILE * fd, char *line)
{
	char *c;
	for (c = line; *c; c++) {
		/* It's a variable, find out which */
		if (*c == '$') {
			char *var, *varbegin;

			if (*(c + 1))
				c++;
			else
				continue;
			for (var = varbegin = c; var; var++) {
				if (!isalnum(*var) && *var != '_')
					break;
			}
			if (var != varbegin) {
				char tmp = *var;

				*var = 0;
				if (!strcmp(varbegin, "VERSION_MAJOR"))
					fprintf(fd, "%ld", version_major);
				else if (!strcmp(varbegin, "VERSION_MINOR"))
					fprintf(fd, "%ld", version_minor);
				else if (!strcmp(varbegin, "VERSION_PATCH"))
					fprintf(fd, "%ld", version_patch);
				else if (!strcmp(varbegin, "VERSION_EXTRA")) {
					if (version_extra)
						fprintf(fd, "%s", version_extra);
				} else if (!strcmp(varbegin, "VERSION_BUILD"))
					fprintf(fd, "%ld", version_build);
				else if (!strcmp(varbegin, "BUILD"))
					fprintf(fd, "%ld", build);
				else if (!strcmp(varbegin, "VERSION"))
					fprintf(fd, "%s", version);
				else if (!strcmp(varbegin, "VERSIONDOTTED"))
					fprintf(fd, "%s", version_dotted);
				if (tmp) fputc(tmp, fd);
			}
			c = var;
		} else
			fputc(*c, fd);
	}
	/* We only need \n here - we didn't open the file as binary -GD */
	fprintf(fd, "\n");
}
