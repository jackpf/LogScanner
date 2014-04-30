#include <pcre.h>
#include <linux/inotify.h>

typedef void (*ln_callback)(char *);

char *extract_error_message(char *);
void filesystem_set_ln_callback(ln_callback);
bool filesystem_watch_file(char *);
