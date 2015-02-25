#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "lexer.h"
#include "tokens.h"
#include "scanner.h"

/**
 * Scans a file only.
 */
void walrus_scan(char* filename, Options options)
{
    ScannerContext* context = scanner_open(filename);
    int error_count = 0;

    Token token;
    while (token.type != T_EOF) {
        token = lexer_next(context);

        if (token.type == T_ILLEGAL) {
            error_count++;
        } else if (options.print_tokens && token.type != T_EOF) {
            lexer_print_token(token);
        }
    }

    scanner_close(context);
}

/**
 * Main entry point for the application. Parses options and invokes the compiler.
 *
 * @param  argc The number of arguments.
 * @param  argv The argument strings.
 * @return      The exit status.
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
        printf("No input files.\r\n");
        abort();
    }

    if (options.scan_only) {
        walrus_scan(options.files[0], options);
        exit(0);
    }

    return 0;
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
    options.files = (char**)malloc(sizeof(char*));

    // add each file parameter to options
    for (int i = 0; i < options.files_count; i++) {
        options.files[i] = (char*)malloc(strlen(argv[optind + i]) + 1);
        strcpy(options.files[i], argv[optind + i]);
    }

    return options;
}
