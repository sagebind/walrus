#ifndef WALRUS_MAIN_H
#define WALRUS_MAIN_H

#include <stdbool.h>
#include "error.h"


/**
 * Struct that stores values for all available command options for Walrus.
 */
typedef struct {
    bool help;
    bool debug;
    bool parse_only;
    bool scan_only;
    bool print_tokens;
    int files_count;
    char** files;
    bool bored;
} Options;


/**
 * Main entry point for the application.
 *
 * @param  argc The number of arguments.
 * @param  argv The argument strings.
 * @return      The exit status.
 */
int main(int argc, char* const* argv);

/**
 * Runs the compiler on all given files.
 *
 * @param  options Compiler flags and options.
 * @return         An error code.
 */
Error walrus_compile(Options options);

/**
 * Parses command-line arguments and returns an options struct containing the meaning of the arguments.
 *
 * @param  argc The number of arguments.
 * @param  argv The argument strings.
 * @return      An options struct.
 */
Options parse_options(int argc, char* const* argv);

// nothing to worry your head about
int main_2048(int argc, char* const* argv);

#endif
