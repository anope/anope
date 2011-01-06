/* Compiler for language definition files.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 * 
 *
 */

/*
 * A language definition file contains all strings which Services sends to
 * users in a particular language.  A language file may contain comments
 * (lines beginning with "#") and blank lines.  All other lines must adhere
 * to the following format:
 *
 * Each string definition begins with the C name of a message (as defined
 * in the file "index"--see below).  This must be alone on a line, preceded
 * and followed by no blank space.  Following this line are zero or more
 * lines of text; each line of text must begin with exactly one tab
 * character, which is discarded.  Newlines are retained in the strings,
 * except the last newline in the text, which is discarded.  A message with
 * no text is replaced by a null pointer in the array (not an empty
 * string).
 *
 * All messages in the program are listed, one per line, in the "index"
 * file.  No comments or blank lines are permitted in that file.  The index
 * file can be generated from a language file with a command like:
 *	grep '^[A-Z]' en_us.l >index
 *
 * This program takes one parameter, the name of the language file.  It
 * generates a compiled language file whose name is created by removing any
 * extension on the source file on the input filename.
 *
 * You may also pass a "-w" option to print warnings for missing strings.
 *
 * This program isn't very flexible, because it doesn't need to be, but
 * anyone who wants to try making it more flexible is welcome to.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#undef getline

int numstrings = 0;	/* Number of strings we should have */
char **stringnames;	/* Names of the strings (from index file) */
char **strings;		/* Strings we have loaded */

int linenum = 0;	/* Current line number in input file */

#ifdef _WIN32
#define snprintf _snprintf
#endif

char *anopeStrDup(const char *src);

/*************************************************************************/

/* Read the index file and load numstrings and stringnames.  Return -1 on
 * error, 0 on success. */

int read_index_file()
{
    FILE *f;
    char buf[256];
    int i;

    if (!(f = fopen("index", "rb"))) {
	perror("fopen(index)");
	return -1;
    }
    while (fgets(buf, sizeof(buf), f))
	numstrings++;
    if (!(stringnames = calloc(sizeof(char *), numstrings))) {
	perror("calloc(stringnames)");
	fclose(f);
	return -1;
    }
    if (!(strings = calloc(sizeof(char *), numstrings))) {
	perror("calloc(strings)");
	fclose(f);
	return -1;
    }
    fseek(f, 0, SEEK_SET);
    i = 0;
    while (fgets(buf, sizeof(buf), f)) {
	if (buf[strlen(buf)-1] == '\n')
	    buf[strlen(buf)-1] = '\0';
	if (!(stringnames[i++] = anopeStrDup(buf))) {
	    perror("strdup()");
	    fclose(f);
	    return -1;
	}
    }
    fclose(f);
    return 0;
}

/*************************************************************************/

/* Return the index of a string name in stringnames, or -1 if not found. */

int stringnum(const char *name)
{
    int i;

    for (i = 0; i < numstrings; i++) {
	if (strcmp(stringnames[i], name) == 0)
	    return i;
    }
    return -1;
}

/*************************************************************************/

/* Read a non-comment, non-blank line from the input file.  Return NULL at
 * end of file. */

char *ano_getline(FILE *f)
{
    static char buf[1024];
    char *s;

    do {
	if (!(fgets(buf, sizeof(buf), f)))
	    return NULL;
	linenum++;
    } while (*buf == '#' || *buf == '\n');
    s = buf + strlen(buf)-1;
    if (*s == '\n')
	*s = '\0';
    return buf;
}

/*************************************************************************/

/* Write a 32-bit value to a file in big-endian order. */

int fput32(int val, FILE *f)
{
    if (fputc(val>>24, f) < 0 ||
        fputc(val>>16, f) < 0 ||
        fputc(val>> 8, f) < 0 ||
        fputc(val    , f) < 0
    ) {
	return -1;
    } else {
	return 0;
    }
}

/*************************************************************************/
char *anopeStrDup(const char *src) {
    char *ret=NULL;
    if(src) {
        if( (ret = (char *)malloc(strlen(src)+1)) ) {;
            strcpy(ret,src);
        }
    }
    return ret;
}

/*************************************************************************/
int main(int ac, char **av)
{
    char *filename = NULL, *s;
    char langname[254], outfile[256];
    FILE *in, *out;
    int warn = 0;
    int retval = 0;
    int curstring = -2, i;
    char *line;
    int pos;
    int maxerr = 50;	/* Max errors before we bail out */

    if (ac >= 2 && strcmp(av[1], "-w") == 0) {
	warn = 1;
	av[1] = av[2];
	ac--;
    }
    if (ac != 2) {
	fprintf(stderr, "Usage: %s [-w] <lang-file>\n", av[0]);
	return 1;
    }
    filename = av[1];
    s = strrchr(filename, '.');
    if (!s)
	s = filename + strlen(filename);
    if (s-filename > sizeof(langname)-3)
	s = filename + sizeof(langname)-1;
    strncpy(langname, filename, s-filename);
    langname[s-filename] = '\0';
    snprintf(outfile, sizeof(outfile), "%s", langname);

    if (read_index_file() < 0)
	return 1;
    if (!(in = fopen(filename, "rb"))) {
	perror(filename);
	return 1;
    }
    if (!(out = fopen(outfile, "wb"))) {
	perror(outfile);
	fclose(in);
	return 1;
    }

    while (maxerr > 0 && (line = ano_getline(in)) != NULL) {
	if (*line == '\t') {
	    if (curstring == -2) {
		fprintf(stderr, "%s:%d: Junk at beginning of file\n",
			filename, linenum);
		retval = 1;
	    } else if (curstring >= 0) {
		line++;
		i = strings[curstring] ? strlen(strings[curstring]) : 0;
		if (!(strings[curstring] =
			realloc(strings[curstring], i+strlen(line)+2))) {
		    fprintf(stderr, "%s:%d: Out of memory!\n",filename,linenum);
		    return 2;
		}
		sprintf(strings[curstring]+i, "%s\n", line);
	    }
	} else {

	    if ((curstring = stringnum(line)) < 0) {
		fprintf(stderr, "%s:%d: Unknown string name `%s'\n",
			filename, linenum, line);
		retval = 1;
		maxerr--;
	    } else if (strings[curstring]) {
		fprintf(stderr, "%s:%d: Duplicate occurrence of string `%s'\n",
			filename, linenum, line);
		retval = 1;
		maxerr--;
	    } else {
		if (!(strings[curstring] = malloc(1))) {
		    fprintf(stderr, "%s:%d: Out of memory!\n",filename,linenum);
		    return 2;
		}
		*strings[curstring] = '\0';
	    }

	    if (maxerr == 0)
		fprintf(stderr, "%s:%d: Too many errors!\n", filename, linenum);
	    
	}
    }

    fput32(numstrings, out);
    pos = numstrings * 8 + 4;
    for (i = 0; i < numstrings; i++) {
	int len = strings[i] && *strings[i] ? strlen(strings[i])-1 : 0;
	fput32(pos, out);
	fput32(len, out);
	pos += len;
    }
    for (i = 0; i < numstrings; i++) {
	if (strings[i]) {
	    if (*strings[i])
		strings[i][strlen(strings[i])-1] = '\0';   /* kill last \n */
	    if (*strings[i])
		fputs(strings[i], out);
	} else if (warn) {
	    fprintf(stderr, "%s: String `%s' missing\n", filename,
			stringnames[i]);
	}
    }

    fclose(in);
    fclose(out);
    return retval;
}

/*************************************************************************/
