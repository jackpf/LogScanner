#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pcre.h>

void process_line(char *line)
{
    printf("%s", line);

    const char *pattern = "\\[(.*?)\\]";
    const char *error;
    int error_offset;
    int ovector[300];

    pcre *re = pcre_compile(
        pattern,
        0,
        &error,
        &error_offset,
        NULL
    );

    if (re == NULL)
    {
        printf("PCRE compilation failed at offset %d: %s\n", error_offset, error);
        exit(-1);
    }

    int rc = pcre_exec(
        re,
        NULL,
        line,
        strlen(line),
        0,
        0,
        ovector,
        300
    );

    if (rc < 0)
    {
        // No match
        return;
    }
     
    if (rc == 0)
    {
        rc = 300/3;
        printf("ovector only has room for %d captured substrings\n", rc - 1);
    }
     
     
    for (int i = 0; i < rc; i++)
    {
        char *substring_start = line + ovector[2*i];
        int substring_length = ovector[2*i+1] - ovector[2*i];
        printf("%2d: %.*s\n", i, substring_length, substring_start);
    }
}

int main(int argc, char **argv)
{
    FILE *fh = fopen("/var/log/apache2/error.log", "rt");

    if (fh == NULL)
    {
        printf("File does not exist\n");
        exit(-1);
    }

    char line[BUFSIZ];

    while (fgets(line, sizeof line, fh) != NULL)
    {
        process_line(line);
    }

    return 0;
}