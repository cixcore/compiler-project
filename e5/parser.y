/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
%{
    #include "yylvallib.h"
    #include "tree.h"
    #include "types.h"
    #include "ilocgen.h"
    int yylex(void);
    void yyerror (char const *s);
%}

%define parse.error verbose
%define parse.trace

%union{
	struct lex_value_t* valor_lexico;
	struct node* nodo;
    int integer_constant;
}

%token<valor_lexico> TK_PR_INT
%token<valor_lexico> TK_PR_FLOAT
%token<valor_lexico> TK_PR_BOOL
%token<valor_lexico> TK_PR_CHAR
%token<valor_lexico> TK_PR_STRING
%token<valor_lexico> TK_PR_IF
%token<valor_lexico> TK_PR_THEN
%token<valor_lexico> TK_PR_ELSE
%token<valor_lexico> TK_PR_WHILE
%token<valor_lexico> TK_PR_DO
%token<valor_lexico> TK_PR_INPUT
%token<valor_lexico> TK_PR_OUTPUT
%token<valor_lexico> TK_PR_RETURN
%token<valor_lexico> TK_PR_CONST
%token<valor_lexico> TK_PR_STATIC
%token<valor_lexico> TK_PR_FOREACH
%token<valor_lexico> TK_PR_FOR
%token<valor_lexico> TK_PR_SWITCH
%token<valor_lexico> TK_PR_CASE
%token<valor_lexico> TK_PR_BREAK
%token<valor_lexico> TK_PR_CONTINUE
%token<valor_lexico> TK_PR_CLASS
%token<valor_lexico> TK_PR_PRIVATE
%token<valor_lexico> TK_PR_PUBLIC
%token<valor_lexico> TK_PR_PROTECTED
%token<valor_lexico> TK_PR_END
%token<valor_lexico> TK_PR_DEFAULT
%token<valor_lexico> TK_OC_LE
%token<valor_lexico> TK_OC_GE
%token<valor_lexico> TK_OC_EQ
%token<valor_lexico> TK_OC_NE
%token<valor_lexico> TK_OC_AND
%token<valor_lexico> TK_OC_OR
%token<valor_lexico> TK_OC_SL
%token<valor_lexico> TK_OC_SR
%token<valor_lexico> TK_LIT_INT
%token<valor_lexico> TK_LIT_FLOAT
%token<valor_lexico> TK_LIT_FALSE
%token<valor_lexico> TK_LIT_TRUE
%token<valor_lexico> TK_LIT_CHAR
%token<valor_lexico> TK_LIT_STRING
%token<valor_lexico> TK_IDENTIFICADOR
%token<valor_lexico> TOKEN_ERRO

%type<nodo> program func command_block command dec_var_local var_local_list var_local literal attr input output func_call 
%type<nodo> shift shift_op return if_then_else_opt else_opt for_block while_block expr logical_or_or_and logical_and_or_bit_or bit_or_or_bit_and 
%type<nodo> bit_and_or_eq eq_neq_or_compare compare_or_sum sum_sub_or_mult_div_or_pow mult_div_or_pow pow_or_op unary_expr operand unsigned_literal opt_unary_operator
%type<nodo> func_call_parameter func_call_parameters_list

%type<integer_constant> type;

%%

program: 
    program dec_var_global  { $$ = $1; }
    | program func          { createRoot($1, $2); $$ = $2; codeConcat($1); }
    | %empty                { $$ = NULL; };

dec_var_global: static type var_global_list ';'  { global_init_types_and_add_to_scope($2); };
static: 
    TK_PR_STATIC 
    | %empty;
type: 
    TK_PR_INT       { $$ = INT_T; }
    | TK_PR_FLOAT   { $$ = FLOAT_T; }
    | TK_PR_CHAR    { $$ = CHAR_T; }
    | TK_PR_BOOL    { $$ = BOOL_T; }
    | TK_PR_STRING  { $$ = STRING_T; };
var_global_list: 
    var_global ',' var_global_list 
    | var_global;
var_global: 
    TK_IDENTIFICADOR                          { declare_id_entry_missing_type($1); free_lex_val($1); }
    | TK_IDENTIFICADOR '[' TK_LIT_INT ']'     { declare_vector_entry_missing_type($1, $3); free_lex_val($1); free_lex_val($3); };

func: static type TK_IDENTIFICADOR '('parameters')'{ create_func_entry_with_args($3, $2, FUNC_N); setMain($3); createLabelFunc($3); }'{'command_block'}' { $$ = createParentNode1Child(lexToNode($3), $9); funcDecCode($$, $3); pop_scope(true); };

parameters: 
    parameters_list 
    | %empty;
parameters_list: 
    parameter ',' parameters_list 
    | parameter;
