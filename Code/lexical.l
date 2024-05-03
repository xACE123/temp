%{
    #include <stdio.h>
    #include <math.h>
    #include <string.h>
    #include "syntax.tab.h"
    #include "SyntaxTree.h"
    int yycolumn = 1;
    int prev_is_struct = 0;
    extern int lexicalError;
    extern int *lexErrorList;
    extern FILE* f;
    extern void addStruct(char *SID);
    extern int is_SID(char *cur); 
    extern void yyerror(char *msg);
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;

    int my_pow(int base, int exponent) {
        int result = 1;
        int i;
    
        if (exponent >= 0) {
            for (i = 0; i < exponent; i++) {
                result *= base;
            }
        } else {
            for (i = 0; i > exponent; i--) {
                result /= base;
            }
        }
    
        return result;
    }

    char* Oct2Dec(char *octString) {
        char *cur = strdup(octString);
        int octal = atoi(&cur[1]);

        int decimal = 0, i = 0;
    
        while (octal != 0) {
            decimal += (octal % 10) * my_pow(8, i);
            i++;
            octal /= 10;
        }

        char* str = (char *)malloc(sizeof(char) * 32);
        
        sprintf(str, "%d", decimal);

        return str;
    }
    char* Hex2Dec(char *hexString) {
        int decimal = 0, i = 0, val, len;
        char *hex = strdup(&hexString[2]); // dropping '0x'
    
        for (len = 0; hex[len] != '\0'; len++);
    
        for (i = len - 1; i >= 0; i--) {
            if (hex[i] >= '0' && hex[i] <= '9') {
                val = hex[i] - '0';
            } else if (hex[i] >= 'A' && hex[i] <= 'F') {
                val = hex[i] - 'A' + 10;
            } else if (hex[i] >= 'a' && hex[i] <= 'f') {
                val = hex[i] - 'a' + 10;
            } else {
                printf("Invalid hexadecimal number.\n");
                return 0;
            }
        
            decimal += val * my_pow(16, len - 1 - i);
        }
    
        char* str = (char *)malloc(sizeof(char) * 32);
        
        sprintf(str, "%d", decimal);

        return str;
    }
    enum token_type{
        t_semi,
        t_comma,
        t_assignop,
        t_relop,
        t_plus,
        t_minus,
        t_star,
        t_div,
        t_and,
        t_or,
        t_dot,
        t_not,
        t_type,
        t_lp,
        t_rp,
        t_lb,
        t_rb,
        t_lc,
        t_rc,
        t_struct,
        t_return,
        t_if,
        t_else,
        t_while,
        t_id,
        t_sid,
        t_int,
        t_float,
        t_null,
        t_error,
        t_newline
    };
    enum token_type prev_token = t_null;
    
%}
/* for activating the line number tracer */
%option yylineno


/*
* I leave INT, FLOAT and ID the last ones to impl.
* other tokens are defined in the sequence shown in the manual.
*/

ws [ \t] 
SEMI ";"
COMMA ","
ASSIGNOP "="
RELOP >|<|>=|<=|==|!=
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
DOT "."
NOT "!"
TYPE int|float
LP "("
RP ")"
LB "["
RB "]" 
LC "{"
RC "}"
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
letter [_a-zA-Z]
digit [0-9]
ID {letter}[_0-9a-zA-Z]*
/* INT 0|[1-9]+[0-9]*|0[0-7]+|0[xX][0-9a-fA-F]+ */
INT 0|[1-9]+[0-9]*
INT_O 0[0-7]+
INT_H 0[xX][0-9a-fA-F]+
FLOAT {digit}+"."{digit}+
/* \n|\r {op_printf("newline at line: %d\n", yylineno); yycolumn = 1; if (prev_token != t_semi && prev_token != t_else && prev_token != t_lc && prev_token != t_rc && prev_token != t_and && prev_token != t_or && prev_token != t_lp && prev_token != t_rp && prev_token != t_sid && prev_token != t_newline && prev_token != t_null) {lexErrorList[yylineno] = 2; yyerror("missing SEMI"); prev_token = t_newline; return SEMI;} else prev_token = t_newline;} restore the column idx */
%%
    op_printf("\033[33mprev_token: %d\033[0m\n", prev_token);
