#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "scanner.h"
#include "tokens.h"
#include "walrus.h"


/**
 * Main entry point for the application. Parses options and invokes the compiler.
 */
int main(int argc, char* const* argv)
{
    // parse the arguments into options
    Options options = parse_options(argc, argv);

    // the user needs a little help
    if (options.help) {
        printf("A lightweight compiler for the Decaf programming language.\r\n\r\n"
               "Usage: walrus [options] file1 [file2 [...]]\r\n"
               "Options:\r\n\r\n"
               "  --help                   Displays this help message, but you already knew that\r\n"
               "  --debug                  Outputs debugging information\r\n"
               "  -s                       Scan only; do not parse or compile\r\n"
               "  -T, --print-tokens       Print out tokens as they are scanned\r\n\r\n"
               "This walrus knows how to avoid boredom.\r\n\r\n");
        return 0;
    }

    // i'd tell you what this is, but then i'd have to kill you
    if (options.bored) {
        return main_2048(argc, argv);
    }

    // error if no files are given
    if (options.files_count < 1) {
        error_exit(E_NO_INPUT_FILES, "No input files given.");
    }

    // run the compiler
    walrus_compile(options);

    // exit with the last occurred error code
    return error_get_last();
}

/**
 * Runs the compiler on all given files.
 */
Error walrus_compile(Options options)
{
    // run on all given input files
    for (int i = 0; i < options.files_count; i++) {
        // open the file in a scanner
        ScannerContext* context = scanner_open(options.files[i]);

        // check for errors
        if (context == NULL) {
            error(E_FILE_NOT_FOUND, "The file '%s' could not be opened.", options.files[i]);
            continue;
        }

        // create a lexer for the file
        Lexer* lexer = lexer_create(context);

        // manually scan
        if (options.scan_only) {
            Token token;
            do {
                token = lexer_next(lexer);

                if (options.print_tokens && token.type != T_ILLEGAL && token.type != T_EOF) {
                    lexer_print_token(token);
                }
            } while (token.type != T_EOF);
        }

        // go ahead and parse the input
        else {
            ASTNode* ast = parser_parse(lexer);

            if (options.debug) {
                ast_print(ast);
            }
        }

        // clean up after ourselves
        lexer_destroy(&lexer);
        scanner_close(&context);
    }

    return E_SUCCESS;
}

/**
 * Parses command-line arguments using getopt.
 */
Options parse_options(int argc, char* const* argv)
{
    // create our options struct which contains our flags
    Options options = {0, 0, 0, 0, 0, NULL};

    // define our getopt specs
    const char* short_options = "hdsT";
    static struct option long_options[] = {
        {"help",         no_argument, 0, 'h'},
        {"debug",        no_argument, 0, 'd'},
        {"print-tokens", no_argument, 0, 'T'},
        {"bored",        no_argument, 0, 0},
        {0, 0, 0, 0}
    };
    int option_index = 0;

    // parse arguments using getopt and set corresponding flags
    int c;
    opterr = 0;
    while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        // parse options
        if (c == 'h' || c == 0 && long_options[option_index].name == "help") {
            options.help = true;
        } else if (c == 'd' || c == 0 && long_options[option_index].name == "debug") {
            options.debug = true;
        } else if (c == 'T' || c == 0 && long_options[option_index].name == "print-tokens") {
            options.print_tokens = true;
        } else if (c == 0 && long_options[option_index].name == "bored") {
            options.bored = true;
        } else if (c == 's') {
            options.scan_only = true;
        }

        // unknown option
        else if (isprint(optopt)) {
            error(E_UNKNOWN_OPTION, "Unknown option `-%c'.", optopt);
        } else {
            error(E_UNKNOWN_OPTION, "Unknown option character `\\x%x'.", optopt);
        }
    }

    // allocate space for the files given
    options.files_count = argc - optind;
    options.files = (char**)malloc(sizeof(char*) * options.files_count);

    // add each file parameter to options
    for (int i = 0; i < options.files_count; i++) {
        // copy the pointer, not the string itself
        options.files[i] = argv[optind + i];
    }

    return options;
}
