#include"Semantic.h"

int wrong[50000];//Used to record which lines have errors to avoid repeated errors in the same line, the number of size is not important
architecture arc_table[10000];//Used to record which structures were created, the number of size is not important
int arcid;//Used to record the number of structures
style_table *stable;//Table for recording variables
architecture_table *atable;//Table for recording structures
funcion_table *ste_table;//Table for Function declaration
funcion_table *def_table;//Table for Function Definition

extern TreeNode* get_k_son(int k,TreeNode*p );
extern int cmp(char a[40],char b[40]);
extern int cmp_pure_style(pure_style a, pure_style b);
extern pure_style style_to_pure(style a);
extern int cmp_function(function a, function b);

extern void initst(style_table** t);
extern style search_s(style_table* t,char name[40]);
extern void insert_s(style_table* t,style s);
extern void initat(architecture_table** t);
extern architecture search_a(architecture_table* t,char name[40]);
extern void insert_a(architecture_table* t,architecture s);
extern void initft(funcion_table** t);
extern function search_f(funcion_table* t,char name[40]);
extern void insert_f(funcion_table* t, function f);

//Copy the style inherited properties of a to b
void copy_inhernt_style(TreeNode*a,TreeNode*b){
    if(a->inherent.sign!=1){
        printf("a->inherent.sign!=1, at %d\n",a->lineno);
        return;
    }
    style_link*odd,*nee,*t;
    t=a->inherent.sh;
    if(t==NULL){
        b->inherent.sh=NULL;
        return;
    }
    odd=(style_link*)malloc(sizeof(style_link));
    odd->s=t->s;
    odd->next=NULL;
    b->inherent.sh=odd;
    t=t->next;
    while(t!=NULL){
        nee=(style_link*)malloc(sizeof(style_link));
        nee->s=t->s;
        nee->next=NULL;
        odd->next=nee;
        odd=nee;
        t=t->next;
    }
}
//Copy the comprehension inherited properties of a to b
void copy_comprehension_style(TreeNode*a, TreeNode*b){
    if(a->comprehensive.sh==NULL){
        b->comprehensive.sh=NULL;
        return;
    }
    style_link*odd,*nee,*t;
    odd=(style_link*)malloc(sizeof(style_link));
    t=a->comprehensive.sh;
    odd->next=NULL;
    odd->s=t->s;
    b->comprehensive.sh=odd;
    t=t->next;
    while(t!=NULL){
        nee=(style_link*)malloc(sizeof(style_link));
        nee->s=t->s;
        nee->next=NULL;
        odd->next=nee;
        t=t->next;
        odd=nee;
    }
}
//Copy the first pure style inherited properties of a to b
void copy_inhernt_pures_1(TreeNode*a, TreeNode* b){
    b->inherent.ph=(ps_link*)malloc(sizeof(ps_link));
    b->inherent.ph->p=a->inherent.ph->p;
    b->inherent.ph->next=NULL;
}
void copy_insert_comprehension_style(TreeNode*a , TreeNode*b){
    style_link*odd,*nee,*h,*t=a->comprehensive.sh;
    if(t==NULL)return;
    odd=(style_link*)malloc(sizeof(style_link));
    odd->s=t->s;
    odd->next=NULL;
    h=odd;
    t=t->next;
    while(t!=NULL){
        nee=(style_link*)malloc(sizeof(style_link));
        nee->s=t->s;
        nee->next=NULL;
        odd->next=nee;
        t=t->next;
        odd=nee;
    }
    odd->next=b->comprehensive.sh;
    b->comprehensive.sh=h;
}
void copy_comprehension_pures1(TreeNode*a ,TreeNode*b){
    b->comprehensive.ph=(ps_link*)malloc(sizeof(ps_link));
    b->comprehensive.ph->p=a->comprehensive.ph->p;
    b->comprehensive.ph->next=NULL;
}
void copy_insert_comprehension_pure_all(TreeNode*a, TreeNode*b){
    ps_link*odd,*nee,*t=a->comprehensive.ph,*h;
    if(a==NULL){
        b->comprehensive.ph=NULL;
        return;
    }
    odd=(ps_link*)malloc(sizeof(ps_link));
    odd->p=t->p;
    odd->next=NULL;
    t=t->next;
    h=odd;
    while(t!=NULL){
        nee=(ps_link*)malloc(sizeof(ps_link));
        nee->p=t->p;
        nee->next=NULL;
        odd->next=nee;
        odd=nee;
        t=t->next;
    }
    odd->next=b->comprehensive.ph;
    b->comprehensive.ph=h;
}
void copy_comprehension_insert2_inhernt_style(TreeNode*a,TreeNode*b){
    style_link*t=a->comprehensive.sh,*odd,*nee,*h;
    if(t==NULL)return;
    odd=(style_link*)malloc(sizeof(style_link));
    odd->s=t->s;
    odd->next=NULL;
    t=t->next;
    h=odd;
    while(t!=NULL){
        nee=(style_link*)malloc(sizeof(style_link));
        nee->s=t->s;
        nee->next=NULL;
        odd->next=nee;
        odd=nee;
        t=t->next;
    }    
    odd->next=b->inherent.sh;
    b->inherent.sh=h;
}

