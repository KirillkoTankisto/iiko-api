#include "cashshifts_payments_list.h"
#include "../utils/utils.h"
#include <json-c/json.h>
#include <string.h>

static info_t *get_info(json_object *root)
{
    json_object *info_obj = json_object_object_get(root, "info");

    info_t *info = malloc(sizeof(info_t));
    if (!info)
    {
        return NULL;
    }

    json_object *id = json_object_object_get(info_obj, "id");
    json_object *date = json_object_object_get(info_obj, "date");
    json_object *accountId = json_object_object_get(info_obj, "accountId");
    json_object *counteragentId = json_object_object_get(info_obj, "counteragentId");
    json_object *paymentTypeId = json_object_object_get(info_obj, "paymentTypeId");
    json_object *type = json_object_object_get(info_obj, "type");
    json_object *sum = json_object_object_get(info_obj, "sum");
    json_object *comment = json_object_object_get(info_obj, "comment");
    json_object *causeEventId = json_object_object_get(info_obj, "causeEventId");
    json_object *cashierId = json_object_object_get(info_obj, "cashierId");
    json_object *departmentId = json_object_object_get(info_obj, "departmentId");

    json_object *auth = json_object_object_get(info_obj, "auth");
    json_object *user = json_object_object_get(auth, "user");
    json_object *card = json_object_object_get(auth, "card");

    info->id = stupid_strdup(json_object_get_string(id));
    info->date = stupid_strdup(json_object_get_string(date));
    info->accountId = stupid_strdup(json_object_get_string(accountId));
    info->counteragentId = stupid_strdup(json_object_get_string(counteragentId));
    info->paymentTypeId = stupid_strdup(json_object_get_string(paymentTypeId));
    info->type = stupid_strdup(json_object_get_string(type));
    info->sum = json_object_get_int64(sum);
    info->id = stupid_strdup(json_object_get_string(id));
    info->comment = stupid_strdup(json_object_get_string(comment));
    info->causeEventId = stupid_strdup(json_object_get_string(causeEventId));
    info->cashierId = stupid_strdup(json_object_get_string(cashierId));
    info->departmentId = stupid_strdup(json_object_get_string(departmentId));
    info->user = stupid_strdup(json_object_get_string(user));
    info->card = stupid_strdup(json_object_get_string(card));

    return info;
}

static group_enum get_group(cstring group_str)
{
    if (!strncmp(group_str, "CARD", strlen(group_str))) return CARD;
    if (!strncmp(group_str, "CREDIT", strlen(group_str))) return CREDIT;
    if (!strncmp(group_str, "PAYOUT", strlen(group_str))) return PAYOUT;
    if (!strncmp(group_str, "PAYIN", strlen(group_str))) return PAYIN;
    return -1;
}

static inline void fill_records(any_record *records, size_t length, json_object *obj)
{
    json_object *root;

    json_object *actualSum;
    json_object *originalSum;
    json_object *editedPayAccountId;
    json_object *originalPayAccountId;
    json_object *payAgentId;
    json_object *editableComment;
    json_object *group;

    for (size_t i = 0; i < length; i++)
    {
        root = json_object_array_get_idx(obj, i);
        records[i].info = get_info(root);

        actualSum = json_object_object_get(root, "actualSum");
        originalSum = json_object_object_get(root, "originalSum");
        editedPayAccountId = json_object_object_get(root, "editedPayAccountId");
        originalPayAccountId = json_object_object_get(root, "originalPayAccountId");
        payAgentId = json_object_object_get(root, "payAgentId");
        editableComment = json_object_object_get(root, "editableComment");
        group = json_object_object_get(root, "group");

        records[i].actualSum = json_object_get_int64(actualSum);
        records[i].originalSum = json_object_get_int64(originalSum);
        records[i].editedPayAccountId = stupid_strdup(json_object_get_string(editedPayAccountId));
        records[i].originalPayAccountId = stupid_strdup(json_object_get_string(originalPayAccountId));
        records[i].payAgentId = stupid_strdup(json_object_get_string(payAgentId));
        records[i].editableComment = stupid_strdup(json_object_get_string(editableComment));
        records[i].group = get_group(json_object_get_string(group));
    }

    return;
}

cashshifts_payments_list_answer *cashshifts_payments_list_get(cashshifts_payments_list_args args)
{
    char *arguments[] =
    {
        create_arg("key", args.token),
    };

    char *format = malloc(21);
    if (!format)
    {
        free(arguments[0]);
        return NULL;
    }
    snprintf(format, 21, "%zu", args.session_number);

    cstring parts[] = {HTTPS, args.address, "/cashshifts/payments/list/", format};
    char *link = join_strings(parts, 4);
    free(format);

    char *url = parse_args(link, arguments, 1);
    free(link);
    free(arguments[0]);

    curl_get_args cga =
    {
        .url = url,
    };

    curl_get_result *result = curl_get(cga);
    free(url);

    json_object *obj = json_tokener_parse(result->text);

    json_object *sessionId = json_object_object_get(obj, "sessionId");
    json_object *cashlessRecords = json_object_object_get(obj, "cashlessRecords");
    json_object *payInRecords = json_object_object_get(obj, "payInRecords");
    json_object *payOutRecords = json_object_object_get(obj, "payOutRecords");

    size_t cashlessRecords_length = json_object_array_length(cashlessRecords);
    size_t payInRecords_length = json_object_array_length(payInRecords);
    size_t payOutRecords_length = json_object_array_length(payOutRecords);

    cashshifts_payments_list_answer *answer = malloc(sizeof(cashshifts_payments_list_answer));
    any_record *cashless_records = malloc(sizeof(any_record) * cashlessRecords_length);
    any_record *pay_in_records = malloc(sizeof(any_record) * payInRecords_length);
    any_record *pay_out_records = malloc(sizeof(any_record) * payOutRecords_length);

    if (!result || !cashless_records || !pay_in_records || !pay_out_records)
    {
        if (result)             free(result);
        if (cashless_records)   free(cashless_records);
        if (pay_in_records)     free(pay_in_records);
        if (pay_out_records)    free(pay_out_records);
    }

    answer->sessionId = stupid_strdup(json_object_get_string(sessionId));
    answer->cashlessRecords = cashless_records;
    answer->payInRecords = pay_in_records;
    answer->payOutRecords = pay_out_records;

    fill_records(cashless_records, cashlessRecords_length, cashlessRecords);
    fill_records(pay_in_records, payInRecords_length, payInRecords);
    fill_records(pay_out_records, payOutRecords_length, payOutRecords);

    json_object_put(obj);

    return answer;
}
