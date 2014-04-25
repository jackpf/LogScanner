#include <pcre.h>
#include <linux/inotify.h>

#define MAX_RETRY_ATTEMPTS 10

typedef void (*ln_callback)(char *);

char * extract_error_message(char *);
void filesystem_set_ln_callback(ln_callback);
bool filesystem_watch_file(char *);
void filesystem_watch_file_retry(char *);