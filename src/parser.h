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
 * Tries to parse a numerical string and returns its value.
 *
 * @param  string The string containing a number in decimal or hexadecimal format.
 * @return        The parsed value in decimal, or zero if it could not be parsed.
 */
long parser_str_to_long(char* string);

/**
 * Creates a copy of a string with single and double quotes stripped from the
 * ends of the string.
 *
 * @param  string The string to strip.
 * @return        The stripped string.
 */
char* parser_strip_quotes(const char* string);

/**
 * Parses a program.
 *
 * <program> -> class Program { <field_decl_list> <method_decl_list> }
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_program(Lexer* lexer, ASTDecl** node);

/**
 * Parses a field declaration list.
 *
 * <field_decl_list> -> <field_decl> <field_decl_list> | EPSILON
 *
 * @param  lexer   The lexer to parse tokens from.
 * @param  program The parent node of the current position.
 * @return         An error code.
 */
Error parser_parse_field_decl_list(Lexer* lexer, ASTDecl* program);

/**
 * Parses a method declaration list.
 *
 * <method_decl_list> -> <method_decl> <method_decl_list> | EPSILON
 *
 * @param  lexer   The lexer to parse tokens from.
 * @param  program The parent node of the current position.
 * @return         An error code.
 */
Error parser_parse_method_decl_list(Lexer* lexer, ASTDecl* program);

/**
 * Parses a class field declaration.
 *
 * <field_decl> -> <type> <field_id_list>
 *
 * @param  lexer   The lexer to parse tokens from.
 * @param  program The parent node of the current position.
 * @return         An error code.
 */
Error parser_parse_field_decl(Lexer* lexer, ASTDecl* program);

/**
 * Parses a field identifier list.
 *
 * <field_id_list> -> <id> <array_dim_decl> <field_id_list_tail>
 *
 * @param  lexer   The lexer to parse tokens from.
 * @param  type    The data type inherited from from the field declaration.
 * @param  program The parent node of the current position.
 * @return         An error code.
 */
Error parser_parse_field_id_list(Lexer* lexer, DataType type, ASTDecl* program);

/**
 * Parses an array delimiter declaration and retrieves the array length.
 *
 * If no array subscript is found, simply sets the length to 0.
 *
 * <array_dim_decl> -> [ <int_literal> ] | EPSILON
 *
 * @param  lexer  The lexer to parse tokens from.
 * @param  length A pointer to where to store the parsed array length.
 * @return        An error code.
 */
Error parser_parse_array_dim_decl(Lexer* lexer, int* length);

/**
 * Parses the end of a field declaration list.
 *
 * <field_id_list_tail> -> , <field_id_list> | ;
 *
 * @param  lexer   The lexer to parse tokens from.
 * @param  program The parent node of the current position.
 * @return         An error code.
 */
Error parser_parse_field_id_list_tail(Lexer* lexer, DataType type, ASTDecl* program);

/**
 * Parses a method declaration.
 *
 * <method_decl> -> <type> <id> ( <method_param_decl_list> ) <block>
                    | void <id> ( <method_param_decl_list> ) <block>
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_method_decl(Lexer* lexer, ASTDecl** node);

/**
 * Parses a method parameter declaration list.
 *
 * <method_param_decl_list> -> <method_param_decl> <method_param_decl_list_tail> | EPSILON
 *
 * @param  lexer  The lexer to parse tokens from.
 * @param  method The parent node of the current position.
 * @return        An error code.
 */
Error parser_parse_method_param_decl_list(Lexer* lexer, ASTDecl* method);

/**
 * Parses the end of a parameter declaration list.
 *
 * <method_param_decl_list_tail> -> , <method_param_decl> <method_param_decl_list_tail> | EPSILON
 *
 * @param  lexer  The lexer to parse tokens from.
 * @param  method The parent node of the current position.
 * @return        An error code.
 */
Error parser_parse_method_param_decl_list_tail(Lexer* lexer, ASTDecl* method);

/**
 * Parses a method parameter declaration.
 *
 * <method_param_decl> -> <type> <id>
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_method_param_decl(Lexer* lexer, ASTDecl** node);

/**
 * Parses a statement block.
 *
 * <block> -> { <var_decl_list> <statement_list> }
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_block(Lexer* lexer, ASTNode** node);

/**
 * Parses a list of variable declarations.
 *
 * <var_decl_list> -> <var_decl> <var_decl_list> | EPSILON
 *
 * @param  lexer  The lexer to parse tokens from.
 * @param  parent The parent node of the current position.
 * @return        An error code.
 */
Error parser_parse_var_decl_list(Lexer* lexer, ASTNode* parent);

/**
 * Parses a list of statements.
 *
 * <statement_list> -> <statement> <statement_list> | EPSILON
 *
 * @param  lexer  The lexer to parse tokens from.
 * @param  parent The parent node of the current position.
 * @return        An error code.
 */
Error parser_parse_statement_list(Lexer* lexer, ASTNode* parent);

/**
 * Parses a variable declaration.
 *
 * <var_decl> -> <type> <id> <var_id_list_tail>
 *
 * @param  lexer  The lexer to parse tokens from.
 * @param  parent The parent node of the current position.
 * @return        An error code.
 */
Error parser_parse_var_decl(Lexer* lexer, ASTNode* parent);

/**
 * Parses a variable identifier list.
 *
 * <var_id_list_tail> -> , <id> <var_id_list_tail> | ;
 *
 * @param  lexer  The lexer to parse tokens from.
 * @param  type   The data type inherited from the declaration.
 * @param  parent The parent node of the current position.
 * @return        An error code.
 */
Error parser_parse_var_id_list_tail(Lexer* lexer, DataType type, ASTNode* parent);

