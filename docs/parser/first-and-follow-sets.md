# Walrus Parser First and Follow Sets
Stephen Coakley, Trevor Wilson, Alex Laurent, Tyler Wilcock

FIRST(<program>) = { class Program { }
FOLLOW(<program>) = { EOF }

FIRST(<field_decl_list>) = { int boolean EPSILON }
FOLLOW(<field_decl_list>) = { int boolean EPSILON void }

FIRST(<method_decl_list>) = { int boolean EPSILON void }
FOLLOW(<method_decl_list>) = { } }

FIRST(<field_decl>) = { int boolean }
FOLLOW(<field_decl>) = { int boolean EPSILON }

FIRST(<field_id_list>) = { [a-zA-Z] _ }
FOLLOW(<field_id_list>) = { } int boolean EPSILON }

FIRST(<array_dim_decl>) = { [ EPSILON }
FOLLOW(<array_dim_decl>) = { , ; }

FIRST(<field_id_list_tail>) = { , ; }
FOLLOW(<field_id_list_tail>) = { } }

FIRST(<method_decl>) = { int boolean void }
FOLLOW(<method_decl>) = { int boolean EPSILON void } }

FIRST(<method_param_decl_list>) = { int boolean EPSILON }
FOLLOW(<method_param_decl_list>) = { ) }

FIRST(<method_param_decl_list_tail>) = { , EPSILON }
FOLLOW(<method_param_decl_list_tail>) = { ) }

FIRST(<method_param_decl>) = { int boolean }
FOLLOW(<method_param_decl>) = { , EPSILON }

