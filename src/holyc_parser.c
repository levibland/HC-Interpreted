#include "include/holyc_parser.h"
#include <string.h>


const char* STATEMENT_WHILE = "while";
const char* STATEMENT_FOR = "for";
const char* STATEMENT_IF = "if";
const char* STATEMENT_ELSE = "else";
const char* STATEMENT_RETURN = "return";
const char* STATEMENT_BREAK = "break";
const char* STATEMENT_CONTINUE = "continue";
const char* STATEMENT_NEW = "new";
const char* STATEMENT_ITERATE = "iterate";
const char* STATEMENT_ASSERT = "assert";

const char* VALUE_FALSE = "false";
const char* VALUE_TRUE = "true";
const char* VALUE_NULL = "NULL";

/**
 * Initializes the holyc parser.
 */
holyc_parser_T* init_holyc_parser(holyc_lexer_T* holyc_lexer)
{
    holyc_parser_T* holyc_parser = calloc(1, sizeof(struct holyc_PARSER_STRUCT));
    holyc_parser->holyc_lexer = holyc_lexer;
    holyc_parser->current_token = holyc_lexer_get_next_token(holyc_parser->holyc_lexer);
    holyc_parser->prev_token = (void*)0;
    holyc_parser->data_type = (void*)0;

    return holyc_parser;
}

// etc

/**
 * Throws a type error and exit(1).
 */
void holyc_parser_type_error(holyc_parser_T* holyc_parser)
{
    printf("[Line %d] Invalid type for assigned value\n", holyc_parser->holyc_lexer->line_n);
    exit(1);
}

/**
 * Throws a syntax error and exit(1).
 */
void holyc_parser_syntax_error(holyc_parser_T* holyc_parser)
{
    printf("[Line %d] Syntax error\n", holyc_parser->holyc_lexer->line_n);
    exit(1);
}

/**
 * Throws a unexpected token error and exit(1)
 */
void holyc_parser_unexpected_token_error(holyc_parser_T* holyc_parser, int token_type)
{
    printf(
        "[Line %d] Unexpected token `%s`, was expecting `%d`.\n",
        holyc_parser->holyc_lexer->line_n,
        holyc_parser->current_token->value,
        token_type
    );
    exit(1);
}

/**
 * Sets the parent of an AST.
 */
static AST_T* as_object_child(AST_T* ast, AST_T* object)
{
    ast->is_object_child = 1;
    ast->parent = object;

    return ast;
}

/**
 * Checks if a string is a data_type
 */
static unsigned int is_data_type(char* token_value)
{
    return (
        strcmp(token_value, "void") == 0 ||
        strcmp(token_value, "int") == 0 ||
        strcmp(token_value, "string") == 0 ||
        strcmp(token_value, "char") == 0 ||
        strcmp(token_value, "float") == 0 ||
        strcmp(token_value, "bool") == 0 ||
        strcmp(token_value, "object") == 0 ||
        strcmp(token_value, "enum") == 0 ||
        strcmp(token_value, "list") == 0 ||
        strcmp(token_value, "source") == 0
    );
}

/**
 * Checks if a string is a data_type_modifier
 */
static unsigned int is_data_type_modifier(char* token_value)
{
    return strcmp(token_value, "long") == 0;
}

/**
 * Parses a compound with only one statement as a child.
 */
static AST_T* holyc_parser_parse_compound_with_one_statement(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* compound = init_ast_with_line(AST_COMPOUND, holyc_parser->holyc_lexer->line_n);
    compound->scope = (struct holyc_scope_T*) scope;
    AST_T* statement = holyc_parser_parse_statement(holyc_parser, scope);
    holyc_parser_eat(holyc_parser, TOKEN_SEMI);
    dynamic_list_append(compound->compound_value, statement);

    return compound;
}

/**
 * Main entry point of the parser.
 */
AST_T* holyc_parser_parse(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    return holyc_parser_parse_statements(holyc_parser, scope);
}

/**
 * Consumes a token and moves to the next one if the current token
 * is the same as the expected one.
 */
AST_T* holyc_parser_eat(holyc_parser_T* holyc_parser, int token_type)
{
    if (holyc_parser->current_token->type != token_type)
    {
        holyc_parser_unexpected_token_error(holyc_parser, token_type);
    }
    else if (holyc_parser->current_token->type == token_type)
    {
        if (holyc_parser->prev_token)
            token_free(holyc_parser->prev_token);

        holyc_parser->prev_token = holyc_parser->current_token;
        holyc_parser->current_token = holyc_lexer_get_next_token(holyc_parser->holyc_lexer);
    }

    return (void*)0;
}

/**
 * Parses a single statement.
 */
