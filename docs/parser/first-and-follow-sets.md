# Walrus Parser: First and Follow Sets
*Stephen Coakley, Trevor Wilson, Alex Laurent, Tyler Wilcock*

Note that we are using the "❬❭" bracket characters to denote unordered sets; it makes it much easier to distinguish between set boundaries and literal curly braces "{}".

FIRST(<program>) = ❬ class ❭
FOLLOW(<program>) = ❬ EOF ❭
FIRST+(<program>) = ❬ class ❭

FIRST(<field_decl_list>) = ❬ boolean int ϵ ❭
FOLLOW(<field_decl_list>) = ❬ boolean int void ϵ ❭
FIRST+(<field_decl_list>) = ❬ boolean int void ϵ ❭

FIRST(<method_decl_list>) = ❬ boolean int void ϵ ❭
FOLLOW(<method_decl_list>) = ❬ } ❭
FIRST+(<method_decl_list>) = ❬ boolean int void } ϵ ❭

FIRST(<field_decl>) = ❬ boolean int ❭
FOLLOW(<field_decl>) = ❬ boolean int ϵ ❭
FIRST+(<field_decl>) = ❬ boolean int ❭

FIRST(<field_id_list>) = ❬ [a-zA-Z] _ ❭
FOLLOW(<field_id_list>) = ❬ boolean int } ϵ ❭
FIRST+(<field_id_list>) = ❬ [a-zA-Z] _ ❭

FIRST(<array_dim_decl>) = ❬ [ ϵ ❭
FOLLOW(<array_dim_decl>) = ❬ , ; ❭
FIRST+(<array_dim_decl>) = ❬ , ; [ ϵ ❭

FIRST(<field_id_list_tail>) = ❬ , ; ❭
FOLLOW(<field_id_list_tail>) = ❬ } ❭
FIRST+(<field_id_list_tail>) = ❬ , ; ❭

FIRST(<method_decl>) = ❬ boolean int void ❭
FOLLOW(<method_decl>) = ❬ boolean int void } ϵ ❭
FIRST+(<method_decl>) = ❬ boolean int void ❭

FIRST(<method_param_decl_list>) = ❬ boolean int ϵ ❭
FOLLOW(<method_param_decl_list>) = ❬ ) ❭
FIRST+(<method_param_decl_list>) = ❬ boolean int ) ϵ ❭

FIRST(<method_param_decl_list_tail>) = ❬ , ϵ ❭
FOLLOW(<method_param_decl_list_tail>) = ❬ ) ❭
FIRST+(<method_param_decl_list_tail>) = ❬ , ) ϵ ❭

FIRST(<method_param_decl>) = ❬ boolean int ❭
FOLLOW(<method_param_decl>) = ❬ , ϵ ❭
FIRST+(<method_param_decl>) = ❬ boolean int ❭

