//gcc -o test1 test.c cmpfi.c nodelist.c mpfi.c thread.c -lmpfi
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <emmintrin.h>

#include <mpfi.h>
#include <mpfi_io.h>
#include "define.h"

void printTime(struct timespec start,struct timespec end,char str[]);

int main(int argc, char *argv[]){
    int acc = 2000;
    int size = 3;
    int i,j,k;
    char buf[256];
    if (argc == 2) {
        sscanf(argv[1], "%d", &size);
    }else if(argc == 3){
        sscanf(argv[1], "%d", &size);
        sscanf(argv[2], "%d", &acc);
    }
    struct timespec start,end;

    //使用する変数の定義
    mpfi_t A[size][size];
    mpfi_t b[size];
    mpfi_t x[size];
    mpfi_t tmp[size];

    for(i=0; i<size; i++){
        mpfi_init2 (x[i], acc);
        mpfi_init2 (b[i], acc);
        mpfi_init2 (tmp[i], acc);
        for(j=0; j<size; j++){
            mpfi_init2 (A[i][j], acc);
        }
    }

    //計算式の記入

    for(i = 0; i < size; i++){
        mpfi_set_str (x[i] ,"0" ,10);
    }

    for(i = 0; i < size; i++){
        sprintf(buf, "%d", i);
        mpfi_set_str (b[i], buf ,10);
        for(j = 0; j < size; j++){
            sprintf(buf, "%d", i+j);
            mpfi_set_str (A[i][j], buf ,10);
        }
    }
    clock_gettime(CLOCK_REALTIME, &start);   
    for(i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            mpfi_mul(tmp[i],A[i][j],b[i]);
            mpfi_add(x[i],x[i],tmp[i]);
        }
    }

    
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
        mpfi_clear(b[i]);
        mpfi_clear(x[i]);
        mpfi_clear(tmp[i]);
        for(j = 0; j < size; j++){
            mpfi_clear(A[i][j]);
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