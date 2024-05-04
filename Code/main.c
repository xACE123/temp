
#include "SyntaxTree.h"
#include "syntax.tab.h"
#include "Semantic.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>



const int maxline = 1000;

extern TreeNode* root;
extern int yylineno;
extern int yyparse(void);
extern void yyrestart(FILE* f);
extern void fdfs(TreeNode* root);

int lexicalError = FALSE;
int syntaxError = FALSE;
int *lexErrorList; // 0: no error, 1: lexical error 2: syntax error 
char **structIDList;
int structIDCnt = 0;
FILE* f;

void addStruct(char *SID) {
    if (structIDCnt >= 100) {
        printf("TOO much struct defined!\n");
        return ;
    }
    structIDList[structIDCnt++] = (char *)malloc(strlen(SID) + 1);
    strcpy(structIDList[structIDCnt-1], SID); 
}

int is_SID(char *cur) {
    for (int i = 0; i < structIDCnt; i++) {
        if (strcmp(structIDList[i], cur) == 0) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc <= 1) return 1;
    lexErrorList = (int *)malloc(sizeof(int) * 1000);
    f = fopen(argv[1], "r");
    if (!f)  {
        perror(argv[1]);
        return 1;
    }
    else {
        structIDList = (char **)malloc(sizeof(char *) * 100);
    }
    yyrestart(f);
    yyparse();
    // printf("lex: %d syntax: %d\n", lexicalError, syntaxError);
    if ((!lexicalError) && (!syntaxError)) {
       // no problem detected, therefore the syntax tree will be printed.
       // printf("bingo! syntax tree will be printed\n");
       // printSyntaxTree(root,0);
       fdfs(root);
       printSyntaxTree(root,0);
    }
    return 0;
}

