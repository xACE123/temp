%locations
%define parse.error verbose
%{
    #define YYDEBUG 1
    // yydebug = 1;
    #include "SyntaxTree.h"
    #include <stdio.h>
    #include "lex.yy.c"    
    TreeNode* root;
    extern int syntaxError;
    extern int lexicalError;
    extern int* lexErrorList;
    void yyerror(char* msg);
    #define YYERROR_VERBOSE 1
    #define COLOR_ENABLED 0
    // a customized printf function for printing verbose info
    void err_printf(const char* format, ...) {
        if (FALSE) {
            if(COLOR_ENABLED) printf("\033[31m");
            printf("[%d] \"%s\": ", yylineno, yytext);
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
            if(COLOR_ENABLED) printf("\033[0m");
        }
    }
%}


/* types */
%union{
    TreeNode* node;
}


/* Tokens */
%token<node> INT
%token<node> FLOAT
%token<node> ID
%token<node> SEMI
%token<node> COMMA
%token<node> ASSIGNOP
%token<node> RELOP 
%token<node> PLUS
%token<node> MINUS  
%token<node> STAR 
%token<node> DIV
%token<node> AND
%token<node> OR
%token<node> DOT
%token<node> NOT
%token<node> TYPE
%token<node> LP
%token<node> RP 
%token<node> LB
%token<node> RB
%token<node> LC
%token<node> RC
%token<node> STRUCT
%token<node> RETURN
%token<node> IF
%token<node> ELSE 
%token<node> WHILE

/* Added by yunzinan*/
%token<node> SID

/* Non-terminals */

%type <node> Program
%type <node> ExtDefList 
%type <node> ExtDef
%type <node> ExtDecList

%type <node> Specifier
%type <node> StructSpecifier
%type <node> OptTag
%type <node> Tag

%type <node> VarDec
%type <node> FunDec
%type <node> VarList
%type <node> ParamDec

%type <node> CompSt
%type <node> StmtList
%type <node> Stmt

%type <node> DefList 
%type <node> Def
%type <node> DecList
%type <node> Dec

%type <node> Exp
%type <node> Args

/*added by yunzinan*/


/* associativity */
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT
%left LB RB
%left LP RP
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE


%start Program

%%

/* 
    typedef struct YYLTYPE {
        int first_line;
        int first_column;
        int last_line; 
        int last_column;
    }
    e.g.: @$.first_line, @2.first_column.
*/

/* High-level Definitions */
Program:            ExtDefList                              {$$ = createInternalNode(1,@$.first_line, NON_TERMINAL, "Program", 1, $1); root = $$;}
    ;                      
ExtDefList:         ExtDef ExtDefList                       {$$ = createInternalNode(2,@$.first_line, NON_TERMINAL, "ExtDefList", 2, $1, $2);} 
    |                                                       {$$ = createLeafNode(3,@$.first_line, EMPTY_PRODUCTION, "ExtDefList");}
    ;
ExtDef:             Specifier ExtDecList SEMI               {$$ = createInternalNode(4,@$.first_line, NON_TERMINAL, "ExtDef", 3, $1, $2, $3);}
    |               Specifier SEMI                          {$$ = createInternalNode(5,@$.first_line, NON_TERMINAL, "ExtDef", 2, $1, $2);}               
    |               Specifier FunDec CompSt                 {$$ = createInternalNode(6,@$.first_line, NON_TERMINAL, "ExtDef", 3, $1, $2, $3);}
    |               Specifier FunDec SEMI                   {$$ = createInternalNode(7,@$.first_line, NON_TERMINAL, "ExtDef", 3, $1, $2, $3);}         
    ;
ExtDecList:         VarDec                                  {$$ = createInternalNode(8,@$.first_line, NON_TERMINAL, "ExtDecList", 1, $1);}
    |               VarDec COMMA ExtDecList                 {$$ = createInternalNode(9,@$.first_line, NON_TERMINAL, "ExtDecList", 3, $1, $2, $3);}
    ;

/* Specifiers */       
Specifier:          TYPE                                    {$$ = createInternalNode(10,@$.first_line, NON_TERMINAL, "Specifier", 1, $1);}
    |               StructSpecifier                         {$$ = createInternalNode(11,@$.first_line, NON_TERMINAL, "Specifier", 1, $1);}
    ;