AST_T* holyc_parser_parse_statement(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    switch (holyc_parser->current_token->type)
    {
        case TOKEN_ID: {
            char* token_value = holyc_parser->current_token->value;

            if (strcmp(token_value, STATEMENT_WHILE) == 0)
                return holyc_parser_parse_while(holyc_parser, scope);

            if (strcmp(token_value, STATEMENT_FOR) == 0)
                return holyc_parser_parse_for(holyc_parser, scope);

            if (strcmp(token_value, STATEMENT_IF) == 0)
                return holyc_parser_parse_if(holyc_parser, scope);

            if (strcmp(token_value, VALUE_FALSE) == 0 || strcmp(token_value, VALUE_TRUE) == 0)
                return holyc_parser_parse_boolean(holyc_parser, scope);

            if (strcmp(token_value, VALUE_NULL) == 0)
                return holyc_parser_parse_null(holyc_parser, scope);

            if (strcmp(token_value, STATEMENT_RETURN) == 0)
                return holyc_parser_parse_return(holyc_parser, scope);

            if (strcmp(token_value, STATEMENT_BREAK) == 0)
                return holyc_parser_parse_break(holyc_parser, scope);

            if (strcmp(token_value, STATEMENT_CONTINUE) == 0)
                return holyc_parser_parse_continue(holyc_parser, scope);

            if (strcmp(token_value, STATEMENT_NEW) == 0)
                return holyc_parser_parse_new(holyc_parser, scope);

            if (strcmp(token_value, STATEMENT_ITERATE) == 0)
                return holyc_parser_parse_iterate(holyc_parser, scope);

            if (strcmp(token_value, STATEMENT_ASSERT) == 0)
                return holyc_parser_parse_assert(holyc_parser, scope);

            if (is_data_type(token_value) || is_data_type_modifier(token_value))
                return holyc_parser_parse_function_definition(holyc_parser, scope);

            holyc_parser_eat(holyc_parser, TOKEN_ID);

            AST_T* a = holyc_parser_parse_variable(holyc_parser, scope);

            while (holyc_parser->current_token->type == TOKEN_LPAREN)
                a = holyc_parser_parse_function_call(holyc_parser, scope, a);

            while (holyc_parser->current_token->type == TOKEN_DOT)
            {
                holyc_parser_eat(holyc_parser, TOKEN_DOT);
                AST_T* ast = init_ast_with_line(AST_ATTRIBUTE_ACCESS, holyc_parser->holyc_lexer->line_n);
                ast->binop_left = a;
                ast->binop_right = holyc_parser_parse_expr(holyc_parser, scope);

                a = ast;
            }

            while (holyc_parser->current_token->type == TOKEN_LBRACKET)
            {
                AST_T* ast_list_access = init_ast_with_line(AST_LIST_ACCESS, holyc_parser->holyc_lexer->line_n);
                ast_list_access->binop_left = a;
                holyc_parser_eat(holyc_parser, TOKEN_LBRACKET);
                ast_list_access->list_access_pointer = holyc_parser_parse_expr(holyc_parser, scope);
                holyc_parser_eat(holyc_parser, TOKEN_RBRACKET);

                a = ast_list_access;
            }

            if (a)
                return a;
        } break;
        case TOKEN_NUMBER_VALUE: case TOKEN_STRING_VALUE: case TOKEN_CHAR_VALUE: case TOKEN_FLOAT_VALUE: case TOKEN_INTEGER_VALUE: return holyc_parser_parse_expr(holyc_parser, scope); break;
        case TOKEN_ANON_ID: { printf("[Line %d] Unexpected token `%s`\n", holyc_parser->holyc_lexer->line_n, holyc_parser->current_token->value); exit(1); } break;
        default: return init_ast_with_line(AST_NOOP, holyc_parser->holyc_lexer->line_n); break;
    }

    return init_ast_with_line(AST_NOOP, holyc_parser->holyc_lexer->line_n);
}

/**
 * Parses a compound with a list of statements.
 */
AST_T* holyc_parser_parse_statements(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* compound = init_ast_with_line(AST_COMPOUND, holyc_parser->holyc_lexer->line_n);
    compound->scope = (struct holyc_scope_T*) scope;

    AST_T* statement = holyc_parser_parse_statement(holyc_parser, scope);

    dynamic_list_append(compound->compound_value, statement);

    while (holyc_parser->current_token->type == TOKEN_SEMI || statement->type != AST_NOOP)
    {
        if (holyc_parser->current_token->type == TOKEN_SEMI)
            holyc_parser_eat(holyc_parser, TOKEN_SEMI);

        statement = holyc_parser_parse_statement(holyc_parser, scope);

        dynamic_list_append(compound->compound_value, statement);
    }

    return compound;
}

/**
 * Parses a data type with modifiers.
 */
AST_T* holyc_parser_parse_type(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* ast_type = init_ast_with_line(AST_TYPE, holyc_parser->holyc_lexer->line_n);
    ast_type->scope = (struct holyc_scope_T*) scope;

    data_type_T* type = init_data_type();

    int modifier_index = 0;
    while (is_data_type_modifier(holyc_parser->current_token->value))
    {
        if (modifier_index > 3)
        {
            printf("[Line %d] Too many modifiers\n", holyc_parser->holyc_lexer->line_n);
            exit(1);
        }

        int modifier = 0;

        if (strcmp(holyc_parser->current_token->value, "long") == 0)
            modifier = DATA_TYPE_MODIFIER_LONG;

        type->modifiers[modifier_index] = modifier;

        holyc_parser_eat(holyc_parser, TOKEN_ID); // eat modifier token

        modifier_index += 1;
    }

    char* token_value = holyc_parser->current_token->value;

    if (strcmp(token_value, "void") == 0)
        type->type = DATA_TYPE_VOID;
    else
    if (strcmp(token_value, "string") == 0)
        type-> type = DATA_TYPE_STRING;
    else
    if (strcmp(token_value, "char") == 0)
        type-> type = DATA_TYPE_CHAR;
    else
    if (strcmp(token_value, "int") == 0)
        type-> type = DATA_TYPE_INT;
    else
    if (strcmp(token_value, "float") == 0)
        type-> type = DATA_TYPE_FLOAT;
    else
    if (strcmp(token_value, "boolean") == 0)
        type-> type = DATA_TYPE_BOOLEAN;
    else
    if (strcmp(token_value, "object") == 0)
        type-> type = DATA_TYPE_OBJECT;
    else
    if (strcmp(token_value, "enum") == 0)
        type-> type = DATA_TYPE_ENUM;
    else
    if (strcmp(token_value, "list") == 0)
        type-> type = DATA_TYPE_LIST;
    else
    if (strcmp(token_value, "source") == 0)
        type-> type = DATA_TYPE_SOURCE;
    
    ast_type->type_value = type;

    holyc_parser_eat(holyc_parser, TOKEN_ID);

    return ast_type;
}

// values

/**
 * Parses a float value.
 * Simply consumes the current token and converts it to a float.
 */
