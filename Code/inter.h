/*
* @author: yunzinan
* @date: 2024/5/4
* @abstract: statement for functions and variables for intercode generation
*/

#ifndef INTER_H
#define INTER_H

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include "SyntaxTree.h"

#define TMPNAMELEN 20

//*************************** data structures *********************************

typedef enum ir_type {
    IR_0_LABEL, // LABEL x : 
    IR_1_FUNCTION, // FUNCTION f :
    IR_2_ASSIGN, // x := y
    IR_3_ADD, // x := y + z
    IR_4_SUB, // x := y - z
    IR_5_MUL, // x := y * z
    IR_6_DIV, // x := y / z
    IR_7_GET_ADDR, // x := &y
    IR_8_READ_ADDR, // x := *y
    IR_9_WRITE_ADDR, // *x := y
    IR_10_GOTO, // GOTO x
    IR_11_IF, // IF x [relop] y GOTO z
    IR_12_RETURN, // RETURN x
    IR_13_DEC, // DEC x [size]
    IR_14_ARG, // ARG x
    IR_15_CALL, // x := CALL f
    IR_16_PARAM, // PARAM x
    IR_17_READ, // read x
    IR_18_WRITE, // write x
} ir_type;

typedef enum opr_type {
    OPR_VARIABLE, // int a 
    OPR_CONSTANT, // #0
    OPR_LABEL, // label1
    OPR_RELOP, // < > 
    OPR_ADDRESS, 
} opr_type;

// data structure for an operand
typedef struct operand {
    opr_type type; 
    union {
        int val;
        char *name;
    } u;
} operand;

// data structure for a single intercode
typedef struct intercode {
    ir_type type;
    operand* x;
    operand* y;
    operand* z;
    operand* relop; // for IF_GOTO
    int size; // for DEC 

} intercode;

// ir_node, using double linked list
typedef struct intercodeNode intercodeNode;

struct intercodeNode {
    intercode* code;
    intercodeNode* prev;
    intercodeNode* next;
};

typedef struct intercodeList {
    intercodeNode* head;
    intercodeNode* cur; // for inserting new node 
    int num_tmp_var;
    int num_label;
} intercodeList;

typedef struct argNode argNode;

struct argNode {
    operand* opr;
    argNode* nxt;
};

typedef struct argList {
    argNode* head;
    argNode* cur;
} argList;

typedef struct paramNode paramNode;

struct paramNode {
    operand* opr;
    argNode* nxt;
};

typedef struct paramList{
    argNode* head;
    argNode* cur;
} paramList;
//*************************** helper function ******************************

char* newString(char* str);

void IR_error();


//*************************** functions ***************************************

// init the intermediate representation table
void init_ir();

/* High-level Definitions */
void trans_Program(TreeNode* cur);
void trans_ExtDefList(TreeNode* cur);
void trans_ExtDef(TreeNode* cur);
void trans_ExtDecList(TreeNode* cur);
/* Specifiers */
void trans_Specifier(TreeNode* cur);
void trans_StructSpecifier(TreeNode* cur);
void trans_OptTag(TreeNode* cur);
void trans_Tag(TreeNode* cur);
/* Declarators */
void trans_VarDec(TreeNode* cur, operand* place);
void trans_FunDec(TreeNode* cur);
void trans_VarList(TreeNode* cur);
void trans_ParamDec(TreeNode* cur);
/* Statemetns */
void trans_Compst(TreeNode* cur);
void trans_StmtList(TreeNode* cur);
void trans_Stmt(TreeNode* cur);
/* Local Definitions */
void trans_DefList(TreeNode* cur);
void trans_Def(TreeNode* cur);
void trans_DecList(TreeNode* cur);
void trans_Dec(TreeNode* cur);

/* Expressions */
void trans_Exp(TreeNode* cur, operand* place);
void trans_Args(TreeNode* cur, argList* argLst);
void trans_Cond(TreeNode* cur, operand* trueLabel, operand* falseLabel);





// print the intercodeList to the file 
void printInterCode(FILE* fout);

operand* newOperand(opr_type type, int val, char* name);
void printOperand(FILE* fout, operand* opr);

void newIntercode(ir_type type, int argc, ...);
void insertIntercode(intercode* code);

operand* newTmpVar();
operand* newLabel();

#endif // INTER_H
