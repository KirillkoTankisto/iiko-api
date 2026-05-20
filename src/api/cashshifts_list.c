#include "cashshifts_list.h"
#include "../utils/utils.h"

#include <json-c/json.h>

static int cmp_cashshifts(const void *a, const void *b)
{
    const cashshifts_list_answer_element *aa = a;
    const cashshifts_list_answer_element *bb = b;

    if (aa->sessionNumber < bb->sessionNumber) return -1;
    if (aa->sessionNumber > bb->sessionNumber) return 1;
    return 0;
}

static void sort_cashshifts(cashshifts_list_answer *list)
{
    qsort(list->elements, list->size, sizeof(cashshifts_list_answer_element), cmp_cashshifts);
}

static inline void cashshifts_list_answer_fill(cashshifts_list_answer *answer, json_object *obj)
{
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

    for (size_t index = 0; index < answer->size; index++)
    {
        cashshifts_list_answer_element elem;
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
        elem.cashRegSerial = stupid_strdup(json_object_get_string(cashRegSerial));
        elem.openDate = stupid_strdup(json_object_get_string(openDate));
        elem.closeDate = stupid_strdup(json_object_get_string(closeDate));
        elem.acceptDate = stupid_strdup(json_object_get_string(acceptDate));
        elem.managerId = stupid_strdup(json_object_get_string(managerId));
        elem.responsibleUserId = stupid_strdup(json_object_get_string(responsibleUserId));
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
        elem.sessionStatus = stupid_strdup(json_object_get_string(sessionStatus));
        elem.conceptionId = stupid_strdup(json_object_get_string(conceptionId));
        elem.pointOfSaleId = stupid_strdup(json_object_get_string(pointOfSaleId));

        answer->elements[index] = elem;
    }
}

cashshifts_list_answer *cashshifts_list_get(cashshifts_list_args args)
{
    char *arguments[7] =
    {
        create_arg("key", args.token),
        create_arg("openDateFrom", args.prompt.openDateFrom),
        create_arg("openDateTo", args.prompt.openDateTo),
        create_arg("status", args.prompt.status),
        NULL,
        NULL,
        NULL,
    };

    size_t args_pos = 4;

    if (args.prompt.departmentId)
    {
        arguments[args_pos] = create_arg("departmentId", args.prompt.departmentId);
        args_pos += 1;
    }

    if (args.prompt.groupId)
    {
        arguments[args_pos] = create_arg("groupId", args.prompt.groupId);
        args_pos += 1;
    }

    if (args.prompt.revision_from != -1)
    {
        char string[32];
        snprintf(string, sizeof(string), "%d", args.prompt.revision_from);
        args_pos += 1;
    }

    char *link = create_link(HTTPS, args.address, "/resto/api/v2/cashshifts/list");

    char *url = parse_args(link, arguments, args_pos);
    free(link);
    for (size_t i = 0; i < args_pos - 1; i++)
    {
        free(arguments[i]);
    }

    curl_get_args cga =
    {
        .url = url,
    };

    curl_get_result *result = curl_get(cga);
    free(url);

    json_object* obj = json_tokener_parse(result->text);

    size_t array_length = json_object_array_length(obj);

    cashshifts_list_answer *answer = malloc(sizeof(cashshifts_list_answer));
    answer->elements = malloc(sizeof(*answer->elements) * array_length);
    answer->size = array_length;

    cashshifts_list_answer_fill(answer, obj);
    json_object_put(obj);

    sort_cashshifts(answer);

    return answer;
}

void cashshifts_list_destroy(cashshifts_list_answer *list)
{
    free(list->elements);
    free(list);
}
