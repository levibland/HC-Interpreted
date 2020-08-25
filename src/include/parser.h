#ifndef HOLYC_PARSER_H
#define HOLYC_PARSER_H

#include "lexer.h"
#include "ast.h"
#include "token.h"
#include "holyc_scope.h"

typedef struct HOLYC_PARSER_STRUCT
{
    holyc_lexer_T* holyc_lexer;
    token_T* prev_token;
    token_T* current_token;
    data_type_T* data_type;
} holyc_parser_T;

holyc_parser_T* init_holyc_parser(holyc_parser_T* holyc_parser);

void holyc_parser_type_error(holyc_parser_T* holyc_parser);

void holyc_parser_syntax_error(holyc_parser_T* holyc_parser);

void holyc_parser_unexpected_token(holyc_parser_T* holyc_parser, int token_type);

AST_T* holyc_parser_parse(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_eat(holyc_parser_T* holyc_parser, int token_type);

AST_T* holyc_parser_parse_statement(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_statements(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parser_type(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

// variables

AST_T* holyc_parser_parse_float(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_string(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_char(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_integer(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_boolean(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_null(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_variable(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_object(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_enum(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_list(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

// Math

AST_T* holyc_parser_parse_factor(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_term(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_expr(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

// Statements

AST_T* holyc_parser_parse_break(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_continue(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_return(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_if(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_ternary(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_new(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_iterate(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_assert(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

// Loops

AST_T* holyc_parser_parse_while(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_for(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

// Functions

AST_T* holyc_parser_parse_function_call(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

AST_T* holyc_parser_parse_function_definition(holyc_parser_T* holyc_parser, holyc_scope_T* scope);

#endif