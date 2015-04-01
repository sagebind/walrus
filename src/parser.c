#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "lexer.h"
#include "parser.h"


/**
 * Parses the tokens yielded by a given lexer.
 */
Error parser_parse(Lexer* lexer)
{
    // the source file should contain a single program (duh!)
    if (!parser_parse_program(lexer)) {
        return E_PARSE_ERROR;
    }

    return E_SUCCESS;
}

/**
 * Displays a parser error at the current token.
 */
Error parser_error(Lexer* lexer, char* message)
{
    // get the current token
    Token token = lexer->current_node->token;

    // make a new message string containing extra info
    char* full_message = (char*)malloc(64 + strlen(token.file) + strlen(message));
    sprintf(full_message, "in file \"%s\" near line %d, column %d:\n\t%s", token.file, token.line, token.column, message);
    return error(E_PARSE_ERROR, full_message);
}

/**
 * <program> -> class Program { <field_decl_list> <method_decl_list> }
 */
bool parser_parse_program(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type != T_CLASS) {
        parser_error(lexer, "A program starts with 'class', you fool.");
        return false;
    }

    token = lexer_next(lexer);
    if (token.type != T_PROGRAM) {
        parser_error(lexer, "Expecting 'Program'");
        return false;
    }

    token = lexer_next(lexer);
    if (token.type != T_BRACE_LEFT) {
        parser_error(lexer, "Expected {");
        return false;
    }

    // both can be empty, so no errors unique to here
    if (!parser_parse_field_decl_list(lexer)) {
        return false;
    }
    if (!parser_parse_method_decl_list(lexer)) {
        return false;
    }

    token = lexer_next(lexer);
    if (token.type != T_BRACE_RIGHT) {
        parser_error(lexer, "Expected }");
        return false;
    }

    if (lexer_next(lexer).type != T_EOF) {
        parser_error(lexer, "Expected end of file.");
        return false;
    }

    return true;
}

/**
 * <field_decl_list> -> <field_decl> <field_decl_list> | EPSILON
 */
bool parser_parse_field_decl_list(Lexer* lexer)
{
    // do a lookahead
    Token token = lexer_lookahead(lexer, 1);

    // try to parse a field decl?
    if (token.type == T_BOOLEAN || token.type == T_INT) {
        // might be a field decl, do a further lookahead
        token = lexer_lookahead(lexer, 3);

        if (token.type == T_BRACKET_LEFT || token.type == T_COMMA || token.type == T_STATEMENT_END) {
            if (!parser_parse_field_decl(lexer)) {
                return false;
            }

            // if that worked, we must have a field_decl_list next
            if (!parser_parse_field_decl_list(lexer)) {
                return false;
            }
        }
    }

    // empty string works
    return true;
}

/**
 * <method_decl_list> -> <method_decl> <method_decl_list> | EPSILON
 */
bool parser_parse_method_decl_list(Lexer* lexer)
{
    // try to parse a method decl
    if (parser_parse_method_decl(lexer)) {
        // if that worked, we must have a method_decl_list
        if (!parser_parse_method_decl_list(lexer)) {
            parser_error(lexer, "Expected method declaration list.");
            return false;
        }
    }

    // empty string works
    return true;
}

/**
 * <field_decl> -> <type> <field_id_list>
 */
bool parser_parse_field_decl(Lexer* lexer)
{
    return parser_parse_type(lexer)
        && parser_parse_field_id_list(lexer);
}

/**
 * <field_id_list> -> <id> <array_dim_decl> <field_id_list_tail>
 */
bool parser_parse_field_id_list(Lexer* lexer)
{
    return parser_parse_id(lexer)
        && parser_parse_array_dim_decl(lexer)
        && parser_parse_field_id_list_tail(lexer);
}

/**
 * <array_dim_decl> -> [ <int_literal> ] | EPSILON
 */
