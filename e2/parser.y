%{
    #include <stdio.h>
    extern int get_line_number();
    int yylex(void);
    void yyerror (char const *s);
%}

%token TK_PR_INT
%token TK_PR_FLOAT
%token TK_PR_BOOL
%token TK_PR_CHAR
%token TK_PR_STRING
%token TK_PR_IF
%token TK_PR_THEN
%token TK_PR_ELSE
%token TK_PR_WHILE
%token TK_PR_DO
%token TK_PR_INPUT
%token TK_PR_OUTPUT
%token TK_PR_RETURN
%token TK_PR_CONST
%token TK_PR_STATIC
%token TK_PR_FOREACH
%token TK_PR_FOR
%token TK_PR_SWITCH
%token TK_PR_CASE
%token TK_PR_BREAK
%token TK_PR_CONTINUE
%token TK_PR_CLASS
%token TK_PR_PRIVATE
%token TK_PR_PUBLIC
%token TK_PR_PROTECTED
%token TK_PR_END
%token TK_PR_DEFAULT
%token TK_OC_LE
%token TK_OC_GE
%token TK_OC_EQ
%token TK_OC_NE
%token TK_OC_AND
%token TK_OC_OR
%token TK_OC_SL
%token TK_OC_SR
%token TK_LIT_INT
%token TK_LIT_FLOAT
%token TK_LIT_FALSE
%token TK_LIT_TRUE
%token TK_LIT_CHAR
%token TK_LIT_STRING
%token TK_IDENTIFICADOR
%token TOKEN_ERRO

%%

program: progama dec_var_global | program func | %empty;

dec_var_global: static type var_global_list ';';
static: TK_PR_STATIC | %empty;
type: TK_PR_INT | TK_PR_FLOAT | TK_PR_CHAR | TK_PR_BOOL | TK_PR_STRING;
var_global_list: var_global ',' var_global_list | var_global;
var_global: TK_IDENTIFICADOR | TK_IDENTIFICADOR '[' TK_LIT_INT ']';

func: static TK_IDENTIFICADOR '('parameters')' '{'command_block'}';
parameters: parameters_list | %empty;
parameters_list: parameter ',' parameters_list | parameter;
parameter: const type TK_IDENTIFICADOR;
const: TK_PR_CONST | %empty;
command_block: command ';' command_block | %empty;
command: dec_var_local | attr | input | output | func_call | shift | return | TK_PR_BREAK | TK_PR_CONTINUE | if_then_else_opt | for_block | while_block;

dec_var_local: static const type var_local_list;
var_local_list: var_local ',' var_local_list | var_local;
var_local: TK_IDENTIFICADOR | TK_IDENTIFICADOR TK_OC_LE TK_IDENTIFICADOR | TK_IDENTIFICADOR TK_OC_LE literal;
literal: opt_sign TK_LIT_INT | opt_sign TK_LIT_FLOAT | TK_LIT_FALSE | TK_LIT_TRUE | TK_LIT_CHAR | TK_LIT_STRING;
opt_sign: '+' | '-' | %empty


attr: TK_IDENTIFICADOR '=' expr | TK_IDENTIFICADOR'[' expr ']' '=' expr;

input:  TK_PR_INPUT TK_IDENTIFICADOR;
output: TK_PR_OUTPUT TK_IDENTIFICADOR | TK_PR_OUTPUT literal;

func_call: TK_IDENTIFICADOR '('func_call_parameters_list')';
func_call_parameters_list: func_call_parameter ',' func_call_parameter_list | func_call_parameter;
func_call_parameter: TK_IDENTIFICADOR | literal | expr | %empty;

shift: TK_IDENTIFICADOR shift_op TK_LIT_INT | TK_IDENTIFICADOR'[' expr ']' shift_op TK_LIT_INT;
shift_op: TK_OC_SL | TK_OC_SR;

return: TK_PR_RETURN expr;

%%

void yyerror (char const *s) {
    printf("%s\n", s);
}