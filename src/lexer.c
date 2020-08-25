#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "include/lexer.h"
#include "include/stringutils.h"
#include "include/token.h"

// Create a new holyc-i lexer
holyc_lexer_T* init_holyc_lexer(char* contents)
{
    holyc_lexer_T* holyc_lexer = calloc(1, sizeof(struct HOLYC_LEXER_STRUCT));

    holyc_lexer->contents = contents;
    holyc_lexer->contents_length = strlen(contents);

    holyc_lexer->char_index = 0;
    holyc_lexer->line_n = 1;
    holyc_lexer->current_char = holyc_lexer->contents[holyc_lexer->char_index];

    return holyc_lexer; 
}

// Deallocate a holyc-i lexer
void holyc_lexer_free(holyc_lexer_T* holyc_lexer)
{
    free(holyc_lexer->contents);
    free(holyc_lexer);
}

// Get the next token from the lexer
token_T* holyc_lexer_get_next_token(holyc_lexer_T* holyc_lexer)
{
    while (holyc_lexer->current_char != '\0' && holyc_lexer->char_index < holyc_lexer->contents_length)
    {
        if (holyc_lexer->current_char == ' ' || (int) holyc_lexer->current_char == 10 || (int) holyc_lexer->current_char == 13)
            holyc_lexer_skip_whitespace(holyc_lexer);
        
        if (isdigit(holyc_lexer->current_char))
            return holyc_lexer_collct_number(holyc_lexer);

        if (isalnum(holyc_lexer->current_char))
            return holyc_lexer_collect_id(holyc_lexer);

        if (holyc_lexer->current_char == '+')
        {
            char* value = holyc_lexer_current_charstr(holyc_lexer);

            int type = TOKEN_PLUS;
            holyc_lexer_advance(holyc_lexer);

            if (holyc_lexer->current_char == '=')   // +=
            {
                type = TOKEN_PLUS_EQUALS;
                value = realloc(value, strlen(value) + 2);
                char* strchar = holyc_lexer_current_charstr(holyc_lexer);
                strcat(value, strchar);
                free(strchar);

                holyc_lexer_advance(holyc_lexer);
            }

            return init_token(type, value);
        }

        if (holyc_lexer->current_char == '-')
        {
            char* value = holyc_lexer_current_charstr(holyc_lexer);
            int type = TOKEN_MINUS;
            holyc_lexer_advance(holyc_lexer);

            if (holyc_lexer->current_char == '=')   // -=
            {
                type = TOKEN_MINUS_EQUALS;
                value = realloc(value, strlen(value) + 2);
                char* strchar = holyc_lexer_current_charstr(holyc_lexer);
                strcat(value, strchar);
                free(strchar);

                holyc_lexer_advance(holyc_lexer);
            }

            return init_token(type, value);
        }

        if (holyc_lexer->current_char == '*')
        {
            char* value = holyc_lexer_current_charstr(holyc_lexer);
            int type = TOKEN_STAR;
            holyc_lexer_advance(holyc_lexer);

            if (holyc_lexer->current_char == '=')   // -=
            {
                type = TOKEN_STAR_EQUALS;
                value = realloc(value, strlen(value) + 2);
                char* strchar = holyc_lexer_current_charstr(holyc_lexer);
                strcat(value, strchar);
                free(strchar);

                holyc_lexer_advance(holyc_lexer);
            }

            return init_token(type, value);
        }

        if (holyc_lexer->current_char == '&')
        {
            char* value = holyc_lexer_current_charstr(holyc_lexer);
            
            holyc_lexer_advance(holyc_lexer);

            if (holyc_lexer->current_char == '&')   // &&
            {
                value = realloc(value, strlen(value) + 2);
                char* strchar = holyc_lexer_current_charstr(holyc_lexer);
                strcat(value, strchar);
                free(strchar);

                holyc_lexer_advance(holyc_lexer);

                return init_token(TOKEN_AND, value);
            }
        }

        if (holyc_lexer->current_char == '=')
        {
            char* value = holyc_lexer_current_charstr(holyc_lexer);

            holyc_lexer_advance(holyc_lexer);

            if (holyc_lexer->current_char == '=')   // ==
            {
                value = realloc(value, strlen(value) + 2);
                char* strchar = holyc_lexer_current_charstr(holyc_lexer);
                strcat(value, strchar);
                free(strchar);

                holyc_lexer_advance(holyc_lexer);

                token_T* token = init_token(TOKEN_EQUALS_EQUALS, value);
                free(value);
                return token;
            }
            else
            {
                token_T* token = init_token(TOKEN_EQUALS, value);
                free(value);
                return token;
            }
        }

        if (holyc_lexer->current_char == '!')
        {
            char* value = holyc_lexer_current_charstr(holyc_lexer);

            holyc_lexer_advance(holyc_lexer);

            if (holyc_lexer->current_char == '=')  // !=
            {
                value = realloc(value, strlen(value) + 2);
                char* strchar = holyc_lexer_current_charstr(holyc_lexer);
                strcat(value, strchar);
                free(strchar);

                holyc_lexer_advance(holyc_lexer);

                token_T* token = init_token(TOKEN_NOT_EQUALS, value);
                free(value);
                return token;
            }

            token_T* token = init_token(TOKEN_NOT, value);
            free(value);
            return token;
        }

        if (holyc_lexer->current_char == '#')
        {
            holyc_lexer_advance(holyc_lexer);
            holyc_lexer_skip_inline_comment(holyc_lexer);
            continue;
        }
        
        if (holyc_lexer->current_char == '/')
        {
            holyc_lexer_advance(holyc_lexer);

            if (holyc_lexer->current_char == '/')
            {
                holyc_lexer_advance(holyc_lexer);
                holyc_lexer_skip_inline_comment(holyc_lexer);
                continue;
            }
            else
            if (holyc_lexer->current_char == '*')
            {
                holyc_lexer_advance(holyc_lexer);
                holyc_lexer_skip_block_comment(holyc_lexer);
                continue;
            }
            else
            if (holyc_lexer->current_char != '/')
            {
                return init_token(TOKEN_DIV, "/");
            }
        }

        switch(holyc_lexer->current_char)
        {
            case '"': return holyc_lexer_collect_string(holyc_lexer); break;
            case '\'': return holyc_lexer_collect_char(holyc_lexer); break;
            case '{': return holyc_lexer_advance_with_token(holyc_lexer, TOKEN_LBRACE); break;
            case '}': return holyc_lexer_advance_with_token(holyc_lexer, TOKEN_RBRACE); break;
            case '[': return holyc_lexer_advance_with_token(holyc_lexer, TOKEN_LBRACKET); break;
            case ']': return holyc_lexer_advance_with_token(holyc_lexer, TOKEN_RBRACKET); break;
            case '(': return holyc_lexer_advance_with_token(holyc_lexer, TOKEN_LPAREN); break;
            case ')': return holyc_lexer_advance_with_token(holyc_lexer, TOKEN_RPAREN); break;
            case ';': return holyc_lexer_advance_with_token(holyc_lexer, TOKEN_SEMI); break;
            case ',': return holyc_lexer_advance_with_token(holyc_lexer, TOKEN_COMMA); break;
            case '%': return holyc_lexer_advance_with_token(holyc_lexer, TOKEN_PERCENTAGE); break;
            case '.': return holyc_lexer_advance_with_token(holyc_lexer, TOKEN_DOT); break;
            case '<': return holyc_lexer_advance_with_token(holyc_lexer, TOKEN_LESS_THAN); break;
            case '>': return holyc_lexer_advance_with_token(holyc_lexer, TOKEN_LARGER_THAN); break;
            case '@': return holyc_lexer_advance_with_token(holyc_lexer, TOKEN_ANON_ID); break;
            case '?': return holyc_lexer_advance_with_token(holyc_lexer, TOKEN_QUESTION); break;
            case ':': return holyc_lexer_advance_with_token(holyc_lexer, TOKEN_COLON); break;
            case '\0': return init_token(TOKEN_EOF, "\0"); break;
            default: printf("[Line %d] Unexpected %c\n", holyc_lexer->line_n, holyc_lexer->current_char); exit(1); break;
        }
    }

    return init_token(TOKEN_EOF, "\0");
}

