#ifndef HOLYC_STRING_UTILS_H
#define HOLYC_STRING_UTILS_H
#include <stdlib.h>
char* holyc_char_to_string(char c);

char* holyc_init_str(const char* value);

char* holyc_get_stdin(const char* printstr);

char *holyc_strrev(char *instr);
#endif
