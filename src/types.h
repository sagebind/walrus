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

/**
 * Gets a string name for a data type.
 *
 * @param  type The data type in question.
 * @return      A string naming the given type.
 */
char* data_type_string(DataType type);

#endif
