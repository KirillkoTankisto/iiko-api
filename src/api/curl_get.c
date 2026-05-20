#include "curl_get.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

struct memory
{
    char *response;
    size_t size;
};

static size_t write_to_string(char *input, size_t size, size_t nmemb, void* output)
{
    size_t realsize = size * nmemb;
    struct memory *mem = (struct memory*) output;

    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if (!ptr) return 0;

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), input, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

static CURL *curl_init(void)
{
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_USERAGENT, UAGENT);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
    return curl;
}

static void curl_cleanup(CURL *curl)
{
    curl_easy_cleanup(curl);
    return;
}

curl_get_result *curl_get(curl_get_args args)
{
    puts(args.url);

    struct memory mem;
    mem.response = NULL;
    mem.size = 0;

    CURL *curl = curl_init();
    curl_easy_setopt(curl, CURLOPT_URL, args.url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &mem);

    curl_get_result *result = malloc(sizeof(curl_get_result));
    result->status = curl_easy_perform(curl);
    curl_cleanup(curl);

    result->text = mem.response;
    puts(result->text ? result->text : "(null)");

    return result;
}
