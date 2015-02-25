#ifndef WALRUS_MAIN_H
#define WALRUS_MAIN_H

#include <stdbool.h>

/**
 * Struct that stores values for all available command options for Walrus.
 */
typedef struct {
    bool help;
    bool scan_only;
    bool print_tokens;
    int files_count;
    char** files;
} Options;

/**
 * Scans a file only.
 *
 * @param filename The name of the file to scan.
 * @param options  Compiler flags and options.
 */
void walrus_scan(char* filename, Options options);

/**
 * Parses command-line arguments and returns an options struct containing the meaning of the arguments.
 *
 * @param  argc The number of arguments.
 * @param  argv The argument strings.
 * @return      An options struct.
 */
Options parse_options(int argc, char* const* argv);

/**
 * Main entry point for the application.
 *
 * @param  argc The number of arguments.
 * @param  argv The argument strings.
 * @return      The exit status.
 */
int main(int argc, char* const* argv);

#endif
