#ifndef MAIN_H
#define MAIN_H

struct options {
    int help;
    int scan_only;
    int print_tokens;
    int files_count;
    char** files;
};

struct options parse_options(int argc, char* const* argv);

#endif
