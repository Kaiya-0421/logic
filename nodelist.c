#include "gmpfi.h"

volatile int atomicFlag=-1;
volatile int atomicGetFlag=-1;
volatile int atomicAddFlag=-1;

int *node_list=NULL;
int entrypoint;
int endpoint;
volatile int data_count;
int list_size = LIST_SIZE;

void initList(){
    list_size = size*size;
    node_list = (int *)malloc(list_size * sizeof(int ));
    if(node_list== NULL){
        printf("node_list malloc error\n");
    }
    endpoint=0;
    entrypoint=0;
    data_count=0;
}

void freeList(){
    free(node_list);
}

int addNodeList(int threadID,int id){
    while(!__sync_bool_compare_and_swap(&atomicFlag, -1, threadID)){
        _mm_pause();
    }  //第一引数のポインタ先が第二と等価なら第三引数を代入
    if(data_count<list_size){
        node_list[entrypoint]=id;
        // printf("%d: %d\n",data_count+1,node_list[entrypoint]);
        data_count++;
        if(entrypoint<list_size-1){
            entrypoint++;
        }else{
            entrypoint=0;
        }
    }else{
        //データがあふれたときの処理
        // printf("node_list is full\n");
        int *tmp = NULL;
        list_size = list_size + ADD_LIST_MEM;
        #ifdef MEM_REALLOC
        tmp = (int *)realloc(node_list, list_size * sizeof(int ));
        if(tmp == NULL){
            printf("node_list realloc error\n");
        }else{
            node_list = tmp;
            if(endpoint!=0){
                if(entrypoint<ADD_LIST_MEM){
                    memcpy(&node_list[list_size-ADD_LIST_MEM],node_list,sizeof(int) * entrypoint);   //後ろにデータを移動
                    entrypoint = list_size - ADD_LIST_MEM + entrypoint;       //entrypointを更新
                }else{
                    memcpy(&node_list[list_size-ADD_LIST_MEM],node_list,sizeof(int) * ADD_LIST_MEM);     //後ろにデータを移動
                    memcpy(node_list,&node_list[ADD_LIST_MEM],sizeof(int) * (entrypoint-ADD_LIST_MEM));  //先頭にデータを詰める
                    entrypoint = entrypoint - ADD_LIST_MEM;       //entrypointを更新
                }
            }
        #else
        tmp = (int *)malloc(list_size * sizeof(int ));
        if(tmp == NULL){
            printf("node_list malloc error\n");
        }else{
            if(endpoint!=0){
                memcpy(tmp, &node_list[endpoint],sizeof(int)*(list_size-ADD_LIST_MEM-endpoint));
                memcpy(tmp, node_list,sizeof(int)*(entrypoint));
            }else{
                memcpy(tmp, node_list,sizeof(int)*(list_size-ADD_LIST_MEM));
            }
            endpoint = 0;
            entrypoint = list_size - ADD_LIST_MEM;
            node_list = tmp;
        #endif
            node_list[entrypoint]=id;
            data_count++;
            if(entrypoint<list_size-1){
                entrypoint++;
            }else{
                entrypoint=0;
            }
        }
    }
    while(!__sync_bool_compare_and_swap(&atomicFlag, threadID,-1));  //第一引数のポインタ先が第二と等価なら第三引数を代入
    
    return 0;
}

int addNodeListSequentially(int id){
    if(data_count<list_size){
        node_list[entrypoint]=id;
        // printf("%d: %d\n",data_count+1,node_list[entrypoint]);
        data_count++;
        if(entrypoint<list_size-1){
            entrypoint++;
        }else{
            entrypoint=0;
        }
    }else{
        //データがあふれたときの処理
        // printf("node_list is full\n");
        int *tmp = NULL;
        list_size = list_size + ADD_LIST_MEM;
        #ifdef MEM_REALLOC
        tmp = (int *)realloc(node_list, list_size * sizeof(int ));
        if(tmp == NULL){
            printf("node_list realloc error\n");
        }else{
            node_list = tmp;
            if(endpoint!=0){
                if(entrypoint<ADD_LIST_MEM){
                    memcpy(&node_list[list_size-ADD_LIST_MEM],node_list,sizeof(int) * entrypoint);   //後ろにデータを移動
                    entrypoint = list_size - ADD_LIST_MEM + entrypoint;       //entrypointを更新
                }else{
                    memcpy(&node_list[list_size-ADD_LIST_MEM],node_list,sizeof(int) * ADD_LIST_MEM);     //後ろにデータを移動
                    memcpy(node_list,&node_list[ADD_LIST_MEM],sizeof(int) * (entrypoint-ADD_LIST_MEM));  //先頭にデータを詰める
                    entrypoint = entrypoint - ADD_LIST_MEM;       //entrypointを更新
                }
            }
        #else
        tmp = (int *)malloc(list_size * sizeof(int ));
        if(tmp == NULL){
            printf("node_list malloc error\n");
        }else{
            if(endpoint!=0){
                memcpy(tmp, &node_list[endpoint],sizeof(int)*(list_size-ADD_LIST_MEM-endpoint));
                memcpy(tmp, node_list,sizeof(int)*(entrypoint));
            }else{
                memcpy(tmp, node_list,sizeof(int)*(list_size-ADD_LIST_MEM));
            }
            endpoint = 0;
            entrypoint = list_size - ADD_LIST_MEM;
            node_list = tmp;
        #endif
            node_list[entrypoint]=id;
            // printf("%d: %d\n",data_count+1,node_list[entrypoint]);
            __sync_lock_test_and_set(&data_count, data_count+1);
            if(entrypoint<list_size-1){
                entrypoint++;
            }else{
                entrypoint=0;
            }
        }
    }
    return 0;
}

int getNodeList(int threadID){
    int id=-1;
    while(!__sync_bool_compare_and_swap(&atomicFlag, -1, threadID)){
        _mm_pause();
    }
    if(data_count > 0){     //要素がある場合
        id = node_list[endpoint];   
        node_list[endpoint] = -1;
        data_count--;
        if(data_count==0){
            endpoint=0;
            entrypoint=0;
        }else{
            if(endpoint<(list_size-1)){     
                endpoint++;
            }else{
                endpoint=0;
            }
        }
        // printf("%d: %d\n",data_count+1,id);
        if(threadID!=coresize&&id>=0)
            sync_data(threadID,USE);
    }
    while(!__sync_bool_compare_and_swap(&atomicFlag, threadID, -1));
    return id;
}

int getNodeListForAdd(){
    int id=-1;
    if(data_count > 0){
        id = node_list[endpoint];
        node_list[endpoint] = -1;
        __sync_lock_test_and_set(&data_count, data_count-1);
        if(endpoint<list_size-1){
            endpoint++;
        }else{
            endpoint=0;
        }
        // printf("%d: %d\n",data_count+1,id);
    }
    return id;
}

void freeHeadList(){
    struct wait_t *next;
    next = wait_h->last;        
    free (wait_h);
    wait_h = next;
    if(wait_h==NULL){
        wait_l = NULL;
    }
}

int atomicGetList(int threadID){
    #ifndef CHANGE_LIST
    int id=-1;
    while(!__sync_bool_compare_and_swap(&atomicFlag, -1, threadID)){
        _mm_pause();
    }  //第一引数のポインタ先が第二と等価なら第三引数を代入 
    if(wait_h!=NULL){
        id=wait_h->id;
        if(threadID!=coresize)
            sync_data(threadID,USE);
        freeHeadList();
    }
    while(!__sync_bool_compare_and_swap(&atomicFlag, threadID, -1));
    // atomicFlag=-1;
    return  id;
    #else
    return getNodeList(threadID);
    #endif
}

int atomicAddList(int threadID, int nodeID){
    int id = deleteEdge(threadID, nodeID);
    __sync_lock_test_and_set(&op[nodeID], NUM);
    // op[nodeID]=NUM; 
    if(threadID<coresize){
        if(id<0){
            sync_data(threadID,EMPTY);
        }else{
            sync_data(threadID,USE);
        } 
    }
    return id;
}

void addlist(int threadID,int id){
    struct wait_t *next_node;
    if ((next_node = (struct wait_t *) malloc(sizeof(wait_t))) == NULL) {
		printf("malloc error\n");
		exit(EXIT_FAILURE);
	}
    next_node->id = id;
    next_node->last = NULL;
    while(!__sync_bool_compare_and_swap(&atomicFlag, -1, threadID)){
        _mm_pause();
    }  //第一引数のポインタ先が第二と等価なら第三引数を代入
    if(wait_h == NULL){
        wait_h = next_node;
        wait_l = wait_h;
    }else{
        wait_l->last = next_node;
        wait_l = next_node;
    }
    while(!__sync_bool_compare_and_swap(&atomicFlag, threadID, -1));  //第一引数のポインタ先が第二と等価なら第三引数を代入
}

void addlistSequentially(int id){
    struct wait_t *next_node;
    if ((next_node = (struct wait_t *) malloc(sizeof(wait_t))) == NULL) {
		printf("malloc error\n");
		exit(EXIT_FAILURE);
	}
    next_node->id = id;
    next_node->last = NULL;
    if(wait_h == NULL){
        wait_h = next_node;
        wait_l = wait_h;
    }else{
        wait_l->last = next_node;
        wait_l = next_node;
    }
}

///////////////////////////////////////////////////////
struct wait_t *getListHead(){
    struct wait_t *next_node;
    next_node = wait_h;
    wait_h = wait_h->last;
    return next_node;
}

//リストの要素を出力
void printList(){
    struct wait_t *next_node;
    next_node = wait_h;
    while (next_node!=NULL)
    {
        printf("%d\n",next_node->id);
        next_node = next_node->last;
    }
    printf("\n");
}

//リストの要素を出力
int printListSize(){
    struct wait_t *next_node;
    int i=0;
    next_node = wait_h;
    while (next_node!=NULL)
    {
        next_node = next_node->last;
        i++;
    }
    return i;
}
//リストの最後のノードを出力
void printLast(){
    if(wait_l!=NULL)
        printf("%d\n",wait_l->id);
}

//リスト全てを削除
void removeList(){
    struct wait_t *next_node;
    next_node = wait_h;
    while (wait_h!=NULL)
    {
        next_node = wait_h->last;        
        free (wait_h);
        wait_h = next_node;
    }
    if(wait_h==NULL){
        wait_l = NULL;
    }
}
