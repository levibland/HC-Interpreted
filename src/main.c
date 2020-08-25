#include "include/holyc_lexer.h"
#include "include/holyc_parser.h"
#include "include/holyc_runtime.h"
#include "include/holyc_cleanup.h"
#include "include/string_utils.h"
#include "include/io.h"
#include <signal.h>


volatile unsigned int interactive;


void int_handler(int sig)
{
    interactive = 0;
}

int main(int argc, char* argv[])
{
    interactive = 0;
    signal(SIGINT, int_handler);

    runtime_T* runtime = init_runtime();
    holyc_lexer_T* lexer = (void*)0;
    holyc_parser_T* parser = (void*)0;
    AST_T* node = (void*)0;

    if (argc < 2)
    {
        interactive = 1;
        printf("---- * Interactive holyc Shell * ----\n");

        while (interactive)
        {
            char* str = holyc_get_stdin(">: ");

            lexer = init_holyc_lexer(str);

            parser = init_holyc_parser(lexer);
            node = holyc_parser_parse(parser, (void*) 0);
            runtime_visit(runtime, node);
        }

        holyc_cleanup(lexer, parser, runtime, node);

        printf("---- * Interactive Shell Terminated. * ----\n");

        return 0;
    }

    lexer = init_holyc_lexer(holyc_read_file(argv[1]));
    parser = init_holyc_parser(lexer);
    node = holyc_parser_parse(parser, (void*) 0);
    runtime_visit(runtime, node);

    holyc_cleanup(lexer, parser, runtime, node);

    return 0;
}
