#include "common.h"
#include "filesystem.h"

#define REGEX_GROUPS    (2)
#define EVENT_SIZE      (sizeof (struct inotify_event))
#define EVENT_BUF_LEN   (1024 * (EVENT_SIZE + 16))

#define TAG_DATE        0
#define TAG_TYPE        1
#define TAG_PID         2

static ln_callback _callback = NULL;
static long int f_offset = 0;

void filesystem_set_ln_callback(ln_callback callback)
{
    _callback = callback;
}

char * extract_error_message(char *line)
{
    char *pch = strtok(line, "]"), *last, *end;

    while (pch != NULL) {
        last = pch;
        pch = strtok(NULL, "]");
    }

    while(isspace(*last)) last++;

    if(*last == 0)  // All spaces?
        return last;

    // Trim trailing space
    end = last + strlen(last) - 1;
    while (end > last && isspace(*end)) end--;

    // Write new null terminator
    *(end + 1) = 0;

    return last;
}

static char * extract_comparable_date(char *date)
{
    return strtok(date, ".");
}

static bool process_line(char *line)
{
    static char *last_date;

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
            case TAG_DATE:
                if (last_date != NULL && strcmp(extract_comparable_date(capture), extract_comparable_date(last_date)) == 0) {
                    return false; // Always skip since we're probably just in a massive stack trace
                }
                last_date = (char *) malloc(strlen(capture) + 1);
                strcpy(last_date, capture); // TODO: Get rid of microseconds
            break;
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
        printf("File %s does not exist\n", filename);
        exit(-1);
    }

    FILE *fh = fopen(filename, "rt");
    fseek(fh, f_offset, SEEK_SET);

    char line[BUFSIZ];

    while (fgets(line, BUFSIZ, fh) != NULL) {
        f_offset = ftell(fh);
        if (process_line(line)) {
            if (_callback != NULL) {
                _callback(line);
            }
        }
    }

    fclose (fh);
}

static void init_f_offset(char *filename)
{
    FILE *fh = fopen(filename, "rt");
    fseek(fh, 0, SEEK_END);
    f_offset = ftell(fh);
    fclose(fh);
}

void filesystem_watch_file(char *filename)
{
    if (!file_exists(filename)) {
        printf("File %s does not exist\n", filename);
        exit(-1);
    }

    // Set offset to the end of the file so we'll start checking errors for new entries only
    init_f_offset(filename);

    int length;
    int fd, wd;
    char buffer[EVENT_BUF_LEN];

    fd = inotify_init();

    if (fd < 0) {
        printf("Inotify returned %d", fd);
        exit(-1);
    }

    wd = inotify_add_watch(fd, filename, IN_MODIFY);

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
            if (event->mask & IN_MODIFY) {
                process_file(filename);
            }

            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(fd, wd);

    close(fd);
}