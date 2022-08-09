// CORE数と演算子毎の実行ステップ数の設定
#define CORE_SIZE 32

// #define OUTPUT_DATA
#define OUTPUT_TIME
// #define MAX_STORE_SIZE

#define USE_ATOMIC   //状態変更にアトミック関数を使うか否か

#define CHANGE_LIST     //リングバッファを使うか
#ifdef CHANGE_LIST
#define MEM_REALLOC     //リングバッファでreallocを使うか
#endif

// #define DIV_RUN      //分割実行するか
#ifdef DIV_RUN   
// #define PRINT_IDLING
#endif

#define VER10_TIME
#define VER11       //融合演算


#define VER12
#define BUFF        //tmpを使いリングバッファによる管理
#define STORE_USE   //tmpを使うか


#define TWO
#define MPFI_INIT
#ifdef VER11   
#define GET_ID       //待ち行列に追加するIDの一つを戻り値とする
#endif

#define COND_WAIT

// 以降は変更しない //////////////////////////////////////////////////////////////

#define MAX_STACK 10000000
#define LIST_SIZE 1000          //バッファの初期確保数
#define ADD_LIST_MEM 50         //待ち行列のメモリを拡張して確保する

#define STORE_SIZE 100       
#define ADD_STORE_SIZE 100

#define ADD 0
#define SUB 1
#define MUL 2
#define DIV 3
#define NUM 4
#define MULSUB 5

#define CANCEL 2
#define USE 1
#define EMPTY 0 
#define CHANGE 3 
#define IDLING 4

#define NO_TASK -1
#define GET_STORE -1
#define NOT_USE -1

#ifdef __linux
#define CLOCK_REALTIME 0
#endif