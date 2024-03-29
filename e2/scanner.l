/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
%{  
    #include "parser.tab.h"
    int get_line_number();
%}

%option yylineno

%%

"int"           return TK_PR_INT;
"float"         return TK_PR_FLOAT;
"bool"          return TK_PR_BOOL;
"char"          return TK_PR_CHAR;
"string"        return TK_PR_STRING; 
"if"            return TK_PR_IF; 
"then"          return TK_PR_THEN; 
"else"          return TK_PR_ELSE; 
"while"         return TK_PR_WHILE; 
"do"            return TK_PR_DO; 
"input"         return TK_PR_INPUT; 
"output"        return TK_PR_OUTPUT; 
"return"        return TK_PR_RETURN; 
"const"         return TK_PR_CONST; 
"static"        return TK_PR_STATIC; 
"foreach"       return TK_PR_FOREACH; 
"for"           return TK_PR_FOR; 
"switch"        return TK_PR_SWITCH; 
"case"          return TK_PR_CASE; 
"break"         return TK_PR_BREAK; 
"continue"      return TK_PR_CONTINUE; 
"class"         return TK_PR_CLASS; 
"private"       return TK_PR_PRIVATE; 
"public"        return TK_PR_PUBLIC; 
"protected"     return TK_PR_PROTECTED; 
"end"           return TK_PR_END; 
"default"       return TK_PR_DEFAULT; 

[\,\;\:\(\)\[\]\{\}\+\-\*\/\<\>\=\!\&\%\#\^\.\$\?\|]      return yytext[0];

"<="    return TK_OC_LE;
">="    return TK_OC_GE;
"=="    return TK_OC_EQ;
"!="    return TK_OC_NE;   
"&&"    return TK_OC_AND;
"||"    return TK_OC_OR;
">>"    return TK_OC_SL;
"<<"    return TK_OC_SR;     

"true"      return TK_LIT_TRUE;
"false"     return TK_LIT_FALSE;

[_a-zA-Z][_0-9a-zA-Z]*           return TK_IDENTIFICADOR;

[0-9]+\.[0-9]+([Ee](\+|\-)?[0-9]+){0,1}     return TK_LIT_FLOAT; 
[0-9]+                                      return TK_LIT_INT; 

'("\\".|[^'\n\t])'        return TK_LIT_CHAR;
\"("\\".|[^"\n\t])*\"       return TK_LIT_STRING;

"/*"(\n|.)*"*/"   {}
"//".*            {}

[ \t]  {}

\n {}

"/*"    return TOKEN_ERRO;
.       return TOKEN_ERRO;


%%

int get_line_number(){
    return yylineno;
}
