#include "curl_get.h"

typedef struct
{
    CURL *curl;
    const char *address;
    const char *token;
} logout_args;

curl_get_result *logout(logout_args args);
