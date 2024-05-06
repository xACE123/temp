/*
* @author: yunzinan
* @date: 2024/5/5
* @abstract: source file of the syntax tree.
*/

#include "inter.h"
#include "Semantic.h"

// symbol tables: import from semantic.c
extern funcion_table *def_table;
extern function search_f(funcion_table* t,char name[40]);

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

void newIntercode(ir_type type, int argc, ...) {
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
    // now insert the code
    insertIntercode(code);
}


void printInterCode(FILE* fp) {
    // fprintf(fp, "printInterCode\n");
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
    if (ir_list.head == NULL) {
        intercodeNode* cu = (intercodeNode*)malloc(sizeof(intercodeNode)); 
        cu->code = code;
        cu->next = NULL;
        cu->prev = NULL;
        ir_list.head = cu;
        ir_list.cur = cu;
    }
    else {
        intercodeNode* last = ir_list.cur;
        intercodeNode* cur = (intercodeNode*)malloc(sizeof(intercodeNode)); 
        cur->code = code;
        cur->next = NULL;
        cur->prev = last;
        last->next = cur;
        ir_list.cur = cur;
    }
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
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "Program") == 0);
    trans_ExtDefList(get_k_son(1, cur));
}
void trans_ExtDefList(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "ExtDefList") == 0);
    switch (cur->product_id)
    {
    case 2:
        trans_ExtDef(get_k_son(1, cur));
        trans_ExtDefList(get_k_son(2, cur));
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
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "ExtDef") == 0);
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
        assert(0);
        break;
    }
}
void trans_ExtDecList(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "ExtDecList") == 0);
    switch (cur->product_id)
    {
    case 8: {
        operand* ret = newTmpVar();
        trans_VarDec(get_k_son(1, cur), ret);
        break;
    }
    case 9: {
        operand* ret = newTmpVar();
        trans_VarDec(get_k_son(1, cur), ret); 
        trans_ExtDecList(get_k_son(3, cur));
        break;
    }
    default:
        assert(0);
        break;
    }
}
/* Specifiers */
void trans_Specifier(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "Specifier") == 0);
    switch (cur->product_id)
    {
    case 10:
        break;
    case 11:
        trans_StructSpecifier(get_k_son(1, cur));
        break; 
    default:
        assert(0);
        break;
    }
}
void trans_StructSpecifier(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "StructSpecifier") == 0);
    switch (cur->product_id)
    {
    case 12:
        trans_OptTag(get_k_son(2, cur));
        trans_DefList(get_k_son(4, cur));
        break;
    case 13:
        trans_Tag(get_k_son(2, cur));
        break;
    default:
        assert(0);
        break;
    }
}
void trans_OptTag(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "OptTag") == 0);
    switch (cur->product_id)
    {
    case 14:
        break;
    case 15:
        break; 
    default:
        assert(0);
        break;
    }
}
void trans_Tag(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "Tag") == 0);
    switch (cur->product_id)
    {
    case 16:
        break;
    default:
        assert(0);
        break;
    }
}
/* Declarators */
void trans_VarDec(TreeNode* cur, operand* place) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "VarDec") == 0);
    switch (cur->product_id)
    {
  case 17:
    {
        TreeNode*s1=cur->firstChild;
        char name[40];
        int i=0;
        while(s1->name[i]!='\0'){
            name[i]=s1->name[i];
            ++i;
        }
        name[i]='\0';
        style s=search_s(stable,name);
        if(s.dimension>1){
            int ans=4;
            for(int i=1;i<s.dimension;++i){
                ans*=s.size[i];
            }
             operand* x = newOperand(OPR_VARIABLE, 0, newString(name));
            newIntercode(IR_13_DEC, 2, x, ans);
        }
        break; 
    }
    case 18:{
        TreeNode*s1=cur->firstChild;
        char name[40];
        int i=0;
        while(s1->name[i]!='\0'){
            name[i]=s1->name[i];
            ++i;
        }
        name[i]='\0';
        style s=search_s(stable,name);
        if(s.dimension>1){
            int ans=4;
            for(int i=1;i<s.dimension;++i){
                ans*=s.size[i];
            }
             operand* x = newOperand(OPR_VARIABLE, 0, newString(name));
            newIntercode(IR_13_DEC, 2, x, ans);
        }
        break; 
    }
    case 119: {
        // TODO: need to generate DEC [size] //
        
        operand* ret = newTmpVar();
        trans_VarDec(get_k_son(1, cur), ret);
        break;
    }
    default:
        assert(0);
        break;
    }
}
void trans_FunDec(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "FunDec") == 0);
    // generate FUNCTION code
    operand* funct = newOperand(OPR_VARIABLE, 0, newString(get_k_son(1, cur)->name));
    newIntercode(IR_1_FUNCTION, 1, funct);
    switch (cur->product_id)
    {
    case 19: {
        // TODO: need to lookup the function symbol table to get the funciton params
        // TODO: need to generate PARAM x
        char function_name_t[40];
        strcpy(function_name_t, get_k_son(1, cur)->name);
        function f = search_f(def_table, function_name_t);
        style_link* cur = f.head;
        while(cur != NULL) {
            style cur_s = cur->s;
            if (cur_s.type == 1) {
                if (cur_s.dimension == 1) {
                    // so is int
                    operand* param_t = newOperand(OPR_VARIABLE, 0, newString(cur_s.s_name));
                    newIntercode(IR_16_PARAM, 1, param_t);
                } 
                else if(cur_s.dimension == 2) {
                    // int a[i] 
                    // TODO
                    operand* param_t = newOperand(OPR_VARIABLE, 0, newString(cur_s.s_name));
                    newIntercode(IR_16_PARAM, 1, param_t);
                }
                else {
                    // int a[i][j]...
                    // since only 1 dimension array can be supported
                    IR_error();
                }
            }
            else {
                IR_error();
            }
            cur = cur->next;
        }
        // trans_VarList(get_k_son(3, cur)); 
        break;
    }
   case 20:{
        // TODO: need to lookup the function symbol table to get the funciton params
        // TODO: need to generate PARAM x
        char function_name_t[40];
        strcpy(function_name_t, get_k_son(1, cur)->name);
        function f = search_f(def_table, function_name_t);
        style_link* cur = f.head;
        while(cur != NULL) {
            style cur_s = cur->s;
            if (cur_s.type == 1) {
                if (cur_s.dimension == 1) {
                    // so is int
                    operand* param_t = newOperand(OPR_VARIABLE, 0, newString(cur_s.s_name));
                    newIntercode(IR_16_PARAM, 1, param_t);
                } 
                else if(cur_s.dimension == 2) {
                    // int a[i] 
                    // TODO
                    operand* param_t = newOperand(OPR_VARIABLE, 0, newString(cur_s.s_name));
                    newIntercode(IR_16_PARAM, 1, param_t);
                }
                else {
                    // int a[i][j]...
                    // since only 1 dimension array can be supported
                    IR_error();
                }
            }
            else {
                IR_error();
            }
            cur = cur->next;
        }
        // trans_VarList(get_k_son(3, cur)); 
        break;
    }
    case 21:
        break;
    case 22:
        break;
    default:
        assert(0);
        break;
    } 
}

