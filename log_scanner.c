#include "common.h"
#include "filesystem.h"
#include "config.h"
#include "pushover.h"

char * get_arg(int argc, char **argv, char *key)
{
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], key) == 0 && argc - 1 >= i + 1) {
            return argv[i + 1];
        }
    }

    printf("Usage: %s --config-file [/path/to/config.yml]\n", argv[0]);
    exit(-1);
}

void error_line(char *line)
{
    printf("New error found!\n");

    /*if (pushover_send(config_get("pushover_token"), config_get("pushover_user"), line)) {
        printf("Notification sent\n");
    } else {
        printf("Error sending notification\n");
    }*/
}

int main(int argc, char **argv)
{
    config_init(get_arg(argc, argv, "--config-file"));

    //pushover_send(config_get("pushover_token"), config_get("pushover_user"), "LogScanner startup :D");

    filesystem_set_ln_callback(&error_line);
    filesystem_watch_file(config_get("file"));

    return 0;
}