AST_T* holyc_parser_parse_float(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* ast_float = init_ast_with_line(AST_FLOAT, holyc_parser->holyc_lexer->line_n);
    ast_float->scope = (struct holyc_scope_T*) scope;
    ast_float->float_value = (float) atof(holyc_parser->current_token->value);

    holyc_parser_eat(holyc_parser, TOKEN_FLOAT_VALUE);

    return ast_float;
}

/**
 * Parses a string value.
 * Consumes the current token and puts the string value on a AST.
 */
AST_T* holyc_parser_parse_string(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* ast_string = init_ast_with_line(AST_STRING, holyc_parser->holyc_lexer->line_n);
    ast_string->scope = (struct holyc_scope_T*) scope;
    ast_string->string_value = calloc(strlen(holyc_parser->current_token->value) + 1, sizeof(char));
    strcpy(ast_string->string_value, holyc_parser->current_token->value);

    holyc_parser_eat(holyc_parser, TOKEN_STRING_VALUE);

    return ast_string;
}

/**
 * Parses a char value.
 * Uses the current token to extract the char.
 */
AST_T* holyc_parser_parse_char(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* ast_string = init_ast_with_line(AST_CHAR, holyc_parser->holyc_lexer->line_n);
    ast_string->scope = (struct holyc_scope_T*) scope;
    ast_string->char_value = holyc_parser->current_token->value[0];

    holyc_parser_eat(holyc_parser, TOKEN_CHAR_VALUE);

    return ast_string;
}

/**
 * Parses an integer within the context of the current data_type
 * (if there is any).
 * Otherwise just simply parses an integer.
 */
AST_T* holyc_parser_parse_integer(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* ast_integer = init_ast_with_line(AST_INTEGER, holyc_parser->holyc_lexer->line_n);
    ast_integer->scope = (struct holyc_scope_T*) scope;

    unsigned int value_is_set = 0;
    
    if (holyc_parser->data_type != (void*)0)
    {
        if (data_type_has_modifier(holyc_parser->data_type, DATA_TYPE_MODIFIER_LONG))
        {
            ast_integer->long_int_value = atoi(holyc_parser->current_token->value);
            value_is_set = 1;
            holyc_parser->data_type = (void*)0;
        }
    }

    if (!value_is_set)
        ast_integer->int_value = atoi(holyc_parser->current_token->value);

    holyc_parser_eat(holyc_parser, TOKEN_INTEGER_VALUE);

    return ast_integer;
}

/**
 * Parses a boolean value. (false / true)
 */
AST_T* holyc_parser_parse_boolean(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* ast_boolean = init_ast_with_line(AST_BOOLEAN, holyc_parser->holyc_lexer->line_n);
    ast_boolean->scope = (struct holyc_scope_T*) scope;
    
    if (strcmp(holyc_parser->current_token->value, "false") == 0)
        ast_boolean->boolean_value = 0; 
    else if (strcmp(holyc_parser->current_token->value, "true") == 0)
        ast_boolean->boolean_value = 1;
    else
        printf("%p is not a boolean value\n", holyc_parser->current_token->value);

    holyc_parser_eat(holyc_parser, TOKEN_ID);

    return ast_boolean;
}

/**
 * Parses a null value.
 */
AST_T* holyc_parser_parse_null(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* ast_null = init_ast_with_line(AST_NULL, holyc_parser->holyc_lexer->line_n);
    ast_null->scope = (struct holyc_scope_T*) scope;

    holyc_parser_eat(holyc_parser, TOKEN_ID);

    return ast_null;
}

/**
 * Parses a variable and also a variable assignment if a TOKEN_EQUALS
 * is encountered.
 * Also parses a variable modifier if it encounters it.
 */
AST_T* holyc_parser_parse_variable(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* ast_variable = init_ast_with_line(AST_VARIABLE, holyc_parser->holyc_lexer->line_n);
    ast_variable->scope = (struct holyc_scope_T*) scope;
    ast_variable->variable_name = calloc(strlen(holyc_parser->prev_token->value) + 1, sizeof(char));
    strcpy(ast_variable->variable_name, holyc_parser->prev_token->value);

    if (holyc_parser->current_token->type == TOKEN_EQUALS)
    {
        holyc_parser_eat(holyc_parser, TOKEN_EQUALS);
        AST_T* ast_assign = init_ast_with_line(AST_VARIABLE_ASSIGNMENT, holyc_parser->holyc_lexer->line_n);
        ast_assign->variable_assignment_left = ast_variable;
        ast_assign->variable_value = holyc_parser_parse_expr(holyc_parser, scope);
        ast_assign->scope = (struct holyc_scope_T*) scope;

        return ast_assign;
    }

    if (
        holyc_parser->current_token->type == TOKEN_PLUS_EQUALS ||
        holyc_parser->current_token->type == TOKEN_MINUS_EQUALS ||
        holyc_parser->current_token->type == TOKEN_STAR_EQUALS
    )
    {
        token_T* operator = token_copy(holyc_parser->current_token);

        holyc_parser_eat(holyc_parser, operator->type);
        AST_T* ast_variable_modifier = init_ast_with_line(AST_VARIABLE_MODIFIER, holyc_parser->holyc_lexer->line_n);
        ast_variable_modifier->binop_left = ast_variable;
        ast_variable_modifier->binop_right = holyc_parser_parse_expr(holyc_parser, scope);
        ast_variable_modifier->binop_operator = operator;
        ast_variable_modifier->scope = (struct holyc_scope_T*) scope;

        return ast_variable_modifier;
    }

    return ast_variable;
}

/**
 * Parses an object with its children.
 */
