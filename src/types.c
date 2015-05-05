#include "types.h"


/**
 * Gets a string name for a data type.
 */
char* data_type_string(DataType type)
{
    if (type == TYPE_BOOLEAN) {
        return "boolean";
    }

    if (type == TYPE_INT) {
        return "int";
    }

    if (type == TYPE_CHAR) {
        return "char";
    }

    if (type == TYPE_STRING) {
        return "string";
    }

    return "void";
}
