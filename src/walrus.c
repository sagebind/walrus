#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "tokens.h"
#include "scanner.h"
#include "walrus.h"


/**
 * Main entry point for the application. Parses options and invokes the compiler.
 */
int main(int argc, char* const* argv)
{
    // parse the arguments into options
    Options options = parse_options(argc, argv);

    if (options.help) {
        printf("Usage: walrus [options] file1 [file2 [...]]\r\n"
               "A lightweight compiler for the Decaf programming language\r\n"
               "Options:\r\n\r\n"
               "  --help                   Displays this help message, but you already knew that\r\n"
               "  -s                       Scan only; do not parse or compile\r\n"
               "  -T, --print-tokens       Print out tokens as they are scanned\r\n\r\n");
        return 0;
    }

    if (options.files_count < 1) {
        error_exit(E_NO_INPUT_FILES, "No input files given.");
    }

    // run the compiler and return the error code
    return walrus_compile(options);
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
            char message[1024];
            sprintf(message, "The file '%s' could not be opened.\n", options.files[i]);
            error_exit(E_FILE_NOT_FOUND, message);
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
    Options options = {0, 0, 0, 0, NULL};

    // define our getopt specs
    const char* short_options = "sT";
    static struct option long_options[] = {
        {"help",         no_argument, 0, 0},
        {"print-tokens", no_argument, 0, 'T'},
        {0, 0, 0, 0}
    };
    int option_index = 0;

    // parse arguments using getopt and set corresponding flags
    int c;
    opterr = 0;
    while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        switch (c) {
            case 0:
                if (long_options[option_index].name == "help") {
                    options.help = true;
                } else if (long_options[option_index].name == "print-tokens") {
                    options.print_tokens = true;
                }
                break;
            case 's':
                options.scan_only = true;
                break;
            case 'T':
                options.print_tokens = true;
                break;
            case '?':
                if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                break;
            default:
                abort();
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