// Advances and returns a token
token_T* holyc_lexer_advance_with_token(holyc_lexer_T* holyc_lexer, int type)
{
    char* value = holyc_lexer_current_charstr(holyc_lexer);
    holyc_lexer_advance(holyc_lexer);
    token_T* token = init_token(type, value);
    free(value);

    // ensures that the lexer state is correct if exited through this function.
    holyc_lexer_skip_whitespace(holyc_lexer);

    return token;
}

// Advance, M=move to the next char
void holyc_lexer_advance(holyc_lexer_T* holyc_lexer)
{
    if (holyc_lexer->current_char == '\n' || holyc_lexer->current_char == 10)
        holyc_lexer->line_n += 1;

    if (holyc_lexer->current_char != '\0' && holyc_lexer->char_index < holyc_lexer->contents_length)
    {
        holyc_lexer->char_index += 1;
        holyc_lexer->current_char = holyc_lexer->contents[holyc_lexer->char_index];
    }
    else
    {
        holyc_lexer->current_char = '\0';
    }
}

// Expect the current char to be `c`, if not, throw an error
void holyc_lexer_expect_char(holyc_lexer_T* holyc_lexer, char c)
{
    if (holyc_lexer->current_char != c)
    {
        printf("Error: [Line %d] Lexer expected the current char to be `%c`, but it was `%c`.", c, holyc_lexer->current_char, holyc_lexer->line_n);
        exit(1);
    }
}

// Move to the next char until there is no more whitespace.
void holyc_lexer_skip_whitespace(holyc_lexer_T* holyc_lexer)
{
    while (holyc_lexer->current_char == ' ' || (int) holyc_lexer->current_char == 10 || (int) holyc_lexer->current_char == 13)
        holyc_lexer_advance(holyc_lexer);
}

