#include <winsock.h>
#include <stdio.h>
#include <mysql.h>

MYSQL *mysql;
MYSQL_RES *result = NULL;
MYSQL_ROW row;

int __stdcall mysql_Connect(char *server, char *user, char *pass)
{
    mysql = mysql_init(NULL);
    return (int) mysql_real_connect(mysql, server, user, pass, NULL, 0,
                                    NULL, 0);
}

int __stdcall mysql_SelectDb(char *db)
{
    return (int) mysql_select_db(mysql, db);
}

const char *__stdcall mysql_Error()
{
    return mysql_error(mysql);
}

int __stdcall mysql_Query(char *query)
{
    if (result) {
        mysql_free_result(result);
        result = NULL;
    }
    return (int) mysql_real_query(mysql, query, strlen(query));
}

int __stdcall mysql_NumRows()
{
    if (!result)
        result = mysql_store_result(mysql);
    return mysql_num_rows(result);
}

char *strip(char *str)
{
    char *c;
    if ((c = strrchr(str, '\n')))
        *c = 0;
    if ((c = strrchr(str, '\r')))
        *c = 0;
    return str;
}

void add_line(char **buf, char *line)
{
    int oldlen;
    char *tmp;

    if (*buf != NULL) {
        oldlen = strlen(*buf);
        tmp = malloc(oldlen + 1);
        strcpy(tmp, *buf);
        *buf = realloc(*buf, oldlen + strlen(line) + 1);
        strcpy(*buf, tmp);
        strcat(*buf, line);
        free(tmp);
    } else
        *buf = strdup(line);
}

int __stdcall mysql_LoadFromFile(char *file)
{
    FILE *fd = fopen(file, "r");
    char line[1024];
    char *query = NULL;


    if (!fd)
        return 0;
    while (fgets(line, 1024, fd)) {
        int len;
        strip(line);
        len = strlen(line);
        if (!*line || (*line == '-' && *(line + 1) == '-'))
            continue;
        else if (line[len - 1] == ';') {        /* End of a query */
            line[len - 1] = 0;
            add_line(&query, line);
            if (mysql_real_query(mysql, query, strlen(query))) {
                free(query);
                fclose(fd);
                return 0;
            }
            free(query);
            query = NULL;
        }
        else
            add_line(&query, line);
    }
    fclose(fd);
    return 1;
}

int __stdcall mysql_NumFields()
{
    if (!result)
        result = mysql_store_result(mysql);
    return mysql_num_fields(result);
}

int __stdcall mysql_FetchRow()
{
    if (!result)
        result = mysql_store_result(mysql);
    row = mysql_fetch_row(result);
    return (int) row;
}

char *__stdcall mysql_FetchField(int i)
{
    if (i >= mysql_num_fields(result))
        return NULL;
    else
        return row[i];
}
