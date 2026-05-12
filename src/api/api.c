#include <curl/curl.h>
#include <json-c/json.h>
#include <openssl/sha.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"

struct memory
{
    char *response;
    size_t size;
};

size_t write_to_string(char *input, size_t size, size_t nmemb, void* output)
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

typedef struct
{
    CURL *curl;
    const char *url;
} curl_get_args;

typedef struct
{
    const char *text;
    int status;
} curl_get_result;

void *curl_get(void *ptr)
{
    curl_get_args *args = ptr;

    CURL *curl = args->curl;
    const char *url = args->url;

    puts(url);

    struct memory mem;
    mem.response = NULL;
    mem.size = 0;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&mem);

    curl_get_result *result = malloc(sizeof(curl_get_result));

    result->status = curl_easy_perform(curl);
    curl_easy_reset(curl);

    result->text = mem.response;
    puts(result->text ? result->text : "(null)");

    pthread_exit(result);
}

typedef struct
{
    CURL *curl;
    const char *address;
    const char *login;
    const char *pass;
} auth_args;

void *auth(void *ptr)
{
    auth_args *args = ptr;

    const char *link = create_link("https://", args->address, "/resto/api/auth");


    const char *login_arg = create_arg("login", args->login);
    const char *pass_arg = create_arg("pass", calculate_sha1(args->pass));

    const char *arguments[] = {login_arg,pass_arg};
    const char *url = parse_args(link, arguments, 2);

    curl_get_args *cga = malloc(sizeof(curl_get_args));

    cga->curl = args->curl;
    cga->url = url;

    curl_get_result *result = NULL;
    pthread_t curl_thread;
    pthread_create(&curl_thread, NULL, curl_get, cga);
    pthread_join(curl_thread, (void **) &result);

    pthread_exit(result);
}

typedef struct
{
    CURL *curl;
    const char *address;
    const char *token;
} logout_args;

void *logout(void *ptr)
{
    logout_args *args = ptr;

    const char *arg = create_arg("key", args->token);
    const char *arguments[] = {arg};

    const char *link = create_link("https://", args->address, "/resto/api/logout");
    const char *url = parse_args(link, arguments, 1);

    curl_get_args *cga = malloc(sizeof(curl_get_args));
    cga->curl = args->curl;
    cga->url = url;

    curl_get_result *result = NULL;
    pthread_t curl_thread;
    pthread_create(&curl_thread, NULL, curl_get, cga);
    pthread_join(curl_thread, (void**) &result);

    pthread_exit(result);
}

struct cashshifts_list
{
    char *openDateFrom;
    char *openDateTo;
    char *departmentId;
    char *groupId;
    char *status;
    int *revision_from;
};

struct cashshifts_list_answer_element
{
    int id;
    int sessionNumber;
    int fiscalNumber;
    int cashRegNumber;
    const char *cashRegSerial;
    const char *openDate;
    const char *closeDate;
    const char *acceptDate;
    const char *managerId;
    const char *responsibleUserId;
    int sessionStartCash;
    int payOrders;
    int sumWriteoffOrders;
    int salesCash;
    int salesCredit;
    int salesCard;
    int payIn;
    int payOut;
    int payIncome;
    int cashRemain;
    int cashDiff;
    const char *sessionStatus;
    const char *conceptionId;
    const char *pointOfSaleId;
};

struct cashshifts_list_answer
{
    struct cashshifts_list_answer_element* elements;
    size_t size;
};

typedef struct
{
    CURL *curl;
    char *token;
    char *address;
    struct cashshifts_list prompt;
} cashshifts_list_args;

