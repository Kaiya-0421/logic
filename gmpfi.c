#include "gmpfi.h"

int gmpfi_write_edge(int a, int b, int c){
    if(op[b]==NUM){
        if(op[c]==NUM){
            head[a]->head = -1;
            #ifndef CHANGE_LIST
            addlistSequentially(a);
            #else
            addNodeListSequentially(a);
            #endif
        }else{
            head[a]->head = 1;
            pushEdge(c,a);  //aに(a->b)エッジを追加 
        }
    }else if(op[c]==NUM){
        head[a]->head = 1;
        pushEdge(b,a);  //aに(a->b)エッジを追加 
    }else{
        head[a]->head = 2;
        pushEdge(b,a);  //aに(a->b)エッジを追加
        pushEdge(c,a);  //aに(a->b)エッジを追加 
    }
    return 0;
}

#ifdef VER11
int gmpfi_write_edge3(int a, int b, int c, int d){
    if(op[b]==NUM){
        if(op[c]==NUM){
            if(op[d]==NUM){
                head[a]->head = -1;
                #ifndef CHANGE_LIST
                addlistSequentially(a);
                #else
                addNodeListSequentially(a);
                #endif
            }else{
                head[a]->head = 1;
                pushEdge(d,a);  
            }
        }else{
            if(op[d]==NUM){
                head[a]->head = 1;
                pushEdge(c,a);  
            }else{
                head[a]->head = 2;
                pushEdge(c,a);  
                pushEdge(d,a);
            }
        }
    }else{
        if(op[c]==NUM){
            if(op[d]==NUM){
                head[a]->head = 1;
                pushEdge(b,a); 
            }else{
                head[a]->head = 2;
                pushEdge(b,a);  
                pushEdge(d,a); 
            }
        }else{
            if(op[d]==NUM){
                head[a]->head = 2;
                pushEdge(b,a);  
                pushEdge(c,a); 
            }else{
                head[a]->head = 3;
                pushEdge(b,a);  
                pushEdge(c,a);  
                pushEdge(d,a);
            }
        }
    }
    return 0;
}
#endif


int gmpfi_add(gmpfi_ptr a, gmpfi_ptr b, gmpfi_ptr c){
    initEdge(count);
    #ifdef VER12
    if(a->store==1){
        data_mpfi[count].index=-1;
        // data_mpfi[count].a = &(a->mpfi);
    }else{
        data_mpfi[count].index=-2;
        data_mpfi[count].a = &(a->mpfi);
    }
    #else
    data_mpfi[count].a = &(a->mpfi);
    #endif
    data_mpfi[count].b = b->id;
    data_mpfi[count].c = c->id;
    op[count]=ADD;
    gmpfi_write_edge(count, b->id, c->id);
    a->id = count;
    count++;
    return 1;
}

int gmpfi_sub(gmpfi_ptr a, gmpfi_ptr b, gmpfi_ptr c){
    initEdge(count);
    #ifdef VER12
    if(a->store==1){
        data_mpfi[count].index=-1;
        // data_mpfi[count].a = &(a->mpfi);
    }else{
        data_mpfi[count].index=-2;
        data_mpfi[count].a = &(a->mpfi);
    }
    #else
    data_mpfi[count].a = &(a->mpfi);
    #endif
    data_mpfi[count].b = b->id;
    data_mpfi[count].c = c->id;
    op[count]=SUB;
    gmpfi_write_edge(count, b->id, c->id);
    a->id = count;
    count++;
    return 1;
}

int gmpfi_mul(gmpfi_ptr a, gmpfi_ptr b, gmpfi_ptr c){
    initEdge(count);
    #ifdef VER12
    if(a->store==1){
        data_mpfi[count].index=-1;
        // data_mpfi[count].a = &(a->mpfi);
    }else{
        data_mpfi[count].index=-2;
        data_mpfi[count].a = &(a->mpfi);
    }
    #else
    data_mpfi[count].a = &(a->mpfi);
    #endif
    data_mpfi[count].b = b->id;
    data_mpfi[count].c = c->id;
    op[count]=MUL;
    gmpfi_write_edge(count, b->id, c->id);
    a->id = count;
    count++;
    return 1;
}

int gmpfi_div(gmpfi_ptr a, gmpfi_ptr b, gmpfi_ptr c){
    initEdge(count);
    #ifdef VER12
    if(a->store==1){
        data_mpfi[count].index=-1;
        // data_mpfi[count].a = &(a->mpfi);
    }else{
        data_mpfi[count].index=-2;
        data_mpfi[count].a = &(a->mpfi);
    }
    #else
    data_mpfi[count].a = &(a->mpfi);
    #endif
    data_mpfi[count].b = b->id;
    data_mpfi[count].c = c->id;
    op[count]=DIV;
    gmpfi_write_edge(count, b->id, c->id);
    a->id = count;
    count++;
    return 1;
}

#ifdef VER11
int gmpfi_mulsub(gmpfi_ptr a, gmpfi_ptr b, gmpfi_ptr c, gmpfi_ptr d){
    initEdge(count);
    #ifdef VER12
    if(a->store==1){
        data_mpfi[count].index=-1;
        data_mpfi[count].a = &(a->mpfi);
    }else{
        data_mpfi[count].index=-2;
        data_mpfi[count].a = &(a->mpfi);
    }
    #else
    data_mpfi[count].a = &(a->mpfi);
    #endif
    data_mpfi[count].b = b->id;
    data_mpfi[count].c = c->id;
    data_mpfi[count].d = d->id;
    op[count]=MULSUB;
    gmpfi_write_edge3(count, b->id, c->id, d->id);
    a->id = count;
    count++;
    return 1;
}
#endif

void gmpfi_set_str (gmpfi_ptr a , const char *str,int n){
    initEdge(count);
    op[count]=NUM;
    a->id = count;
    #ifdef VER12
    if(a->store==1){
        data_mpfi[count].index=-1;
        // data_mpfi[count].a = &(a->mpfi);
    }else{
        data_mpfi[count].index=-2;
        data_mpfi[count].a = &(a->mpfi);
    }
    #else
    data_mpfi[count].a = &(a->mpfi);
    #endif
    count++;
    mpfi_set_str(a->mpfi, str, n);
}

void gmpfi_init2(gmpfi_ptr a,int prec){
    if(isWrite==0){
        cmpfi_create();
    }
    mpfi_init2(a->mpfi, prec);
    #ifdef VER12
    a->store=0;
    #endif
}

void gmpfi_clear(gmpfi_ptr a){
    mpfi_clear(a->mpfi);
}