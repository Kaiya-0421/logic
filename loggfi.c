#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpfi_io.h>
#include "mpfr.h"
#include "mpfi.h"
#include "gmpfi.h"
#define MAX_NUM 128   //反復回数や配列の値の個数
#define acc 1024  //仮数部長
int main()
{
	
	struct timespec start, end;
	int i;
	unsigned long prec;
	gmpfi_t x[MAX_NUM],one,four;
//	mpfr_t relerr;

	//printf("prec(bits) = "); while(scanf("%ld", &prec) < 1);
	mpfr_set_default_prec(prec);


	// initialize variables
//	mpfr_init(relerr);
	gmpfi_init2(one,acc);//1
	gmpfi_set_str(one, "1.0", 10);
	gmpfi_init2(four,acc);
	gmpfi_set_str(four, "4.0", 10);
    gmpfi_init2(x[0],acc);
    gmpfi_set_str (x[0] ,"0.7501" ,10);//初期値
	for(i = 1; i < MAX_NUM; i++)
		gmpfi_init2(x[i],acc);
	
	
	///実行スタート
    clock_gettime(CLOCK_REALTIME, &start);
	for(i = 0; i <= 100; i++)
	{
		//x[i] = 4 * x[i-1] * (1 - x[i-1])の計算;
		gmpfi_sub(x[i+1], one, x[i]);
		gmpfi_mul(x[i+1], x[i+1], x[i]);
		gmpfi_mul(x[i+1], x[i+1], four);

	}
	//実行終了
    clock_gettime(CLOCK_REALTIME, &end);
    cmpfi_cal();

///　　ロジスティック写像の表示
    //#ifdef TEST_OUT
    putchar ('\n');
    for(i = 0; i < 100; i++){

		if(i%5==0){
		printf("%d",i);
        mpfi_out_str (stdout , 10, 0, x[i]->mpfi);
		mpfi_out_str(stdout, 10, 17, x[i]->mpfi);
		printf("\n");
		}
        //putchar ('\n');
    }
    // mpfi_out_str (stdout , 10, 0, x[0]->mpfi);
    // putchar ('\n');
    // mpfi_out_str (stdout , 10, 0, x[size-1]->mpfi);
    // putchar ('\n');

    //#endif



	// delete variables
	//mpfr_clear(relerr);
	gmpfi_clear(one);
	gmpfi_clear(four);
	//時間結果
	double elasped_time = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) * 1e-9;

    printf("%lf,", elasped_time);

	for(i = 0; i < MAX_NUM; i++)
		gmpfi_clear(x[i]);
	return 0;
}