#include <stdlib.h>

char* join_strings(char* strings[], size_t length);

char* calculate_sha1(char* string);

char* parse_args(const char* base, const char* args[], size_t args_len);

char* create_link(char* first, char* middle, char* last);

char* create_arg(char* key, char* value);

char* get_current_date(void);
