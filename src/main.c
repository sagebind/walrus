#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

/**
 * Parses command-line arguments and returns an options struct containing the meaning of the arguments.
 *
 * @param  argc The number of arguments.
 * @param  argv The argument strings.
 * @return      An options struct.
 */
struct options parse_options(int argc, char* const* argv)
{
    int c;
    struct options options = {0, 0, 0, 0, NULL};

    // parse arguments using getopt and set corresponding tags flags
    opterr = 0;
    while ((c = getopt(argc, argv, "hsT")) != -1) {
        switch (c) {
            case 'h':
                options.help = 1;
                break;
            case 's':
                options.scan_only = 1;
                break;
            case 'T':
                options.print_tokens = 1;
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
        options.files[i] = (char*)malloc(sizeof(char) * strlen(argv[optind + i]));
        strcpy(options.files[i], argv[optind + i]);
    }

    return options;
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
    const char* helpText = "Usage: walrus [options] file1 [file2 [...]]\r\n"
                           "A lightweight compiler for the Decaf programming language\r\n"
                           "Options:\r\n\r\n"
                           "  -h\t\t\tDisplays this help message\r\n"
                           "  -s\t\t\tScan only; do not parse or assemble\r\n"
                           "  -T\t\t\tPrint out tokens as they are scanned\r\n\r\n";
    struct options options = parse_options(argc, argv);

    if (options.help) {
        printf("%s", helpText);
        return 0;
    }

    if (options.print_tokens) {
        printf("Will print tokens.\r\n");
    }

    return 0;
}