bool parser_parse_array_dim_decl(Lexer* lexer)
{
    Token token = lexer_lookahead(lexer, 1);

    if (token.type == T_BRACKET_LEFT) {
        lexer_next(lexer);

        if (!parser_parse_int_literal(lexer)) {
            return false;
        }

        token = lexer_next(lexer);
        if (token.type != T_BRACKET_RIGHT) {
            parser_error(lexer, "Expected closing bracket for array declaration.");
            return false;
        }
    }

    // empty string works
    return true;
}

/**
 * <field_id_list_tail> -> , <field_id_list> | ;
 */
bool parser_parse_field_id_list_tail(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type == T_COMMA) {
        //first derivation
        if (!parser_parse_field_id_list(lexer)) {
            parser_error(lexer, "Expected field id list.");
            return false;
        }

        return true;
    } else if (token.type == T_STATEMENT_END) {
        return true;
    }

    parser_error(lexer, "Expected , or ; when parsing field_id_list_tail and got neither.");
    return false;
}

/**
 * <method_decl> -> <type> <id> ( <method_param_decl_list> ) <block>
                  | void <id> ( <method_param_decl_list> ) <block>
 */
bool parser_parse_method_decl(Lexer* lexer)
{
    // can start with <type> or void
    if (lexer_lookahead(lexer, 1).type == T_VOID) {
        lexer_next(lexer);
    } else if (!parser_parse_type(lexer)) {
        parser_error(lexer, "Expected type name or void.");
        return false;
    }

    // must have an identifier next
    if (!parser_parse_id(lexer)) {
        parser_error(lexer, "Expected identifier.");
        return false;
    }

    // left paren next ...
    if (lexer_next(lexer).type != T_PAREN_LEFT) {
        parser_error(lexer, "Expected left parentheses when parsing method_decl and did not get one.");
        return false;
    }

    // ... then the params ...
    if (!parser_parse_method_param_decl_list(lexer)) {
        parser_error(lexer, "Expected method argument list.");
        return false;
    }

    // ... right paren next ...
    if (lexer_next(lexer).type != T_PAREN_RIGHT) {
        parser_error(lexer, "Expected right parentheses when parsing method_decl and did not get one.");
        return false;
    }

    // ... then finally a block
    if (!parser_parse_block(lexer)) {
        parser_error(lexer, "Expected block following method definition.");
        return false;
    }

    // we made it!
    return true;
}

/**
 * <method_param_decl_list> -> <method_param_decl> <method_param_decl_list_tail> | EPSILON
 */
bool parser_parse_method_param_decl_list(Lexer* lexer)
{
    // epsilon
    if (lexer_lookahead(lexer, 1).type == T_PAREN_RIGHT) {
        return true;
    }

    if (!parser_parse_method_param_decl(lexer)) {
        parser_error(lexer, "Expected parameter declaration.");
        return false;
    }

    if (!parser_parse_method_param_decl_list_tail(lexer)) {
        parser_error(lexer, "Expected parameter list tail.");
        return false;
    }

    return true;
}

/**
 * <method_param_decl_list_tail> -> , <method_param_decl> <method_param_decl_list_tail> | EPSILON
 */
bool parser_parse_method_param_decl_list_tail(Lexer* lexer)
{
    // first derivation
    if (lexer_lookahead(lexer, 1).type == T_COMMA) {
        lexer_next(lexer);

        if (!parser_parse_method_param_decl(lexer)) {
            parser_error(lexer, "Expected method parameter declaration.");
            return false;
        }

        if (!parser_parse_method_param_decl_list_tail(lexer)) {
            parser_error(lexer, "Expected parameter list tail.");
            return false;
        }
    }

    // epsilon
    return true;
}

/**
 * <method_param_decl> -> <type> <id>
 */
bool parser_parse_method_param_decl(Lexer* lexer)
{
    return parser_parse_type(lexer)
        && parser_parse_id(lexer);
}