// XXX: this function may be useless
void trans_VarList(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "VarList") == 0);
    switch (cur->product_id)
    {
    case 23:
        trans_ParamDec(get_k_son(1, cur));
        trans_VarList(get_k_son(3, cur));
        break;
    case 24:
        trans_ParamDec(get_k_son(1, cur));
        break;
    default:
        assert(0);
        break;
    }
}

// XXX: this function may be useless
void trans_ParamDec(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "ParamDec") == 0);
    trans_Specifier(get_k_son(1, cur));
    operand* ret = newTmpVar();
    trans_VarDec(get_k_son(2, cur), ret);
}
/* Statemetns */
void trans_Compst(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "CompSt") == 0);
    trans_DefList(get_k_son(2, cur));
    trans_StmtList(get_k_son(3, cur));
}
void trans_StmtList(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "StmtList") == 0);
    if (cur->product_id == 27) {
        trans_Stmt(get_k_son(1, cur));
        trans_StmtList(get_k_son(2, cur));
    }
    else if (cur->product_id == 28) {
    }
    else {
        assert(0);
    }
}
void trans_Stmt(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "Stmt") == 0);
    switch (cur->product_id)
    {
    case 29: {
        // XXX: I think here we dont need to create a tmpVar, but what if `foo();`?
        operand* ret = newTmpVar();
        trans_Exp(get_k_son(1, cur), ret);
        break;
    }
    case 30:
        trans_Compst(get_k_son(1, cur)); 
        break;
    case 31: {
        operand* ret = newTmpVar();
        trans_Exp(get_k_son(2, cur), ret);
        newIntercode(IR_12_RETURN, 1, ret);
        break;
    }
    case 32:{
        operand* truelabel = newLabel();
        operand* falselabel = newLabel();
        trans_Cond(get_k_son(3, cur), truelabel, falselabel);
        newIntercode(IR_0_LABEL, 1, truelabel);
        trans_Stmt(get_k_son(5, cur));
        newIntercode(IR_0_LABEL, 1, falselabel);
        break;
    }
    case 33: {
        operand* l1 = newLabel();
        operand* l2 = newLabel();
        operand* l3 = newLabel();
        trans_Cond(get_k_son(3, cur), l1, l2);
        newIntercode(IR_0_LABEL, 1, l1);
        trans_Stmt(get_k_son(5, cur));
        newIntercode(IR_10_GOTO, 1, l3);
        newIntercode(IR_0_LABEL, 1, l2);
        trans_Stmt(get_k_son(7, cur));
        newIntercode(IR_0_LABEL, 1, l3);
        break;
    }
    case 34: {
        operand* l1 = newLabel();
        operand* l2 = newLabel();
        operand* l3 = newLabel();
        newIntercode(IR_0_LABEL, 1, l1);
        trans_Cond(get_k_son(3, cur), l1, l2);
        newIntercode(IR_0_LABEL, 1, l2);
        trans_Stmt(get_k_son(5, cur));
        newIntercode(IR_10_GOTO, 1, l1);
        newIntercode(IR_0_LABEL, 1, l3);
        break;
    }
    default:
        assert(0);
        break;
    }
}
/* Local Definitions */
void trans_DefList(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "DefList") == 0);
    switch (cur->product_id)
    {
    case 35:
        trans_Def(get_k_son(1,cur));
        trans_DefList(get_k_son(2,cur));
        break;
    case 36:
        break;
    default:
        assert(0);
        break;
    }
}
void trans_Def(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "Def") == 0);
    trans_Specifier(get_k_son(1,cur));
    trans_DecList(get_k_son(2,cur));
}
void trans_DecList(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "DecList") == 0);
    switch (cur->product_id)
    {
    case 38:
        trans_Dec(get_k_son(1,cur));
        break;
    case 39:
        trans_Dec(get_k_son(1,cur));
        trans_DecList(get_k_son(3,cur));
        break;
    default:
        assert(0);
        break;
    }
}
void trans_Dec(TreeNode* cur) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "Dec") == 0);
    switch (cur->product_id)
    {
    case 40: {
        operand* ret = newTmpVar();
        trans_VarDec(get_k_son(1, cur), ret);
        break;
    }
    case 41: {  
        operand* t1 = newTmpVar();
        trans_VarDec(get_k_son(1, cur), t1);
        operand* t2 = newTmpVar();
        trans_Exp(get_k_son(3, cur), t2); 
        // VarDec := Exp
        break; 
    }
    default:
        break;
    }
}

