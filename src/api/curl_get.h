#if !defined(CURL_GET_H)
#define CURL_GET_H
#include <curl/curl.h>

#define HTTPS "https://"

typedef const char *cstring;

typedef struct
{
    cstring url;
} curl_get_args;

typedef struct
{
    const char *text;
    int status;
} curl_get_result;

curl_get_result *curl_get(curl_get_args args);

#endif