parameter: const type TK_IDENTIFICADOR  { collect_arg($3, $2); free_lex_val($3); };
const: 
    TK_PR_CONST 
    | %empty;

command_block: 
    command ';' command_block   { $$ = connect($1, $3); codeConcatCommands($1,$3); }
    | %empty                    { $$ = NULL; };

command: 
    '{' { push_scope(); } command_block'}'     { $$ = $3; pop_scope(false); }
    | dec_var_local         { $$ = $1; }
    | attr                  { $$ = $1; }
    | input                 { $$ = $1; }
    | output                { $$ = $1; }
    | func_call             { $$ = $1; }
    | shift                 { $$ = $1; }
    | return                { $$ = $1; }
    | TK_PR_BREAK           { $$ = createLeaf($1); }
    | TK_PR_CONTINUE        { $$ = createLeaf($1); }
    | if_then_else_opt      { $$ = $1; }
    | for_block             { $$ = $1; }
    | while_block           { $$ = $1; };

dec_var_local: static const type var_local_list { $$ = $4; local_init_types_validate_and_add_to_scope($3); initCode($$); };
var_local_list: 
    var_local ',' var_local_list                { $$ = connect($1, $3); }
    | var_local                                 { $$ = $1; };
var_local: 
    TK_IDENTIFICADOR                                { declare_id_entry_missing_type($1); free_lex_val($1); $$ = NULL; }
    | TK_IDENTIFICADOR TK_OC_LE TK_IDENTIFICADOR    { declare_id_entry_missing_type_init_id($1, $3); $$ = createParentNode2Children(lexToNode($2), lexToNode($1), lexToNode($3)); loadVar($$->children[1], $3->token.str); }
    | TK_IDENTIFICADOR TK_OC_LE literal             { declare_id_entry_missing_type_init_lit($1, $3); $$ = createParentNode2Children(lexToNode($2), lexToNode($1), $3); loadLit($$->children[1]); };

literal: 
    '-' TK_LIT_INT                  { $$ = createNLeaf($2); $$->type = INT_T; add_symtable_lit($2); }
    | '+' TK_LIT_INT                { $$ = createLeaf($2); $$->type = INT_T; add_symtable_lit($2); }
    |  TK_LIT_INT                   { $$ = createLeaf($1); $$->type = INT_T; add_symtable_lit($1); }
    | '+' TK_LIT_FLOAT              { $$ = createLeaf($2); $$->type = FLOAT_T; add_symtable_lit($2); }
    | '-' TK_LIT_FLOAT              { $$ = createNLeaf($2); $$->type = FLOAT_T; add_symtable_lit($2); }
    | TK_LIT_FLOAT                  { $$ = createLeaf($1); $$->type = FLOAT_T; add_symtable_lit($1); }
    | TK_LIT_FALSE                  { $$ = createLeaf($1); $$->type = BOOL_T; add_symtable_lit($1); }
    | TK_LIT_TRUE                   { $$ = createLeaf($1); $$->type = BOOL_T; add_symtable_lit($1); }
    | TK_LIT_CHAR                   { $$ = createLeaf($1); $$->type = CHAR_T; add_symtable_lit($1); }
    | TK_LIT_STRING                 { $$ = createLeaf($1); $$->type = STRING_T; add_symtable_lit($1); };


attr: 
    TK_IDENTIFICADOR '=' expr                   { $$ = createParentNode2Children(lexToNode(lexValueFromSC('=')), lexToNode($1), $3); validate_attr_expr($1, VAR_N, $3); }
    | TK_IDENTIFICADOR'[' expr ']' '=' expr     { $$ = createParentNode2Children(lexToNode(lexValueFromSC('=')), createParentNode2Children(lexToNode(lexValueFromOC("[]")), lexToNode($1), $3), $6); 
                                                    validate_attr_expr($1, VEC_N, $6); };

input:  TK_PR_INPUT TK_IDENTIFICADOR     { $$ = createParentNode1Child(lexToNode($1), createLeaf($2)); validate_input_id($2); };
output: 
    TK_PR_OUTPUT TK_IDENTIFICADOR        { $$ = createParentNode1Child(lexToNode($1), createLeaf($2)); validate_output_id($2); }
    | TK_PR_OUTPUT literal               { $$ = createParentNode1Child(lexToNode($1), $2); validate_output_lit($2->type); };

func_call: TK_IDENTIFICADOR '('func_call_parameters_list')'     { $$ = createParentNode1Child(createFuncCallNode($1), $3); 
                                                                    $$->type = get_type_or_err_undeclared_symbol(*($1), FUNC_N);
                                                                    validate_func_cal_parameters($1); funcCallCode($$); };
