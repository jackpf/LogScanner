#pragma once
#include <curl/curl.h>

#define PUSHOVER_URL "https://api.pushover.net/1/messages.json"
#define PUSHOVER_REQ "token=%s&user=%s&message=%s"

typedef struct write_string
{
    char *ptr;
    size_t len;
} write_string;

typedef struct pushover_data
{
    char *title;
    char *message;
} pushover_data;

bool pushover_send(char *token, char *user, pushover_data data);