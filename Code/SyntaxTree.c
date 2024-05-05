#include "SyntaxTree.h"

extern int syntaxError;
extern int lexicalError;
// const int op_verbose = FALSE;
const int op_verbose = TRUE;

//return the k-th child of the input TreeNode
TreeNode* get_k_son(int k,TreeNode* p){
    p=p->firstChild;
    for(int i=1;i < k && p != NULL;++i) p=p->nextSibling;
    return p;
}

// general function for creating nodes, return the created pointer
TreeNode* createTreeNode(int lineno, op_type type, char *name,int product_id) {
    int nameLen = strlen(name) + 1; // +1 is for the '\0'
    TreeNode* ret = (TreeNode*)malloc(sizeof(TreeNode));

    assert(ret != NULL); // otherwise the malloc failed

    ret->name = (char*)malloc(sizeof(char) * nameLen);

    assert(ret->name != NULL); // otherwise the malloc failed
    strncpy(ret->name, name, nameLen);
    ret->name[nameLen-1]='\0';
    ret->lineno = lineno;
    ret->type = type;
    ret->firstChild = NULL;
    ret->nextSibling = NULL;
    ret->product_id=product_id;
    op_printf("[TreeNode] lineno: %d, %s, type: %d\n", lineno, name, type);
    return ret;
}

// create leaf nodes(tokens), return their pointer (TreeNode*)
TreeNode* createLeafNode(int product_id,int lineno, op_type type, char *name) {
    TreeNode* ret = createTreeNode(lineno, type, name,product_id);
    return ret;
}

// create internal nodes(non-terminals), return their pointer(TreeNode*)
TreeNode* createInternalNode(int product_id,int lineno, op_type type, char *name, int argc, ...) {
    op_printf("[Internal] lineno: %d, %s, argc: %d\n", lineno, name, argc);
    va_list debug;
    va_start(debug, argc);
    for (int i = 0; i < argc; i++) {
        TreeNode* ptr = va_arg(debug, TreeNode*);
        if (ptr != NULL) {
            op_printf("    [ptr] %d: %s, type: %d\n", i, ptr->name, type);
        }
        else {
            op_printf("    [ptr] %d: NULL\n", i);
        }
    }
    op_printf("\n");
    va_end(debug);
    if (lexicalError || syntaxError) return NULL;
    va_list args;
    va_start(args, argc);

    TreeNode* ret = createTreeNode(lineno, type, name,product_id);
    
    ret->firstChild = va_arg(args, TreeNode*);

    TreeNode* cur = ret->firstChild;

    for (int i = 1; i < argc; i++) {
        assert(cur != NULL);
        cur->nextSibling = va_arg(args, TreeNode*);
        cur = cur->nextSibling;
    } 

    va_end(args);
    return ret;
}


// print the syntax tree in the first-order sequence recursively.
void printSyntaxTree(TreeNode* cur, int dep) {
    if (cur == NULL) {
        return ;
    }
    op_printf("[Tree] lineno: %d, %s, type: %d\t", cur->lineno, cur->name, cur->type);
    TreeNode *debug = cur;
    while(debug->nextSibling != NULL) {
        op_printf("%s ", debug->nextSibling->name);
        debug = debug->nextSibling;
    }
    op_printf("\n");
    if (cur->type == EMPTY_PRODUCTION) {
        printSyntaxTree(cur->nextSibling, dep);
        return ;
    }
    for (int i = 0; i < dep; i++) {
        printf("  ");
    }
    printf(" %d ",cur->product_id);
    switch(cur->type) {
        case TOKEN_INT:
            printf("INT: %d\n", atoi(cur->name));
            break;
        case TOKEN_FLOAT:
            printf("FLOAT: %.6f\n", strtof(cur->name, NULL));
            // printf("FLOAT: %.6f\n", atof(cur->name));
            break;
        case TOKEN_TYPE:
            printf("TYPE: %s\n", cur->name);
            break;
        case TOKEN_ID:
            printf("ID: %s\n", cur->name);
            break;
        case TOKEN_OTHER:
            printf("%s\n", cur->name);
            break;
        case NON_TERMINAL:
            printf("%s (%d)\n", cur->name, cur->lineno);
            break;
        default:
            assert(0); // shouldn't go there
            break;
    }
    printSyntaxTree(cur->firstChild, dep+1);
    printSyntaxTree(cur->nextSibling, dep);
}

// a customized printf function for printing verbose info
void op_printf(const char* format, ...) {
    if (op_verbose) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}