void *cashshifts_list_get(void *ptr)
{
    cashshifts_list_args *args = ptr;

    const char *arguments[7] =
    {
        create_arg("key", args->token),
        create_arg("openDateFrom", args->prompt.openDateFrom),
        create_arg("openDateTo", args->prompt.openDateTo),
        create_arg("status", args->prompt.status),
    };

    size_t args_pos = 4;

    if (args->prompt.departmentId)
    {
        arguments[args_pos] = create_arg("departmentId", args->prompt.departmentId);
        args_pos += 1;
    }

    if (args->prompt.groupId)
    {
        arguments[args_pos] = create_arg("groupId", args->prompt.groupId);
        args_pos += 1;
    }

    if (args->prompt.revision_from)
    {
        char string[32];
        snprintf(string, sizeof(string), "%d", *args->prompt.revision_from);
        args_pos += 1;
    }

    const char *link = create_link("https://", args->address, "/resto/api/v2/cashshifts/list");
    const char *url = parse_args(link, arguments, args_pos);

    curl_get_args *cga = malloc(sizeof(curl_get_args));
    cga->curl = args->curl;
    cga->url = url;

    curl_get_result *result = NULL;
    pthread_t curl_thread;
    pthread_create(&curl_thread, NULL, curl_get, cga);
    pthread_join(curl_thread, (void**) &result);

    json_object* obj = json_tokener_parse(result->text);

    struct cashshifts_list_answer *answer = malloc(sizeof(struct cashshifts_list_answer));

    size_t answer_size = json_object_array_length(obj);

    answer->elements = malloc(sizeof(*answer->elements) * answer_size);
    answer->size = answer_size;

    json_object *array_member;
    json_object
        *id,
        *sessionNumber,
        *fiscalNumber,
        *cashRegNumber,
        *cashRegSerial,
        *openDate,
        *closeDate,
        *acceptDate,
        *managerId,
        *responsibleUserId,
        *sessionStartCash,
        *payOrders,
        *sumWriteoffOrders,
        *salesCash,
        *salesCredit,
        *salesCard,
        *payIn,
        *payOut,
        *payIncome,
        *cashRemain,
        *cashDiff,
        *sessionStatus,
        *conceptionId,
        *pointOfSaleId;

    for (size_t index = 0; index < answer_size; index++)
    {
        struct cashshifts_list_answer_element elem;
        array_member = json_object_array_get_idx(obj, index);

        id = json_object_object_get(array_member, "id");
        sessionNumber = json_object_object_get(array_member, "sessionNumber");
        fiscalNumber = json_object_object_get(array_member, "fiscalNumber");
        cashRegNumber = json_object_object_get(array_member, "cashRegNumber");
        cashRegSerial = json_object_object_get(array_member, "cashRegSerial");
        openDate = json_object_object_get(array_member, "openDate");
        closeDate = json_object_object_get(array_member, "closeDate");
        acceptDate = json_object_object_get(array_member, "acceptDate");
        managerId = json_object_object_get(array_member, "managerId");
        responsibleUserId = json_object_object_get(array_member, "responsibleUserId");
        sessionStartCash = json_object_object_get(array_member, "sessionStartCash");
        payOrders = json_object_object_get(array_member, "payOrders");
        sumWriteoffOrders = json_object_object_get(array_member, "sumWriteoffOrders");
        salesCash = json_object_object_get(array_member, "salesCash");
        salesCredit = json_object_object_get(array_member, "salesCredit");
        salesCard = json_object_object_get(array_member, "salesCard");
        payIn = json_object_object_get(array_member, "payIn");
        payOut = json_object_object_get(array_member, "payOut");
        payIncome = json_object_object_get(array_member, "payIncome");
        cashRemain = json_object_object_get(array_member, "cashRemain");
        cashDiff = json_object_object_get(array_member, "cashDiff");
        sessionStatus = json_object_object_get(array_member, "sessionStatus");
        conceptionId = json_object_object_get(array_member, "conceptionId");
        pointOfSaleId = json_object_object_get(array_member, "pointOfSaleId");

        elem.id = json_object_get_int(id);
        elem.sessionNumber = json_object_get_int(sessionNumber);
        elem.fiscalNumber = json_object_get_int(fiscalNumber);
        elem.cashRegNumber = json_object_get_int(cashRegNumber);
        elem.cashRegSerial = json_object_get_string(cashRegSerial);
        elem.openDate = json_object_get_string(openDate);
        elem.closeDate = json_object_get_string(closeDate);
        elem.acceptDate = json_object_get_string(acceptDate);
        elem.managerId = json_object_get_string(managerId);
        elem.responsibleUserId = json_object_get_string(responsibleUserId);
        elem.sessionStartCash = json_object_get_int(sessionStartCash);
        elem.payOrders = json_object_get_int(payOrders);
        elem.sumWriteoffOrders = json_object_get_int(sumWriteoffOrders);
        elem.salesCash = json_object_get_int(salesCash);
        elem.salesCredit = json_object_get_int(salesCredit);
        elem.salesCard = json_object_get_int(salesCard);
        elem.payIn = json_object_get_int(payIn);
        elem.payOut = json_object_get_int(payOut);
        elem.payIncome = json_object_get_int(payIncome);
        elem.cashRemain = json_object_get_int(cashRemain);
        elem.cashDiff = json_object_get_int(cashDiff);
        elem.sessionStatus = json_object_get_string(sessionStatus);
        elem.conceptionId = json_object_get_string(conceptionId);
        elem.pointOfSaleId = json_object_get_string(pointOfSaleId);

        answer->elements[index] = elem;
    }

    pthread_exit(answer);
}