AST_T* holyc_parser_parse_object(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* ast_object = init_ast_with_line(AST_OBJECT, holyc_parser->holyc_lexer->line_n);
    ast_object->scope = (struct holyc_scope_T*) scope;
    ast_object->object_children = init_dynamic_list(sizeof(struct AST_STRUCT));
    holyc_scope_T* new_scope = init_holyc_scope(0);

    if (scope)
        if (scope->owner)
            new_scope->owner = scope->owner;

    holyc_parser_eat(holyc_parser, TOKEN_LBRACE);

    if (holyc_parser->current_token->type != TOKEN_RBRACE)
    {
        /**
         * Parsing object children
         */

        if (holyc_parser->current_token->type == TOKEN_ID)
            dynamic_list_append(ast_object->object_children, as_object_child(holyc_parser_parse_function_definition(holyc_parser, new_scope), ast_object));

        while (holyc_parser->current_token->type == TOKEN_SEMI)
        {
            holyc_parser_eat(holyc_parser, TOKEN_SEMI);

            if (holyc_parser->current_token->type == TOKEN_ID)
                dynamic_list_append(ast_object->object_children, as_object_child(holyc_parser_parse_function_definition(holyc_parser, new_scope), ast_object));
        }
    }
    
    holyc_parser_eat(holyc_parser, TOKEN_RBRACE);

    return ast_object;
}

/**
 * Parses an enum with variables as children.
 */
AST_T* holyc_parser_parse_enum(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* ast_enum = init_ast_with_line(AST_ENUM, holyc_parser->holyc_lexer->line_n);
    ast_enum->scope = (struct holyc_scope_T*) scope;
    ast_enum->enum_children = init_dynamic_list(sizeof(struct AST_STRUCT));
    holyc_scope_T* new_scope = init_holyc_scope(0);

    if (scope)
        if (scope->owner)
            new_scope->owner = scope->owner;

    holyc_parser_eat(holyc_parser, TOKEN_LBRACE);

    if (holyc_parser->current_token->type != TOKEN_RBRACE)
    {
        if (holyc_parser->current_token->type == TOKEN_ID)
        {
            holyc_parser_eat(holyc_parser, TOKEN_ID);
            dynamic_list_append(ast_enum->enum_children, holyc_parser_parse_variable(holyc_parser, new_scope));
        }

        while (holyc_parser->current_token->type == TOKEN_COMMA)
        {
            holyc_parser_eat(holyc_parser, TOKEN_COMMA);

            if (holyc_parser->current_token->type == TOKEN_ID)
            {
                holyc_parser_eat(holyc_parser, TOKEN_ID);
                dynamic_list_append(ast_enum->enum_children, holyc_parser_parse_variable(holyc_parser, new_scope));
            }
        }
    }
    
    holyc_parser_eat(holyc_parser, TOKEN_RBRACE);

    return ast_enum;
}

/**
 * Parses a list with its chilren.
 */
AST_T* holyc_parser_parse_list(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    holyc_parser_eat(holyc_parser, TOKEN_LBRACKET);
    AST_T* ast_list = init_ast_with_line(AST_LIST, holyc_parser->holyc_lexer->line_n);
    ast_list->scope = (struct holyc_scope_T*) scope;
    ast_list->list_children = init_dynamic_list(sizeof(struct AST_STRUCT));

    if (holyc_parser->current_token->type != TOKEN_RBRACKET)
        dynamic_list_append(ast_list->list_children, holyc_parser_parse_expr(holyc_parser, scope));

    /**
     * Parsing list items
     */
    while (holyc_parser->current_token->type == TOKEN_COMMA)
    {
        holyc_parser_eat(holyc_parser, TOKEN_COMMA);
        dynamic_list_append(ast_list->list_children, holyc_parser_parse_expr(holyc_parser, scope));
    }
    
    holyc_parser_eat(holyc_parser, TOKEN_RBRACKET);

    return ast_list;
}

// math

/**
 * Parses a factor.
 */
AST_T* holyc_parser_parse_factor(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    /**
     * First we're checking for unary operations.
     */
    while (
        holyc_parser->current_token->type == TOKEN_PLUS ||
        holyc_parser->current_token->type == TOKEN_MINUS
    )
    {
        token_T* unop_operator = token_copy(holyc_parser->current_token);
        holyc_parser_eat(holyc_parser, unop_operator->type);

        AST_T* ast_unop = init_ast_with_line(AST_UNOP, holyc_parser->holyc_lexer->line_n);
        ast_unop->scope = (struct holyc_scope_T*) scope;

        ast_unop->unop_operator = unop_operator;
        ast_unop->unop_right = holyc_parser_parse_term(holyc_parser, scope);

        return ast_unop;
    }

    if (strcmp(holyc_parser->current_token->value, VALUE_FALSE) == 0 || strcmp(holyc_parser->current_token->value, VALUE_TRUE) == 0)
        return holyc_parser_parse_boolean(holyc_parser, scope);

    if (strcmp(holyc_parser->current_token->value, VALUE_NULL) == 0)
        return holyc_parser_parse_null(holyc_parser, scope);

    if (strcmp(holyc_parser->current_token->value, STATEMENT_NEW) == 0)
        return holyc_parser_parse_new(holyc_parser, scope);

    if (holyc_parser->current_token->type == TOKEN_ID)
    {
        holyc_parser_eat(holyc_parser, TOKEN_ID);

        /**
         * Since no other evaluations were true, we are assuming it's
         * a variable.
         */
        AST_T* a = holyc_parser_parse_variable(holyc_parser, scope);

        if (holyc_parser->current_token->type == TOKEN_DOT)
        {
            holyc_parser_eat(holyc_parser, TOKEN_DOT);
            AST_T* ast = init_ast_with_line(AST_ATTRIBUTE_ACCESS, holyc_parser->holyc_lexer->line_n);
            ast->binop_left = a;
            ast->binop_right = holyc_parser_parse_factor(holyc_parser, scope);

            a = ast;
        }

        while (holyc_parser->current_token->type == TOKEN_LBRACKET)
        {
            AST_T* ast_list_access = init_ast_with_line(AST_LIST_ACCESS, holyc_parser->holyc_lexer->line_n);
            ast_list_access->binop_left = a;
            holyc_parser_eat(holyc_parser, TOKEN_LBRACKET);
            ast_list_access->list_access_pointer = holyc_parser_parse_expr(holyc_parser, scope);
            holyc_parser_eat(holyc_parser, TOKEN_RBRACKET);

            a = ast_list_access;
        }

        while(holyc_parser->current_token->type == TOKEN_LPAREN)
            a = holyc_parser_parse_function_call(holyc_parser, scope, a);

        if (a)
            return a;
    }

    /**
     * This is to be able to do things like:
     *  1 * (5 + 5) * (5 * 1) + 3
     */
    if (holyc_parser->current_token->type == TOKEN_LPAREN)
    {
        holyc_parser_eat(holyc_parser, TOKEN_LPAREN);
        AST_T* ast_expr = holyc_parser_parse_expr(holyc_parser, scope);
        holyc_parser_eat(holyc_parser, TOKEN_RPAREN);

        return ast_expr;
    } 

    switch (holyc_parser->current_token->type)
    {
        case TOKEN_NUMBER_VALUE: return holyc_parser_parse_integer(holyc_parser, scope); break;
        case TOKEN_INTEGER_VALUE: return holyc_parser_parse_integer(holyc_parser, scope); break;
        case TOKEN_FLOAT_VALUE: return holyc_parser_parse_float(holyc_parser, scope); break;
        case TOKEN_STRING_VALUE: return holyc_parser_parse_string(holyc_parser, scope); break;
        case TOKEN_CHAR_VALUE: return holyc_parser_parse_char(holyc_parser, scope); break;
        case TOKEN_LBRACE: return holyc_parser_parse_object(holyc_parser, scope); break;
        case TOKEN_LBRACKET: return holyc_parser_parse_list(holyc_parser, scope); break;
        default: printf("Unexpected %s\n", holyc_parser->current_token->value); exit(1); break;
    }
}

