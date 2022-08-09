//gcc -o test1 test.c cmpfi.c nodelist.c gmpfi.c thread.c -lmpfi
#include "gmpfi.h"

void printTime(struct timespec start,struct timespec end,char str[]);

int main(int argc, char *argv[]){
    int acc = 2000;
    int i,j,k;
    char buf[256];
    if (argc == 2) {
        sscanf(argv[1], "%d", &size);
    }else if(argc == 3){
        sscanf(argv[1], "%d", &size);
        sscanf(argv[2], "%d", &coresize);
    }else if(argc == 4){
        sscanf(argv[1], "%d", &size);
        sscanf(argv[2], "%d", &coresize);
        sscanf(argv[3], "%d", &acc);
    }
    struct timespec start,end;

    //使用する変数の定義
    gmpfi_t A[size][size];
    gmpfi_t b[size];
    gmpfi_t x[size];
    gmpfi_t tmp[size];
    
    clock_gettime(CLOCK_REALTIME, &start); 
    //計算式の記入
    for(i = 0; i < size; i++){
        gmpfi_init2 (x[i], acc);
        gmpfi_set_str (x[i] ,"0" ,10);
    }

    for(i = 0; i < size; i++){
        gmpfi_init2 (b[i], acc);
        sprintf(buf, "%d", i);
        gmpfi_set_str (b[i], buf ,10);
        for(j = 0; j < size; j++){
            gmpfi_init2 (A[i][j], acc);
            sprintf(buf, "%d", i+j);
            gmpfi_set_str (A[i][j], buf ,10);
        }
    }

    for(i = 0; i < size; i++){
        gmpfi_init2 (tmp[i], acc);
        for(j = 0; j < size; j++){
            gmpfi_mul(tmp[i],A[i][j],b[i]);
            gmpfi_add(x[i],x[i],tmp[i]);
        }
    }
    clock_gettime(CLOCK_REALTIME, &start); 
    cmpfi_cal();
    clock_gettime(CLOCK_REALTIME, &end);
    #ifndef SYNC_TIME
    #ifndef WORK_TIME
    printTime(start,end,"");   
    #endif
    #endif  
    // mpfi_out_str (stdout , 10, 0, b[0]->mpfi);
    // printf("\n");

    // for(i=0;i<count;i++){
    //     printf("%d ",log_node[i]);
    // }
    // printf("\n");

    //変数のメモリ解放
   
    for(i = 0; i < size ; i++){
        gmpfi_clear(b[i]);
        gmpfi_clear(x[i]);
        gmpfi_clear(tmp[i]);
        for(j = 0; j < size; j++){
            gmpfi_clear(A[i][j]);
        }
    }
    return 0;
}

void printTime(struct timespec start,struct timespec end,char str[]){
    if((end.tv_nsec - start.tv_nsec) < 0){ 
        end.tv_nsec += 1000000000; 
        end.tv_sec  -= 1; 
    }
    #ifndef OUTPUT_TIME
    printf("%s %10ld.%09ld\n",str ,end.tv_sec-start.tv_sec, end.tv_nsec-start.tv_nsec);
    #else
    printf("%10ld.%09ld,",end.tv_sec-start.tv_sec, end.tv_nsec-start.tv_nsec);
    #endif
}