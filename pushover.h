#pragma once
#include <curl/curl.h>

#define PUSHOVER_URL "https://api.pushover.net/1/messages.json"
#define PUSHOVER_REQ "token=%s&user=%s&message=%s"

typedef int pushover_h;

struct string {
    char *ptr;
    size_t len;
};

pushover_h pushover_send(char *token, char *user, char *message);