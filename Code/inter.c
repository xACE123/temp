/*
* @author: yunzinan
* @date: 2024/5/5
* @abstract: source file of the syntax tree.
*/

#include "inter.h"

// symbol tables: import from semantic.c


intercodeList ir_list;
int intercodeErr = FALSE;

char* newString(char* str) {
    char *ret = (char*)malloc((strlen(str) + 1) * sizeof(char));
    strcpy(ret, str);
    return ret;
}


void init_ir() {
    ir_list.cur = NULL;
    ir_list.head = NULL;
    ir_list.num_label = 0;
    ir_list.num_tmp_var = 0;
}

operand* newOperand(opr_type type, int val, char* name) {
    operand* opr = (operand*)malloc(sizeof(operand));
    opr->type = type;
    switch (opr->type)
    {
    case OPR_VARIABLE:
        opr->u.name = name;
        break; 
    case OPR_CONSTANT:
        opr->u.val = val;
        break;
    case OPR_LABEL:
        opr->u.name = name;
        break;
    case OPR_RELOP:
        opr->u.name = name;
        break;
    default:
        assert(0);
        break;
    }
    return opr;
}

void printOperand(FILE* fout, operand* opr) {
    switch (opr->type)
    {
    case OPR_CONSTANT:
        fprintf(fout, "#%d", opr->u.val);
        break;
    case OPR_VARIABLE:
        fprintf(fout, "%s", opr->u.name);
        break;
    case OPR_LABEL:
        fprintf(fout, "%s", opr->u.name);
        break;
    case OPR_RELOP:
        fprintf(fout, "%s", opr->u.name);
        break;
    default:
        assert(0);
        break;
    }
} 

intercode* newIntercode(ir_type type, int argc, ...) {
    intercode* code = (intercode*)malloc(sizeof(intercode));
    code->type = type;
    code->x = NULL;
    code->y = NULL;
    code->z = NULL;
    code->relop= NULL;
    code->size = 0;
    va_list vaList;
    va_start(vaList, argc);
    switch (type)
    {
    case IR_0_LABEL: // LABEL x
        code->x = va_arg(vaList, operand*);
        break;
    case IR_1_FUNCTION: // FUNCTION x
        code->x = va_arg(vaList, operand*);
        break;
    case IR_2_ASSIGN: // x := y
        code->x = va_arg(vaList, operand*);
        code->y = va_arg(vaList, operand*);
        break;
    case IR_3_ADD: // x := y + z
        code->x = va_arg(vaList, operand*);
        code->y = va_arg(vaList, operand*);
        code->z = va_arg(vaList, operand*);
        break;
    case IR_4_SUB: // x := y - z
        code->x = va_arg(vaList, operand*);
        code->y = va_arg(vaList, operand*);
        code->z = va_arg(vaList, operand*);
        break;
    case IR_5_MUL: // x := y * z
        code->x = va_arg(vaList, operand*);
        code->y = va_arg(vaList, operand*);
        code->z = va_arg(vaList, operand*);
        break;
    case IR_6_DIV: // x := y / z 
        code->x = va_arg(vaList, operand*);
        code->y = va_arg(vaList, operand*);
        code->z = va_arg(vaList, operand*);
        break;
    case IR_7_GET_ADDR: // x := &y
        code->x = va_arg(vaList, operand*);
        code->y = va_arg(vaList, operand*);
        break;
    case IR_8_READ_ADDR: // x := *y
        code->x = va_arg(vaList, operand*);
        code->y = va_arg(vaList, operand*);
        break;
    case IR_9_WRITE_ADDR: // *x := y 
        code->x = va_arg(vaList, operand*);
        code->y = va_arg(vaList, operand*);
        break;
    case IR_10_GOTO: // GOTO x
        code->x = va_arg(vaList, operand*);
        break;
    case IR_11_IF: // IF x [relop] y GOTO z
        code->x = va_arg(vaList, operand*);
        code->y = va_arg(vaList, operand*);
        code->z = va_arg(vaList, operand*);
        code->relop = va_arg(vaList, operand*);
        break;
    case IR_12_RETURN: // RETURN x
        code->x = va_arg(vaList, operand*);
        break; 
    case IR_13_DEC: // DEC x [size]
        code->x = va_arg(vaList, operand*);
        code->size = va_arg(vaList, int);
        break;
    case IR_14_ARG: // ARG x
        code->x = va_arg(vaList, operand*);
        break;
    case IR_15_CALL: // x := CALL y
        code->x = va_arg(vaList, operand*);
        code->y = va_arg(vaList, operand*);
        break;
    case IR_16_PARAM: // PARAM x
        code->x = va_arg(vaList, operand*);
        break;
    case IR_17_READ: // READ x
        code->x = va_arg(vaList, operand*);
        break;
    case IR_18_WRITE: // WRITE x
        code->x = va_arg(vaList, operand*);
        break;
    default:
        assert(0);
        break;
    }
    va_end(vaList);
    return code;
}


