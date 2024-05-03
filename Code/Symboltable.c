#include "Symboltable.h"

#define MOD 100003
#define bool int
#define false 0
#define true 1
bool cmp(char a[40],char b[40]){
    int i;
    for(i=0;a[i]!='\0' && b[i]!='\0';++i){
        if(a[i]!=b[i])
            return false;
    }
    if(a[i]!='\0' || b[i]!='\0')return false;
    return true;
}

void print_pure_style(pure_style a){
    printf("type=%d dim=%d stid=%d\n",a.type,a.dimension,a.struct_id);
}
void print_style(style a){
    printf("name=%s, type=%d dim=%d stid=%d\n",a.s_name,a.type,a.dimension,a.struct_id);
}

int cmp_pure_style(pure_style a, pure_style b){
   // print_pure_style(a);print_pure_style(b);
    if(a.dimension!=b.dimension || a.type!=b.type)return 0;
    if(a.type==3){
        if(a.struct_id!=b.struct_id)return 0;
    }
    return 1;
}

pure_style style_to_pure(style a){
    pure_style b;
    b.dimension=a.dimension;
    b.struct_id=a.struct_id;
    b.type=a.type;
    return b;
}

void initst(style_table** t){
    *(t)=(style_table*)malloc(sizeof(style_table));
    for(int i=0;i<MOD;++i)
        (*t)->h[i]=NULL;
}
style search_s(style_table* t,char name[40]){
    int ans=0;
    for(int i=0;name[i]!='\0';++i){
        ans=ans*23%MOD;
        ans=(ans+(int)name[i])%MOD;
    }
 //   printf("search, %d\n",ans);
    style_link* head=t->h[ans];
    while(head!=NULL){
      //  print_style(head->s);
        if(cmp(head->s.s_name,name))
            return head->s;
        head=head->next;
    }
    // cant find
    style f;
    f.s_name[0]='\0';
    return f;
}
void insert_s(style_table* t, style f){
    char name[40];
    strncpy(name, f.s_name, 40);
     int ans=0;
    for(int i=0;name[i]!='\0';++i){
        ans=ans*23%MOD;
        ans=(ans+(int)name[i])%MOD;
    }
    style_link* p=(style_link*)malloc(sizeof(style_link));
    p->s=f;
    p->next=t->h[ans];
    t->h[ans]=p;
 //   printf("insert,%d\n",ans);
}
int cmp_style(style a, style b){
    if(a.dimension!=b.dimension || a.type!=b.type)return 0;
    if(a.type==3){
        if(a.struct_id!=b.struct_id)return 0;
    }
    return 1;
}



void initat(architecture_table** t){
    *t=(architecture_table*)malloc(sizeof(architecture_table));
    for(int i=0;i<MOD;++i)
        (*t)->h[i]=NULL;
}
architecture search_a(architecture_table* t,char name[40]){
    int ans=0;
    for(int i=0;name[i]!='\0';++i){
        ans=ans*23%MOD;
        ans=(ans+(int)name[i])%MOD;
    }
    architecture_link* head=t->h[ans];
    while(head!=NULL){
        if(cmp(head->a.s_name,name))
            return head->a;
        head=head->next;
    }
    // cant find
    architecture f;
    f.s_name[0]='\0';
    return f;
}
void insert_a(architecture_table* t,architecture f){
    char name[40];
    strncpy(name, f.s_name, 40);
     int ans=0;
    for(int i=0;name[i]!='\0';++i){
        ans=ans*23%MOD;
        ans=(ans+(int)name[i])%MOD;
    }
    architecture_link* p=(architecture_link*)malloc(sizeof(architecture_link));
    p->a=f;
    p->next=t->h[ans];
    t->h[ans]=p;
}





void print_function(function f){
    printf("%s, %d, ",f.s_name,f.return_type.type);
    ps_link*p=f.head;
    while(p!=NULL){
        printf("%d  ",p->p.type);
        p=p->next;
    }
    printf("\n");
}
void initft(funcion_table** t){
    *t=(funcion_table*)malloc(sizeof(funcion_table));
    for(int i=0;i<MOD;++i)
        (*t)->h[i]=NULL;
}
function search_f(funcion_table* t,char name[40]){
   // printf("search_f, %s\n",name);
    int ans=0;
    for(int i=0;name[i]!='\0';++i){
        ans=ans*23%MOD;
        ans=(ans+(int)name[i])%MOD;
    }
    function_link* head=t->h[ans];
    while(head!=NULL){
        if(cmp(head->f.s_name,name))
            return head->f;
        head=head->next;
    }
    // cant find

    function f;
    f.s_name[0]='\0';
    return f;
}
void insert_f(funcion_table* t, function f){
   // print_function(f);
    char name[40];
    strncpy(name, f.s_name, 40);
     int ans=0;
    for(int i=0;name[i]!='\0';++i){
        ans=ans*23%MOD;
        ans=(ans+(int)name[i])%MOD;
    }
    function_link* p=(function_link*)malloc(sizeof(function_link));
 //   printf("ans=%d %s\n",ans,f.s_name);
    p->f=f;
    p->next=t->h[ans];
    t->h[ans]=p;
}

int cmp_function(function a, function b){
   // print_function(a);print_function(b);
    if(cmp_pure_style(a.return_type,b.return_type)==0)return 0;
    ps_link*s1 =a.head,*s2=b.head;
    while(s1!=NULL && s2!=NULL){
        if(cmp_pure_style(s1->p,s2->p)==0)return 0;
        s1=s1->next;
        s2=s2->next;
    }
    if(s1!=NULL || s2!=NULL)return 0;
    return 1;
}