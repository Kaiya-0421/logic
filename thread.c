
#include "gmpfi.h"

struct timespec sync_time;
int workersize = CORE_SIZE;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int thread_acc=2000;
int printflag = -1;

#ifdef COND_WAIT
pthread_mutex_t *mutex_th;
pthread_cond_t *cond_th;
#endif

void sync_data(int data_num,int state){
    #ifdef USE_ATOMIC
    __sync_lock_test_and_set(&data_core[data_num].state, state);
    #else
    // printf("1\n");
    _mm_mfence();
    data_core[data_num].state = state;
    #endif
}

void createThread(int core){
    int i;
    #ifdef VER11
    mpfi_init2(main_data,thread_acc);
    #endif
    #ifdef COND_WAIT
    cond_th = (pthread_cond_t *)malloc(sizeof(pthread_cond_t) * core);
    mutex_th = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * core);
    #endif
    workersize = core;
    for(i=0;i<workersize;i++){
        data_core[i].coreid = i;
        #ifdef VER11
        mpfi_init2(data_core[i].mpfi_data,thread_acc);
        #endif
        #ifdef COND_WAIT
        pthread_mutex_init(&mutex_th[i], NULL);
        pthread_cond_init(&cond_th[i], NULL);
        #endif
        sync_data(i,EMPTY);
        if(pthread_create(&thread_id[i],NULL,(void *)thread_func,(void *)&data_core[i])!=0){
            perror("pthread_create() error");
            exit(1);
        }
    }    
}

void cancelThread(){
    int i;
    for(i=0;i<workersize;i++){
        sync_data(i,CANCEL);
        #ifdef VER11
        mpfi_clear(data_core[i].mpfi_data);
        #endif
        #ifdef COND_WAIT
        pthread_mutex_destroy(&mutex_th[i]);
        pthread_cond_destroy(&cond_th[i]);
        #endif
    }
    #ifdef VER11
    mpfi_clear(main_data);
    #endif
    #ifdef VER12
    cancel_storeData();
    #endif
}

void idolThread(){
    int i;
    #ifdef COND_WAIT
    for(i=0;i<workersize;i++){
        pthread_mutex_lock(&mutex_th[i]);
        sync_data(i,IDLING);
        pthread_mutex_unlock(&mutex_th[i]);
    }
    #else
    pthread_mutex_lock(&mutex);
    for(i=0;i<workersize;i++){
        sync_data(i,IDLING);
        _mm_mfence();
    }
    pthread_mutex_unlock(&mutex);
    #endif
    #ifdef PRINT_IDLING
    printf("subthread idoling\n");
    #endif
}

void emptyThread(){
    int i;
    #ifdef COND_WAIT
    for(i=0;i<workersize;i++){
        pthread_mutex_lock(&mutex_th[i]);
        sync_data(i,EMPTY);
        pthread_mutex_unlock(&mutex_th[i]);
        int ret = pthread_cond_signal(&cond_th[i]);
    }
    #else
    pthread_mutex_lock(&mutex);
    for(i=0;i<workersize;i++){
        sync_data(i,EMPTY);
        _mm_mfence();
    }
    pthread_mutex_unlock(&mutex);
    #endif
}

void addTime(struct timespec start,struct timespec end){
    if((end.tv_nsec - start.tv_nsec) < 0){ 
        end.tv_nsec += 1000000000; 
        end.tv_sec  -= 1; 
    }
    sync_time.tv_sec = sync_time.tv_sec + (end.tv_sec-start.tv_sec);
    sync_time.tv_nsec = sync_time.tv_nsec + (end.tv_nsec-start.tv_nsec);
    if(sync_time.tv_nsec>=1000000000){
        sync_time.tv_sec += 1;
        sync_time.tv_nsec -= 1000000000;
    }
}

void mpfi_cal(int threadID, int id){
    int b = data_mpfi[id].b;
    int c = data_mpfi[id].c;
    #ifdef VER12
    if(data_mpfi[id].index==GET_STORE){
        getStore_MPFI(threadID, id);
    }
    if(data_mpfi[b].index>=0){
        data_mpfi[b].a = getStore(data_mpfi[b].index);
    }
    if(data_mpfi[c].index>=0){
        data_mpfi[c].a = getStore(data_mpfi[c].index);
    }
    #endif
    
    if(op[id] == ADD){
        mpfi_add(*data_mpfi[id].a,*data_mpfi[b].a,*data_mpfi[c].a);
    }else if(op[id] == SUB){
        mpfi_sub(*data_mpfi[id].a,*data_mpfi[b].a,*data_mpfi[c].a);
    }else if(op[id] == MUL){
        mpfi_mul(*data_mpfi[id].a,*data_mpfi[b].a,*data_mpfi[c].a);
    }else if(op[id] == DIV){
        mpfi_div(*data_mpfi[id].a,*data_mpfi[b].a,*data_mpfi[c].a);
    }
    #ifdef VER11
    else if(op[id] == MULSUB){
        int d = data_mpfi[id].d;
        #ifdef VER12
        if(data_mpfi[d].index>=0){
            data_mpfi[d].a = getStore(data_mpfi[d].index);
        }
        #endif
        if(threadID != workersize){
            mpfi_mul(data_core[threadID].mpfi_data,*data_mpfi[c].a,*data_mpfi[d].a);
            mpfi_sub(*data_mpfi[id].a,*data_mpfi[b].a,data_core[threadID].mpfi_data);
        }else{
            mpfi_mul(main_data,*data_mpfi[c].a,*data_mpfi[d].a);
            mpfi_sub(*data_mpfi[id].a,*data_mpfi[b].a,main_data);
        } 
        #ifdef VER12
        decreaseCount(threadID,d);
        #endif
    }
    #endif

    #ifdef VER12
    decreaseCount(threadID,b);
    decreaseCount(threadID,c);
    #endif
}


void *thread_func(void *arg){
    __data_core *data = (__data_core *)malloc(sizeof(__data_core *));      //引数を保存するためのポインタ
    data = arg;
    int threadID=data->coreid;
    int id=NO_TASK;
    while(1){
        if(id==NO_TASK){
            id = atomicGetList(threadID);
            if(id!=NO_TASK){
                mpfi_cal(threadID,id);
                id = atomicAddList(threadID, id);
            }else {
                #ifdef COND_WAIT
                pthread_mutex_lock(&mutex_th[threadID]);
                #endif
                if(data_core[threadID].state == IDLING){
                    #ifdef COND_WAIT
                    // pthread_mutex_lock(&mutex_th[threadID]);
                    while(data_core[threadID].state == IDLING){
                        pthread_cond_wait(&cond_th[threadID], &mutex_th[threadID]);
                    }
                    // pthread_mutex_unlock(&mutex_th[threadID]);
                    #else
                    while(data_core[threadID].state == IDLING){
                        _mm_pause();
                    }
                    _mm_mfence();
                    #endif
                    id = NO_TASK;
                }else if(data_core[threadID].state == CANCEL){
                    #ifdef COND_WAIT
                    pthread_mutex_unlock(&mutex_th[threadID]);
                    #endif
                    break; //cancelフラグでスレッドを破棄
                }
                #ifdef COND_WAIT
                pthread_mutex_unlock(&mutex_th[threadID]);
                #endif
                _mm_pause();
            }
            _mm_mfence();
        }else{
            mpfi_cal(threadID,id);
            id = atomicAddList(threadID, id);
        }
    }
    pthread_exit(NULL);
}