FIRST(<block>) = { { }
FOLLOW(<block>) = { { } else int boolean void [a-zA-Z] _ callout if ( for return break continue EPSILON }

FIRST(<var_decl_list>) = { int boolean EPSILON }
FOLLOW(<var_decl_list>) = { [a-zA-Z] _ callout if ( for return break continue { EPSILON }

FIRST(<statement_list>) = { [a-zA-Z] _ callout if ( for return break continue { EPSILON }
FOLLOW(<statement_list>) = { } }

FIRST(<var_decl>) = { int boolean }
FOLLOW(<var_decl>) = { int boolean EPSILON }

FIRST(<var_id_list_tail>) = { , ; }
FOLLOW(<var_id_list_tail>) = { int boolean EPSILON }

FIRST(<type>) = { int boolean }
FOLLOW(<type>) = { [a-zA-Z] _ }

FIRST(<statement>) = { [a-zA-Z] _ callout if ( for return break continue { }
FOLLOW(<statement>) = { [a-zA-Z] _ callout if ( for return break continue { EPSILON }

FIRST(<else_expr>) = { else EPSILON }
FOLLOW(<else_expr>) = { [a-zA-Z] _ callout if ( for return break continue { EPSILON }

FIRST(<expr_option>) = { [a-zA-Z] _ [0-9] ‘ true false callout - ! ( EPSILON }
FOLLOW(<expr_option>) = { ; }

FIRST(<assign_op>) = { = += -= }
FOLLOW(<assign_op>) = { [a-zA-Z] _ [0-9] ‘ true false callout - ! ( }

FIRST(<method_call>) = { [a-zA-Z] _ callout }
FOLLOW(<method_call>) = { ; ) , ] { + - / * % < > <= >= == != && || EPSILON }

FIRST(<expr_list>) = { [a-zA-Z] _ [0-9] ‘ true false callout - ! ( EPSILON }
FOLLOW(<expr_list>) = { ) }

FIRST(<expr_list_tail>) = { , EPSILON }
FOLLOW(<expr_list_tail>) = { ) }

FIRST(<callout_arg_list>) = { , EPSILON }
FOLLOW(<callout_arg_list>) = { ) }

FIRST(<method_name>) = { [a-zA-Z] _ }
FOLLOW(<method_name>) = { ( }

FIRST(<location>) = { [a-zA-Z] _ }
FOLLOW(<location>) = { = += -= ; ) , ] { + - / * % < > <= >= == != && || EPSILON }

FIRST(<array_subscript_expr>) = { [ EPSILON }
FOLLOW(<array_subscript_expr>) = { = += -= ; ) , ] { + - / * % < > <= >= == != && || EPSILON }

FIRST(<expr>) = { [a-zA-Z] _ [0-9] ‘ true false callout - ! ( }
FOLLOW(<expr>) = { ; ) , ] { EPSILON }

FIRST(<expr_part>) = { [a-zA-Z] _ [0-9] ‘ true false callout - ! ( }
FOLLOW(<expr_part>) = { ; ) , ] { + - / * % < > <= >= == != && || EPSILON }

FIRST(<expr_end>) = { + - / * % < > <= >= == != && || EPSILON }
FOLLOW(<expr_end>) = { ; ) , ] { EPSILON }

FIRST(<callout_arg>) = { [a-zA-Z] _ [0-9] ‘ true false callout - ! ( ” }
FOLLOW(<callout_arg>) = { , EPSILON }

FIRST(<bin_op>) = { + - / * % < > <= >= == != && || }
FOLLOW(<bin_op>) = { [a-zA-Z] _ [0-9] ‘ true false callout - ! ( }

FIRST(<arith_op>) = { + - / * % }
FOLLOW(<arith_op>) = { [a-zA-Z] _ [0-9] ‘ true false callout - ! ( }

FIRST(<rel_op>) = { < > <= >= }
FOLLOW(<rel_op>) = { [a-zA-Z] _ [0-9] ‘ true false callout - ! ( }

FIRST(<eq_op>) = { == != }
FOLLOW(<eq_op>) = { [a-zA-Z] _ [0-9] ‘ true false callout - ! ( }

FIRST(<cond_op>) = { && || }
FOLLOW(<cond_op>) = { [a-zA-Z] _ [0-9] ‘ true false callout - ! ( }

FIRST(<literal>) = { [0-9]  ‘ true false }
FOLLOW(<literal>) = { [a-zA-Z] _ callout if ( for return break continue { EPSILON }

FIRST(<id>) = { [a-zA-Z] _ }
FOLLOW(<id>) = { ( = [ EPSILON , ; ( }

FIRST(<alpha_num_string>) = { 0-9 a-z A-Z _ EPSILON }
FOLLOW(<alpha_num_string>) = { ( = [ EPSILON , ; ( }

FIRST(<alpha_num>) = { [0-9] [a-z] [A-Z] _ }
FOLLOW(<alpha_num>) = { [0-9] [a-zA-Z] _ EPSILON }

FIRST(<alpha>) = { [a-zA-Z] _ }
FOLLOW(<alpha>) = { [0-9] [a-zA-Z] _ EPSILON }

FIRST(<digit>) = { [0-9] }
FOLLOW(<digit>) = { [0-9] ; ) , ] { + - / * % < > <= >= == != && || EPSILON }

FIRST(<hex_digit>) = { [0-9] [a-f] [A-F] }
FOLLOW(<hex_digit>) = { [0-9] [a-f] [A-F] EPSILON }

FIRST(<int_literal>) = { [0-9] }
FOLLOW(<int_literal>) = { ; ) , ] { + - / * % < > <= >= == != && || EPSILON }

FIRST(<decimal_literal>) = { [0-9] }
FOLLOW(<decimal_literal>) = { ; ) , ] { + - / * % < > <= >= == != && || EPSILON }

FIRST(<digit_list>) = { [0-9] EPSILON }
FOLLOW(<digit_list>) = { ; ) , ] { + - / * % < > <= >= == != && || EPSILON }

FIRST(<hex_literal>) = { 0x }
FOLLOW(<hex_literal>) = { ; ) , ] { + - / * % < > <= >= == != && || EPSILON }

FIRST(<hex_digit_list>) = { [0-9] [a-f] [A-F] EPSILON }
FOLLOW(<hex_digit_list>) = { ; ) , ] { + - / * % < > <= >= == != && || EPSILON }

FIRST(<bool_literal>) = { true false }
FOLLOW(<bool_literal>) = { [a-zA-Z] _ callout if ( for return break continue { EPSILON }

FIRST(<char_literal>) = { ‘ }
FOLLOW(<char_literal>) = { [a-zA-Z] _ callout if ( for return break continue { EPSILON }

FIRST(<string_literal>) = { “ }
FOLLOW(<string_literal>) = { \ , EPSILON }