/* Expressions */
void trans_Exp(TreeNode* cur, operand* place) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "Exp") == 0);
    switch (cur->product_id)
    {
    case 42: {
        operand* t1 = newTmpVar();
        trans_Exp(get_k_son(3, cur), t1);
        operand* variable = newOperand(OPR_VARIABLE, 0, newString(get_k_son(1, get_k_son(1, cur))->name));
        newIntercode(IR_2_ASSIGN, 2, variable, t1);
        // XXX: the following two codes can be reduced
        newIntercode(IR_2_ASSIGN, 2, place, variable);
        break;
    }
    case 43: {
        operand* l1 = newLabel();
        operand* l2 = newLabel();
        operand* zero = newOperand(OPR_CONSTANT, 0, NULL);
        newIntercode(IR_2_ASSIGN, 2, place, zero);
        trans_Cond(cur, l1, l2);
        newIntercode(IR_0_LABEL, 1, l1);
        operand* one = newOperand(OPR_CONSTANT, 1, NULL);
        newIntercode(IR_2_ASSIGN, 2, place, one);
        newIntercode(IR_0_LABEL, 1, l2);
        break;
    }
    case 44: {
        operand* l1 = newLabel();
        operand* l2 = newLabel();
        operand* zero = newOperand(OPR_CONSTANT, 0, NULL);
        newIntercode(IR_2_ASSIGN, 2, place, zero);
        trans_Cond(cur, l1, l2);
        newIntercode(IR_0_LABEL, 1, l1);
        operand* one = newOperand(OPR_CONSTANT, 1, NULL);
        newIntercode(IR_2_ASSIGN, 2, place, one);
        newIntercode(IR_0_LABEL, 1, l2);
        break;
    }
    case 45: {
        operand* l1 = newLabel();
        operand* l2 = newLabel();
        operand* zero = newOperand(OPR_CONSTANT, 0, NULL);
        newIntercode(IR_2_ASSIGN, 2, place, zero);
        trans_Cond(cur, l1, l2);
        newIntercode(IR_0_LABEL, 1, l1);
        operand* one = newOperand(OPR_CONSTANT, 1, NULL);
        newIntercode(IR_2_ASSIGN, 2, place, one);
        newIntercode(IR_0_LABEL, 1, l2);
        break;
    }
    case 46: {
        operand* t1 = newTmpVar();
        operand* t2 = newTmpVar();
        trans_Exp(get_k_son(1, cur), t1);
        trans_Exp(get_k_son(3, cur), t2);
        newIntercode(IR_3_ADD, 3, place, t1, t2);
        break;
    }
    case 47: {
        operand* t1 = newTmpVar();
        operand* t2 = newTmpVar();
        trans_Exp(get_k_son(1, cur), t1);
        trans_Exp(get_k_son(3, cur), t2);
        newIntercode(IR_4_SUB, 3, place, t1, t2);
        break;
    }
    case 48: {
        operand* t1 = newTmpVar();
        operand* t2 = newTmpVar();
        trans_Exp(get_k_son(1, cur), t1);
        trans_Exp(get_k_son(3, cur), t2);
        newIntercode(IR_5_MUL, 3, place, t1, t2);
        break;
    }
    case 49: {
        operand* t1 = newTmpVar();
        operand* t2 = newTmpVar();
        trans_Exp(get_k_son(1, cur), t1);
        trans_Exp(get_k_son(3, cur), t2);
        newIntercode(IR_6_DIV, 3, place, t1, t2);
        break;
    }
    case 50: {
        trans_Exp(get_k_son(2, cur), place);
        break;
    }
    case 51: {
        operand* t1 = newTmpVar();
        trans_Exp(get_k_son(2, cur), t1);
        operand* zero = newOperand(OPR_CONSTANT, 0, NULL);
        newIntercode(IR_4_SUB, 3, place, zero, t1);
        break;
    }
    case 52: {
        operand* l1 = newLabel();
        operand* l2 = newLabel();
        operand* zero = newOperand(OPR_CONSTANT, 0, NULL);
        newIntercode(IR_2_ASSIGN, 2, place, zero);
        trans_Cond(cur, l1, l2);
        newIntercode(IR_0_LABEL, 1, l1);
        operand* one = newOperand(OPR_CONSTANT, 1, NULL);
        newIntercode(IR_2_ASSIGN, 2, place, one);
        newIntercode(IR_0_LABEL, 1, l2);
        break;
    }
    case 53: {
        // ID LP Args RP
        operand* function = newOperand(OPR_VARIABLE, 0, newString(get_k_son(1, cur)->name));
        argList* _argList = (argList*)malloc(sizeof(argList));
        _argList->head = NULL;
        _argList->cur = NULL;
        trans_Args(get_k_son(3, cur), _argList);
        if (strcmp(function->u.name, "write") == 0) {
            newIntercode(IR_18_WRITE, 1, _argList->head->opr);
            operand* zero = newOperand(OPR_CONSTANT, 0, NULL);
            newIntercode(IR_2_ASSIGN, 2, place, zero);
        }
        else {
            // ARG arg_list[i]
            for (argNode* cur = _argList->head; cur != NULL; cur = cur->nxt) {
                newIntercode(IR_14_ARG, 1, cur->opr);
            }
            newIntercode(IR_15_CALL, 2, place, function);
        }
        break;
    }
    case 54: {
        // SID LP Args RP
        operand* function = newOperand(OPR_VARIABLE, 0, newString(get_k_son(1, cur)->name));
        argList* _argList = (argList*)malloc(sizeof(argList));
        _argList->head = NULL;
        _argList->cur = NULL;
        trans_Args(get_k_son(3, cur), _argList);
        if (strcmp(function->u.name, "write") == 0) {
            newIntercode(IR_18_WRITE, 1, _argList->head->opr);
            operand* zero = newOperand(OPR_CONSTANT, 0, NULL);
            newIntercode(IR_2_ASSIGN, 2, place, zero);
        }
        else {
            // ARG arg_list[i]
            for (argNode* cur = _argList->head; cur != NULL; cur = cur->nxt) {
                newIntercode(IR_14_ARG, 1, cur->opr);
            }
            newIntercode(IR_15_CALL, 2, place, function);
        }
        break;
    }
    case 55: {
        // ID LP RP
        operand* function = newOperand(OPR_VARIABLE, 0, newString(get_k_son(1, cur)->name));
        if (strcmp(function->u.name, "read") == 0) {
            newIntercode(IR_17_READ, 1, place);
        }
        else {
            newIntercode(IR_15_CALL, 2, place, function);
        }
        break;
    }
    case 56: {
        // SID LP RP
        operand* function = newOperand(OPR_VARIABLE, 0, newString(get_k_son(1, cur)->name));
        if (strcmp(function->u.name, "read") == 0) {
            newIntercode(IR_17_READ, 1, place);
        }
        else {
            newIntercode(IR_15_CALL, 2, place, function);
        }
        break;
    }
    case 57: {
        // TODO
        break;
    }
    case 58: {
        intercodeErr = TRUE;
        IR_error();
        break;
    }
    case 59: {
        intercodeErr = TRUE;
        IR_error();
        break;
    }
    case 60: {
        operand* variable = newOperand(OPR_VARIABLE, 0, newString(get_k_son(1, cur)->name));
        newIntercode(IR_2_ASSIGN, 2, place, variable);
        break;
    }
    case 61: {
        operand* variable = newOperand(OPR_VARIABLE, 0, newString(get_k_son(1, cur)->name));
        newIntercode(IR_2_ASSIGN, 2, place, variable);
        break;
    }
    case 62: {
        operand* value = newOperand(OPR_CONSTANT, atoi(get_k_son(1, cur)->name), NULL);
        newIntercode(IR_2_ASSIGN, 2, place, value);
        break;
    }
    case 63: {
        // float should not appear
        intercodeErr = TRUE;
        IR_error();
        break;
    }
    default:
        assert(0);
        break;
    }


}
void trans_Args(TreeNode* cur, argList* argLst) {
    op_printf("[%d] %s\n", cur->lineno, cur->name);
    assert(strcmp(cur->name, "Args") == 0);
    if (cur->product_id == 64) {
        trans_Args(get_k_son(3, cur), argLst);
        operand* t1 = newTmpVar();
        trans_Exp(get_k_son(1, cur), t1);
        if (argLst->head == NULL) {
            argLst->head = (argNode*)malloc(sizeof(argNode));
            argLst->head->nxt = NULL;
            argLst->head->opr = t1;
            argLst->cur = argLst->head;
        }
        else {
            argLst->cur->nxt = (argNode*)malloc(sizeof(argNode));
            argLst->cur = argLst->cur->nxt;
            argLst->cur->opr = t1;
            argLst->cur->nxt = NULL;
        }
    }
    else if (cur->product_id == 65) {
        operand* t1 = newTmpVar();
        trans_Exp(get_k_son(1, cur), t1);
        if (argLst->head == NULL) {
            argLst->head = (argNode*)malloc(sizeof(argNode));
            argLst->head->nxt = NULL;
            argLst->head->opr = t1;
            argLst->cur = argLst->head;
        }
        else {
            argLst->cur->nxt = (argNode*)malloc(sizeof(argNode));
            argLst->cur = argLst->cur->nxt;
            argLst->cur->opr = t1;
            argLst->cur->nxt = NULL;
        }
    }
    else {
        assert(0);
    }

}

