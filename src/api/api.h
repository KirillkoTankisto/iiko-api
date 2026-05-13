#include <curl/curl.h>

typedef struct
{
    const char *text;
    int status;
} curl_get_result;

///

typedef struct
{
    CURL *curl;
    const char *address;
    const char *login;
    const char *pass;
} auth_args;

void *auth(void *ptr);

///

typedef struct
{
    CURL *curl;
    const char *address;
    const char *token;
} logout_args;

void *logout(void *ptr);

///

struct cashshifts_list
{
    const char *openDateFrom;
    const char *openDateTo;
    const char *departmentId;
    const char *groupId;
    const char *status;
    const int *revision_from;
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
    const char *token;
    const char *address;
    struct cashshifts_list prompt;
} cashshifts_list_args;

void *cashshifts_list_get(void *ptr);