/**
 * <block> -> { <var_decl_list> <statement_list> }
 */
bool parser_parse_block(Lexer* lexer)
{
    if (lexer_next(lexer).type != T_BRACE_LEFT) {
        parser_error(lexer, "Expected left curly brace when parsing block and did not get one.");
        return false;
    }

    if (!parser_parse_var_decl_list(lexer)) {
        parser_error(lexer, "Expected variable declaration list.");
        return false;
    }

    if (!parser_parse_statement_list(lexer)) {
        parser_error(lexer, "Expected statement list.");
        return false;
    }

    if (lexer_next(lexer).type != T_BRACE_RIGHT) {
        parser_error(lexer, "Expected right curly brace when parsing block during parsing and did not get one.");
        return false;
    }

    return true;
}

/**
 * <var_decl_list> -> <var_decl> <var_decl_list> | EPSILON
 */
bool parser_parse_var_decl_list(Lexer* lexer)
{
    // first derivation
    if (parser_parse_var_decl(lexer)) {
        if (!parser_parse_var_decl_list(lexer)) {
            parser_error(lexer, "Expected variable declaration list.");
            return false;
        }
    }

    // epsilon
    return true;
}

/**
 * <statement_list> -> <statement> <statement_list> | EPSILON
 */
bool parser_parse_statement_list(Lexer* lexer)
{
    // epsilon
    if (lexer_lookahead(lexer, 1).type == T_BRACE_RIGHT) {
        return true;
    }

    // first derivation
    if (!parser_parse_statement(lexer)) {
        parser_error(lexer, "Expected statement.");
        return false;
    }

    if (!parser_parse_statement_list(lexer)) {
        parser_error(lexer, "Expected statement list.");
        return false;
    }

    return true;
}

/**
 * <var_decl> -> <type> <id> <var_id_list_tail>
 */
bool parser_parse_var_decl(Lexer* lexer)
{
    return parser_parse_type(lexer)
        && parser_parse_id(lexer)
        && parser_parse_var_id_list_tail(lexer);
}

/**
 * <var_id_list_tail> -> , <id> <var_id_list_tail> | ;
 */
bool parser_parse_var_id_list_tail(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type == T_COMMA) {
        //first derivation

        if (!parser_parse_id(lexer)) {
            parser_error(lexer, "Expected identifier.");
            return false;
        }

        if (!parser_parse_var_id_list_tail(lexer)) {
            parser_error(lexer, "Expected variable identifier list tail.");
            return false;
        }

        return true;
    } else if (token.type == T_STATEMENT_END) {
        return true;
    }

    parser_error(lexer, "Expected , or ; when parsing var_id_list_tail and got neither.");
    return false;
}

/**
 * <type> -> int | boolean
 */
bool parser_parse_type(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type != T_BOOLEAN && token.type != T_INT) {
        parser_error(lexer, "Expected int or boolean during parse and did not get them.");
        return false;
    }

    return true;
}

/**
 * <statement> -> <location> <assign_op> <expr> ;
 *              | <method_call> ;
 *              | if ( <expr> ) <block> <else_expr>
 *              | for <id> = <expr> , <expr> <block>
 *              | return <expr_option> ;
 *              | break ;
 *              | continue ;
 *              | <block>
 */