StructSpecifier:    STRUCT OptTag LC DefList RC             {$$ = createInternalNode(12,@$.first_line, NON_TERMINAL, "StructSpecifier", 5, $1, $2, $3, $4, $5);}
    |               STRUCT Tag                              {$$ = createInternalNode(13,@$.first_line, NON_TERMINAL, "StructSpecifier", 2, $1, $2);}
    |               SID                                     {syntaxError = TRUE; yyerrok; yyerror("missing STRUCT"); err_printf("StructSpecifier -> Tag\n");}
    ;
OptTag:             SID                                     {$$ = createInternalNode(14,@$.first_line, NON_TERMINAL, "OptTag", 1, $1);}
    |                                                       {$$ = createLeafNode(15,@$.first_line, EMPTY_PRODUCTION, "OptTag");}
    ;
Tag:                SID                                     {$$ = createInternalNode(16,@$.first_line, NON_TERMINAL, "Tag", 1, $1);}
    ;

/* Declarators */       
VarDec:             ID                                      {$$ = createInternalNode(17,@$.first_line, NON_TERMINAL, "VarDec", 1, $1);} 
    |               SID                                     {$$ = createInternalNode(18,@$.first_line, NON_TERMINAL, "VarDec", 1, $1);} 
    |               VarDec LB INT RB                        {$$ = createInternalNode(119,@$.first_line, NON_TERMINAL, "VarDec", 4, $1, $2, $3, $4);}
    /* |               error RB                                {syntaxError = TRUE; yyerrok; err_printf("VarDec -> error RB\n");} */
    ;
FunDec:             ID LP VarList RP                        {$$ = createInternalNode(19,@$.first_line, NON_TERMINAL, "FunDec", 4, $1, $2, $3, $4);}
    |               SID LP VarList RP                       {$$ = createInternalNode(20,@$.first_line, NON_TERMINAL, "FunDec", 4, $1, $2, $3, $4);}
    |               ID LP RP                                {$$ = createInternalNode(21,@$.first_line, NON_TERMINAL, "FunDec", 3, $1, $2, $3);}
    |               SID LP RP                               {$$ = createInternalNode(22,@$.first_line, NON_TERMINAL, "FunDec", 3, $1, $2, $3);}
    |               error RP                                {syntaxError = TRUE; yyerrok; err_printf("FunDec -> error RP\n");}
    ;
VarList:            ParamDec COMMA VarList                  {$$ = createInternalNode(23,@$.first_line, NON_TERMINAL, "VarList", 3, $1, $2, $3);}
    |               ParamDec                                {$$ = createInternalNode(24,@$.first_line, NON_TERMINAL, "VarList", 1, $1);}   
    ;
ParamDec:           Specifier VarDec                        {$$ = createInternalNode(25,@$.first_line, NON_TERMINAL, "ParamDec", 2, $1, $2);}
    ;

/* Statements */       
CompSt:             LC DefList StmtList RC                  {$$ = createInternalNode(26,@$.first_line, NON_TERMINAL, "CompSt", 4, $1, $2, $3, $4);}
    |               error RC                                {syntaxError = TRUE; yyerrok; err_printf("Compst -> error RC\n");}
    ;
StmtList:           Stmt StmtList                           {$$ = createInternalNode(27,@$.first_line, NON_TERMINAL, "StmtList", 2, $1, $2);}
    |                                                       {$$ = createLeafNode(28,@$.first_line, EMPTY_PRODUCTION, "StmtList");}
    ;