/**
 * Parses a term.
 */
AST_T* holyc_parser_parse_term(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    char* token_value = holyc_parser->current_token->value;

    if (is_data_type(token_value) || is_data_type_modifier(token_value)) // this is to be able to have variable definitions inside of function definition parantheses.
        return holyc_parser_parse_function_definition(holyc_parser, scope);

    AST_T* node = holyc_parser_parse_factor(holyc_parser, scope);
    AST_T* ast_binop = (void*) 0;

    if (holyc_parser->current_token->type == TOKEN_LPAREN)
        node = holyc_parser_parse_function_call(holyc_parser, scope, node);

    while (
        holyc_parser->current_token->type == TOKEN_DIV ||        
        holyc_parser->current_token->type == TOKEN_STAR
    )
    {
        token_T* binop_operator = token_copy(holyc_parser->current_token);
        holyc_parser_eat(holyc_parser, binop_operator->type);

        ast_binop = init_ast_with_line(AST_BINOP, holyc_parser->holyc_lexer->line_n);
        
        ast_binop->binop_left = node;
        ast_binop->binop_operator = binop_operator;
        ast_binop->binop_right = holyc_parser_parse_factor(holyc_parser, scope);

        node = ast_binop;
    }
    
    return node;
}

/**
 * Parses an expression
 */
AST_T* holyc_parser_parse_expression(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* node = holyc_parser_parse_term(holyc_parser, scope);
    AST_T* ast_binop = (void*) 0;

    while (
        holyc_parser->current_token->type == TOKEN_PLUS ||
        holyc_parser->current_token->type == TOKEN_MINUS ||
        holyc_parser->current_token->type == TOKEN_EQUALS_EQUALS ||
        holyc_parser->current_token->type == TOKEN_NOT_EQUALS
    )
    {
        token_T* binop_operator = token_copy(holyc_parser->current_token);
        holyc_parser_eat(holyc_parser, binop_operator->type);

        ast_binop = init_ast_with_line(AST_BINOP, holyc_parser->holyc_lexer->line_n);
        ast_binop->scope = (struct holyc_scope_T*) scope;

        ast_binop->binop_left = node;
        ast_binop->binop_operator = binop_operator;
        ast_binop->binop_right = holyc_parser_parse_term(holyc_parser, scope);

        node = ast_binop;
    }

    while (
        holyc_parser->current_token->type == TOKEN_LESS_THAN ||
        holyc_parser->current_token->type == TOKEN_LARGER_THAN
    )
    {
        token_T* binop_operator = token_copy(holyc_parser->current_token);
        holyc_parser_eat(holyc_parser, binop_operator->type);

        ast_binop = init_ast_with_line(AST_BINOP, holyc_parser->holyc_lexer->line_n);
        ast_binop->scope = (struct holyc_scope_T*) scope;

        ast_binop->binop_left = node;
        ast_binop->binop_operator = binop_operator;
        ast_binop->binop_right = holyc_parser_parse_expression(holyc_parser, scope);

        node = ast_binop;
    }



    return node;
}


