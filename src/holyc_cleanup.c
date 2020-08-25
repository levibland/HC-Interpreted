#include "include/holyc_cleanup.h"


/**
 * Clean & deallocate holyc
 */
void holyc_cleanup(
    holyc_lexer_T* holyc_lexer,
    holyc_parser_T* parser,
    runtime_T* runtime,
    AST_T* node
)
{
    ast_free(node);

    if (parser->current_token)
        token_free(parser->current_token);

    if (parser->prev_token)
        token_free(parser->prev_token);

    free(parser);

    holyc_lexer_free(holyc_lexer);
    //holyc_scope_free(runtime->scope);
    
    /* ==== Free definitions ==== */

    for (int i = 0; i < runtime->list_methods->size; i++)
    {
        AST_T* fdef = (AST_T*) runtime->list_methods->items[i];

        if (fdef->function_definition_type)
            ast_free(fdef->function_definition_type);

        if (fdef->function_name)
            free(fdef->function_name);

        if (fdef->function_definition_body)
            ast_free(fdef->function_definition_body);

        if (fdef->function_definition_arguments)
        {
            for (int x = 0; x < fdef->function_definition_arguments->size; x++)
            {
                if ((AST_T*)fdef->function_definition_arguments->items[x] != fdef)
                    ast_free((AST_T*)fdef->function_definition_arguments->items[x]);
            }

            free(fdef->function_definition_arguments->items);
            free(fdef->function_definition_arguments);
            fdef->function_definition_arguments = (void*) 0;
        }

        if (fdef->composition_children)
        {
            for (int i = 0; i < fdef->composition_children->size; i++)
            {
                if ((AST_T*)fdef->composition_children->items[i] != fdef)
                    ast_free((AST_T*)fdef->composition_children->items[i]);
            }

            free(fdef->composition_children->items);
            free(fdef->composition_children);
            fdef->composition_children = (void*) 0;
        } 

        free(fdef);
    }

    /* ==== Free runtime ==== */

    free(runtime->list_methods->items);
    free(runtime->list_methods);
    free(runtime);
}
