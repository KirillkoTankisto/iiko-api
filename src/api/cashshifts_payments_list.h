#include "curl_get.h"

typedef struct
{
    char *address;
    char *token;
    size_t session_number;
} cashshifts_payments_list_args;

typedef enum
{
    CARD,
    CREDIT,
    PAYOUT,
    PAYIN,
} group_enum;

typedef struct
{
    cstring id;
    cstring date;
    cstring accountId;
    cstring counteragentId;
    cstring paymentTypeId;
    cstring type;
    size_t sum;
    cstring comment;
    cstring causeEventId;
    cstring cashierId;
    cstring departmentId;
    cstring user;
    cstring card;
} info_t;

typedef struct
{
    info_t *info;
    size_t actualSum;
    size_t originalSum;
    cstring editedPayAccountId;
    cstring originalPayAccountId;
    cstring payAgentId;
    cstring editableComment;

    group_enum group;
} any_record;

typedef struct
{
    cstring sessionId;
    any_record *cashlessRecords;
    any_record *payInRecords;
    any_record *payOutRecords;
} cashshifts_payments_list_answer;
