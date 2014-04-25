#include <pcre.h>
#include <linux/inotify.h>

typedef void (*ln_callback)(char *);

char * extract_error_message(char *line);
void filesystem_set_ln_callback(ln_callback callback);
void filesystem_watch_file(char *filename);