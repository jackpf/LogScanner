#include "common.h"
#include "filesystem.h"

#define REGEX_GROUPS    (2)
#define EVENT_SIZE      (sizeof (struct inotify_event))
#define EVENT_BUF_LEN   (1024 * (EVENT_SIZE + 16))

#define TAG_DATE        0
#define TAG_TYPE        1
#define TAG_PID         2

static ln_callback _callback = NULL;

void filesystem_set_ln_callback(ln_callback callback)
{
    _callback = callback;
}

static bool process_line(char *line)
{
    const char *pattern = "\\[(.*?)\\]";
    const char *error;
    int error_offset;
    pcre *re;
    int rc;
    int ovector[REGEX_GROUPS * 3];
    bool snd = false;

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

    for (int tag = 0; true; tag++) {
        rc = pcre_exec(
            re,
            NULL,
            line,
            strlen(line),
            tag == 0 ? 0 : ovector[1],
            0,
            ovector,
            REGEX_GROUPS * 3
        );

        if (rc < 0) {
            break;
        }

        int group = 1; // Group to capture
        char *substring_start = line + ovector[2 * group];
        int substring_length = ovector[2 * group + 1] - ovector[2 * group];

        char *capture = (char *) malloc(substring_length + 1);
        sprintf(capture, "%.*s", substring_length, substring_start);

        switch (tag) {
            case TAG_TYPE:
                if (strcmp(capture, ":error") == 0) {
                    snd = true;
                }
            break;
        }
    }

    return snd;
}

static void process_file(char *filename)
{
    if (!file_exists(filename)) {
        printf("File does not exist\n");
        exit(-1);
    }

    FILE *fh = fopen(filename, "rt");

    char line[BUFSIZ];

    while (fgets(line, BUFSIZ, fh) != NULL) {
        if (process_line(line)) {
            if (_callback != NULL) {
                _callback(line);
            }
        }
    }

    fclose (fh);
}

void filesystem_watch_file(char *filename)
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
                process_file(filename);
            }

            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(fd, wd);

    close(fd);
}