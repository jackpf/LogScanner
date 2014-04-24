#include <pcre.h>
#include <linux/inotify.h>

void process_line(char *line);
void process_file(char *filename);
void watch_file(char *filename, void (*callback)(char *));