AST_T* holyc_parser_parse_expr(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{

    AST_T* node = holyc_parser_parse_expression(holyc_parser, scope);
    AST_T* ast_binop = (void*) 0;

    while (holyc_parser->current_token->type == TOKEN_AND)
    {
        token_T* binop_operator = token_copy(holyc_parser->current_token);
        holyc_parser_eat(holyc_parser, binop_operator->type);

        ast_binop = init_ast_with_line(AST_BINOP, holyc_parser->holyc_lexer->line_n);
        ast_binop->scope = (struct holyc_scope_T*) scope;

        ast_binop->binop_left = node;
        ast_binop->binop_operator = binop_operator;
        ast_binop->binop_right = holyc_parser_parse_expression(holyc_parser, scope);

        node = ast_binop;
    }

    if (holyc_parser->current_token->type == TOKEN_QUESTION){
        return holyc_parser_parse_ternary(holyc_parser, scope, node);
    }

    return node;
}

// statements

/**
 * Parses a break statement.
 */
AST_T* holyc_parser_parse_break(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    holyc_parser_eat(holyc_parser, TOKEN_ID);  // break token

    return init_ast_with_line(AST_BREAK, holyc_parser->holyc_lexer->line_n);
}

/**
 * Parses a continue statement.
 */
AST_T* holyc_parser_parse_continue(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    holyc_parser_eat(holyc_parser, TOKEN_ID);  // continue token

    return init_ast_with_line(AST_CONTINUE, holyc_parser->holyc_lexer->line_n);
}

/**
 * Parses a return statement.
 */
AST_T* holyc_parser_parse_return(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    holyc_parser_eat(holyc_parser, TOKEN_ID);
    AST_T* ast_return = init_ast_with_line(AST_RETURN, holyc_parser->holyc_lexer->line_n);
    ast_return->scope = (struct holyc_scope_T*) scope;

    if (holyc_parser->current_token->type != TOKEN_SEMI)
        ast_return->return_value = holyc_parser_parse_expr(holyc_parser, scope);

    return ast_return;
}

/**
 * Parses an if statement. With or without braces.
 */
AST_T* holyc_parser_parse_if(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* ast_if = init_ast_with_line(AST_IF, holyc_parser->holyc_lexer->line_n);
    
    holyc_parser_eat(holyc_parser, TOKEN_ID); // IF token

    holyc_parser_eat(holyc_parser, TOKEN_LPAREN);
    ast_if->if_expr = holyc_parser_parse_expr(holyc_parser, scope);
    holyc_parser_eat(holyc_parser, TOKEN_RPAREN);
    
    ast_if->scope = (struct holyc_scope_T*) scope;

    if (holyc_parser->current_token->type == TOKEN_LBRACE)
    {
        holyc_parser_eat(holyc_parser, TOKEN_LBRACE);
        ast_if->if_body = holyc_parser_parse_statements(holyc_parser, scope);
        holyc_parser_eat(holyc_parser, TOKEN_RBRACE);
    }
    else // accept if-statement without braces. (will only parse one statement)
    {
        ast_if->if_body = holyc_parser_parse_compound_with_one_statement(holyc_parser, scope);
    }

    if (strcmp(holyc_parser->current_token->value, STATEMENT_ELSE) == 0)
    {
        holyc_parser_eat(holyc_parser, TOKEN_ID); // ELSE token

        if (strcmp(holyc_parser->current_token->value, STATEMENT_IF) == 0)
        {
            ast_if->if_otherwise = holyc_parser_parse_if(holyc_parser, scope);
            ast_if->if_otherwise->scope = (struct holyc_scope_T*) scope;
        }
        else
        {

            if (holyc_parser->current_token->type == TOKEN_LBRACE)
            {
                holyc_parser_eat(holyc_parser, TOKEN_LBRACE);
                ast_if->else_body = holyc_parser_parse_statements(holyc_parser, scope);
                ast_if->else_body->scope = (struct holyc_scope_T*) scope;
                holyc_parser_eat(holyc_parser, TOKEN_RBRACE);
            }
            else // parse else without braces. (only parses one statement)
            {
                AST_T* compound = init_ast_with_line(AST_COMPOUND, holyc_parser->holyc_lexer->line_n);
                compound->scope = (struct holyc_scope_T*) scope;
                AST_T* statement = holyc_parser_parse_statement(holyc_parser, scope);
                holyc_parser_eat(holyc_parser, TOKEN_SEMI);
                dynamic_list_append(compound->compound_value, statement);

                ast_if->else_body = compound;
                ast_if->else_body->scope = (struct holyc_scope_T*) scope;
            }
        }
    }

    return ast_if;
}

/**
 * Parses a ternary. Example:
 *  x > 2 ? 5 : 3
 */
AST_T* holyc_parser_parse_ternary(holyc_parser_T* holyc_parser, holyc_scope_T* scope, AST_T* expr)
{
    AST_T* ternary = init_ast_with_line(AST_TERNARY, holyc_parser->holyc_lexer->line_n);
    ternary->ternary_expr = expr;

    holyc_parser_eat(holyc_parser, TOKEN_QUESTION);

    ternary->ternary_body = holyc_parser_parse_term(holyc_parser, scope);
    
    holyc_parser_eat(holyc_parser, TOKEN_COLON);
    
    ternary->ternary_else_body = holyc_parser_parse_term(holyc_parser, scope);

    return ternary;
}

/**
 * Parses a `new` statement.
 */
AST_T* holyc_parser_parse_new(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    holyc_parser_eat(holyc_parser, TOKEN_ID);
    AST_T* ast_new = init_ast_with_line(AST_NEW, holyc_parser->holyc_lexer->line_n);
    ast_new->new_value = holyc_parser_parse_expr(holyc_parser, scope);

    return ast_new;
}

/**
 * Parses an iterate statement with a function definition or a variable
 * as iterator.
 */
AST_T* holyc_parser_parse_iterate(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    holyc_parser_eat(holyc_parser, TOKEN_ID); // iterate
    AST_T* ast_var = holyc_parser_parse_expr(holyc_parser, scope); // variable
    holyc_parser_eat(holyc_parser, TOKEN_ID); // with
    

    AST_T* ast_fname = (void*)0;

    if (is_data_type(holyc_parser->current_token->value) || is_data_type_modifier(holyc_parser->current_token->value))
    {
        /**
         * Here we make the assumption that a private function is being
         * defined instead of passing varaible to the iterate_function
         */
        ast_fname = holyc_parser_parse_function_definition(holyc_parser, scope);
    }
    else
    {
        holyc_parser_eat(holyc_parser, TOKEN_ID);
        ast_fname = holyc_parser_parse_variable(holyc_parser, scope); // function name
    }

    AST_T* ast_iterate = init_ast_with_line(AST_ITERATE, holyc_parser->holyc_lexer->line_n);
    ast_iterate->iterate_iterable = ast_var;
    ast_iterate->iterate_function = ast_fname;

    return ast_iterate;
}

/**
 * Parses an assert statement with its right-hand expression.
 */
AST_T* holyc_parser_parse_assert(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    holyc_parser_eat(holyc_parser, TOKEN_ID);
    AST_T* ast_assert = init_ast_with_line(AST_ASSERT, holyc_parser->holyc_lexer->line_n);
    ast_assert->assert_expr = holyc_parser_parse_expr(holyc_parser, scope);

    return ast_assert;
}

// loops

/**
 * Parses a while loop statement with its body and boolean expression.
 */
AST_T* holyc_parser_parse_while(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    holyc_parser_eat(holyc_parser, TOKEN_ID);
    holyc_parser_eat(holyc_parser, TOKEN_LPAREN);
    AST_T* ast_while = init_ast_with_line(AST_WHILE, holyc_parser->holyc_lexer->line_n);
    ast_while->while_expr = holyc_parser_parse_expr(holyc_parser, scope);  // boolean expression
    holyc_parser_eat(holyc_parser, TOKEN_RPAREN);

    if (holyc_parser->current_token->type == TOKEN_LBRACE)
    {
        holyc_parser_eat(holyc_parser, TOKEN_LBRACE);
        ast_while->while_body = holyc_parser_parse_statements(holyc_parser, scope);
        holyc_parser_eat(holyc_parser, TOKEN_RBRACE);
        ast_while->scope = (struct holyc_scope_T*) scope;
    }
    else
    {
        ast_while->while_body = holyc_parser_parse_compound_with_one_statement(holyc_parser, scope);
        ast_while->while_body->scope = (struct holyc_scope_T*) scope;
    }

    return ast_while;
}

/**
 * Parses a for loop.
 */
AST_T* holyc_parser_parse_for(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* ast_for = init_ast(AST_FOR);

    holyc_parser_eat(holyc_parser, TOKEN_ID); // for
    holyc_parser_eat(holyc_parser, TOKEN_LPAREN);

    // init statement
    ast_for->for_init_statement = holyc_parser_parse_statement(holyc_parser, scope);
    holyc_parser_eat(holyc_parser, TOKEN_SEMI);

    // test expression
    ast_for->for_test_expr = holyc_parser_parse_expr(holyc_parser, scope);
    holyc_parser_eat(holyc_parser, TOKEN_SEMI);

    // update statement
    ast_for->for_update_statement = holyc_parser_parse_statement(holyc_parser, scope);

    holyc_parser_eat(holyc_parser, TOKEN_RPAREN);
    
    if (holyc_parser->current_token->type == TOKEN_LBRACE)
    {
        holyc_parser_eat(holyc_parser, TOKEN_LBRACE);
        ast_for->for_body = holyc_parser_parse_statements(holyc_parser, scope);
        ast_for->for_body->scope = (struct holyc_scope_T*) scope;
        holyc_parser_eat(holyc_parser, TOKEN_RBRACE);
    }
    else
    {
        /**
         * Parse for loop without braces.
         */

        ast_for->for_body = holyc_parser_parse_compound_with_one_statement(holyc_parser, scope);
        ast_for->for_body->scope = (struct holyc_scope_T*) scope;
    }

    return ast_for;
}

// functions

/**
 * Parses a function call
 */
AST_T* holyc_parser_parse_function_call(holyc_parser_T* holyc_parser, holyc_scope_T* scope, AST_T* expr)
{
    AST_T* ast_function_call = init_ast_with_line(AST_FUNCTION_CALL, holyc_parser->holyc_lexer->line_n);
    ast_function_call->function_call_expr = expr;
    holyc_parser_eat(holyc_parser, TOKEN_LPAREN);
    ast_function_call->scope = (struct holyc_scope_T*) scope;

    if (holyc_parser->current_token->type != TOKEN_RPAREN)
    {
        AST_T* ast_expr = holyc_parser_parse_expr(holyc_parser, scope);

        if (ast_expr->type == AST_FUNCTION_DEFINITION)
            ast_expr->scope = (struct holyc_scope_T*) init_holyc_scope(0);

        dynamic_list_append(ast_function_call->function_call_arguments, ast_expr);

        /**
         * Parsing function call arguments
         */
        while (holyc_parser->current_token->type == TOKEN_COMMA)
        {
            holyc_parser_eat(holyc_parser, TOKEN_COMMA);
            ast_expr = holyc_parser_parse_expr(holyc_parser, scope);
            
            if (ast_expr->type == AST_FUNCTION_DEFINITION)
                ast_expr->scope = (struct holyc_scope_T*) init_holyc_scope(0);

            dynamic_list_append(ast_function_call->function_call_arguments, ast_expr);
        }
    }

    holyc_parser_eat(holyc_parser, TOKEN_RPAREN);

    return ast_function_call;
}

/**
 * Parses a function_definition OR a variable_definition depending on
 * if it encounters parantheses or not.
 *
 * @param holyc_parser_T* holyc_parser
 * @param holyc_scope_T* scope
 *
 * @return AST_T*
 */
AST_T* holyc_parser_parse_function_definition(holyc_parser_T* holyc_parser, holyc_scope_T* scope)
{
    AST_T* ast_type = holyc_parser_parse_type(holyc_parser, scope);

    holyc_parser->data_type = ast_type->type_value;

    char* function_name = (void*)0;
    unsigned int is_enum = 0;

    if (ast_type->type_value->type != DATA_TYPE_ENUM)
    {
        function_name = calloc(strlen(holyc_parser->current_token->value) + 1, sizeof(char));
        strcpy(function_name, holyc_parser->current_token->value);

        if (holyc_parser->current_token->type == TOKEN_ID)
            holyc_parser_eat(holyc_parser, TOKEN_ID);
        else
            holyc_parser_eat(holyc_parser, TOKEN_ANON_ID);
    }
    else
    {
        is_enum = 1;
    }

    if (holyc_parser->current_token->type == TOKEN_LPAREN)
    {
        /**
         * Here we make the assumption that a function definition is to be
         * parsed, because we encountered a left parentheses.
         */

        AST_T* ast_function_definition = init_ast_with_line(AST_FUNCTION_DEFINITION, holyc_parser->holyc_lexer->line_n);
        holyc_scope_T* new_scope = init_holyc_scope(0);
        new_scope->owner = ast_function_definition;

        ast_function_definition->function_name = function_name;
        ast_function_definition->function_definition_type = ast_type;
        ast_function_definition->function_definition_arguments = init_dynamic_list(sizeof(struct AST_STRUCT));

        holyc_parser_eat(holyc_parser, TOKEN_LPAREN);
        
        /**
         * Parsing function definition arguments.
         */
        if (holyc_parser->current_token->type != TOKEN_RPAREN)
        {
            dynamic_list_append(ast_function_definition->function_definition_arguments, holyc_parser_parse_expr(holyc_parser, scope));

            while (holyc_parser->current_token->type == TOKEN_COMMA)
            {
                holyc_parser_eat(holyc_parser, TOKEN_COMMA);
                dynamic_list_append(ast_function_definition->function_definition_arguments, holyc_parser_parse_expr(holyc_parser, scope));
            }
        } 

        holyc_parser_eat(holyc_parser, TOKEN_RPAREN);

        if (holyc_parser->current_token->type == TOKEN_EQUALS)
        {
            /**
             * Parsing compositions
             */

            holyc_parser_eat(holyc_parser, TOKEN_EQUALS);

            AST_T* child_def = (void*)0;

            if (is_data_type(holyc_parser->current_token->value) || is_data_type_modifier(holyc_parser->current_token->value))
            {
                child_def = holyc_parser_parse_function_definition(holyc_parser, scope);
            }
            else
            {
                holyc_parser_eat(holyc_parser, TOKEN_ID);
                child_def = holyc_parser_parse_variable(holyc_parser, scope);
            }

            child_def->scope = (struct holyc_scope_T*) new_scope;
            dynamic_list_append(ast_function_definition->composition_children, child_def);

            /**
             * Parse child functions of composition.
             */
            while (holyc_parser->current_token->type == TOKEN_COMMA)
            {
                holyc_parser_eat(holyc_parser, TOKEN_COMMA);
                
                if (is_data_type(holyc_parser->current_token->value) || is_data_type_modifier(holyc_parser->current_token->value))
                {
                    child_def = holyc_parser_parse_function_definition(holyc_parser, scope);
                }
                else
                {
                    holyc_parser_eat(holyc_parser, TOKEN_ID);
                    child_def = holyc_parser_parse_variable(holyc_parser, scope);
                }

                child_def->scope = (struct holyc_scope_T*) new_scope;
                dynamic_list_append(ast_function_definition->composition_children, child_def);
            }

            return ast_function_definition;
        }

        holyc_parser_eat(holyc_parser, TOKEN_LBRACE);
        ast_function_definition->function_definition_body = holyc_parser_parse_statements(holyc_parser, new_scope);
        ast_function_definition->function_definition_body->scope = (struct holyc_scope_T*) new_scope;
        holyc_parser_eat(holyc_parser, TOKEN_RBRACE);

        return ast_function_definition;
    }
    else
    {
        /**
         * Here we make the assumption that we are to parse
         * a variable_definition since we did not encounter a left parantheses.
         */

        AST_T* ast_variable_definition = init_ast_with_line(AST_VARIABLE_DEFINITION, holyc_parser->holyc_lexer->line_n);
        ast_variable_definition->scope = (struct holyc_scope_T*) scope;
        ast_variable_definition->variable_name = function_name;
        ast_variable_definition->variable_type = ast_type;

        // Special case
        if (is_enum)
        {
            ast_variable_definition->variable_value = holyc_parser_parse_enum(holyc_parser, scope);
            ast_variable_definition->variable_name = calloc(strlen(holyc_parser->current_token->value) + 1, sizeof(char));
            strcpy(ast_variable_definition->variable_name, holyc_parser->current_token->value);
            holyc_parser_eat(holyc_parser, TOKEN_ID);
        }
        
        if (holyc_parser->current_token->type == TOKEN_EQUALS)
        {
            if (is_enum)
                holyc_parser_syntax_error(holyc_parser);

            holyc_parser_eat(holyc_parser, TOKEN_EQUALS);

            ast_variable_definition->variable_value = holyc_parser_parse_expr(holyc_parser, scope);

            /**
             * Performing all the kinds of type-checks we can possibly cover,
             * this is only for reducing type-errors, we cannot possibly cover
             * everything here since the value might be kind of unknown, but
             * this is better than nothing.
             */
            switch(ast_variable_definition->variable_value->type)
            {
                case AST_OBJECT: if (ast_type->type_value->type != DATA_TYPE_OBJECT) holyc_parser_type_error(holyc_parser); break;
                case AST_ENUM: if (ast_type->type_value->type != DATA_TYPE_ENUM) holyc_parser_type_error(holyc_parser); break;
                case AST_STRING: if (ast_type->type_value->type != DATA_TYPE_STRING) holyc_parser_type_error(holyc_parser); break;
                case AST_INTEGER: if (ast_type->type_value->type != DATA_TYPE_INT) holyc_parser_type_error(holyc_parser); break;
                case AST_FLOAT: if (ast_type->type_value->type != DATA_TYPE_FLOAT) holyc_parser_type_error(holyc_parser); break;
                case AST_BOOLEAN: if (ast_type->type_value->type != DATA_TYPE_BOOLEAN) holyc_parser_type_error(holyc_parser); break;
                case AST_LIST: if (ast_type->type_value->type != DATA_TYPE_LIST) holyc_parser_type_error(holyc_parser); break;
                case AST_CHAR: if (ast_type->type_value->type != DATA_TYPE_CHAR) holyc_parser_type_error(holyc_parser); break;
                case AST_COMPOUND: if (ast_type->type_value->type != DATA_TYPE_SOURCE) holyc_parser_type_error(holyc_parser); break;
                default: /* silence */; break;
            }
        }

        return ast_variable_definition;
    }
}