Stmt:               Exp SEMI                                {$$ = createInternalNode(29,@$.first_line, NON_TERMINAL, "Stmt", 2, $1, $2);}
    |               CompSt                                  {$$ = createInternalNode(30,@$.first_line, NON_TERMINAL, "Stmt", 1, $1);}
    |               RETURN Exp SEMI                         {$$ = createInternalNode(31,@$.first_line, NON_TERMINAL, "Stmt", 3, $1, $2, $3);}
    |               IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$ = createInternalNode(32,@$.first_line, NON_TERMINAL, "Stmt", 5, $1, $2, $3, $4, $5);}
    |               IF LP Exp RP Stmt ELSE Stmt             {$$ = createInternalNode(33,@$.first_line, NON_TERMINAL, "Stmt", 7, $1, $2, $3, $4, $5, $6, $7);}
    |               WHILE LP Exp RP Stmt                    {$$ = createInternalNode(34,@$.first_line, NON_TERMINAL, "Stmt", 5, $1, $2, $3, $4, $5);}
    |               RETURN SEMI                             {syntaxError = TRUE; yyerrok; yyerror("missing Exp after RETURN"); err_printf("Stmt -> RETURN SEMI\n");}
    |               WHILE LP RP Stmt                        {syntaxError = TRUE; yyerrok; yyerror("missing Exp after RETURN"); err_printf("Stmt -> WHILE LP RP Stmt\n");}
    |               WHILE LP error Stmt                     {syntaxError = TRUE; yyerrok; err_printf("Stmt -> WHILT LP error RP Stmt\n");}
    |               error SEMI                              {syntaxError = TRUE; yyerrok; err_printf("Stmt -> error SEMI\n");}
    |               error Stmt                              {syntaxError = TRUE; yyerrok; err_printf("Stmt -> error Stmt\n");}
    /* |               WHILE LP error                          {syntaxError = TRUE; yyclearin; yyerrok; err_printf("Stmt -> WHILE LP error\n");} */
    ;

/* Local Definitions */       
DefList:            Def DefList                             {$$ = createInternalNode(35,@$.first_line, NON_TERMINAL, "DefList", 2, $1, $2);}
    |                                                       {$$ = createLeafNode(36,@$.first_line, EMPTY_PRODUCTION, "DefList");}
    ;
Def:                Specifier DecList SEMI                  {$$ = createInternalNode(37,@$.first_line, NON_TERMINAL, "Def", 3, $1, $2, $3);}
    |               Specifier error SEMI                    {syntaxError = TRUE; yyerrok; err_printf("Def -> Specifier error SEMI\n");}
    |               error Def                               {syntaxError = TRUE; yyerrok; err_printf("Def -> error Def\n");}
    /* |               Tag DecList SEMI                        {syntaxError = TRUE; yyclearin; yyerrok; yyerror("missing STRUCT"); err_printf("Def -> error DecList SEMI\n");} */
    ;
DecList:            Dec                                     {$$ = createInternalNode(38,@$.first_line, NON_TERMINAL, "DecList", 1, $1);}
    |               Dec COMMA DecList                       {$$ = createInternalNode(39,@$.first_line, NON_TERMINAL, "DecList", 3, $1, $2, $3);}
    ;
Dec:                VarDec                                  {$$ = createInternalNode(40,@$.first_line, NON_TERMINAL, "Dec", 1, $1);}
    |               VarDec ASSIGNOP Exp                     {$$ = createInternalNode(41,@$.first_line, NON_TERMINAL, "Dec", 3, $1, $2, $3);}
    ;