/**
 * Parses and fetches a type keyword.
 *
 * <type> -> int | boolean
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  type  A pointer to where to store the parsed type.
 * @return       An error code.
 */
Error parser_parse_type(Lexer* lexer, DataType* type);

/**
 * Parses a single statement.
 *
 * <statement> -> <location> <assign_op> <expr> ;
 *              | <method_call> ;
 *              | if ( <expr> ) <block> <else_expr>
 *              | for <id> = <expr> , <expr> <block>
 *              | return <expr_option> ;
 *              | break ;
 *              | continue ;
 *              | <block>
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_statement(Lexer* lexer, ASTNode** node);

/**
 * Parses an optional else statement.
 *
 * <else_expr> -> else <block> | EPSILON
 *
 * @param  lexer  The lexer to parse tokens from.
 * @param  parent The parent node of the current position.
 * @return        An error code.
 */
Error parser_parse_else_expr(Lexer* lexer, ASTNode* parent);

/**
 * Parses an optional expression block.
 *
 * <expr_option> -> <expr> | EPSILON
 *
 * @param  lexer  The lexer to parse tokens from.
 * @param  parent The parent node of the current position.
 * @return        An error code.
 */
Error parser_parse_expr_option(Lexer* lexer, ASTNode* parent);

/**
 * Parses an assignment operation.
 *
 * <assign_op> -> = | += | -=
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_assign_op(Lexer* lexer, ASTOperation** node);

/**
 * Parses a method call.
 *
 * <method_call> -> <method_name> ( <expr_list> )
 *                  | callout ( <string_literal> <callout_arg_list> )
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_method_call(Lexer* lexer, ASTReference** node);

/**
 * Parses an expression list.
 *
 * <expr_list> -> <expr> <expr_list_tail> | EPSILON
 *
 * @param  lexer  The lexer to parse tokens from.
 * @param  parent The parent node of the current position.
 * @return        An error code.
 */
Error parser_parse_expr_list(Lexer* lexer, ASTNode* parent);

/**
 * Parses the end of an expression list.
 *
 * <expr_list_tail> -> , <expr> <expr_list_tail> | EPSILON
 *
 * @param  lexer  The lexer to parse tokens from.
 * @param  parent The parent node of the current position.
 * @return        An error code.
 */
Error parser_parse_expr_list_tail(Lexer* lexer, ASTNode* parent);

/**
 * Parses a callout argument list.
 *
 * <callout_arg_list> -> , <callout_arg> <callout_arg_list> | EPSILON
 *
 * @param  lexer  The lexer to parse tokens from.
 * @param  parent The parent node of the current position.
 * @return        An error code.
 */
Error parser_parse_callout_arg_list(Lexer* lexer, ASTReference* parent);

/**
 * Parses a method name.
 *
 * <method_name> -> <id>
 *
 * @param  lexer      The lexer to parse tokens from.
 * @param  identifier A pointer to where to store the identifier name.
 * @return            An error code.
 */
Error parser_parse_method_name(Lexer* lexer, char** identifier);

/**
 * Parses a variable location.
 *
 * <location> -> <id> <array_subscript_expr>
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_location(Lexer* lexer, ASTReference** node);

/**
 * Parses an array subscript accessor.
 *
 * <array_subscript_expr> -> [ <expr> ] | EPSILON
 *
 * @param  lexer  The lexer to parse tokens from.
 * @param  parent The parent node of the current position.
 * @return        An error code.
 */
Error parser_parse_array_subscript_expr(Lexer* lexer, ASTReference* parent);

/**
 * Recursively parses a generic expression.
 *
 * Note that this function parses the following 2 grammar rules at once:
 *
 * <expr> -> <expr_part> <expr_end>
 * <expr_end> -> <bin_op> <expr> | EPSILON
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_expr(Lexer* lexer, ASTNode** node);

/**
 * Parses the left operand of an expression.
 *
 * <expr_part> -> <location>
 *              | <method_call>
 *              | <literal>
 *              | - <expr>
 *              | ! <expr>
 *              | ( <expr> )
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_expr_part(Lexer* lexer, ASTNode** node);

/**
 * Parses a single callout argument.
 *
 * <callout_arg> -> <expr> | <string_literal>
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_callout_arg(Lexer* lexer, ASTNode** node);

/**
 * Parses a binary operation expression.
 *
 * <bin_op> -> <arith_op> | <rel_op> | <eq_op> | <cond_op>
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_bin_op(Lexer* lexer, ASTOperation** node);

/**
 * Parses any kind of literal expression.
 *
 * <literal> -> <int_literal> | <char_literal> | <bool_literal>
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_literal(Lexer* lexer, ASTNode** node);

/**
 * Parses an identifier and gets the identifier name.
 *
 * <id> -> <alpha> <alpha_num_string>
 *
 * @param  lexer      The lexer to parse tokens from.
 * @param  identifier A pointer to where to store the identifier name.
 * @return            An error code.
 */
Error parser_parse_id(Lexer* lexer, char** identifier);

/**
 * Parses an integer literal.
 *
 * <int_literal> -> <decimal_literal> | <hex_literal>
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_int_literal(Lexer* lexer, ASTNode** node);

/**
 * Parses a boolean literal.
 *
 * <bool_literal> -> true | false
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_bool_literal(Lexer* lexer, ASTNode** node);

/**
 * Parses a char literal.
 *
 * <char_literal> -> ' <char> '
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_char_literal(Lexer* lexer, ASTNode** node);

/**
 * Parses a string literal.
 *
 * <string_literal> -> " <char> "
 *
 * @param  lexer The lexer to parse tokens from.
 * @param  node  A pointer to where to store the created node.
 * @return       An error code.
 */
Error parser_parse_string_literal(Lexer* lexer, ASTNode** node);

#endif
