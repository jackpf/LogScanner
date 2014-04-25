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

    pushover_data data = new_pushover_data;
    data.title = config_get("server_name");
    data.message = extract_error_message(line);

    if (pushover_send(config_get("pushover_token"), config_get("pushover_user"), data)) {
        printf("Notification sent\n");
    } else {
        printf("Error sending notification\n");
    }
}

int main(int argc, char **argv)
{
    config_init(get_arg(argc, argv, "--config-file"));

    filesystem_set_ln_callback(&error_line);

    for (int i = 1; i <= 10 /* max retry attempts */ && !filesystem_watch_file(config_get("file")); i++) {
        printf("Retry attempt %d\n", i);
        sleep(5);
    }

    pushover_data data = new_pushover_data;
    data.title = "LogScanner Error";
    data.message = "An error has occured while attempting to watch files";

    pushover_send(config_get("pushover_token"), config_get("pushover_user"), data);
    printf("%s\n", data.message);

    return 0;
}