void printInterCode(FILE* fp) {
    fprintf(fp, "printInterCode\n");
    intercodeNode* cur = ir_list.head;
    while(cur != NULL) {
        intercode* code = cur->code;
        switch (code->type)
        {
        case IR_0_LABEL: // LABEL x :
            fprintf(fp, "LABEL ");
            printOperand(fp, code->x);
            fprintf(fp, " :");
            break;
        case IR_1_FUNCTION: // FUNCTION x :
            fprintf(fp, "FUNCTION ");
            printOperand(fp, code->x);
            fprintf(fp, " :");
            break;
        case IR_2_ASSIGN: // x := y
            printOperand(fp, code->x);
            fprintf(fp, " := ");
            printOperand(fp, code->y);
            break;
        case IR_3_ADD: // x := y + z
            printOperand(fp, code->x);
            fprintf(fp, " := ");
            printOperand(fp, code->y);
            fprintf(fp, " + ");
            printOperand(fp, code->z);
            break;
        case IR_4_SUB: // x := y - z
            printOperand(fp, code->x);
            fprintf(fp, " := ");
            printOperand(fp, code->y);
            fprintf(fp, " - ");
            printOperand(fp, code->z);
            break;
        case IR_5_MUL: // x := y * z
            printOperand(fp, code->x);
            fprintf(fp, " := ");
            printOperand(fp, code->y);
            fprintf(fp, " * ");
            printOperand(fp, code->z);
            break;
        case IR_6_DIV: // x := y / z 
            printOperand(fp, code->x);
            fprintf(fp, " := ");
            printOperand(fp, code->y);
            fprintf(fp, " / ");
            printOperand(fp, code->z);
            break;
        case IR_7_GET_ADDR: // x := &y
            printOperand(fp, code->x);
            fprintf(fp, " := &");
            printOperand(fp, code->y);
            break;
        case IR_8_READ_ADDR: // x := *y
            printOperand(fp, code->x);
            fprintf(fp, " := *");
            printOperand(fp, code->y);
            break;
        case IR_9_WRITE_ADDR: // *x := y 
            fprintf(fp, "*");
            printOperand(fp, code->x);
            fprintf(fp, " := ");
            printOperand(fp, code->y);
            break;
        case IR_10_GOTO: // GOTO x
            fprintf(fp, "GOTO ");
            printOperand(fp, code->x);
            break;
        case IR_11_IF: // IF x [relop] y GOTO z
            fprintf(fp, "IF ");
            printOperand(fp, code->x);
            fprintf(fp, " ");
            printOperand(fp, code->relop);
            fprintf(fp, " ");
            printOperand(fp, code->y);
            fprintf(fp, " GOTO ");
            printOperand(fp, code->z);
            break;
        case IR_12_RETURN: // RETURN x
            fprintf(fp, "RETURN ");
            printOperand(fp, code->x);
            break; 
        case IR_13_DEC: // DEC x [size]
            fprintf(fp, "DEC ");
            printOperand(fp, code->x);
            fprintf(fp, " ");
            fprintf(fp, "%d", code->size);
            break;
        case IR_14_ARG: // ARG x
            fprintf(fp, "ARG ");
            printOperand(fp, code->x);
            break;
        case IR_15_CALL: // x := CALL y
            printOperand(fp, code->x);
            fprintf(fp, " := CALL ");
            printOperand(fp, code->y);
            break;
        case IR_16_PARAM: // PARAM x
            fprintf(fp, "PARAM ");
            printOperand(fp, code->x);
            break;
        case IR_17_READ: // READ x
            fprintf(fp, "READ ");
            printOperand(fp, code->x);
            break;
        case IR_18_WRITE: // WRITE x
            fprintf(fp, "WRITE ");
            printOperand(fp, code->x);
            break;
        default:
            assert(0);
            break;
        }
        fprintf(fp, "\n");

        cur = cur->next;
    }
}

