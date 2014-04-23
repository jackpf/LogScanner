#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pcre.h>
#include <linux/inotify.h>

#define REGEX_GROUPS    (2)
#define EVENT_SIZE      (sizeof (struct inotify_event))
#define EVENT_BUF_LEN   (1024 * (EVENT_SIZE + 16))

bool file_exists(char *filename)
{
    FILE *fh = fopen(filename, "r");
    bool exists = fh != NULL;
    fclose(fh);

    return exists;
}

void process_line(char *line)
{
    const char *pattern = "\\[(.*?)\\]";
    const char *error;
    int error_offset;
    int ovector[REGEX_GROUPS * 3];
    pcre *re;
    int rc;

    re = pcre_compile(
        pattern,
        0,
        &error,
        &error_offset,
        NULL
    );

    if (re == NULL) {
        printf("PCRE compilation failed at offset %d: %s\n", error_offset, error);
        exit(-1);
    }


    do {
        rc = pcre_exec(
            re,
            NULL,
            line,
            strlen(line),
            ovector[1] < 0 ? 0 : ovector[1],
            0,
            ovector,
            REGEX_GROUPS * 3
        );

        printf("%s", line);
        for (int i = 0; i < rc; i++) {
            char *substring_start = line + ovector[2*i];
            int substring_length = ovector[2*i+1] - ovector[2*i];
            printf("%2d: %.*s\n", i, substring_length, substring_start);
        }
        printf("\n");
    } while (rc >= 0);
}

void process_file(char *filename)
{
    if (!file_exists(filename)) {
        printf("File does not exist\n");
        exit(-1);
    }

    FILE *fh = fopen(filename, "rt");

    char line[BUFSIZ];

    while (fgets(line, sizeof line, fh) != NULL) {
        process_line(line);
    }

    fclose (fh);
}

void inotify(char *filename, void (*callback)(char *))
{
    if (!file_exists(filename)) {
        printf("File does not exist\n");
        exit(-1);
    }

    int length;
    int fd, wd;
    char buffer[EVENT_BUF_LEN];

    fd = inotify_init();

    if (fd < 0) {
        printf("Inotify returned %d", fd);
        exit(-1);
    }

    wd = inotify_add_watch(fd, filename, IN_CLOSE_WRITE);

    printf("Watching %s...\n", filename);

    while (true) {
        // TODO: Use select()
        length = read(fd, buffer, EVENT_BUF_LEN); 

        if (length < 0) {
            printf("Read error\n");
            exit(-1);
        }

        for (int i = 0; i < length;) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->mask & IN_CLOSE_WRITE) {
                printf("File modified!\n");

                (*callback)(filename);
            }

            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(fd, wd);

    close(fd);
}

int main(int argc, char **argv)
{
    char *filename = "/home/s40admin/installs/error.log";

    inotify(filename, &process_file);

    return 0;
}