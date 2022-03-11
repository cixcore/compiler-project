/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
%{  
    #include "parser.tab.h"
    #include "yylvallib.h"
    int get_line_number();
%}

%option yylineno

%%

"int"           return TK_PR_INT;
"float"         return TK_PR_FLOAT;
"bool"          return TK_PR_BOOL;
"char"          return TK_PR_CHAR;
"string"        return TK_PR_STRING; 

"if"            { yylval_string(PR, strdup(yytext)); return TK_PR_IF; }
"then"          return TK_PR_THEN; 
"else"          return TK_PR_ELSE; 

"while"         { yylval_string(PR, strdup(yytext)); return TK_PR_WHILE; } 
"do"            return TK_PR_DO; 

"input"         { yylval_string(PR, strdup(yytext)); return TK_PR_INPUT; }
"output"        { yylval_string(PR, strdup(yytext)); return TK_PR_OUTPUT; }
"return"        { yylval_string(PR, strdup(yytext)); return TK_PR_RETURN; }
"for"           { yylval_string(PR, strdup(yytext)); return TK_PR_FOR; } 
"break"         { yylval_string(PR, strdup(yytext)); return TK_PR_BREAK; } 
"continue"      { yylval_string(PR, strdup(yytext)); return TK_PR_CONTINUE; } 

"const"         return TK_PR_CONST; 
"static"        return TK_PR_STATIC; 
"foreach"       return TK_PR_FOREACH; 
"switch"        return TK_PR_SWITCH; 
"case"          return TK_PR_CASE; 
"class"         return TK_PR_CLASS; 
"private"       return TK_PR_PRIVATE; 
"public"        return TK_PR_PUBLIC; 
"protected"     return TK_PR_PROTECTED; 
"end"           return TK_PR_END; 
"default"       return TK_PR_DEFAULT; 

[\,\;\:\(\)\[\]\{\}\+\-\*\/\<\>\=\!\&\%\#\^\.\$\?\|]      return yytext[0];

"<=" { yylval_string(OC, strdup(yytext)); return TK_OC_LE; }
">=" { yylval_string(OC, strdup(yytext)); return TK_OC_GE; }
"==" { yylval_string(OC, strdup(yytext)); return TK_OC_EQ; }
"!=" { yylval_string(OC, strdup(yytext)); return TK_OC_NE; }
"&&" { yylval_string(OC, strdup(yytext)); return TK_OC_AND; }
"||" { yylval_string(OC, strdup(yytext)); return TK_OC_OR; }
">>" { yylval_string(OC, strdup(yytext)); return TK_OC_SL; }
"<<" { yylval_string(OC, strdup(yytext)); return TK_OC_SR; }    

"true" { yylval_bool(TRUE); return TK_LIT_TRUE; }
"false" { yylval_bool(FALSE); return TK_LIT_FALSE; }

[_a-zA-Z][_0-9a-zA-Z]*   { yylval_string(ID, strdup(yytext)); return TK_IDENTIFICADOR; }

[0-9]+\.[0-9]+([Ee](\+|\-)?[0-9]+)?     { yylval_float(atof(yytext)); return TK_LIT_FLOAT; }
[0-9]+                                  { yylval_int(atoi(yytext)); return TK_LIT_INT; }

'("\\".|[^'\n\t])'        {
    char* char_remove_sing_quotes = strdup(yytext);
    yylval_char(char_remove_sing_quotes[1]); 
    return TK_LIT_CHAR;
}

\"("\\".|[^"\n\t])*\"     {
    char* str_remove_quotes = strdup(&yytext[1]);
    str_remove_quotes[yyleng-2] = '\0';
    yylval_string(LIT_STR, str_remove_quotes); 
    return TK_LIT_STRING;
}

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
