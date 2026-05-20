#include "curl_get.h"

typedef struct
{
    const char *openDateFrom;
    const char *openDateTo;
    const char *departmentId;
    const char *groupId;
    const char *status;
    const int revision_from;
} cashshifts_list;

typedef struct
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
} cashshifts_list_answer_element;

typedef struct
{
    cashshifts_list_answer_element* elements;
    size_t size;
} cashshifts_list_answer;

typedef struct
{
    const char *token;
    const char *address;
    cashshifts_list prompt;
} cashshifts_list_args;

cashshifts_list_answer *cashshifts_list_get(cashshifts_list_args args);

void cashshifts_list_destroy(cashshifts_list_answer *list);
