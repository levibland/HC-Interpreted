#ifndef HOLYC_LEXER_H
#define HOLYC_LEXER_H
#include "token.h"
#include <stdlib.h>


typedef struct holyc_LEXER_STRUCT
{
    char* contents;
    size_t contents_length;

    char current_char;
    unsigned int char_index;
    unsigned int line_n;
} holyc_lexer_T;

holyc_lexer_T* init_holyc_lexer(char* contents);

void holyc_lexer_free(holyc_lexer_T* holyc_lexer);

token_T* holyc_lexer_get_next_token(holyc_lexer_T* holyc_lexer);

token_T* holyc_lexer_advance_with_token(holyc_lexer_T* holyc_lexer, int type);

void holyc_lexer_advance(holyc_lexer_T* holyc_lexer);

void holyc_lexer_expect_char(holyc_lexer_T* holyc_lexer, char c);

void holyc_lexer_skip_whitespace(holyc_lexer_T* holyc_lexer);

void holyc_lexer_skip_inline_comment(holyc_lexer_T* holyc_lexer);

void holyc_lexer_skip_block_comment(holyc_lexer_T* holyc_lexer);

token_T* holyc_lexer_collect_string(holyc_lexer_T* holyc_lexer);

token_T* holyc_lexer_collect_char(holyc_lexer_T* holyc_lexer);

token_T* holyc_lexer_collect_number(holyc_lexer_T* holyc_lexer);

token_T* holyc_lexer_collect_id(holyc_lexer_T* holyc_lexer);

char* holyc_lexer_current_charstr(holyc_lexer_T* holyc_lexer);
#endif
