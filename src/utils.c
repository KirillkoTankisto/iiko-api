#include <openssl/sha.h>

#include <stdlib.h>
#include <string.h>

char* join_strings(char* strings[], size_t length)
{
    size_t final_length = 0;
    for (size_t i = 0; i < length; i++)
    {
        final_length += strlen(strings[i]);
    }

    char* final_string = malloc(final_length + 1);
    char* ptr = final_string;

    for (size_t i = 0; i < length; i++)
    {
        size_t str_len = strlen(strings[i]);
        memcpy(ptr, strings[i], str_len);
        ptr += str_len;
    }
    *ptr = 0;

    return final_string;
}

static const char* hex =  "0123456789abcdef";
char* calculate_sha1(char* string)
{
    char* hash = malloc(SHA_DIGEST_LENGTH);
    SHA1((const unsigned char*) string, strlen(string), (unsigned char*) hash);
    char* text = malloc(SHA_DIGEST_LENGTH * 2 + 1);
    char* ptr = text;
    for (unsigned int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        unsigned char c = hash[i];
        *ptr++ = hex[c>>4];
        *ptr++ = hex[c&0xf];
    }

    *ptr = 0;

    return text;;
}

char* parse_args(const char* base, const char* args[], size_t args_len)
{
    size_t base_len = strlen(base);
    char* string = malloc(512); // TODO: dynamic allocation

    memcpy(string, base, base_len);
    string[base_len] = '?';
    char* ptr = string + base_len + 1;

    for (int i = 0; i < args_len; i++)
    {
        if (i > 0)  // don't put '&' at the start
        {
            *ptr = '&';
            ptr += 1;
        }
        size_t arg_len = strlen(args[i]);
        memcpy(ptr, args[i], arg_len);
        ptr += arg_len;
    }
    *ptr = 0; // NULL

    return string;
}

char* create_link(char* first, char* middle, char* last)
{
    char* strings[3] = {first, middle, last};
    return join_strings(strings, 3);
}

char* create_arg(char* key, char* value)
{
    char* strings[3] = {key, "=", value};
    return join_strings(strings, 3);
}
