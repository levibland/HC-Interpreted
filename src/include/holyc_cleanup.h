#ifndef HOLYC_CLEANUP_H
#define HOLYC_CLEANUP_H
#include "holyc_lexer.h"
#include "holyc_parser.h"
#include "holyc_runtime.h"


void holyc_cleanup(
    holyc_lexer_T* holyc_lexer,
    holyc_parser_T* parser,
    runtime_T* runtime,
    AST_T* node
);
#endif
