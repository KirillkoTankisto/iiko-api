#include <stdlib.h>

char *join_strings(const char *strings[], size_t length);

char *calculate_sha1(const char *string);

char *parse_args(const char *base, char *args[], size_t args_len);

char *create_link(const char *first, const char *middle, const char *last);

char *create_arg(const char *key, const char *value);

char *stupid_strdup(const char *input);
