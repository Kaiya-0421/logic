#include "gmpfi.h"

#ifdef VER12
int storesize=STORE_SIZE;
volatile int store_mpfi_flag=-1;
volatile int store_mpfi_count=0;
int max = -1;
#ifdef BUFF 
volatile int in=0;
volatile int out=0;
int *store_list=NULL;
int store_list_count;
#endif


int clearStore_MPFI(int threadID,int dataID);

void init_storeData(int acc){
    int i;
    store_mpfi = (__store_mpfi *)malloc(storesize * sizeof(__store_mpfi));
    if(store_mpfi == NULL){
        printf("store_mpfi malloc error\n");
        exit(0);
    }
    #ifdef BUFF 
    store_list = (int *)malloc(storesize * sizeof(int));
    if(store_list == NULL){
        printf("store_list malloc error\n");
        exit(0);
    }
    #endif
    for(i=0;i<storesize;i++){
        mpfi_init2(store_mpfi[i].mpfi,acc);
        store_mpfi[i].id = NO_TASK;
        #ifdef BUFF 
        store_list[i]=i;
        #endif
    }
    #ifdef BUFF 
    store_list_count = storesize;
    #endif
}

void cancel_storeData(){
    int i;
    for(i=0;i<storesize;i++){
        mpfi_clear(store_mpfi[i].mpfi);
    }
    free(store_mpfi);
    #ifdef BUFF
    free(store_list);
    #endif
    #ifdef MAX_STORE_SIZE
    printf("%d,",max);
    #endif
}
mpfi_t *getStore(int index){
    return &store_mpfi[index].mpfi;
}

int getStore_MPFI(int threadID,int nodeID){
    int i,n=-1;
    while(!__sync_bool_compare_and_swap(&store_mpfi_flag, -1, threadID)){
        _mm_pause();
    }  //第一引数のポインタ先が第二と等価なら第三引数を代入
    #ifdef BUFF
    if(store_list_count>=1){
        i = store_list[out];
        store_mpfi[i].id = nodeID;
        store_mpfi_count++;
        data_mpfi[nodeID].index=i;
        data_mpfi[nodeID].a=&store_mpfi[i].mpfi;
        n=i;
        store_list_count--;
        if(store_mpfi_count>max){
            max=store_mpfi_count;
        }
        if(out==storesize-1){
            out=0;
        }else{
            out++;
        }
    }else{
        int old_size = storesize;
        storesize = storesize + ADD_STORE_SIZE;
        __store_mpfi *tmp1 = NULL;
        tmp1 = (__store_mpfi *)realloc(store_mpfi, storesize * sizeof(__store_mpfi ));
        if(tmp1 == NULL){
            printf("store_mpfi realloc error\n");
            exit(0);
        }else{
            store_mpfi = tmp1;
            int *tmp2 = NULL;
            tmp2 = (int *)realloc(store_list, storesize * sizeof(int ));
            if(tmp2 == NULL){
                printf("store_list realloc error\n");
                exit(0);
            }else{
                store_list = tmp2;
                int i,k=0;
                for(i=old_size;i<storesize;i++){
                    mpfi_init2(store_mpfi[i].mpfi,thread_acc);
                    store_mpfi[i].id = NOT_USE;
                    store_list[k] = i;
                    k++;
                }
                in = ADD_STORE_SIZE;
                i= store_list[0];
                store_mpfi[i].id = nodeID;
                store_mpfi_count++;
                data_mpfi[nodeID].index=i;
                data_mpfi[nodeID].a=&store_mpfi[i].mpfi;
                out=1;
                store_list_count=ADD_STORE_SIZE-1;
                if(store_mpfi_count>max){
                    max=store_mpfi_count;
                }
                n=i;
            } 
        }
    }
    #else
    if(store_mpfi_count<storesize){
        
        for(i=0;i<storesize;i++){
            if(store_mpfi[i].id == NOT_USE){
                // data_mpfi[nodeID].a = &(store_mpfi[i].mpfi);
                store_mpfi[i].id = nodeID;
                __sync_lock_test_and_set(&store_mpfi_count, store_mpfi_count+1);
                data_mpfi[nodeID].index=i;
                data_mpfi[nodeID].a=&store_mpfi[i].mpfi;
                n=i;
                if(i>max){
                    max=i;
                }
                break;
            }
        }
    }else{
        __store_mpfi *tmp = NULL;
        storesize = storesize + ADD_STORE_SIZE;
        tmp = (__store_mpfi *)realloc(store_mpfi, storesize * sizeof(__store_mpfi ));
        if(tmp == NULL){
            printf("store_mpfi realloc error\n");
        }else{
            store_mpfi = tmp;
            int i;
            for(i=storesize - ADD_STORE_SIZE;i<storesize;i++){
                mpfi_init2(store_mpfi[i].mpfi,thread_acc);
                store_mpfi[i].id = NOT_USE;
            }
            i=storesize - ADD_STORE_SIZE;
            store_mpfi[i].id = nodeID;
            __sync_lock_test_and_set(&store_mpfi_count, store_mpfi_count+1);
            data_mpfi[nodeID].index=i;
            data_mpfi[nodeID].a=&store_mpfi[i].mpfi;
            n=i;
            if(i>max){
                    max=i;
            }
        }
    }
    #endif
    // printf("out %d\n\n",threadID);
    while(!__sync_bool_compare_and_swap(&store_mpfi_flag, threadID, -1)){
        _mm_pause();
    }  //第一引数のポインタ先が第二と等価なら第三引数を代入
    return n;
}

int decreaseCount(int threadID,int nodeID){
    int dataID=data_mpfi[nodeID].index;
    if(dataID!=-1&&dataID!=-2){
        while(!__sync_bool_compare_and_swap(&(head[nodeID]->flag), -1, threadID)){
            _mm_pause();
        }  //第一引数のポインタ先が第二と等価なら第三引数を代入
        g[nodeID]->head--;
        if(g[nodeID]->head==0){
            clearStore_MPFI(threadID, dataID);
        } 
        while(!__sync_bool_compare_and_swap(&(head[nodeID]->flag), threadID, -1)){
            _mm_pause();
        }  //第一引数のポインタ先が第二と等価なら第三引数を代入
    }
    return 0;
}

int clearStore_MPFI(int threadID,int dataID){
    // printf("free %d\n\n",dataID);
    #ifdef BUFF
    while(!__sync_bool_compare_and_swap(&store_mpfi_flag, -1, threadID)){
        // _mm_pause();
    } 
    store_mpfi[dataID].id = NOT_USE;
    store_list[in]=dataID;
    store_mpfi_count--;
    store_list_count++;
    if(in==storesize-1){
        in=0;
    }else{
        in++;
    }
    while(!__sync_bool_compare_and_swap(&store_mpfi_flag, threadID, -1)){
        // _mm_pause();
    }  //第一引数のポインタ先が第二と等価なら第三引数を代入
    #else
    __sync_lock_test_and_set(&store_mpfi[dataID].id, NOT_USE);
    __sync_lock_test_and_set(&store_mpfi_count, store_mpfi_count-1);
    #endif
    return 0;
}

#endif