FIRST(<block>) = ❬ { ❭
FOLLOW(<block>) = ❬ [a-zA-Z] _ else boolean int void callout if for return break continue ( { } ϵ ❭
FIRST+(<block>) = ❬ { ❭

FIRST(<var_decl_list>) = ❬ boolean int ϵ ❭
FOLLOW(<var_decl_list>) = ❬ [a-zA-Z] _ callout if for return break continue ( { ϵ ❭
FIRST+(<var_decl_list>) = ❬ [a-zA-Z] _ boolean int callout if for return break continue ( { ϵ ❭

FIRST(<statement_list>) = ❬ [a-zA-Z] _ callout if for return break continue ( { ϵ ❭
FOLLOW(<statement_list>) = ❬ } ❭
FIRST+(<statement_list>) = ❬ [a-zA-Z] _ callout if for return break continue ( { } ϵ ❭

FIRST(<var_decl>) = ❬ boolean int ❭
FOLLOW(<var_decl>) = ❬ boolean int ϵ ❭
FIRST+(<var_decl>) = ❬ boolean int ❭

FIRST(<var_id_list_tail>) = ❬ , ; ❭
FOLLOW(<var_id_list_tail>) = ❬ boolean int ϵ ❭
FIRST+(<var_id_list_tail>) = ❬ , ; ❭

FIRST(<type>) = ❬ boolean int ❭
FOLLOW(<type>) = ❬ [a-zA-Z] _ ❭
FIRST+(<type>) = ❬ boolean int ❭

FIRST(<statement>) = ❬ [a-zA-Z] _ callout if for return break continue ( { ❭
FOLLOW(<statement>) = ❬ [a-zA-Z] _ callout if for return break continue ( { ϵ ❭
FIRST+(<statement>) = ❬ [a-zA-Z] _ callout if for return break continue ( { ❭

FIRST(<else_expr>) = ❬ else ϵ ❭
FOLLOW(<else_expr>) = ❬ [a-zA-Z] _ callout if for return break continue ( { ϵ ❭
FIRST+(<else_expr>) = ❬ [a-zA-Z] _ else callout if for return break continue ( { ϵ ❭

FIRST(<expr_option>) = ❬ [a-zA-Z] _ [0-9] ' true false callout - ! ( ϵ ❭
FOLLOW(<expr_option>) = ❬ ; ❭
FIRST+(<expr_option>) = ❬ [a-zA-Z] _ [0-9] ' ; true false callout - ! ( ϵ ❭

FIRST(<assign_op>) = ❬ = += -= ❭
FOLLOW(<assign_op>) = ❬ [a-zA-Z] _ [0-9] ' true false callout - ! ( ❭
FIRST+(<assign_op>) = ❬ = += -= ❭

FIRST(<method_call>) = ❬ [a-zA-Z] _ callout ❭
FOLLOW(<method_call>) = ❬ ; ) , ] { + - / * % < > <= >= == != && || ϵ ❭
FIRST+(<method_call>) = ❬ [a-zA-Z] _ callout ❭

FIRST(<expr_list>) = ❬ [a-zA-Z] _ [0-9] ' true false callout - ! ( ϵ ❭
FOLLOW(<expr_list>) = ❬ ) ❭
FIRST+(<expr_list>) = ❬ [a-zA-Z] _ [0-9] ' true false callout - ! ( ) ϵ ❭

FIRST(<expr_list_tail>) = ❬ , ϵ ❭
FOLLOW(<expr_list_tail>) = ❬ ) ❭
FIRST+(<expr_list_tail>) = ❬ ) , ϵ ❭

FIRST(<callout_arg_list>) = ❬ , ϵ ❭
FOLLOW(<callout_arg_list>) = ❬ ) ❭
FIRST+(<callout_arg_list>) = ❬ ) , ϵ ❭

FIRST(<method_name>) = ❬ [a-zA-Z] _ ❭
FOLLOW(<method_name>) = ❬ ( ❭
FIRST+(<method_name>) = ❬ [a-zA-Z] _ ❭

FIRST(<location>) = ❬ [a-zA-Z] _ ❭
FOLLOW(<location>) = ❬ = += -= ; ) , ] { + - / * % < > <= >= == != && || ϵ ❭
FIRST+(<location>) = ❬ [a-zA-Z] _ ❭

FIRST(<array_subscript_expr>) = ❬ [ ϵ ❭
FOLLOW(<array_subscript_expr>) = ❬ = += -= ; , + - / * % < > <= >= == != && || ) ] { ϵ ❭
FIRST+(<array_subscript_expr>) = ❬ = += -= ; , + - / * % < > <= >= == != && || ) [ ] { ϵ ❭

FIRST(<expr>) = ❬ [a-zA-Z] _ [0-9] ' true false callout - ! ( ❭
FOLLOW(<expr>) = ❬ ; ) , ] { + - / * % < > <= >= == != && || ϵ ❭
FIRST+(<expr>) = ❬ [a-zA-Z] _ [0-9] ' true false callout - ! ( ❭

FIRST(<expr_part>) = ❬ [a-zA-Z] _ [0-9] ' true false callout - ! ( ❭
FOLLOW(<expr_part>) = ❬ ; ) , ] { + - / * % < > <= >= == != && || ϵ ❭
FIRST+(<expr_part>) = ❬ [a-zA-Z] _ [0-9] ' true false callout - ! ( ❭

FIRST(<expr_end>) = ❬ + - / * % < > <= >= == != && || ϵ ❭
FOLLOW(<expr_end>) = ❬ ; ) , ] { ϵ ❭
FIRST+(<expr_end>) = ❬ + - / * % < > <= >= == != && || ; ) , ] { ϵ ❭

FIRST(<callout_arg>) = ❬ [a-zA-Z] _ [0-9] ' true false callout - ! ( " ❭
FOLLOW(<callout_arg>) = ❬ , ϵ ❭
FIRST+(<callout_arg>) = ❬ [a-zA-Z] _ [0-9] ' true false callout - ! ( " ❭

FIRST(<bin_op>) = ❬ + - / * % < > <= >= == != && || ❭
FOLLOW(<bin_op>) = ❬ [a-zA-Z] _ [0-9] ' true false callout - ! ( ❭
FIRST+(<bin_op>) = ❬ + - / * % < > <= >= == != && || ❭

FIRST(<arith_op>) = ❬ + - / * % ❭
FOLLOW(<arith_op>) = ❬ [a-zA-Z] _ [0-9] ' true false callout - ! ( ❭
FIRST+(<arith_op>) = ❬ + - / * % ❭

FIRST(<rel_op>) = ❬ < > <= >= ❭
FOLLOW(<rel_op>) = ❬ [a-zA-Z] _ [0-9] ' true false callout - ! ( ❭
FIRST+(<rel_op>) = ❬ < > <= >= ❭

FIRST(<eq_op>) = ❬ == != ❭
FOLLOW(<eq_op>) = ❬ [a-zA-Z] _ [0-9] ' true false callout - ! ( ❭
FIRST+(<eq_op>) = ❬ == != ❭

FIRST(<cond_op>) = ❬ && || ❭
FOLLOW(<cond_op>) = ❬ [a-zA-Z] _ [0-9] ' true false callout - ! ( ❭
FIRST+(<cond_op>) = ❬ && || ❭

FIRST(<literal>) = ❬ [0-9] ' true false ❭
FOLLOW(<literal>) = ❬ [a-zA-Z] _ callout if ( for return break continue { ϵ ❭
FIRST+(<literal>) = ❬ [0-9] ' true false ❭

FIRST(<id>) = ❬ [a-zA-Z] _ ❭
FOLLOW(<id>) = ❬ ( = [ , ; ( ϵ ❭
FIRST+(<id>) = ❬ [a-zA-Z] _ ❭

FIRST(<alpha_num_string>) = ❬ [0-9] [a-zA-Z] _ ϵ ❭
FOLLOW(<alpha_num_string>) = ❬ ( = [ , ; ϵ ❭
FIRST+(<alpha_num_string>) = ❬ [0-9] [a-zA-Z] _ ( = [ , ; ϵ ❭

FIRST(<alpha_num>) = ❬ [0-9] [a-z] [A-Z] _ ❭
FOLLOW(<alpha_num>) = ❬ [0-9] [a-zA-Z] _ ϵ ❭
FIRST+(<alpha_num>) = ❬ [0-9] [a-z] [A-Z] _ ❭

FIRST(<alpha>) = ❬ [a-zA-Z] _ ❭
FOLLOW(<alpha>) = ❬ [0-9] [a-zA-Z] _ ϵ ❭
FIRST+(<alpha>) = ❬ [a-zA-Z] _ ❭

FIRST(<digit>) = ❬ [0-9] ❭
FOLLOW(<digit>) = ❬ [0-9] ; ) , ] { + - / * % < > <= >= == != && || ϵ ❭
FIRST+(<digit>) = ❬ [0-9] ❭

FIRST(<hex_digit>) = ❬ [0-9] [a-f] [A-F] ❭
FOLLOW(<hex_digit>) = ❬ [0-9] [a-f] [A-F] ϵ ❭
FIRST+(<hex_digit>) = ❬ [0-9] [a-f] [A-F] ❭

FIRST(<int_literal>) = ❬ [0-9] ❭
FOLLOW(<int_literal>) = ❬ ; ) , ] { + - / * % < > <= >= == != && || ϵ ❭
FIRST+(<int_literal>) = ❬ [0-9] ❭

FIRST(<decimal_literal>) = ❬ [0-9] ❭
FOLLOW(<decimal_literal>) = ❬ ; ) , ] { + - / * % < > <= >= == != && || ϵ ❭
FIRST+(<decimal_literal>) = ❬ [0-9] ❭

FIRST(<digit_list>) = ❬ [0-9] ϵ ❭
FOLLOW(<digit_list>) = ❬ ; ) , ] { + - / * % < > <= >= == != && || ϵ ❭
FIRST+(<digit_list>) = ❬ [0-9] ; ) , ] { + - / * % < > <= >= == != && || ϵ ❭

FIRST(<hex_literal>) = ❬ 0x ❭
FOLLOW(<hex_literal>) = ❬ ; ) , ] { + - / * % < > <= >= == != && || ϵ ❭
FIRST(<hex_literal>) = ❬ 0x ❭

FIRST(<hex_digit_list>) = ❬ [0-9] [a-f] [A-F] ϵ ❭
FOLLOW(<hex_digit_list>) = ❬ ; ) , ] { + - / * % < > <= >= == != && || ϵ ❭
FIRST(<hex_digit_list>) = ❬ [0-9] [a-f] [A-F] ; ) , ] { + - / * % < > <= >= == != && || ϵ ❭

FIRST(<bool_literal>) = ❬ true false ❭
FOLLOW(<bool_literal>) = ❬ [a-zA-Z] _ callout if ( for return break continue { ϵ ❭
FIRST+(<bool_literal>) = ❬ true false ❭

FIRST(<char_literal>) = ❬ ' ❭
FOLLOW(<char_literal>) = ❬ [a-zA-Z] _ callout if ( for return break continue { ϵ ❭
FIRST+(<char_literal>) = ❬ ' ❭

FIRST(<string_literal>) = ❬ " ❭
FOLLOW(<string_literal>) = ❬ \ , ϵ ❭
FIRST+(<string_literal>) = ❬ " ❭