void insertIntercode(intercode* code) {
    intercodeNode* last = ir_list.cur;
    intercodeNode* cur = (intercodeNode*)malloc(sizeof(intercodeNode)); 
    cur->code = code;
    cur->next = NULL;
    cur->prev = last;
    last->next = cur;
    ir_list.cur = cur;
}

operand* newTmpVar() {
    char t_name[TMPNAMELEN] = {0};
    sprintf(t_name, "t%d", ir_list.num_tmp_var);
    ir_list.num_tmp_var += 1;
    operand* ret = newOperand(OPR_VARIABLE, 0, newString(t_name));
    return ret;
}

operand* newLabel() {
    char t_name[TMPNAMELEN] = {0};
    sprintf(t_name, "label%d", ir_list.num_label);
    ir_list.num_label += 1;
    operand* ret = newOperand(OPR_LABEL, 0, newString(t_name));
    return ret;
}

// translation recursive functions
/* High-level Definitions */
void trans_Program(TreeNode* cur) {
    trans_ExtDefList(cur->firstChild);
}
void trans_ExtDefList(TreeNode* cur) {
    switch (cur->product_id)
    {
    case 2:
        trans_Def(get_k_son(1, cur));
        trans_ExtDecList(get_k_son(2, cur));
        break;
    case 3:
        // epsilon
        break;
    default:
        assert(0);
        break;
    }
}
void trans_ExtDef(TreeNode* cur) {
    switch (cur->product_id)
    {
    case 4:
        trans_Specifier(get_k_son(1, cur));
        trans_ExtDecList(get_k_son(2, cur));
        break;
    case 5:
        trans_Specifier(get_k_son(1, cur));
        break;
    case 6:
        trans_Specifier(get_k_son(1, cur));
        trans_FunDec(get_k_son(2, cur));
        trans_Compst(get_k_son(3, cur));
        break;
    case 7:
        trans_Specifier(get_k_son(1, cur));
        trans_FunDec(get_k_son(2, cur));
        break;
    default:
        break;
    }
}
void trans_ExtDecList(TreeNode* cur) {
    switch (cur->product_id)
    {
    case 8:
        operand* ret = newTmpVar();
        trans_VarDec(get_k_son(1, cur), ret);
        break;
    case 9:
        operand* ret = newTmpVar();
        trans_VarDec(get_k_son(1, cur), ret); 
        break;
    default:
        break;
    }
}
/* Specifiers */
void trans_Specifier(TreeNode* cur) {

}
void trans_StructSpecifier(TreeNode* cur) {

}
void trans_OptTag(TreeNode* cur) {

}
void trans_Tag(TreeNode* cur) {

}
/* Declarators */
void trans_VarDec(TreeNode* cur, operand* place) {

}
void trans_FunDec(TreeNode* cur) {

}
void trans_ParamDec(TreeNode* cur) {

}
/* Statemetns */
void trans_Compst(TreeNode* cur) {

}
void trans_StmtList(TreeNode* cur) {

}
void trans_Stmt(TreeNode* cur) {

}
/* Local Definitions */
void trans_DefList(TreeNode* cur) {

}
void trans_Def(TreeNode* cur) {

}
void trans_DecList(TreeNode* cur) {

}
void trans_Dec(TreeNode* cur) {

}

/* Expressions */
void trans_Exp(int prod_id, TreeNode* cur, operand* place) {

}
void trans_Args(int prod_id, TreeNode* cur, argList* argLst) {

}
void trans_Cond(int prod_id, TreeNode* cur, operand* trueLabel, operand* falseLabel) {

}