"//".* {yyerror("this is an one-line-comment, skipped");}
"/*" {
    yyerror("this is an multi-line-comment, skipped"); 
    char c;
    int flag = 1;
    while(flag) {
        c = input();
        if (c == '\n') {
            yyerror("this is an multi-line-comment, skipped"); 
            yycolumn = 1;
        }      
        else if (c == '*') {
            c = input();
            if (c == '/') {
                flag = 0;
            }
            else if (c == '\n') {
                yyerror("this is an multi-line-comment, skipped"); 
                yycolumn = 1;
            }
        }
    } 
}
\n|\r {yycolumn = 1; prev_token = t_newline;}
{ws}+ {;}
{SEMI} {op_printf(";\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "SEMI"); prev_token = t_semi; return SEMI;}
{COMMA} {op_printf(",\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "COMMA"); prev_token = t_comma; return COMMA;}
{ASSIGNOP} {op_printf("=\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "ASSIGNOP"); prev_token = t_assignop; return ASSIGNOP;}
{RELOP} {op_printf("RELOP\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "RELOP"); prev_token = t_relop; return RELOP;}
{PLUS} {op_printf("+\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "PLUS"); prev_token = t_plus; return PLUS;}
{MINUS} {op_printf("-\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "MINUS"); prev_token = t_minus; return MINUS;}
{STAR} {op_printf("*\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "STAR"); prev_token = t_star; return STAR;}
{DIV} {op_printf("/\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "DIV"); prev_token = t_div; return DIV;}
{AND} {op_printf("AND\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "AND"); prev_token = t_and; return AND;}
{OR} {op_printf("OR\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "OR"); prev_token = t_or; return OR;}
{DOT} {op_printf("DOT\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "DOT"); prev_token = t_dot; return DOT;}
{NOT} {op_printf("NOT\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "NOT"); prev_token = t_not; return NOT;}
{TYPE} {op_printf("TYPE\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_TYPE, yytext); prev_token = t_type; return TYPE;}
{LP} {op_printf("LP\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "LP"); prev_token = t_lp; return LP;}
{RP} {op_printf("RP\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "RP"); prev_token = t_rp; return RP;}
{LB} {op_printf("LB\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "LB"); prev_token = t_lb; return LB;}
{RB} {op_printf("RB\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "RB"); prev_token = t_rb; return RB;}
{LC} {op_printf("LC\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "LC"); prev_token = t_lc; return LC;}
{RC} {op_printf("RC\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "RC"); prev_token = t_rc; return RC;}
{STRUCT} {op_printf("struct\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "STRUCT"); prev_token = t_struct; return STRUCT;}
{RETURN} {op_printf("return\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "RETURN"); prev_token = t_return; return RETURN;}
{IF} {op_printf("if\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "IF"); prev_token = t_if; return IF;}
{ELSE} {op_printf("else\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "ELSE"); prev_token = t_else; return ELSE;}
{WHILE} {op_printf("while\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_OTHER, "WHILE"); prev_token = t_while; return WHILE;}
{ID} {op_printf("ID\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_ID, yytext); if(prev_token == t_struct) {addStruct(yytext); prev_token = t_sid; return SID;} else if (is_SID(yytext) == 1) {prev_token = t_sid; return SID;} else {prev_token = t_id; return ID;}}
{INT} {op_printf("INT\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_INT, yytext); prev_token = t_int; return INT;}
{INT_O} {op_printf("INT_O\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_INT, Oct2Dec(yytext)); prev_token = t_int; return INT;}
{INT_H} {op_printf("INT_H\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_INT, Hex2Dec(yytext)); prev_token = t_int; return INT;}
{FLOAT} {op_printf("FLOAT\n"); yylval.node = createLeafNode(1e9+7,yylineno, TOKEN_FLOAT, yytext); prev_token = t_float; return FLOAT;}
{digit}+{ID} {lexicalError = TRUE; printf("Error type A at Line %d: Illegal ID \"%s\", digits cannot be the first bit.\n", yylineno, yytext);lexErrorList[yylineno] = 1; prev_token = t_id; return ID;}
"."{digit}+ {lexicalError = TRUE; printf("Error type A at Line %d: Illegal FLOAT \"%s\", missing digits before dot.\n", yylineno, yytext); lexErrorList[yylineno] = 1; prev_token = t_float; return FLOAT;} 
{digit}+"." {lexicalError = TRUE; printf("Error type A at Line %d: Illegal FLOAT \"%s\", missing digits after dot.\n", yylineno, yytext); lexErrorList[yylineno] = 1; prev_token = t_float; return FLOAT;} 
. {
    lexicalError = TRUE;
    printf("Error type A at Line %d: Mysterious characters \"%s\"\n", 
        yylineno, yytext); 
    lexErrorList[yylineno] = 1;
    prev_token = t_error;
    // return SEMI;
}

%%

// int main(int argc, char** argv) {
    // if (argc > 1) {
    // if (!(yyin = fopen(argv[1], "r"))) {
        // perror(argv[1]);
        // return 1;
    // }
// }
    // yylex();
    // /* printf("%d\n",lines); */
    // return 0;
// }