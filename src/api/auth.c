#include "auth.h"
#include "../utils/utils.h"

curl_get_result *auth(auth_args args)
{
    char *link = create_link(HTTPS, args.address, "/resto/api/auth");
    char *login_arg = create_arg("login", args.login);
    char *pass_arg = create_arg("pass", calculate_sha1(args.pass));

    char *arguments[] = {login_arg,pass_arg};
    char *url = parse_args(link, arguments, 2);
    free(link);
    free(login_arg);
    free(pass_arg);

    curl_get_args cga =
    {
        .url = url,
    };

    curl_get_result *result = curl_get(cga);
    free(url);

    return result;
}