func_call_parameters_list: 
	func_call_parameter ',' func_call_parameters_list 	{ $$ = connect($1, $3); collect_param($1); }
	| func_call_parameter					            { $$ = $1; collect_param($1);};
func_call_parameter: 
	expr 							{ $$ = $1; $$->type = $1->type; }
	| TK_LIT_CHAR 					{ $$ = createLeaf($1); $$->type = CHAR_T; }
	| TK_LIT_STRING 				{ $$ = createLeaf($1); $$->type = STRING_T; }
	| TK_LIT_FALSE 					{ $$ = createLeaf($1); $$->type = BOOL_T; }
	| TK_LIT_TRUE 					{ $$ = createLeaf($1); $$->type = BOOL_T; }
	| %empty						{ $$ = NULL; };

shift: 
    TK_IDENTIFICADOR shift_op TK_LIT_INT                    { $$ = createParentNode2Children($2, lexToNode($1), lexToNode($3)); validate_shift($3); }
    | TK_IDENTIFICADOR '[' expr ']' shift_op TK_LIT_INT     { $$ = createParentNode2Children($5, createParentNode2Children(lexToNode(lexValueFromOC("[]")), lexToNode($1), $3), lexToNode($6)); validate_shift($6); };

shift_op: 
    TK_OC_SL        { $$ = createLeaf($1); }
    | TK_OC_SR      { $$ = createLeaf($1); };

return: TK_PR_RETURN expr   { $$ = createParentNode1Child(lexToNode($1), $2); validate_return($2->type); returnCode($$); };

if_then_else_opt: TK_PR_IF'('expr')' '{' { push_scope(); } command_block '}' else_opt { $$ = createParentNode3Children(lexToNode($1), $3, $7, $9); ifCode($$); pop_scope(false); };
else_opt: 
    TK_PR_ELSE '{' { push_scope(); } command_block'}'  { $$ = $4; pop_scope(false); }
    | %empty                        { $$ = NULL; };

for_block: 
    TK_PR_FOR'(' attr ':' expr ':' attr ')' '{' { push_scope(); } command_block '}'   { $$ = createParentNode4Children(lexToNode($1), $3, $5, $7, $11);  pop_scope(false); codeFor($$); };

while_block: TK_PR_WHILE'(' expr ')' TK_PR_DO '{' { push_scope(); } command_block '}' { $$ = createParentNode2Children(lexToNode($1), $3, $8); pop_scope(false); codeWhile($$); };

expr: 
    logical_or_or_and '?' expr ':' expr     { $$ = createParentNode3Children(lexToNode(lexValueFromOC("?:")), $1, $3, $5); ; $$->type = get_inferred_type_node($3, $5); codeTernaryOp($$); }
    | logical_or_or_and                     { $$ = $1; };

logical_or_or_and: 
    logical_or_or_and TK_OC_OR logical_and_or_bit_or { $$ = createParentNode2Children(lexToNode($2), $1, $3); validate_node_type($1, $3); $$->type = BOOL_T; codeOr($$);}
    | logical_and_or_bit_or                          { $$ = $1; };
logical_and_or_bit_or: 
    logical_and_or_bit_or TK_OC_AND bit_or_or_bit_and { $$ = createParentNode2Children(lexToNode($2), $1, $3); validate_node_type($1, $3); $$->type = BOOL_T; codeAnd($$);}
    | bit_or_or_bit_and                               { $$ = $1; };  
bit_or_or_bit_and: 
    bit_or_or_bit_and '|' bit_and_or_eq             { $$ = createParentNode2Children(lexToNode(lexValueFromSC('|')), $1, $3); validate_node_type($1, $3); $$->type = BOOL_T; }
    | bit_and_or_eq                                 { $$ = $1; };  
bit_and_or_eq: 
    bit_and_or_eq '&' eq_neq_or_compare             { $$ = createParentNode2Children(lexToNode(lexValueFromSC('&')), $1, $3); validate_node_type($1, $3); $$->type = BOOL_T; }
    | eq_neq_or_compare                             { $$ = $1; };  
eq_neq_or_compare: 
    eq_neq_or_compare TK_OC_EQ compare_or_sum       { $$ = createParentNode2Children(lexToNode($2), $1, $3); validate_node_type($1, $3); $$->type = BOOL_T; codeRelOp($$, CMP_EQ);}
    | eq_neq_or_compare TK_OC_NE compare_or_sum     { $$ = createParentNode2Children(lexToNode($2), $1, $3); validate_node_type($1, $3); $$->type = BOOL_T; codeRelOp($$, CMP_NE);}
    | compare_or_sum                                { $$ = $1; };