// Skip an inline comment
void holyc_lexer_skip_inline_comment(holyc_lexer_T* holyc_lexer)
{
    while (holyc_lexer->current_char != '\n' && holyc_lexer->current_char != 10)
        holyc_lexer_advance(holyc_lexer);
}

// Skip a block comment
void holyc_lexer_skip_block_comment(holyc_lexer_T* holyc_lexer)
{
    while (1)
    {
        holyc_lexer_advance(holyc_lexer);

        if (holyc_lexer->current_char == '*')
        {
            holyc_lexer_advance(holyc_lexer);

            if (holyc_lexer->current_char == '/')
            {
                holyc_lexer_advance(holyc_lexer);
                return;
            }
        }
    }
}

// Collect a string token
token_T* holyc_lexer_collect_string(holyc_lexer_T* holyc_lexer)
{
     holyc_lexer_expect_char(holyc_lexer, '"');
     holyc_lexer_advance(holyc_lexer);

     size_t initial_index = holyc_lexer->char_index;

     while (holyc_lexer->current_char != '"')
     {
         if (holyc_lexer->current_char == '\0')
         {
             printf("[Line %d] Missing closing quotation mark\n", holyc_lexer->line_n); exit(1);
         }

         holyc_lexer_advance(holyc_lexer);
     }

     size_t length = holyc_lexer->char_index - initial_index + 1;
     char* buffer = calloc(length, sizeof(char));
     memcpy(buffer, &holyc_lexer->contents[initial_index], length);
     buffer[length - 1] = '\0';

     holyc_lexer_advance(holyc_lexer);

     token_T* token = init_token(TOKEN_STRING_VALUE, buffer);
     free(buffer);

     return token;
}

// Collect a char token
token_T* holyc_lexer_collect_char(holyc_lexer_T* holyc_lexer)
{
    holyc_lexer_expect_char(holyc_lexer, '\'');
    holyc_lexer_advance(holyc_lexer);
    char* buffer = calloc(1, sizeof(char));
    buffer[0] = '\0';

    int c = 0;

    while (holyc_lexer->current_char != '\'')
    {
        if (c > 1)
        {
            printf("Error: [Line %d] Chars can only contain one character\n", holyc_lexer->line_n);
            exit(1);
        }

        char* strchar = holyc_lexer_current_charstr(holyc_lexer);
        buffer = realloc(buffer, strlen(buffer) + 2);
        strcat(buffer, strchar);
        free(strchar);

        holyc_lexer_advance(holyc_lexer);

        c++;
    }

    holyc_lexer_advance(holyc_lexer);

    return init_token(TOKEN_CHAR_VALUE, buffer);
}

// Collect a numeric token
token_T* holyc_lexer_collect_number(holyc_lexer_T* holyc_lexer)
{
    int type = TOKEN_INTEGER_VALUE;

    char* buffer = calloc(1, sizeof(char));
    buffer[0] = '\0';

    while (isdigit(holyc_lexer->current_char))
    {
        char* strchar = holyc_lexer_current_charstr(holyc_lexer);
        buffer = realloc(buffer, strlen(buffer) + 2);
        strcat(buffer, strchar);
        free(strchar);

        holyc_lexer_advance(holyc_lexer);
    }

    if (holyc_lexer->current_char == '.')
    {
        char* strchar = holyc_lexer_current_charstr(holyc_lexer);
        buffer = realloc(buffer, strlen(buffer) + 2);
        strcat(buffer, strchar);
        free(strchar);

        holyc_lexer_advance(holyc_lexer);

        type = TOKEN_FLOAT_VALUE;

        while (isdigit(holyc_lexer->current_char))
        {
            char* strchar = holyc_lexer_current_charstr(holyc_lexer);
            buffer = realloc(buffer, strlen(buffer) + 2);
            strcat(buffer, strchar);
            free(strchar);

            holyc_lexer_advance(holyc_lexer);
        }
    }

    return init_token(type, buffer);
}

// Collect an ID token
token_T* holyc_lexer_collect_id(holyc_lexer_T* holyc_lexer)
{
    size_t initial_index = holyc_lexer->char_index;

    while (isalnum(holyc_lexer->current_char) || holyc_lexer->current_char == '_')
    {
        holyc_lexer_advance(holyc_lexer);
    }

    size_t length = holyc_lexer->char_index - initial_index + 1;
    char* buffer = calloc(length, sizeof(char));
    memcpy(buffer, &holyc_lexer->contents[initial_index], length);
    buffer[length - 1] = '\0';

    token_T* token = init_token(TOKEN_ID, buffer);
    free(buffer);

    return token;
}

// Returns the current char of the holyc lexer as a string
char* holyc_lexer_current_charstr(holyc_lexer_T* holyc_lexer)
{
    char* str = calloc(2, sizeof(char));
    str[0] = holyc_lexer->current_char;
    str[1] = '\0';

    return str;
}