/* Expressions */       
Exp:                Exp ASSIGNOP Exp                        {$$ = createInternalNode(42,@$.first_line, NON_TERMINAL, "Exp", 3, $1, $2, $3);}
    |               Exp AND Exp                             {$$ = createInternalNode(43,@$.first_line, NON_TERMINAL, "Exp", 3, $1, $2, $3);}
    |               Exp OR Exp                              {$$ = createInternalNode(44,@$.first_line, NON_TERMINAL, "Exp", 3, $1, $2, $3);}
    |               Exp RELOP Exp                           {$$ = createInternalNode(45,@$.first_line, NON_TERMINAL, "Exp", 3, $1, $2, $3);}
    |               Exp PLUS  Exp                           {$$ = createInternalNode(46,@$.first_line, NON_TERMINAL, "Exp", 3, $1, $2, $3);}
    |               Exp MINUS Exp                           {$$ = createInternalNode(47,@$.first_line, NON_TERMINAL, "Exp", 3, $1, $2, $3);}
    |               Exp STAR  Exp                           {$$ = createInternalNode(48,@$.first_line, NON_TERMINAL, "Exp", 3, $1, $2, $3);}
    |               Exp DIV   Exp                           {$$ = createInternalNode(49,@$.first_line, NON_TERMINAL, "Exp", 3, $1, $2, $3);}
    |               LP Exp RP                               {$$ = createInternalNode(50,@$.first_line, NON_TERMINAL, "Exp", 3, $1, $2, $3);}
    |               MINUS Exp                               {$$ = createInternalNode(51,@$.first_line, NON_TERMINAL, "Exp", 2, $1, $2);}
    |               NOT Exp                                 {$$ = createInternalNode(52,@$.first_line, NON_TERMINAL, "Exp", 2, $1, $2);}
    |               ID LP Args RP                           {$$ = createInternalNode(53,@$.first_line, NON_TERMINAL, "Exp", 4, $1, $2, $3, $4);}
    |               SID LP Args RP                          {$$ = createInternalNode(54,@$.first_line, NON_TERMINAL, "Exp", 4, $1, $2, $3, $4);}
    |               ID LP RP                                {$$ = createInternalNode(55,@$.first_line, NON_TERMINAL, "Exp", 3, $1, $2, $3);}
    |               SID LP RP                               {$$ = createInternalNode(56,@$.first_line, NON_TERMINAL, "Exp", 3, $1, $2, $3);}
    |               Exp LB Exp RB                           {$$ = createInternalNode(57,@$.first_line, NON_TERMINAL, "Exp", 4, $1, $2, $3, $4);} 
    |               Exp DOT ID                              {$$ = createInternalNode(58,@$.first_line, NON_TERMINAL, "Exp", 3, $1, $2, $3);}
    |               Exp DOT SID                             {$$ = createInternalNode(59,@$.first_line, NON_TERMINAL, "Exp", 3, $1, $2, $3);}
    |               ID                                      {$$ = createInternalNode(60,@$.first_line, NON_TERMINAL, "Exp", 1, $1);}
    |               SID                                     {$$ = createInternalNode(61,@$.first_line, NON_TERMINAL, "Exp", 1, $1);}
    |               INT                                     {$$ = createInternalNode(62,@$.first_line, NON_TERMINAL, "Exp", 1, $1);}
    |               FLOAT                                   {$$ = createInternalNode(63,@$.first_line, NON_TERMINAL, "Exp", 1, $1);}
    |               Exp ASSIGNOP error                      {syntaxError = TRUE; yyerrok; err_printf("Exp error\n");}
    |               Exp AND error                           {syntaxError = TRUE; yyerrok; err_printf("Exp error\n");}
    |               Exp OR error                            {syntaxError = TRUE; yyerrok; err_printf("Exp error\n");}
    |               Exp RELOP error                         {syntaxError = TRUE; yyerrok; err_printf("Exp error\n");}
    |               Exp PLUS error                          {syntaxError = TRUE; yyerrok; err_printf("Exp error\n");}
    |               Exp MINUS error                         {syntaxError = TRUE; yyerrok; err_printf("Exp error\n");}
    |               Exp STAR error                          {syntaxError = TRUE; yyerrok; err_printf("Exp error\n");}
    |               Exp DIV error                           {syntaxError = TRUE; yyerrok; err_printf("Exp error\n");}
    |               LP error RP                             {syntaxError = TRUE; yyerrok; err_printf("Exp error\n");}
    |               MINUS error                             {syntaxError = TRUE; yyerrok; err_printf("Exp error\n");}
    |               NOT error                               {syntaxError = TRUE; yyerrok; err_printf("Exp error\n");}
    |               Exp LB error RB                         {syntaxError = TRUE; yyerrok; err_printf("Exp error\n");}
    |               Exp DOT error                           {syntaxError = TRUE; yyerrok; err_printf("Exp error\n");}
    ;
Args:               Exp COMMA Args                          {$$ = createInternalNode(64,@$.first_line, NON_TERMINAL, "Args", 3, $1, $2, $3);}
    |               Exp                                     {$$ = createInternalNode(65,@$.first_line, NON_TERMINAL, "Args", 1, $1);}
    |               error COMMA Args                        {syntaxError = TRUE; yyerrok; err_printf("Args -> error COMMA Args\n");}
    ;
%%

void yyerror(char *msg) {
    if (lexErrorList[yylineno] == 0) {
        printf("Error type B at Line %d: %s.\n", yylineno, msg);
        syntaxError = TRUE;
        lexErrorList[yylineno] = 2;
    }
}