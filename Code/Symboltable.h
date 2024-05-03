#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include<stdlib.h>

#define MOD 100003
//type of varibles

int cmp(char a[40],char b[40]);//Used to compare 40 size strings for equality



//Linked list of type int
typedef struct int_link
{
    int a;
    struct int_link* next;
}int_link;

//variable type

typedef struct pure_style
{
    int type;//0 means bool, 1 means int, 2=float, 3=struct, 4=null_type
    int dimension;//
    int struct_id;// if it is a struct
}pure_style;
typedef struct ps_link
{
    pure_style p;
    struct ps_link*next;
}ps_link;

int cmp_pure_style(pure_style a, pure_style b);
void print_pure_style(pure_style a);

//variable type and its name

typedef struct style
{
    char s_name[40];
    int type;//0 means bool, 1 means int, 2=float, 3=struct
    int dimension;//
    int struct_id;// if it is a struct
}style;
int cmp_style(style a, style b);

typedef struct style_link
{
    style s;
    struct style_link*next;
}style_link;

typedef struct style_table
{
    style_link* h[MOD];
}style_table;
// init means initialize a form
// search means search a element by its name
//insert means insert a element to the table
void initst(style_table **t);
style search_s(style_table *t,char name[40]);
void insert_s(style_table *t,style s);
pure_style style_to_pure(style a);

//used to record Structure
typedef struct architecture
{
    char s_name[40];
    int struct_id;
    style_link* head;

}architecture;
typedef struct architecture_link
{
    architecture a;
    struct architecture_link *next;
}architecture_link;
typedef struct architecture_table
{
    architecture_link *h[MOD];
}architecture_table;
void initat(architecture_table **t);
architecture search_a(architecture_table* t,char name[40]);
void insert_a(architecture_table *t,architecture s);

// Used to record funcions

typedef struct  function
{
     char s_name[40];
     pure_style return_type;
     ps_link* head;//list of required parameters
     int lines;
}function;


typedef struct function_link
{
    function f;
    struct function_link* next;
}function_link;

typedef struct funcion_table
{
    function_link* h[MOD];
}funcion_table;

void initft(funcion_table** t);
function search_f(funcion_table *t,char name[40]);
void insert_f(funcion_table *t, function f);
int cmp_function(function a, function b);
#endif