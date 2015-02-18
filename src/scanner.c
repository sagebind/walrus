#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"

/**
 * Opens a file in read-only mode and creates a scanner context for it.
 */
ScannerContext* scanner_open(char* filename)
{
    // open the given file
    FILE* stream = fopen(filename, "r");
    if (stream == NULL) {
        return NULL;
    }

    // create a context pointer
    ScannerContext* context = (ScannerContext*)malloc(sizeof(ScannerContext));
    context->line = 0;
    context->column = 0;
    context->stream = stream;

    return context;
}

/**
 * Opens a string as a stream and creates a scanner context for it.
 */
ScannerContext* scanner_open_string(char* string)
{
    // create a stream for the char array
    FILE* stream = fmemopen(string, strlen(string), "r");
    if (stream == NULL) {
        return NULL;
    }

    // create a context pointer
    ScannerContext* context = (ScannerContext*)malloc(sizeof(ScannerContext));
    context->line = 0;
    context->column = 0;
    context->stream = stream;

    return context;
}

/**
 * Closes a scanner context and its associated input stream.
 */
Error scanner_close(ScannerContext* context)
{
    // context shouldn't be null
    assert(context != NULL);

    // close the stream handle
    fclose(context->stream);

    // free the pointer
    free(context);

    return E_SUCCESS;
}
