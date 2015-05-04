#ifndef WALRUS_TYPES_H
#define WALRUS_TYPES_H


/**
 * All data types allowed in Decaf.
 */
typedef enum {
    TYPE_NONE, // <- indicates unknown type
    TYPE_INT,
    TYPE_BOOLEAN,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_VOID
} DataType;

#endif