compare_or_sum: 
    compare_or_sum TK_OC_LE sum_sub_or_mult_div_or_pow       { $$ = createParentNode2Children(lexToNode($2), $1, $3); validate_node_type($1, $3); $$->type = BOOL_T;codeRelOp($$, CMP_LE); }
    | compare_or_sum TK_OC_GE sum_sub_or_mult_div_or_pow     { $$ = createParentNode2Children(lexToNode($2), $1, $3); validate_node_type($1, $3); $$->type = BOOL_T;codeRelOp($$, CMP_GE); } 
    | compare_or_sum '<' sum_sub_or_mult_div_or_pow          { $$ = createParentNode2Children(lexToNode(lexValueFromSC('<')), $1, $3); validate_node_type($1, $3); $$->type = BOOL_T;codeRelOp($$, CMP_LT); }
    | compare_or_sum '>' sum_sub_or_mult_div_or_pow          { $$ = createParentNode2Children(lexToNode(lexValueFromSC('>')), $1, $3); validate_node_type($1, $3); $$->type = BOOL_T;codeRelOp($$, CMP_GT); }  
    | sum_sub_or_mult_div_or_pow                             { $$ = $1; };  

sum_sub_or_mult_div_or_pow: 
    sum_sub_or_mult_div_or_pow '+' mult_div_or_pow       { $$ = createParentNode2Children(lexToNode(lexValueFromSC('+')), $1, $3); $$->type = get_inferred_type_node($1, $3); codeBinaryOp($$, ADD); }
    | sum_sub_or_mult_div_or_pow '-' mult_div_or_pow     { $$ = createParentNode2Children(lexToNode(lexValueFromSC('-')), $1, $3); $$->type = get_inferred_type_node($1, $3); codeBinaryOp($$, SUB);}
    | mult_div_or_pow                                    { $$ = $1; };  
mult_div_or_pow: 
    mult_div_or_pow '*' pow_or_op       { $$ = createParentNode2Children(lexToNode(lexValueFromSC('*')), $1, $3); $$->type = get_inferred_type_node($1, $3); codeBinaryOp($$, MULT); }
    | mult_div_or_pow '/' pow_or_op     { $$ = createParentNode2Children(lexToNode(lexValueFromSC('/')), $1, $3); $$->type = get_inferred_type_node($1, $3); codeBinaryOp($$, DIV);}
    | mult_div_or_pow '%' pow_or_op     { $$ = createParentNode2Children(lexToNode(lexValueFromSC('%')), $1, $3); $$->type = get_inferred_type_node($1, $3); }
    | pow_or_op                         { $$ = $1; };
pow_or_op: 
    pow_or_op '^' unary_expr            { $$ = createParentNode2Children(lexToNode(lexValueFromSC('^')), $1, $3); $$->type = get_inferred_type_node($1, $3); }
    | unary_expr                        { $$ = $1; };

unary_expr: 
    opt_unary_operator unary_expr   { $$ = connect($1, $2); #codeUnaryOp($$, $2); }
    | operand                       { $$ = $1; $$->type = $1->type; };
operand: 
    unsigned_literal                    { $$ = $1; $$->type = $1->type; loadLit($$); }
    | TK_IDENTIFICADOR                  { $$ = createLeaf($1); $$->type = get_type_or_err_undeclared_symbol(*($1), VAR_N);#loadVar($$, $1->token.str); }
    | TK_IDENTIFICADOR '[' expr ']'     { $$ = createParentNode2Children(lexToNode(lexValueFromOC("[]")), lexToNode($1), $3); $$->type = get_type_or_err_undeclared_symbol(*($1), VEC_N);#loadVar($$, $1->token.str); }
    | func_call                         { $$ = $1; $$->type = $1->type;}
    | '('expr')'                        { $$ = $2; $$->type = $2->type;};
unsigned_literal: 
    TK_LIT_INT                   { $$ = createLeaf($1); $$->type = INT_T; add_symtable_lit($1); }
    | TK_LIT_FLOAT               { $$ = createLeaf($1); $$->type = FLOAT_T; add_symtable_lit($1); };

opt_unary_operator: 
    '+'                    { $$ = lexToNode(lexValueFromSC('+')); }
    | '-'                  { $$ = lexToNode(lexValueFromSC('-')); }
    | '!'                  { $$ = lexToNode(lexValueFromSC('!')); }
    | '&'                  { $$ = lexToNode(lexValueFromSC('&')); }
    | '*'                  { $$ = lexToNode(lexValueFromSC('*')); }
    | '?'                  { $$ = lexToNode(lexValueFromSC('?')); }
    | '#'                  { $$ = lexToNode(lexValueFromSC('#')); };


%%

void yyerror (char const *s) {
    printf("%d: %s\n", get_line_number(), s);
}
