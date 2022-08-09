#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <emmintrin.h>
#include <unistd.h>

#include <assert.h>
#include <sys/stat.h>

#include <pthread.h>

#include <mpfi.h>
#include <mpfi_io.h>

#include "define.h"

typedef struct
{
 mpfi_t mpfi;
 volatile int id;
 int store;
}__gmpfi_t;

typedef __gmpfi_t gmpfi_t[1];
typedef __gmpfi_t *gmpfi_ptr;
typedef const __gmpfi_t *gmpfi_srcptr;

// struct gmpfi_t tmp_mpfi;

typedef struct{   
    volatile int head;
    volatile int *data;
}__edge_t;
typedef __edge_t edge_t[1];

#ifdef VER12
//データ構造
typedef struct{
    mpfi_t mpfi;
    int id;
}__store_mpfi;
__store_mpfi *store_mpfi;
volatile int store_mpfi_flag;
volatile int store_mpfi_count;

__gmpfi_t tmp_mpfi[1];

void initTmp();

void init_storeData(int acc);
void cancel_storeData();
int decreaseCount(int threadID,int nodeID);
int getStore_MPFI(int threadID,int nodeID);
mpfi_t *getStore(int index);
#endif

#ifdef VER11
typedef struct{
    mpfi_t *a;
    int b;
    int c;
    int d;
    int index;
}__data_mpfi;
__data_mpfi *data_mpfi;
#else
typedef struct{
    mpfi_t *a;
    int b;
    int c;
}__data_mpfi;
__data_mpfi *data_mpfi;
#endif


typedef struct{   
    volatile int head;
    volatile int flag;
}__head_t;
typedef __head_t head_t[1];
extern head_t *head; 
extern int *op;
extern edge_t *g;

extern int *log_node;

#ifdef VER11
extern   mpfi_t main_data;
extern int thread_acc;
typedef struct{
    volatile int coreid;
    volatile int state;
    mpfi_t mpfi_data;
}__data_core;
__data_core *data_core;
#else
typedef struct{
    volatile int coreid;
    volatile int id;
    volatile int state;
}__data_core;
__data_core *data_core;
#endif

pthread_t thread_id[CORE_SIZE];

typedef struct wait_t{
 volatile int id;
 struct wait_t *last;
}wait_t;

extern struct wait_t *wait_h;
extern struct wait_t *wait_l;

extern int count;
extern int isWrite;
extern int coresize;
extern int size;

int gmpfi_write_edge(int a, int b, int c);

void gmpfi_init2(gmpfi_ptr a,int prec);
void gmpfi_set_str (gmpfi_ptr a , const char *str,int n);
void gmpfi_clear(gmpfi_ptr a);

int gmpfi_add(gmpfi_ptr a, gmpfi_ptr b, gmpfi_ptr c);
int gmpfi_sub(gmpfi_ptr a, gmpfi_ptr b, gmpfi_ptr c);
int gmpfi_mul(gmpfi_ptr a, gmpfi_ptr b, gmpfi_ptr c);
int gmpfi_div(gmpfi_ptr a, gmpfi_ptr b, gmpfi_ptr c);
void mpfi_cal(int threadID, int id);


void countNode();
void initEdge(int id);
void initCore();
void pushEdge(int id ,int to);

int cmpfi_create();
int cmpfi_cancel();
void cmpfi_cal();
void tmp_cal();

void searchZero();          //入力次数が0のノードを探索，待機配列にスタック
int deleteEdge(int threadID, int id);    //計算終了したノードから伸びるエッジから次のノードの入力次数を-1する
void addlistSequentially(int id);
int checkCORE();


//待ち行列操作用関数
void freeHeadList();
void addlist(int threadID,int id);
void printList();
int printListSize();
void printLast();
void removeList();
struct wait_t *getListHead();
int atomicGetList(int threadID);
int atomicAddList(int threadID, int nodeID);

//ワーカスレッド操作用関数
void sync_data(int data_num,int state);
void createThread(int core);
void cancelThread();
void emptyThread();
void idolThread();
void *thread_func(void *arg);
void *createWorker(void *arg);

void printTime(struct timespec start,struct timespec end,char str[]);

//データのメモリ管理用関数
int cmpfi_free();
int cmpfi_malloc();

int getNodeListForAdd();
int addNodeList(int threadID,int id);
int addNodeListSequentially(int id);
int deleteEdgeForWorker(int threadID,int id);
extern volatile int data_count;
void initList();
void freeList();

#ifdef VER11
int gmpfi_write_edge3(int a, int b, int c, int d);
int gmpfi_mulsub(gmpfi_ptr a, gmpfi_ptr b, gmpfi_ptr c, gmpfi_ptr d);
#endif