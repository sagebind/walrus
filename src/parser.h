#ifndef WALRUS_PARSER_H
#define WALRUS_PARSER_H

#include <stdbool.h>
#include "ast.h"
#include "lexer.h"
#include "tokens.h"


/**
 * Parses the tokens yielded by a given lexer.
 *
 * @param  lexer The lexer to read tokens to parse from.
 * @return       An error code.
 */
ASTNode* parser_parse(Lexer* lexer);

/**
 * Displays a parser error.
 *
 * @param  lexer   The active lexer.
 * @param  message The error message.
 * @return         An error code.
 */
Error parser_error(Lexer* lexer, char* message);

/**
 * <program> -> class Program { <field_decl_list> <method_decl_list> }
 */
Error parser_parse_program(Lexer* lexer, ASTClassDecl** node);

/**
 * <field_decl_list> -> <field_decl> <field_decl_list> | EPSILON
 */
Error parser_parse_field_decl_list(Lexer* lexer, ASTClassDecl* program);

/**
 * <method_decl_list> -> <method_decl> <method_decl_list> | EPSILON
 */
Error parser_parse_method_decl_list(Lexer* lexer, ASTClassDecl* program);

/**
 * <field_decl> -> <type> <field_id_list>
 */
Error parser_parse_field_decl(Lexer* lexer, ASTClassDecl* program);

/**
 * <field_id_list> -> <id> <array_dim_decl> <field_id_list_tail>
 */
Error parser_parse_field_id_list(Lexer* lexer, DataType type, ASTClassDecl* program);

/**
 * <array_dim_decl> -> [ <int_literal> ] | EPSILON
 */
Error parser_parse_array_dim_decl(Lexer* lexer, int* length);

/**
 * <field_id_list_tail> -> , <field_id_list> | ;
 */
Error parser_parse_field_id_list_tail(Lexer* lexer, DataType type, ASTClassDecl* program);

/**
 * <method_decl> -> <type> <id> ( <method_param_decl_list> ) <block>
                    | void <id> ( <method_param_decl_list> ) <block>
 */
Error parser_parse_method_decl(Lexer* lexer, ASTMethodDecl** node);

/**
 * <method_param_decl_list> -> <method_param_decl> <method_param_decl_list_tail> | EPSILON
 */
Error parser_parse_method_param_decl_list(Lexer* lexer);

/**
 * <method_param_decl_list_tail> -> , <method_param_decl> <method_param_decl_list_tail> | EPSILON
 */
Error parser_parse_method_param_decl_list_tail(Lexer* lexer);

/**
 * <method_param_decl> -> <type> <id>
 */
Error parser_parse_method_param_decl(Lexer* lexer);

/**
 * <block> -> { <var_decl_list> <statement_list> }
 */
Error parser_parse_block(Lexer* lexer, ASTNode** node);

/**
 * <var_decl_list> -> <var_decl> <var_decl_list> | EPSILON
 */
Error parser_parse_var_decl_list(Lexer* lexer);

/**
 * <statement_list> -> <statement> <statement_list> | EPSILON
 */
Error parser_parse_statement_list(Lexer* lexer);

/**
 * <var_decl> -> <type> <id> <var_id_list_tail>
 */
Error parser_parse_var_decl(Lexer* lexer, ASTNode** node);

/**
 * <var_id_list_tail> -> , <id> <var_id_list_tail> | ;
 */
Error parser_parse_var_id_list_tail(Lexer* lexer);

/**
 * <type> -> int | boolean
 */
Error parser_parse_type(Lexer* lexer, DataType* type);

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
Error parser_parse_statement(Lexer* lexer, ASTNode** node);

/**
 * <else_expr> -> else <block> | EPSILON
 */
Error parser_parse_else_expr(Lexer* lexer);

/**
 * <expr_option> -> <expr> | EPSILON
 */
Error parser_parse_expr_option(Lexer* lexer);

/**
 * <assign_op> -> = | += | -=
 */
Error parser_parse_assign_op(Lexer* lexer);

/**
 * <method_call> -> <method_name> ( <expr_list> )
 *                  | callout ( <string_literal> <callout_arg_list> )
 */
Error parser_parse_method_call(Lexer* lexer);

/**
 * <expr_list> -> <expr> <expr_list_tail> | EPSILON
 */
Error parser_parse_expr_list(Lexer* lexer);

/**
 * <expr_list_tail> -> , <expr> <expr_list_tail> | EPSILON
 */
Error parser_parse_expr_list_tail(Lexer* lexer);

/**
 * <callout_arg_list> -> , <callout_arg> <callout_arg_list> | EPSILON
 */
Error parser_parse_callout_arg_list(Lexer* lexer);

/**
 * <method_name> -> <id>
 */
Error parser_parse_method_name(Lexer* lexer);

/**
 * <location> -> <id> <array_subscript_expr>
 */
Error parser_parse_location(Lexer* lexer);

/**
 * <array_subscript_expr> -> [ <expr> ] | EPSILON
 */
Error parser_parse_array_subscript_expr(Lexer* lexer);

/**
 * <expr> -> <expr_part> <expr_end>
 */
Error parser_parse_expr(Lexer* lexer);

/**
 * <expr_part> -> <location>
 *              | <method_call>
 *              | <literal>
 *              | - <expr>
 *              | ! <expr>
 *              | ( <expr> )
 */
Error parser_parse_expr_part(Lexer* lexer);

/**
 * <expr_end> -> <bin_op> <expr> | EPSILON
 */
Error parser_parse_expr_end(Lexer* lexer);

/**
 * <callout_arg> -> <expr> | <string_literal>
 */
Error parser_parse_callout_arg(Lexer* lexer);

/**
 * <bin_op> -> <arith_op> | <rel_op> | <eq_op> | <cond_op>
 */
Error parser_parse_bin_op(Lexer* lexer, ASTNode** node);

/**
 * <literal> -> <int_literal> | <char_literal> | <bool_literal>
 */
Error parser_parse_literal(Lexer* lexer, ASTNode** node);

/**
 * <id> -> <alpha> <alpha_num_string>
 */
Error parser_parse_id(Lexer* lexer, char** id);

/**
 * <int_literal> -> <decimal_literal> | <hex_literal>
 */
Error parser_parse_int_literal(Lexer* lexer, ASTNode** node);

/**
 * <bool_literal> -> true | false
 */
Error parser_parse_bool_literal(Lexer* lexer, ASTNode** node);

/**
 * <char_literal> -> ’ <char> ’
 */
Error parser_parse_char_literal(Lexer* lexer, ASTNode** node);

/**
 * <string_literal> -> " <char> "
 */
Error parser_parse_string_literal(Lexer* lexer, ASTNode** node);

#endif
