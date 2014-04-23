#include "config.h"

void parse(void)
{
    yaml_parser_t parser;
    yaml_event_t event;

    int done = 0;

    /* Create the Parser object. */
    yaml_parser_initialize(&parser);

    /* Set a file input. */
    FILE *input = fopen("/home/s40admin/installs/config.yml", "rb");

    yaml_parser_set_input_file(&parser, input);

    /* Read the event sequence. */
    while (!done) {

        /* Get the next event. */
        if (!yaml_parser_parse(&parser, &event))
            goto end;

        /*
          ...
          Process the event.
          ...
        */

        /* Are we finished? */
        done = (event.type == YAML_STREAM_END_EVENT);

        /* The application is responsible for destroying the event object. */
        yaml_event_delete(&event);

    }

    end:
        yaml_parser_delete(&parser);
}