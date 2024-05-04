/*
* @author: yunzinan
* @date: 2024/3/9
* @abstract: header file of the syntax tree.
*/

#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "Symboltable.h"

#define TRUE 1
#define FALSE 0


// type of the tree node
typedef enum op_type {
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_TYPE,
    TOKEN_ID,
    TOKEN_OTHER,
    NON_TERMINAL,
    EMPTY_PRODUCTION // e.g. Compst -> epsilon
} op_type;

typedef struct attribute
{
    ps_link* ph;
    style_link *sh;
    int sign;
    char name[40];
    int in_statement;//Used to determine if a variable is within a function declaration
}attribute;


typedef struct TreeNode{
    int product_id;
    attribute comprehensive;
    attribute inherent;
    int lineno;
    op_type type;
    char *name;
    struct TreeNode* firstChild;
    struct TreeNode* nextSibling;
} TreeNode;

/*
e.g.:
    1.23 => (lineno, TOKEN_FLOAT, "1.23", TreeNode*, TreeNode*)
    Program => (lineno, NON_TERMINAL, "Program", TreeNode*, TreeNode*)

*/

TreeNode* get_k_son(int, TreeNode*);

TreeNode* createTreeNode(int, op_type, char*, int);

TreeNode* createLeafNode(int,int, op_type, char*);

TreeNode* createInternalNode(int ,int, op_type, char*, int, ...);

void printSyntaxTree(TreeNode*, int);   

void op_printf(const char* format, ...);
#endif
