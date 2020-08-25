#ifndef HOLYC_SCOPE_H
#define HOLYC_SCOPE_H
#include "ast.h"
#include "dynamic_list.h"


typedef struct holyc_SCOPE_STRUCT
{
    AST_T* owner;
    dynamic_list_T* variable_definitions;
    dynamic_list_T* function_definitions;
    unsigned int global;
} holyc_scope_T;

holyc_scope_T* init_holyc_scope(unsigned int global);

void holyc_scope_free(holyc_scope_T* holyc_scope);

void holyc_scope_clear_variable_definitions(holyc_scope_T* scope);

void holyc_scope_clear_function_definitions(holyc_scope_T* scope);
#endif
