#include <stdlib.h>

const char *join_strings(const char *strings[], size_t length);

const char *calculate_sha1(const char *string);

const char *parse_args(const char *base, const char *args[], size_t args_len);

const char *create_link(const char *first, const char *middle, const char *last);

const char *create_arg(const char *key, const char *value);

const char *get_current_date(void);
