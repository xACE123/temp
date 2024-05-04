/*
* @author: yunzinan
* @date: 2024/3/9
* @abstract: source file of the syntax tree.
*/

#include "inter.h"

intercodeList ir_list;


void init_ir() {
    ir_list.cur = NULL;
    ir_list.head = NULL;
    ir_list.num_label = 0;
    ir_list.num_tmp_var = 0;
}

operand* newOperand(opr_type type, int argc, ...) {
    operand* opr = (operand*)malloc(sizeof(operand));
    opr->type = type;
    // TODO
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

// traverse functions
void genInterCode(TreeNode* cur) {

}