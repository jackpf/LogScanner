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

int main(int argc, char **argv)
{
    config_init(get_arg(argc, argv, "--config-file"));

    //pushover_send(config_get("pushover_token"), config_get("pushover_user"), "LogScanner startup :D");

    watch_file(config_get("file"), &process_file);

    return 0;
}