bool parser_parse_statement(Lexer* lexer)
{
    Token token = lexer_lookahead(lexer, 1);

    if (token.type == T_IDENTIFIER) {
        // second derivation
        if (lexer_lookahead(lexer, 2).type == T_PAREN_LEFT) {
            if (!parser_parse_method_call(lexer)) {
                parser_error(lexer, "Expected method call.");
                return false;
            }

            if (lexer_next(lexer).type != T_STATEMENT_END) {
                parser_error(lexer, "Missing semicolon at end of statement.");
                return false;
            }

            return true;
        }

        // first derivation
        else {
            if (!parser_parse_location(lexer)) {
                parser_error(lexer, "Expected location.");
                return false;
            }

            if (!parser_parse_assign_op(lexer)) {
                parser_error(lexer, "Expected assignment operator.");
                return false;
            }

            if (!parser_parse_expr(lexer)) {
                parser_error(lexer, "Expected expression.");
                return false;
            }

            if (lexer_next(lexer).type != T_STATEMENT_END) {
                parser_error(lexer, "Missing semicolon at end of statement.");
                return false;
            }
        }

        return true;
    }

    // eighth derivation
    else if (token.type == T_BRACE_LEFT) {
        if (!parser_parse_block(lexer)) {
            parser_error(lexer, "Expected block.");
            return false;
        }

        return true;
    }

    // terminals
    else {
        lexer_next(lexer);

        // third derivation - if statement
        if (token.type == T_IF) {
            if (lexer_next(lexer).type != T_PAREN_LEFT) {
                parser_error(lexer, "Missing opening parenthesis.");
                return false;
            }

            if (!parser_parse_expr(lexer)) {
                parser_error(lexer, "Expected expression.");
                return false;
            }

            if (lexer_next(lexer).type != T_PAREN_RIGHT) {
                parser_error(lexer, "Missing closing parenthesis.");
                return false;
            }

            if (!parser_parse_block(lexer)) {
                parser_error(lexer, "Expected block.");
                return false;
            }

            if (!parser_parse_else_expr(lexer)) {
                parser_error(lexer, "Expected else expression.");
                return false;
            }

            return true;
        }

        // fourth derivation
        else if (token.type == T_FOR) {
            if (!parser_parse_id(lexer)) {
                parser_error(lexer, "Expected identifier.");
                return false;
            }

            if (strcmp(lexer_next(lexer).lexeme, "=") != 0) {
                parser_error(lexer, "Expected equals '=' sign.");
                return false;
            }

            if (!parser_parse_expr(lexer)) {
                parser_error(lexer, "Expected expression.");
                return false;
            }

            if (lexer_next(lexer).type != T_COMMA) {
                parser_error(lexer, "Expected comma ',' after expression.");
                return false;
            }

            if (!parser_parse_expr(lexer)) {
                parser_error(lexer, "Expected expression.");
                return false;
            }

            if (!parser_parse_block(lexer)) {
                parser_error(lexer, "Expected block.");
                return false;
            }

            return true;
        }

        // fifth derivation
        else if (token.type == T_RETURN) {
            if (!parser_parse_expr_option(lexer)) {
                parser_error(lexer, "Expected optional expression. Hmm...");
                return false;
            }

            if (lexer_next(lexer).type != T_STATEMENT_END) {
                parser_error(lexer, "Missing semicolon at end of statement.");
                return false;
            }

            return true;
        }

        // sixth and seventh derivations
        else if (token.type == T_BREAK || token.type == T_CONTINUE) {
            if (lexer_next(lexer).type != T_STATEMENT_END) {
                parser_error(lexer, "Missing semicolon at end of statement.");
                return false;
            }

            return true;
        }
    }

    parser_error(lexer, "Invalid statement.");
    return false;
}

/**
 * <else_expr> -> else <block> | EPSILON
 */
