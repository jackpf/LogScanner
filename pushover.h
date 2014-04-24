#pragma once
#include <curl/curl.h>

#define PUSHOVER_URL "https://api.pushover.net/1/messages.json"
#define PUSHOVER_REQ "token=%s&user=%s&message=%s"

struct string
{
    char *ptr;
    size_t len;
};

bool pushover_send(char *token, char *user, char *message);