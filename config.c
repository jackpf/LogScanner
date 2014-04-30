#include "common.h"
#include "config.h"

static GHashTable *config;

void config_init(char *config_file)
{
    if (!file_exists(config_file)) {
        printf("Config file %s does not exist\n", config_file);
        exit(-1);
    }

    config = g_hash_table_new(g_str_hash, g_str_equal);
    yaml_parser_t parser;
    yaml_event_t event;

    /* Create the Parser object. */
    yaml_parser_initialize(&parser);

    /* Set a file input. */
    FILE *input = fopen(config_file, "rb");

    yaml_parser_set_input_file(&parser, input);

    char *key = NULL, *value = NULL, **type_ptr;

    do {
        if (!yaml_parser_parse(&parser, &event) || event.type == YAML_SEQUENCE_START_EVENT /* currently unsupported, TODO */) {
            printf("Config parse error: %d\n", parser.error);
            goto end;
        }

        if (event.type == YAML_SCALAR_EVENT) {
            type_ptr = key == NULL ? &key : &value;

            *type_ptr = (char *) malloc(strlen((const char *) event.data.scalar.value) + 1);
            strcpy(*type_ptr, (const char *) event.data.scalar.value);

            if (value != NULL) {
                g_hash_table_insert(config, key, value);
                key = value = NULL;
            }
        }
    } while(event.type != YAML_STREAM_END_EVENT);

    end:
        yaml_event_delete(&event);
}

char *config_get(char *key)
{
    if (!g_hash_table_contains(config, key)) {
        printf("Config value for %s not set\n", key);
        exit(-1);
    }

    return g_hash_table_lookup(config, key);
}