bool parser_parse_else_expr(Lexer* lexer)
{
    // @todo
    Token token = lexer_next(lexer);
    if (token.type != T_ELSE) {
        parser_error(lexer, "Expected else while parsing else_expr and did not get it.");
    }

    //parser_parse_else_expr(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <expr_option> -> <expr> | EPSILON
 */
bool parser_parse_expr_option(Lexer* lexer)
{
    // @todo
    // epsilon
    if (lexer_lookahead(lexer, 1).type == T_STATEMENT_END) {
        return true;
    }

    return parser_parse_expr(lexer);
}

/**
 * <assign_op> -> = | += | -=
 */
bool parser_parse_assign_op(Lexer* lexer)
{
    // @todo
    Token token = lexer_next(lexer);
    if (token.lexeme != "=" && token.lexeme != "+=" && token.lexeme != "-=") {
        parser_error(lexer, "Expected one of the following when parsing an assign_op and did not get them: = += -=");
    }
}

/**
 * <method_call> -> <method_name> ( <expr_list> )
 *                  | callout ( <string_literal> <callout_arg_list> )
 */
bool parser_parse_method_call(Lexer* lexer)
{
    // @todo
    Token token = lexer_next(lexer);
    if (token.type == T_CALLOUT) {
        //second derivation

        token = lexer_next(lexer);
        if (token.type != T_PAREN_LEFT) {
            parser_error(lexer, "Expected left parentheses when parsing method_call and did not get one.");
        }
        //parser_parse_string_literal(lexer);
        //parser_parse_callout_arg_list(lexer);

        token = lexer_next(lexer);
        if (token.type != T_PAREN_RIGHT) {
            parser_error(lexer, "Expected right parentheses when parsing method_call and did not get one.");
        }
    } else {
        //first derivation

        //parser_parse_method_name(lexer);
        token = lexer_next(lexer);
        if (token.type != T_PAREN_LEFT) {
            //do we have to move the lexer back now?
            parser_error(lexer, "Expected left parentheses when parsing method_call and didn't get one.");
        }
        //parser_parse_expr_list(lexer);

        token = lexer_next(lexer);
        if (token.type != T_PAREN_RIGHT) {
            //do we have to move the lexer back now?
            parser_error(lexer, "Expected right parentheses when parsing method_call and didn't get one.");
        }
    }
}

/**
 * <expr_list> -> <expr> <expr_list_tail> | EPSILON
 */
bool parser_parse_expr_list(Lexer* lexer)
{
    // @todo
    //parser_parse_expr(lexer);
    //parser_parse_expr_list_tail(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <expr_list_tail> -> , <expr> <expr_list_tail> | EPSILON
 */
bool parser_parse_expr_list_tail(Lexer* lexer)
{
    // @todo
    Token token = lexer_next(lexer);
    if (token.lexeme != ",") {
        parser_error(lexer, "Expected , when parsing expr_list_tail and didn't get it.");
    }

    //parser_parse_expr(lexer);
    //parser_parse_expr_list_tail(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <callout_arg_list> -> , <callout_arg> <callout_arg_list> | EPSILON
 */
bool parser_parse_callout_arg_list(Lexer* lexer)
{
    // @todo
    Token token = lexer_next(lexer);
    if (token.lexeme != ",") {
        parser_error(lexer, "Expected comma when parsing callout_arg_list and didn't get it.");
    }

    //parser_parse_callout_arg(lexer);
    //parser_parse_callout_arg_list(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <method_name> -> <id>
 */
bool parser_parse_method_name(Lexer* lexer)
{
    return parser_parse_id(lexer);
}

/**
 * <location> -> <id> <array_subscript_expr>
 */
bool parser_parse_location(Lexer* lexer)
{
    // @todo
    //parser_parse_id(lexer);
    //parser_parse_array_subscript_expr(lexer);
}

/**
 * <array_subscript_expr> -> [ <expr> ] | EPSILON
 */
bool parser_parse_array_subscript_expr(Lexer* lexer)
{
    // @todo
    Token token = lexer_next(lexer);
    if (token.type != T_BRACKET_RIGHT) {
        parser_error(lexer, "Expected a right bracket when parsing array_subscript_expr but didn't get one.");
    }
    //parser_parse_expr(lexer);
    token = lexer_next(lexer);
    if (token.type != T_BRACKET_LEFT) {
        parser_error(lexer, "Expected a left bracket when parsing array_subscript_expr but didn't get one.");
    }

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <expr> -> <expr_part> <expr_end>
 */
bool parser_parse_expr(Lexer* lexer)
{
    // @todo
    return parser_parse_expr_part(lexer)
        && parser_parse_expr_end(lexer);
}

/**
 * <expr_part> -> <location>
 *              | <method_call>
 *              | <literal>
 *              | - <expr>
 *              | ! <expr>
 *              | ( <expr> )
 */
bool parser_parse_expr_part(Lexer* lexer)
{
    // @todo
}

/**
 * <expr_end> -> <bin_op> <expr> | EPSILON
 */
bool parser_parse_expr_end(Lexer* lexer)
{
    // @todo
    //parser_parse_bin_op(lexer);
    // parser_parse_expr(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <callout_arg> -> <expr> | <string_literal>
 */
bool parser_parse_callout_arg(Lexer* lexer)
{
    // @todo
}

/**
 * <bin_op> -> <arith_op> | <rel_op> | <eq_op> | <cond_op>
 */
bool parser_parse_bin_op(Lexer* lexer)
{
    // @todo
}

/**
 * <arith_op> -> + | - | * | / | %
 */
bool parser_parse_arith_op(Lexer* lexer)
{
    // @todo
    Token token = lexer_next(lexer);
    if (token.lexeme != "+" && token.lexeme != "-" && token.lexeme != "*" && token.lexeme != "/" && token.lexeme != "%") {
        parser_error(lexer, "Expected any of the following and did not receive them during the parse: + - * / %");
    }
}

/**
 * <rel_op> ->   < | > | <= | >=
 */
bool parser_parse_rel_op(Lexer* lexer)
{
    // @todo
    Token token = lexer_next(lexer);
    if (token.lexeme != "<" && token.lexeme != ">" && token.lexeme != "<=" && token.lexeme != ">=") {
        parser_error(lexer, "Expected any of the following and did not receive them during the parse: < > <= >=");
    }
}

/**
 * <eq_op> -> == OR !=
 */
bool parser_parse_eq_op(Lexer* lexer)
{
    // @todo
    Token token = lexer_next(lexer);
    if (token.lexeme != "==" || token.lexeme != "!=") {
        parser_error(lexer, "Expected either == or != during the parse and did not get them.");
    }
}

/**
 * <cond_op> -> && OR ||
 */
bool parser_parse_cond_op(Lexer* lexer)
{
    // @todo
    Token token = lexer_next(lexer);
    if (token.lexeme != "&&" && token.lexeme != "||") {
        parser_error(lexer, "Expected && or || during parse and did not get them.");
    }
}

/**
 * <literal> -> <int_literal> | <char_literal> | <bool_literal>
 */
bool parser_parse_literal(Lexer* lexer)
{
    // @todo
}

/**
 * <id> -> <alpha> <alpha_num_string>
 */
bool parser_parse_id(Lexer* lexer)
{
    // @todo
    return lexer_next(lexer).type == T_IDENTIFIER;
}

/**
 * <int_literal> -> <decimal_literal> | <hex_literal>
 */
bool parser_parse_int_literal(Lexer* lexer)
{
    // @todo
    return lexer_next(lexer).type == T_INT_LITERAL;
}

/**
 * <bool_literal> -> true | false
 */
bool parser_parse_bool_literal(Lexer* lexer)
{
    // @todo
    Token token = lexer_next(lexer);
    if (token.type != T_BOOLEAN_LITERAL) {
        parser_error(lexer, "Expected boolean literal during parse and did not get it.");
        return false;
    }

    return true;
}

/**
 * <char_literal> -> ’ <char> ’
 */
bool parser_parse_char_literal(Lexer* lexer)
{
    // @todo
    return lexer_next(lexer).type == T_CHAR_LITERAL;
}

/**
 * <string_literal> -> " <char> "
 */
bool parser_parse_string_literal(Lexer* lexer)
{
    // @todo
    return lexer_next(lexer).type == T_STRING_LITERAL;
}
