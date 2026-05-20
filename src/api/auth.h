#include "curl_get.h"

typedef struct
{
    char *address;
    char *login;
    char *pass;
} auth_args;

curl_get_result *auth(auth_args args);