void trans_Cond(TreeNode* cur, operand* trueLabel, operand* falseLabel) {
    assert(strcmp(cur->name, "Exp") == 0);
    switch (cur->product_id)
    {
    case 45:{
        operand* t1 = newTmpVar();
        operand* t2 = newTmpVar();
        trans_Exp(get_k_son(1, cur), t1);
        trans_Exp(get_k_son(3, cur), t2);
        operand* op = newOperand(OPR_RELOP, 0, newString(get_k_son(2, cur)->name));
        newIntercode(IR_11_IF, 4, t1, t2, trueLabel, op);
        newIntercode(IR_10_GOTO, 1, falseLabel);
        break;
    }
    case 52:{
        trans_Cond(cur, falseLabel, trueLabel);
        break;
    }
    case 43:{
        operand* l1 = newLabel();
        trans_Cond(get_k_son(1, cur), l1, falseLabel);
        newIntercode(IR_0_LABEL, 1, l1);
        trans_Cond(get_k_son(3, cur), trueLabel, falseLabel);
        break;
    }
    case 44:{
        operand* l1 = newLabel();
        trans_Cond(get_k_son(1, cur), trueLabel, l1);
        newIntercode(IR_0_LABEL, 1, l1);
        trans_Cond(get_k_son(3, cur), trueLabel, falseLabel);
        break; 
    }
    default:{
        operand* t1 = newTmpVar();
        operand* t2 = newOperand(OPR_CONSTANT, 0, NULL);
        operand* op = newOperand(OPR_RELOP, 0, newString("!="));
        trans_Exp(cur, t1);
        newIntercode(IR_11_IF, 4, t1, t2, trueLabel, op);
        newIntercode(IR_10_GOTO, 1, falseLabel); 
        break;
    
    }
    }
}

void IR_error() {
    perror("Cannot Translate.\n");
}
