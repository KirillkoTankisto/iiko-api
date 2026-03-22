#include <curl/curl.h>
#include <json-c/json.h>
#include <openssl/sha.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/utils.h"

struct memory
{
    char* response;
    size_t size;
};

size_t write_to_string(char* input, size_t size, size_t nmemb, void* output)
{
    size_t realsize = nmemb;
    struct memory* mem = (struct memory*) output;

    char* ptr = realloc(mem->response, mem->size + realsize + 1);
    if (!ptr) return 0;

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), input, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

char* curl_get(CURL* curl, char* url)
{
    puts(url);

    struct memory mem;
    mem.response = NULL;
    mem.size = 0;

    if (!curl) goto exit;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&mem);

    curl_easy_perform(curl);

    curl_easy_reset(curl);

    puts(mem.response);

    exit:
    return mem.response;
}

struct token
{
    char* string;
};

struct token auth(CURL* curl, const char* address, const char* login, const char* pass)
{
    char* link = create_link("https://", (char*) address, "/resto/api/auth");


    char* login_arg = create_arg("login", (char*) login);
    char* pass_arg = create_arg("pass", calculate_sha1((char*) pass));

    const char* args[] = {login_arg,pass_arg};
    char* url = parse_args(link, args, 2);

    struct token iiko_token;

    char* string = curl_get(curl, url);

    iiko_token.string = string;

    return iiko_token;
};

void logout(CURL* curl, const char* address, struct token iiko_token)
{
    char* arg = create_arg("key", iiko_token.string);
    const char* args[] = {arg};

    char* link = create_link("https://", (char*) address, "/resto/api/logout");
    char* url = parse_args(link, args, 1);

    puts(curl_get(curl, url));

    return;
}

struct cashshifts_list
{
    char* openDateFrom;
    char* openDateTo;
    char* departmentId;
    char* groupId;
    char* status;
    int* revision_from;
};

struct cashshifts_list_answer_element
{
    int id;
    int sessionNumber;
    int fiscalNumber;
    int cashRegNumber;
    char* cashRegSerial;
    char* openDate;
    char* closeDate;
    char* acceptDate;
    char* managerId;
    char* responsibleUserId;
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
    char* sessionStatus;
    char* conceptionId;
    char* pointOfSaleId;
};

struct cashshifts_list_answer
{
    struct cashshifts_list_answer_element* elements;
    size_t size;
};

struct cashshifts_list_answer cashshifts_list_get(CURL *curl, struct token iiko_token, char* address, struct cashshifts_list prompt)
{
    const char* args[6] =
    {
        create_arg("key", iiko_token.string),
        create_arg("openDateFrom", prompt.openDateFrom),
        create_arg("openDateTo", prompt.openDateTo),
        create_arg("status", prompt.status),
    };

    size_t args_len = 4;

    if (prompt.departmentId)
    {
        args_len += 1;
        args[args_len] = create_arg("departmentId", prompt.departmentId);
    }

    if (prompt.groupId)
    {
        args_len += 1;
        args[args_len] = create_arg("groupId", prompt.groupId);
    }

    if (prompt.revision_from)
    {
        args_len += 1;
        char string[32];
        snprintf(string, sizeof(string), "%d", *prompt.revision_from);
    }

    char* link = create_link("https://", address, "/resto/api/v2/cashshifts/list");
    char* url = parse_args(link, args, args_len);

    char* response = curl_get(curl, url);

    puts(response);

    json_object* obj = json_tokener_parse(response);

    struct cashshifts_list_answer answer;

    size_t answer_size = json_object_array_length(obj);

    answer.elements = malloc(sizeof(*answer.elements) * answer_size);
    answer.size = answer_size;

    json_object *array_member;
    json_object *id , *sessionNumber, *fiscalNumber, *cashRegNumber, *cashRegSerial, *openDate, *closeDate, *acceptDate, *managerId, *responsibleUserId, *sessionStartCash, *payOrders, *sumWriteoffOrders, *salesCash, *salesCredit, *salesCard, *payIn, *payOut, *payIncome, *cashRemain, *cashDiff, *sessionStatus, *conceptionId, *pointOfSaleId;
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

        int id_t = json_object_get_int(id);
        int sessionNumber_t = json_object_get_int(sessionNumber);
        int fiscalNumber_t = json_object_get_int(fiscalNumber);
        int cashRegNumber_t = json_object_get_int(cashRegNumber);
        char* cashRegSerial_t = (char*) json_object_get_string(cashRegSerial);
        char* openDate_t = (char*) json_object_get_string(openDate);
        char* closeDate_t = (char*) json_object_get_string(closeDate);
        char* acceptDate_t = (char*) json_object_get_string(acceptDate);
        char* managerId_t = (char*) json_object_get_string(managerId);
        char* responsibleUserId_t = (char*) json_object_get_string(responsibleUserId);
        int sessionStartCash_t = json_object_get_int(sessionStartCash);
        int payOrders_t = json_object_get_int(payOrders);
        int sumWriteoffOrders_t = json_object_get_int(sumWriteoffOrders);
        int salesCash_t = json_object_get_int(salesCash);
        int salesCredit_t = json_object_get_int(salesCredit);
        int salesCard_t = json_object_get_int(salesCard);
        int payIn_t = json_object_get_int(payIn);
        int payOut_t = json_object_get_int(payOut);
        int payIncome_t = json_object_get_int(payIncome);
        int cashRemain_t = json_object_get_int(cashRemain);
        int cashDiff_t = json_object_get_int(cashDiff);
        char* sessionStatus_t = (char*) json_object_get_string(sessionStatus);
        char* conceptionId_t = (char*) json_object_get_string(conceptionId);
        char* pointOfSaleId_t = (char*) json_object_get_string(pointOfSaleId);

        elem.id = id_t;
        elem.sessionNumber = sessionNumber_t;
        elem.fiscalNumber = fiscalNumber_t;
        elem.cashRegNumber = cashRegNumber_t;
        elem.cashRegSerial = cashRegSerial_t;
        elem.openDate = openDate_t;
        elem.closeDate = closeDate_t;
        elem.acceptDate = acceptDate_t;
        elem.managerId = managerId_t;
        elem.responsibleUserId = responsibleUserId_t;
        elem.sessionStartCash = sessionStartCash_t;
        elem.payOrders = payOrders_t;
        elem.sumWriteoffOrders = sumWriteoffOrders_t;
        elem.salesCash = salesCash_t;
        elem.salesCredit = salesCredit_t;
        elem.salesCard = salesCard_t;
        elem.payIn = payIn_t;
        elem.payOut = payOut_t;
        elem.payIncome = payIncome_t;
        elem.cashRemain = cashRemain_t;
        elem.cashDiff = cashDiff_t;
        elem.sessionStatus = sessionStatus_t;
        elem.conceptionId = conceptionId_t;
        elem.pointOfSaleId = pointOfSaleId_t;

        answer.elements[index] = elem;
    }

    return answer;
}

json_object *olap_columns_get(CURL *curl, struct token iiko_token, char* address)
{
    char* link = create_link("https://", address, "/resto/api/v2/reports/olap/columns");
    char* arg = create_arg("key", iiko_token.string);
    const char* args[] = {arg};

    char* url = parse_args(link, args, 1);
    char* response = curl_get(curl, url);

    json_object *parsed = json_tokener_parse(response);
    return parsed;
}