//for printing errors
void print_bug(int lines, int type){
    if(wrong[lines]==1)
        return;
    wrong[lines]=1;
    printf("Error type %d at Line %d: Here is a wrong\n",type,lines);
    return;
}
//Check whether the declared function is defined
void check_ste(){
    function fuct;
    function_link*fl;
    for(int i=0;i<table_MAX;++i){
        fl=ste_table->h[i];
        while (fl!=NULL)
        {
            fuct=search_f(def_table,fl->f.s_name);
            if(fuct.s_name[0]=='\0')
                print_bug(fl->f.lines,18);
            else{
                if(cmp_function(fl->f,fuct)==0)
                    print_bug(fl->f.lines,18);
            }
            fl=fl->next;
        }
        
    }
}

//Used to discuss and process production classification
void fdfs(TreeNode* p){
  // printf("product::%d\n",p->product_id);
    if(p==NULL)return;
    if(p->product_id==1e9+7)return;//Leaf nodes
    if(p->product_id==1){//Program:ExtDefList 
        initst(&stable);
        initat(&atable);
        initft(&ste_table);
        initft(&def_table);
        arcid=0;
        for(int i=0;i<50000;++i)
            wrong[i]=0;
        fdfs(p->firstChild);
       check_ste();
        return ;
    }else if(p->product_id==2){//ExtDefList:         ExtDef ExtDefList
        fdfs(p->firstChild);
        fdfs(get_k_son(2,p));
        return;
    }else if(p->product_id==3){
        return;
    }else if(p->product_id==4){//ExtDef:             Specifier ExtDecList SEMI 
        TreeNode* s1=p->firstChild,*s2=get_k_son(2,p);
        s1->inherent.sign=0;
        fdfs(s1);
        ps_link* pur=(ps_link*)malloc(sizeof(ps_link));
        pur->next=NULL;
        pur->p=s1->comprehensive.ph->p;
        s2->inherent.ph=pur;
        fdfs(s2);
        return ;
    }else if(p->product_id==5){//ExtDef: Specifier SEMI   
        p->firstChild->inherent.sign=0;
        fdfs(p->firstChild);
        return;
    }else if(p->product_id==6){//ExtDef:Specifier FunDec CompSt 
        p->firstChild->inherent.sign=0;
        TreeNode* s1=p->firstChild,*s2=get_k_son(2,p),*s3=get_k_son(3,p);
        fdfs(s1);
        s2->inherent.in_statement=0;
        fdfs(s2);
        ps_link* pur=(ps_link*)malloc(sizeof(ps_link));
        pur->next=NULL;
        pur->p=s1->comprehensive.ph->p;
        s3->inherent.ph=pur;
        char name[40];
        strncpy(name,s2->comprehensive.name,40);
        function fff=search_f(def_table,name);
        if(fff.s_name[0]!='\0'){
            print_bug(p->lineno,4);
            fdfs(s3);
            return;
        }
        function f;
        f.return_type=s1->comprehensive.ph->p;
        strncpy(f.s_name,name,40);
        f.lines=p->lineno;
        style_link*t=s2->comprehensive.sh;
        if(t==NULL){
            f.head=NULL;
            insert_f(def_table,f);
            fff=search_f(ste_table,name);
            if(fff.s_name[0]=='\0'){
                insert_f(ste_table,f);
            }else{
                if(cmp_function(f,fff)==0){
                    print_bug(p->lineno,19);
                }
            }
            fdfs(s3);
            return;
        }
        ps_link*odd=(ps_link*)malloc(sizeof(ps_link)),*nee;
        odd->p=style_to_pure(t->s);
        odd->next=NULL;
        f.head=odd;
        t=t->next;
        while(t!=NULL){
            nee=(ps_link*)malloc(sizeof(ps_link));
            nee->p=style_to_pure(t->s);
            nee->next=NULL;
            odd->next=nee;
            t=t->next;
            odd=nee;
        }
        insert_f(def_table,f);
            fff=search_f(ste_table,name);
            if(fff.s_name[0]=='\0'){
                insert_f(ste_table,f);
            }else{
                if(cmp_function(f,fff)==0){
                    print_bug(p->lineno,19);
                }
            }
        fdfs(s3);
        return;
    }else if(p->product_id==7){//Specifier FunDec SEMI 
        p->firstChild->inherent.sign=0;
        TreeNode* s1=p->firstChild,*s2=get_k_son(2,p);
        fdfs(s1);
        s2->inherent.in_statement=1;
        fdfs(s2);
        char name[40];
        strncpy(name,s2->comprehensive.name,40);
        function f;
        f.return_type=s1->comprehensive.ph->p;
        strncpy(f.s_name,name,40);
        f.lines=p->lineno;
        style_link*t=s2->comprehensive.sh;
        if(t==NULL){
            f.head=NULL;
        }else{
            ps_link*odd=(ps_link*)malloc(sizeof(ps_link)),*nee;
            odd->p=style_to_pure(t->s);
            odd->next=NULL;
            f.head=odd;
            t=t->next;
            while(t!=NULL){
                nee=(ps_link*)malloc(sizeof(ps_link));
                nee->p=style_to_pure(t->s);
                nee->next=NULL;
                odd->next=nee;
                t=t->next;
                odd=nee;
            }
        }
        function fff=search_f(ste_table,name);
        if(fff.s_name[0]=='\0'){
            insert_f(ste_table,f);
        }else{
            if(cmp_function(f,fff)==0){
                print_bug(p->lineno,19);
            }
        }
        return;
    }else if(p->product_id==8){//ExtDecList:         VarDec 
        p->firstChild->inherent.sign=0;
        TreeNode* s1=p->firstChild;
        ps_link* pur=(ps_link*)malloc(sizeof(ps_link));
        pur->next=NULL;
        pur->p=p->inherent.ph->p;
        s1->inherent.ph=pur;
        s1->inherent.in_statement=0;
        fdfs(s1);
        return;
    }else if(p->product_id==9){//VarDec COMMA ExtDecList  
        p->firstChild->inherent.sign=0;
        TreeNode* s1=p->firstChild,*s3=get_k_son(3,p);
        ps_link* pur=(ps_link*)malloc(sizeof(ps_link));
        pur->next=NULL;
        pur->p=p->inherent.ph->p;
        s1->inherent.ph=pur;
        s1->inherent.in_statement=0;
      //  printf("9-0\n");
        fdfs(s1);
      //  printf("9-1\n");
        pur=(ps_link*)malloc(sizeof(ps_link));
        pur->next=NULL;
        pur->p=p->inherent.ph->p;
        s3->inherent.ph=pur;
        fdfs(s3);
        return;
    }else if(p->product_id==10){//Specifier:          TYPE  
        TreeNode* s1=p->firstChild;
        pure_style pss;
        if(s1->name[0]=='i'){
            pss.type=1;
        }else if(s1->name[0]=='f'){
            pss.type=2;
        }else{
            printf("BUGGGGGGGGGGGGG on product 10,%d\n",p->lineno);
            pss.type=1;
        }
        pss.dimension=1;
        pss.struct_id=-1;
        ps_link* t=(ps_link*)malloc(sizeof(ps_link));
        t->p=pss;
        t->next=NULL;
        p->comprehensive.ph=t;
        return;
    }else if(p->product_id==11){//Specifier: StructSpecifier 
        p->firstChild->inherent.sign=p->inherent.sign;
        if(p->inherent.sign==1){
            copy_inhernt_style(p,p->firstChild);
        }
        TreeNode* s1=p->firstChild;
        fdfs(s1);
        ps_link* t=(ps_link*)malloc(sizeof(ps_link));
        t->p=s1->comprehensive.ph->p;
        t->next=NULL;
        p->comprehensive.ph=t;
        return ;
    }else if(p->product_id==12){//StructSpecifier:    STRUCT OptTag LC DefList RC 
        TreeNode* s2=get_k_son(2,p),*s4=get_k_son(4,p);
        s4->inherent.sign=1;
        s4->inherent.sh=NULL;
        fdfs(s2);
        // printf("12-1\n");
        fdfs(s4);
        char name[40];
        strncpy(name,s2->comprehensive.name,40);
        architecture arc;
        if(name[0]!='\0'){//may be have no name
            arc=search_a(atable,name);
            if(arc.s_name[0]!='\0'){
                print_bug(p->lineno,16);
                ps_link* t=(ps_link*)malloc(sizeof(ps_link));
                t->next=NULL;
                t->p.type=3;
                t->p.dimension=1;
                t->p.struct_id=arc.struct_id;
                p->comprehensive.ph=t;
                return;
            }
            style  ssst=search_s(stable,name);
            if(ssst.s_name[0]!='\0'){
                print_bug(p->lineno,16);
            }
            if(p->inherent.sign==1){
                style_link* slk=p->inherent.sh;
                while(slk!=NULL){
                    if(cmp(slk->s.s_name,name)==1){
                        print_bug(p->lineno,16);
                        break;
                    }
                    slk=slk->next;
                }
            }
        }
        strncpy(arc.s_name,name,40);
        arc.struct_id=++arcid;
        if(s4->comprehensive.sh==NULL){
            arc.head=NULL;
        }else{
            style_link*odd=(style_link*)malloc(sizeof(style_link)),*nee,*t;
            odd->next=NULL;
            odd->s=s4->comprehensive.sh->s;
            arc.head=odd;
            t=s4->comprehensive.sh->next;
            while(t!=NULL){
                nee=(style_link*)malloc(sizeof(style_link));
                nee->s=t->s;
                nee->next=NULL;
                odd->next=nee;
                t=t->next;
                odd=nee;
            }
        }
        arc_table[arcid]=arc;
        if(name[0]!='\0'){
            insert_a(atable,arc);
        }
        ps_link* t=(ps_link*)malloc(sizeof(ps_link));
        t->next=NULL;
        t->p.type=3;
        t->p.dimension=1;
        t->p.struct_id=arc.struct_id;
        p->comprehensive.ph=t;
        return;
    }else if(p->product_id==13){//STRUCT Tag 
        TreeNode*s2=get_k_son(2,p);
        fdfs(s2);
        char name[40];
        strncpy(name,s2->comprehensive.name,40);
        architecture arc;
        arc=search_a(atable,name);
        if(arc.s_name[0]!='\0'){
            ps_link* t=(ps_link*)malloc(sizeof(ps_link));
            t->next=NULL;
            t->p.type=3;
            t->p.dimension=1;
            t->p.struct_id=arc.struct_id;
            p->comprehensive.ph=t;
            return;
        }else{
            print_bug(p->lineno,17);
            strncpy(arc.s_name,name,40);
            arc.head=NULL;
            arc.struct_id=++arcid;
            arc_table[arcid]=arc;
            //insert_a(atable,arc);
            ps_link* t=(ps_link*)malloc(sizeof(ps_link));
            t->next=NULL;
            t->p.type=3;
            t->p.dimension=1;
            t->p.struct_id=arc.struct_id;
            p->comprehensive.ph=t;
            return;
        }
    }else if(p->product_id==14){//OptTag:             SID 
        TreeNode*s1=p->firstChild;
        int i=0;
        while(s1->name[i]!='\0'){
            p->comprehensive.name[i]=s1->name[i];
            ++i;
        }
        p->comprehensive.name[i]='\0';
        return;
    }else if(p->product_id==15){//OptTag: 
        p->comprehensive.name[0]='\0';
        p->comprehensive.name[1]='\0';
        return;
    }else if(p->product_id==16){//Tag:                SID   
         TreeNode*s1=p->firstChild;
        int i=0;
        while(s1->name[i]!='\0'){
            p->comprehensive.name[i]=s1->name[i];
            ++i;
        }
        p->comprehensive.name[i]='\0';
        return;
    }else if(p->product_id==17 || p->product_id==18){//VarDec:             ID  |SID
        TreeNode*s1=p->firstChild;
        char name[40];
        int i=0;
        while(s1->name[i]!='\0'){
            name[i]=s1->name[i];
            ++i;
        }
        name[i]='\0';
        if(p->inherent.in_statement==1){//If it is a function declaration
            style_link* slk=(style_link*)malloc(sizeof(style_link));
            slk->s.dimension=p->inherent.ph->p.dimension;
            slk->s.struct_id=p->inherent.ph->p.struct_id;
            slk->s.type=p->inherent.ph->p.type;
            slk->next=NULL;
            strncpy(slk->s.s_name,name,40);
            p->comprehensive.sh=slk;
            return;
        }
        style sss=search_s(stable,name);
        if(sss.s_name[0]!='\0' && p->inherent.sign==0){
            print_bug(p->lineno,3);
        }
        architecture arc=search_a(atable,name);
        if(arc.s_name[0]!='\0'){
            print_bug(p->lineno,3);
        }
        if(p->inherent.sign==1){
            style_link* slk=p->inherent.sh;
            int jud=0;
            while(slk!=NULL){
                if(cmp(slk->s.s_name,name)==1){
                    print_bug(p->lineno,15);
                    jud=1;
                    break;
                }
                slk=slk->next;
            }
            slk=(style_link*)malloc(sizeof(style_link));
            slk->s.dimension=p->inherent.ph->p.dimension;
            slk->s.struct_id=p->inherent.ph->p.struct_id;
            slk->s.type=p->inherent.ph->p.type;
            slk->next=NULL;
            strncpy(slk->s.s_name,name,40);
            p->comprehensive.sh=slk;
        }else{
            style ss;
            ss.dimension=p->inherent.ph->p.dimension;
            ss.type=p->inherent.ph->p.type;
            ss.struct_id=p->inherent.ph->p.struct_id;
            strncpy(ss.s_name,name,40);
            insert_s(stable,ss);
            style_link* slk=(style_link*)malloc(sizeof(style_link));
            slk->s.dimension=p->inherent.ph->p.dimension;
            slk->s.struct_id=p->inherent.ph->p.struct_id;
            slk->s.type=p->inherent.ph->p.type;
            slk->next=NULL;
            strncpy(slk->s.s_name,name,40);
            p->comprehensive.sh=slk;
        }
        return;
    }else if(p->product_id==119){//  VarDec LB INT RB  
        TreeNode*s1=p->firstChild;
        s1->inherent.sign=p->inherent.sign;
        ps_link* ppp=(ps_link*)malloc(sizeof(ps_link));
        ppp->next=NULL;
        ppp->p=p->inherent.ph->p;
        ppp->p.dimension+=1;
        s1->inherent.ph=ppp;
        if(s1->inherent.sign==1){
            copy_inhernt_style(p,s1);
        }
        s1->inherent.in_statement=p->inherent.in_statement;
        fdfs(s1);
        copy_comprehension_style(s1,p);
        return;
    }else if(p->product_id==19||p->product_id==20){//FunDec:             ID or SID LP VarList RP  
        TreeNode*s1=p->firstChild,*s3=get_k_son(3,p);
        int i=0;
        while(s1->name[i]!='\0'){
            p->comprehensive.name[i]=s1->name[i];
            ++i;
        }
        p->comprehensive.name[i]='\0';
        s3->inherent.in_statement=p->inherent.in_statement;
        fdfs(s3);
        copy_comprehension_style(s3,p);
        return;
    }else if(p->product_id==21||p->product_id==22){// ID or SID LP  RP  
        TreeNode*s1=p->firstChild;
        int i=0;
        while(s1->name[i]!='\0'){
            p->comprehensive.name[i]=s1->name[i];
            ++i;
        }
        p->comprehensive.name[i]='\0';
        p->comprehensive.sh=NULL;
        return;
    }else if(p->product_id==23){//VarList:            ParamDec COMMA VarList 
        TreeNode*s1=get_k_son(1,p),*s3=get_k_son(3,p);
        s1->inherent.in_statement=p->inherent.in_statement;
        s3->inherent.in_statement=p->inherent.in_statement;
        fdfs(s1);
        fdfs(s3);
        copy_comprehension_style(s3,p);
        copy_insert_comprehension_style(s1,p);
        return;
    }else if(p->product_id==24){//VarList: ParamDec   
        TreeNode*s1=p->firstChild;
        s1->inherent.in_statement=p->inherent.in_statement;
        fdfs(s1);
        copy_comprehension_style(s1,p);
    }else if(p->product_id==25){//ParamDec:           Specifier VarDec  
        TreeNode*s1=p->firstChild,*s2=get_k_son(2,p);
        s1->inherent.sign=0;
        s1->inherent.sh=NULL;
        fdfs(s1);
        s2->inherent.sign=0;
        s2->inherent.sh=NULL;
        s2->inherent.ph=(ps_link*)malloc(sizeof(ps_link));
        s2->inherent.ph->p=s1->comprehensive.ph->p;
        s2->inherent.ph->next=NULL;
        s2->inherent.in_statement=p->inherent.in_statement;
        fdfs(s2);
        copy_comprehension_style(s2,p);
    }else if(p->product_id==26){//CompSt:             LC DefList StmtList RC   
        TreeNode*s2=get_k_son(2,p),*s3=get_k_son(3,p);
        s2->inherent.sign=0;
        s2->inherent.sh=NULL;
        fdfs(s2);
        s3->inherent.ph=(ps_link*)malloc(sizeof(ps_link));
        s3->inherent.ph->p=p->inherent.ph->p;
        s3->inherent.ph->next=NULL;
        fdfs(s3);
        return;
    }else if(p->product_id==27){//StmtList:           Stmt StmtList    
        TreeNode*s1=p->firstChild,*s2=get_k_son(2,p);
        copy_inhernt_pures_1(p,s1);
        copy_inhernt_pures_1(p,s2);
        fdfs(s1);
        fdfs(s2);
        return;
    }else if(p->product_id==28){
        return;
    }else if(p->product_id==29){//Stmt:               Exp SEMI   
        fdfs(p->firstChild);
        return;
    }else if(p->product_id==30){//Stmt:       CompSt   
        copy_inhernt_pures_1(p,p->firstChild);
        fdfs(p->firstChild);
        return;
    }else if(p->product_id==31){// RETURN Exp SEMI  
        TreeNode*s2=get_k_son(2,p);
        fdfs(s2);
        if(s2->comprehensive.ph->p.type!=p->inherent.ph->p.type){
            print_bug(p->lineno,8);
        }else if(s2->comprehensive.ph->p.dimension!=p->inherent.ph->p.dimension)
        {
            print_bug(p->lineno,8);
        }else if(p->inherent.ph->p.type==3 && s2->comprehensive.ph->p.struct_id!=p->inherent.ph->p.struct_id){
            print_bug(p->lineno,8);
        }
        return;
    }else if(p->product_id==32){// IF LP Exp RP Stmt
        TreeNode*s3=get_k_son(3,p),*s5=get_k_son(5,p);
        fdfs(s3);
         if((s3->comprehensive.ph->p.type!=0 && s3->comprehensive.ph->p.type!=1 )||(s3->comprehensive.ph->p.dimension!=1)){
            print_bug(p->lineno,7);
        }
        copy_inhernt_pures_1(p,s5);
        fdfs(s5);
    }else if(p->product_id==33){// IF LP Exp RP Stmt ELSE Stmt  
        TreeNode*s3=get_k_son(3,p),*s5=get_k_son(5,p),*s7=get_k_son(7,p);
        fdfs(s3);
         if((s3->comprehensive.ph->p.type!=0 && s3->comprehensive.ph->p.type!=1) || (s3->comprehensive.ph->p.dimension!=1)){
            print_bug(p->lineno,7);
        }
        copy_inhernt_pures_1(p,s5);
        fdfs(s5);
        copy_inhernt_pures_1(p,s7);
        fdfs(s7);
    }else if(p->product_id==34){// WHILE LP Exp RP Stmt   
        TreeNode*s3=get_k_son(3,p),*s5=get_k_son(5,p);
        fdfs(s3);
         if((s3->comprehensive.ph->p.type!=0 && s3->comprehensive.ph->p.type!=1)|| (s3->comprehensive.ph->p.dimension!=1)){
            print_bug(p->lineno,7);
        }
        copy_inhernt_pures_1(p,s5);
        fdfs(s5);
        return;
    }else if(p->product_id==35){//DefList:            Def DefList 
        TreeNode*s1=p->firstChild,*s2=get_k_son(2,p);
        s1->inherent.sign=p->inherent.sign;
        if(s1->inherent.sign==1){
            copy_inhernt_style(p,s1);
        }
        fdfs(s1);
       // printf("35-1\n");
        s2->inherent.sign=p->inherent.sign;
        if(s2->inherent.sign==1){
            copy_inhernt_style(p,s2);
            copy_comprehension_insert2_inhernt_style(s1,s2);
        }
        fdfs(s2);
       // printf("35-2\n");
        copy_comprehension_style(s2,p);
        //printf("35-3\n");
        style_link*odd,*nee,*ttt=s1->comprehensive.sh,*h;
        odd=(style_link*)malloc(sizeof(style_link));
        odd->s=ttt->s;
        odd->next=NULL;
        ttt=ttt->next;
        h=odd;
        while(ttt!=NULL){
            nee=(style_link*)malloc(sizeof(style_link));
            nee->s=ttt->s;
            nee->next=NULL;
            odd->next=nee;
            odd=nee;
            ttt=ttt->next;
        }
        odd->next=p->comprehensive.sh;
        p->comprehensive.sh=h;
        return;
    }else if(p->product_id==36){
        return;
    }else if(p->product_id==37){//Def:                Specifier DecList SEMI  
        TreeNode*s1=p->firstChild,*s2=get_k_son(2,p);
        s1->inherent.sign=p->inherent.sign;
        if(s1->inherent.sign==1){
            copy_inhernt_style(p,s1);
        }
        fdfs(s1);
        s2->inherent.sign=p->inherent.sign;
        if(s2->inherent.sign==1){
            copy_inhernt_style(p,s2);
        }
        s2->inherent.ph=(ps_link*)malloc(sizeof(ps_link));
        s2->inherent.ph->p=s1->comprehensive.ph->p;
        s2->inherent.ph->next=NULL;
        fdfs(s2);
        copy_comprehension_style(s2,p);
        return;
    }else if(p->product_id==38){//DecList:            Dec   
        TreeNode*s1=p->firstChild;
        copy_inhernt_pures_1(p,s1);
        s1->inherent.sign=p->inherent.sign;
        if(p->inherent.sign==1){
            copy_inhernt_style(p,s1);
        }
        fdfs(s1);
        copy_comprehension_style(s1,p);
        return;
    }else if(p->product_id==39){//  Dec COMMA DecList  
        TreeNode*s1=p->firstChild,*s3=get_k_son(3,p);
        copy_inhernt_pures_1(p,s1);
        s1->inherent.sign=p->inherent.sign;
        if(p->inherent.sign==1){
            copy_inhernt_style(p,s1);
            copy_comprehension_insert2_inhernt_style(s1,s3);
        }
        fdfs(s1);
        copy_inhernt_pures_1(p,s3);
        s3->inherent.sign=p->inherent.sign;
        if(p->inherent.sign==1){
            copy_inhernt_style(p,s3);
        }
        fdfs(s3);
        copy_comprehension_style(s3,p);
        copy_insert_comprehension_style(s1,p);
        return;
    }else if(p->product_id==40){//Dec:                VarDec 
        TreeNode*s1=p->firstChild;
        copy_inhernt_pures_1(p,s1);
        s1->inherent.sign=p->inherent.sign;
        if(p->inherent.sign==1){
            copy_inhernt_style(p,s1);
        }
        s1->inherent.in_statement=0;
        fdfs(s1);
        copy_comprehension_style(s1,p);
        return;
    }else if(p->product_id==41){//VarDec ASSIGNOP Exp 
        TreeNode*s1=p->firstChild,*s3=get_k_son(3,p);
        s1->inherent.sign=p->inherent.sign;
        copy_inhernt_pures_1(p,s1);
        if(s1->inherent.sign==1){
            copy_inhernt_style(p,s1);
            print_bug(p->lineno,15);
        }
        s1->inherent.in_statement=0;
        fdfs(s1);
        copy_comprehension_style(s1,p);
        fdfs(s3);
        if( cmp_pure_style( style_to_pure(s1->comprehensive.sh->s), s3->comprehensive.ph->p )==0 ){
            print_bug(p->lineno,5);
        }
         copy_comprehension_style(s1,p);
        return;
    }else if(p->product_id==42){//Exp:                Exp ASSIGNOP Exp  
        TreeNode*s1=p->firstChild,*s3=get_k_son(3,p);
        fdfs(s1);
        fdfs(s3);
        if(s1->comprehensive.sign==1){
            print_bug(p->lineno,6);
        }
        if(cmp_pure_style(s1->comprehensive.ph->p,s3->comprehensive.ph->p)==0){
            print_bug(p->lineno,5);
        }
        p->comprehensive.sign=1;
        copy_comprehension_pures1(s1,p);
        return;
    }else if(p->product_id==43||p->product_id==44||p->product_id==45){// Exp AND/OR/RELOP Exp   
        TreeNode*s1=p->firstChild,*s3=get_k_son(3,p);
        fdfs(s1);
        fdfs(s3);
        if(s1->comprehensive.ph->p.type==1){
            if(s3->comprehensive.ph->p.type!=1 || s1->comprehensive.ph->p.dimension!=1 || s3->comprehensive.ph->p.dimension!=1)
                print_bug(p->lineno,7);
        }else{
            if(s1->comprehensive.ph->p.type==2){
                if(s3->comprehensive.ph->p.type!=2 || s1->comprehensive.ph->p.dimension!=1 || s3->comprehensive.ph->p.dimension!=1)
                    print_bug(p->lineno,7);
            }else{
            print_bug(p->lineno,7);
            }
        }
        p->comprehensive.sign=1;
        p->comprehensive.ph=(ps_link*)malloc(sizeof(ps_link));
        p->comprehensive.ph->next=NULL;
        p->comprehensive.ph->p.dimension=1;
        p->comprehensive.ph->p.type=1;
        p->comprehensive.ph->p.struct_id=-1;
        return;
    }else if(p->product_id==46 || p->product_id==47||p->product_id==48||p->product_id==49){//Exp +/-/*/divide Exp  
        TreeNode*s1=p->firstChild,*s3=get_k_son(3,p);
        fdfs(s1);
        fdfs(s3);
        p->comprehensive.sign=1;
        p->comprehensive.ph=(ps_link*)malloc(sizeof(ps_link));
        p->comprehensive.ph->next=NULL;
        p->comprehensive.ph->p.dimension=1;
        p->comprehensive.ph->p.type=1;
        p->comprehensive.ph->p.struct_id=-1;
        if(s1->comprehensive.ph->p.type==1){
            if(s3->comprehensive.ph->p.type!=1 || s1->comprehensive.ph->p.dimension!=1 || s3->comprehensive.ph->p.dimension!=1){
                print_bug(p->lineno,7);
            }
        }else if(s1->comprehensive.ph->p.type==2){
            if(s3->comprehensive.ph->p.type!=2||s1->comprehensive.ph->p.dimension!=1 || s3->comprehensive.ph->p.dimension!=1)
                print_bug(p->lineno,7);
            p->comprehensive.ph->p.type=2;
        }else{
            print_bug(p->lineno,7);
        }
        return;
    }else if(p->product_id==50){//LP Exp RP
        TreeNode*s2=get_k_son(2,p);
        fdfs(s2);
        copy_comprehension_pures1(s2,p);
        return;
    }else if(p->product_id==51){//MINUS Exp 
        TreeNode*s2=get_k_son(2,p);
        fdfs(s2);
        p->comprehensive.sign=1;
        p->comprehensive.ph=(ps_link*)malloc(sizeof(ps_link));
        p->comprehensive.ph->next=NULL;
        p->comprehensive.ph->p.dimension=1;
        p->comprehensive.ph->p.type=1;
        p->comprehensive.ph->p.struct_id=-1;
        if(s2->comprehensive.ph->p.type==1){
            return;
        }else if(s2->comprehensive.ph->p.type==2){
            p->comprehensive.ph->p.type=2;
            return;
        }else{
            print_bug(p->lineno,7);
            return;
        }
    }else if(p->product_id==52){// NOT Exp  
        TreeNode*s2=get_k_son(2,p);
        fdfs(s2);
        p->comprehensive.sign=1;
        p->comprehensive.ph=(ps_link*)malloc(sizeof(ps_link));
        p->comprehensive.ph->next=NULL;
        p->comprehensive.ph->p.dimension=1;
        p->comprehensive.ph->p.type=1;
        p->comprehensive.ph->p.struct_id=-1;
        if(s2->comprehensive.ph->p.type!=1)
            print_bug(p->lineno,7);
        return;
    }else if(p->product_id==53||p->product_id==54){//ID/SID LP Args RP 
        TreeNode*s1=p->firstChild,*s3=get_k_son(3,p);
        char name[40];
        int i=0;
        while (s1->name[i]!='\0')
        {
            name[i]=s1->name[i];
            ++i;
        }
        name[i]='\0';
        function fff=search_f(ste_table,name);
        p->comprehensive.sign=1;
        p->comprehensive.ph=(ps_link*)malloc(sizeof(ps_link));
        p->comprehensive.ph->next=NULL;
        p->comprehensive.ph->p.dimension=1;
        p->comprehensive.ph->p.type=4;
        p->comprehensive.ph->p.struct_id=-1;
        if(fff.s_name[0]=='\0'){
            style stl=search_s(stable,name);
            if(stl.s_name[0]!='\0')
                print_bug(p->lineno,11);
            else
                print_bug(p->lineno,2);
            return;
        }
        p->comprehensive.ph->p=fff.return_type;
        fdfs(s3);
        ps_link*ps1=fff.head,*ps2=s3->comprehensive.ph;
        while(ps1!=NULL && ps2!=NULL){
            if(cmp_pure_style(ps1->p,ps2->p)==0){
                print_bug(p->lineno,9);
                return;
            }
            ps1=ps1->next;
            ps2=ps2->next;
        }
        if(ps1!=NULL||ps2!=NULL){
            print_bug(p->lineno,9);
            return;
        }
        return;
    }else if(p->product_id==55||p->product_id==56){//ID|SID LP RP
        TreeNode*s1=p->firstChild;
         char name[40];
        int i=0;
        while (s1->name[i]!='\0')
        {
            name[i]=s1->name[i];
            ++i;
        }
        name[i]='\0';
        function fff=search_f(ste_table,name);
        p->comprehensive.sign=1;
        p->comprehensive.ph=(ps_link*)malloc(sizeof(ps_link));
        p->comprehensive.ph->next=NULL;
        p->comprehensive.ph->p.dimension=1;
        p->comprehensive.ph->p.type=4;
        p->comprehensive.ph->p.struct_id=-1;
        if(fff.s_name[0]=='\0'){
            style stl=search_s(stable,name);
            if(stl.s_name[0]!='\0')
                print_bug(p->lineno,11);
            else
                print_bug(p->lineno,2);
            return;
        }
        p->comprehensive.ph->p=fff.return_type;
        if(fff.head!=NULL){
            print_bug(p->lineno,9);
        }
        return;
    }else if(p->product_id==57){//Exp LB Exp RB 
        TreeNode*s1=p->firstChild,*s3=get_k_son(3,p);
        fdfs(s1);
        copy_comprehension_pures1(s1,p);
        p->comprehensive.ph->p.dimension-=1;
        if(p->comprehensive.ph->p.dimension<=0){
            print_bug(p->lineno,10);
            return;
        }
        fdfs(s3);
        if(s3->comprehensive.ph->p.type!=1 || s3->comprehensive.ph->p.dimension!=1)
            print_bug(p->lineno,12);
        return;
    }else if(p->product_id==58 || p->product_id==59){//Exp . ID
        TreeNode*s1=p->firstChild,*s3=get_k_son(3,p);
        fdfs(s1);
        pure_style pst=s1->comprehensive.ph->p;
        copy_comprehension_pures1(s1,p);
        if(pst.type!=3){
            print_bug(p->lineno,13);
            return;
        }
        char name[40];
        int i=0;
        while (s3->name[i]!='\0')
        {
            name[i]=s3->name[i];
            ++i;
        }
        name[i]='\0';
        architecture arc=arc_table[pst.struct_id];
        style_link*sll=arc.head;
        while(sll!=NULL&&cmp(sll->s.s_name,name)==0){
            sll=sll->next;
        }
        if(sll==NULL){
            print_bug(p->lineno,14);
            return;
        }
        p->comprehensive.ph->p=style_to_pure(sll->s);
        return;
    }else if(p->product_id==60||p->product_id==61){//ID | SID
        TreeNode*s1=p->firstChild;
         char name[40];
        int i=0;
        while (s1->name[i]!='\0')
        {
            name[i]=s1->name[i];
            ++i;
        }
        name[i]='\0';
        p->comprehensive.ph=(ps_link*)malloc(sizeof(ps_link));
        p->comprehensive.ph->next=NULL;
        p->comprehensive.sign=0;
        style sty=search_s(stable,name);
        if(sty.s_name[0]=='\0'){
            print_bug(p->lineno,1);
            p->comprehensive.ph->p.dimension=-1;
            p->comprehensive.ph->p.type=4;
            p->comprehensive.ph->p.struct_id=-1;
            return;
        }
        p->comprehensive.ph->p=style_to_pure(sty);
        return;
    }else if(p->product_id==62){//int
        p->comprehensive.ph=(ps_link*)malloc(sizeof(ps_link));
        p->comprehensive.ph->next=NULL;
        p->comprehensive.sign=1;
        p->comprehensive.ph->p.dimension=1;
        p->comprehensive.ph->p.type=1;
        p->comprehensive.ph->p.struct_id=-1;
        return;
    }else if(p->product_id==63){//float
        p->comprehensive.ph=(ps_link*)malloc(sizeof(ps_link));
        p->comprehensive.ph->next=NULL;
        p->comprehensive.sign=1;
        p->comprehensive.ph->p.dimension=1;
        p->comprehensive.ph->p.type=2;
        p->comprehensive.ph->p.struct_id=-1;
        return;
    }else if(p->product_id==64){//Args: Exp COMMA Args 
        TreeNode*s1=p->firstChild,*s3=get_k_son(3,p);
        fdfs(s1);
        fdfs(s3);
        p->comprehensive.ph=NULL;
        copy_insert_comprehension_pure_all(s3,p);
        copy_insert_comprehension_pure_all(s1,p);
        return;
    }else if(p->product_id==65){//Args: Exp
        TreeNode*s1=p->firstChild;
        fdfs(s1);
        copy_comprehension_pures1(s1,p);
        return;
    }
    
}