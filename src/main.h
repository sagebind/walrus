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
 * Parses command-line arguments and returns an options struct containing the meaning of the arguments.
 *
 * @param  argc The number of arguments.
 * @param  argv The argument strings.
 * @return      An options struct.
 */
Options parse_options(int argc, char* const* argv);

#endif
