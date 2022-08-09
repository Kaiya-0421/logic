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
    #ifdef VER11
    thread_acc = acc;
    #endif
    gmpfi_t hilbert[size][size];
    gmpfi_t b[size];
    gmpfi_t set1[size][size];
    gmpfi_t set[size][size];
    
    clock_gettime(CLOCK_REALTIME, &start); 
    gmpfi_init2 (b[0], acc);
    gmpfi_set_str (b[0] ,"1" ,10); //0
    for(i = 1; i < size; i++){
        gmpfi_init2 (b[i], acc);
        gmpfi_set_str (b[i] ,"0" ,10);  //1,2
    }

    for(i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            gmpfi_init2 (set1[i][j], acc);
            gmpfi_set_str (set1[i][j], "1" ,10);    //3,6,9, 12,15,18, 21,24,27
            sprintf(buf, "%d", (i+1)+(j+1)-1);
            gmpfi_init2 (set[i][j], acc);
            gmpfi_set_str (set[i][j] , buf ,10);    //4,7,10, 13,16,19, 22,25,28
            gmpfi_init2 (hilbert[i][j], acc);
            gmpfi_div(hilbert[i][j], set1[i][j], set[i][j]); //5,8,11, 14,17,20, 23,26,29
        }

    }
    //LU decomposition
    #ifndef TWO
    gmpfi_t tmp_lu[size][size][size]; 
    #else
    #ifndef VER11 
    gmpfi_t tmp_lu[size][size]; 
    #endif
    #endif
    for (k = 0; k < size; k++) {
        for (i = k+1; i < size; i++) {
            gmpfi_div(hilbert[i][k],hilbert[i][k], hilbert[k][k]);  //30(1.0),33(2.0),36(2.1),
            // printf("A[%d][%d] = A[%d][%d] / A[%d][%d]\n", i+1, k+1, i+1, k+1 , k+1,k+1);
            // printf("");
            for (j = k+1; j < size; j++) {
                #ifndef TWO
                gmpfi_init2 (tmp_lu[k][i][j], acc);
                gmpfi_mul(tmp_lu[k][i][j]], hilbert[i][k], hilbert[k][j]);       //31,34,37, 
                gmpfi_sub(hilbert[i][j], hilbert[i][j], tmp_lu[k][i][j]);       //32,35,38
                #else
                #ifdef VER11
                #ifndef STORE_USE
                gmpfi_mulsub(hilbert[i][j], hilbert[i][j], hilbert[i][k], hilbert[k][j]);
                #else
                gmpfi_mul(tmp_mpfi, hilbert[i][k], hilbert[k][j]);       //31,34,37, 
                // printf("tmp = A[%d][%d] * A[%d][%d]\n", i+1, k+1 , k+1,j+1);
                gmpfi_sub(hilbert[i][j], hilbert[i][j], tmp_mpfi);       //32,35,38
                // printf("A[%d][%d] = A[%d][%d] - tmp \n", i+1, j+1 , i+1,j+1);
                #endif
                #else
                gmpfi_init2 (tmp_lu[i][j], acc);
                gmpfi_mul(tmp_lu[i][j], hilbert[i][k], hilbert[k][j]);       //31,34,37, 
                gmpfi_sub(hilbert[i][j], hilbert[i][j], tmp_lu[i][j]);       //32,35,38
                #endif
                #endif 
            }
        }
        #ifdef DIV_RUN
        tmp_cal();
        #endif
    }
//forward subs

    #ifndef VER11 
    gmpfi_t tmp_fs[size][size]; 
    #endif
    for (i = 1; i < size; i++) {
        for (j = 0; j <= i - 1; j++) {
            #ifdef VER11
            #ifndef STORE_USE
            gmpfi_mulsub(b[i], b[i], b[j], hilbert[i][j]);
            #else
            gmpfi_mul(tmp_mpfi, b[j], hilbert[i][j]);   //39,41,43
            // printf("tmp = b[%d] * A[%d][%d]\n", j+1, i+1,j+1);
            gmpfi_sub(b[i], b[i], tmp_mpfi);            //40,43,44
            // printf("b[%d] = b[%d] - tmp \n", i+1, i+1);
            #endif
            #else
            gmpfi_init2 (tmp_fs[i][j], acc);
            gmpfi_mul(tmp_fs[i][j], b[j], hilbert[i][j]);   //39,41,43
            gmpfi_sub(b[i], b[i], tmp_fs[i][j]);            //40,43,44
            #endif
        }
    }

//backward subs
    #ifndef VER11 
    gmpfi_t tmp_bs[size][size]; 
    #endif
    for (i = size-1 ; i >= 0; i--) {// i=1,j=1
        for (j = size-1; j > i; j--) {
            #ifdef VER11
            #ifndef STORE_USE
            gmpfi_mulsub(b[i], b[i], b[j], hilbert[i][j]);
            #else
            // printf("i = %d,j = %d", i+1, j+1);
            gmpfi_mul(tmp_mpfi, b[j], hilbert[i][j]);   //39,41,43
            // printf("tmp = b[%d] * A[%d][%d]\n", j+1, i+1,j+1);
            gmpfi_sub(b[i], b[i], tmp_mpfi);            //40,43,44
            // printf("b[%d] = b[%d] - tmp \n", i+1, i+1);
            #endif
            #else
            gmpfi_init2 (tmp_bs[i][j], acc);
            gmpfi_mul(tmp_bs[i][j], b[j], hilbert[i][j]);   //45,47,50,53,55
            gmpfi_sub(b[i], b[i], tmp_bs[i][j]);            //46,48,51,54,56
            #endif
        }
        gmpfi_div(b[i], b[i], hilbert[i][i]);               //49,52,57
    }
    // #ifdef VER12
    // gmpfi_div(tmp_mpfi, b[i], hilbert[i][i]);
    // #endif
    // gmpfi_clear(tmp_mpfi);
    // clock_gettime(CLOCK_REALTIME, &start);

    #ifndef DIV_RUN
    #ifndef VER10_TIME
    clock_gettime(CLOCK_REALTIME, &start); 
    #endif 
    #endif
    cmpfi_cal();
    clock_gettime(CLOCK_REALTIME, &end);
    #ifdef OUTPUT_TIME
    printTime(start,end,"");   
    #endif

    #ifdef  OUTPUT_DATA 
    printf("\n");
    mpfi_out_str (stdout , 10, 0, b[0]->mpfi);
    printf("\n");
    #endif
    // for(i=0;i<count;i++){
    //     printf("%d ",log_node[i]);
    // }
    // printf("\n");

    for(i = 0; i < size ; i++){
        gmpfi_clear(b[i]);
        for(j = 0; j < size; j++){
            gmpfi_clear(set1[i][j]);
            gmpfi_clear(set[i][j]);
            gmpfi_clear(hilbert[i][j]);
        }
    }
    #ifndef VER11
    for (i = size-1 ; i >= 0; i--) {
        for (j = size-1; j > i; j--) {
            gmpfi_clear(tmp_bs[i][j]);
        }
    }
    for (i = 1; i < size; i++) {
        for (j = 0; j <= i - 1; j++) {
            gmpfi_clear(tmp_fs[i][j]);
        }
    }
    #endif
    // for (k = 0; k < size; k++) {
    //     for (i = k+1; i < size; i++) {
    //         for (j = k+1; j < size; j++) {
    //             if(tmp_lu[i][j]!=NULL){
    //                 gmpfi_clear (tmp_lu[i][j]);
    //             }
    //         }
    //     }
    // }
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