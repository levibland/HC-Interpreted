#ifndef HOLYC_STRING_UTILS
#define HOLYC_STRING_UTILS

#include <stdlib.h>

char* char_to_string(char c);

char* holyc_init_string(const char* value);

char* holyc_get_stdin(const char* str);

char *holyc_strrev(char *instr);

#endif