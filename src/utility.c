#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

/**
 * Checks to see if the input string is a reserved keyword.
 */
bool is_keyword(char* inString)
{
 return((strcmp(inString, "boolean")==0)||(strcmp(inString, "int")==0)||(strcmp(inString, "break")==0)||(strcmp(inString, "continue")==0)||(strcmp(inString, "callout")==0)||(strcmp(inString, "class")==0)||(strcmp(inString, "if")==0)||
+            (strcmp(inString, "else")==0)||(strcmp(inString, "true")==0)||(strcmp(inString, "false")==0)||(strcmp(inString, "for")==0)||(strcmp(inString, "return")==0)||(strcmp(inString, "void")==0)||(strcmp(inString, "Program")==0));
}

/**
 * Creates a token based on the passed in keyword.
 */
Token create_keyword_token(char* in_keyword, ScannerContext* context)
{
    Token token;
    //check in_keyword against list of all keywords here; if a match is found, return the appropriate token
    if(strcmp(in_keyword, "boolean")==0) {
        token = token_create(
            context->line,
	    context->column,
            T_BOOLEAN,
            in_keyword
        );
    } else if (strcmp(in_keyword, "int")==0) {
        token = token_create(
            context->line,
            context->column,
            T_INT,
            in_keyword
        );
    } else if (strcmp(in_keyword, "break")==0) {
        token = token_create(
            context->line,
            context->column,
            T_BOOLEAN,
            in_keyword
        );
    } else if (strcmp(in_keyword, "continue")==0) {
        token = token_create(
            context->line,
            context->column,
            T_CONTINUE,
            in_keyword
        );
    } else if (strcmp(in_keyword, "callout")==0) {
        token = token_create(
            context->line,
            context->column,
            T_CALLOUT,
            in_keyword
        );
    } else if (strcmp(in_keyword, "class")==0) {
        token = token_create(
            context->line,
            context->column,
            T_CLASS,
            in_keyword
        );
    } else if (strcmp(in_keyword, "if")==0) {
        token = token_create(
            context->line,
            context->column,
            T_IF,
            in_keyword
        );
    } else if (strcmp(in_keyword, "else")==0) {
        token = token_create(
            context->line,
            context->column,
            T_ELSE,
            in_keyword
        );
    } else if (strcmp(in_keyword, "true")==0) {
        token = token_create(
            context->line,
            context->column,
            T_BOOLEAN_LITERAL,
            in_keyword
        );
    } else if (strcmp(in_keyword, "false")==0) {
        token = token_create(
            context->line,
            context->column,
            T_BOOLEAN_LITERAL,
            in_keyword
        );
    } else if (strcmp(in_keyword, "for")==0) {
        token = token_create(
            context->line,
            context->column,
            T_FOR,
            in_keyword
        );
    } else if (strcmp(in_keyword, "return")==0) {
        token = token_create(
            context->line,
            context->column,
            T_BREAK,
            in_keyword
        );
    } else if (strcmp(in_keyword, "Program")==0) {
        token = token_create(
            context->line,
            context->column,
            T_PROGRAM,
            in_keyword
        );
    } else if(strcmp(in_keyword, "void")==0) {
	token = token_create(
            context->line,
            context->column,
            T_VOID,
            in_keyword
        );
    } else {
        //in_keyword didn't match any of the known keywords - return illegal token
        token = token_create(
            context->line,
            context->column,
            T_ILLEGAL,
            in_keyword
       );
    }

    return token;
}
