/* Needed due to Windows lack of a decent interpreter */

#include <string.h>
#include <stdio.h>

char *strip(char *str)
{
	char *c;
	if ((c = strchr(str,'\n')))
		*c = 0;
	if ((c = strchr(str,'\r')))
		*c = 0;
	return str;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		exit(1);

	/* Build the index file */
	if (!strcmp(argv[1], "index"))
	{
		FILE *fd = fopen("en_us.l", "rb");
		FILE *fdout = fopen("index", "wb");
		char buf[1024];
		if (!fd || !fdout)
			exit(2);

		while (fgets(buf, 1023, fd))
		{
			if (isupper(*buf))
				fprintf(fdout, "%s", buf);
		}
		fclose(fd);
		fclose(fdout);
	}
	/* Build the language.h file */
	else if (!strcmp(argv[1], "language.h"))
	{
		FILE *fd = fopen("index", "r");
		FILE *fdout = fopen("language.h", "w");
		char buf[1024];
		int i = 0;		

		if (!fd || !fdout)
			exit(2);

		fprintf(stderr, "Generating language.h... ");

		while (fgets(buf, 1023, fd))
			fprintf(fdout, "#define %-32s %d\n", strip(buf), i++);

		fprintf(fdout, "#define NUM_STRINGS %d\n", i);
		fprintf(stderr, "%d strings\n", i);
	}
	return 0;
			
}
