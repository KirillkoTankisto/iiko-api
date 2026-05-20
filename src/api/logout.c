#include "logout.h"
#include "../utils/utils.h"

curl_get_result *logout(logout_args args)
{
    char *arg = create_arg("key", args.token);
    char *arguments[] = {arg};

    char *link = create_link(HTTPS, args.address, "/resto/api/logout");
    char *url = parse_args(link, arguments, 1);
    free(link);
    free(arg);

    curl_get_args cga =
    {
        .url = url,
    };

    curl_get_result *result = curl_get(cga);
    free(url);

    return result;
}
