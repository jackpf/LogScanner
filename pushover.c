#include "common.h"
#include "pushover.h"

static void init_string(struct string *s)
{
    s->len = 0;
    s->ptr = malloc(s->len + 1);
    s->ptr[0] = '\0';
}

static size_t curl_write(void *ptr, size_t size, size_t nmemb, struct string *s)
{
    size_t new_len = s->len + size * nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);

    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}

bool pushover_send(char *token, char *user, char *message)
{
    CURL *ch;
    CURLcode res;
 
    char *request_data = (char *) malloc(strlen(PUSHOVER_REQ) + strlen(token) + strlen(user) + strlen(message) + 1);
    sprintf(request_data, PUSHOVER_REQ, token, user, message);
    struct string return_data;
    init_string(&return_data);

    curl_global_init(CURL_GLOBAL_ALL);

    ch = curl_easy_init();

    curl_easy_setopt(ch, CURLOPT_URL, PUSHOVER_URL);
    curl_easy_setopt(ch, CURLOPT_POSTFIELDS, request_data);
    curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, curl_write);
    curl_easy_setopt(ch, CURLOPT_WRITEDATA, &return_data);

    res = curl_easy_perform(ch);

    if (res != CURLE_OK) {
        printf("Curl error: %s\n", curl_easy_strerror(res));
        return false;
    }

    free(return_data.ptr);

    curl_easy_cleanup(ch);

    return true;
}