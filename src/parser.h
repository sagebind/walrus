#ifndef WALRUS_PARSER_H
#define WALRUS_PARSER_H

#include "lexer.h"
#include "tokens.h"


/**
 * A single node in a parse tree.
 */
typedef struct ParseTreeNode {
    /**
     * The token the node represents.
     */
    Token token;

    /**
     * An array of child nodes.
     */
    struct ParseTreeNode* children;
} ParseTreeNode;

/**
 * Parses the tokens yielded by a given lexer.
 *
 * @param  lexer The lexer to read tokens to parse from.
 * @return       The root node of a resulting parse tree.
 */
ParseTreeNode* parser_parse(Lexer* lexer);

#endif
