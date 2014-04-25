#pragma once
#include <curl/curl.h>

#define PUSHOVER_URL "https://api.pushover.net/1/messages.json"
#define PUSHOVER_REQ "token=%s&user=%s&title=%s&message=%s&device=%s&url=%s&url_title=%s&priority=%d&sound=%s"

typedef struct write_string
{
    char *ptr;
    size_t len;
} write_string;

struct pushover_data_s
{
    char *title;
    char *message;
    char *device;
    char *url;
    char *url_title;
    int priority;
    char *sound;
};
typedef struct pushover_data_s pushover_data;
extern const struct pushover_data_s new_pushover_data;

bool pushover_send(char *token, char *user, pushover_data data);