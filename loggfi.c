#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpfi_io.h>
#include "mpfr.h"
#include "mpfi.h"
#include "gmpfi.h"
#define MAX_NUM 128

int main()
{
	
	struct timespec start, end;
	int i;
	unsigned long prec;
	gmpfi_t x[MAX_NUM],one,four;
	mpfr_t relerr;

	//printf("prec(bits) = "); while(scanf("%ld", &prec) < 1);
	mpfr_set_default_prec(prec);

    int acc=1024;

	// initialize variables
	mpfr_init(relerr);
	gmpfi_init2(one,acc);
	gmpfi_init2(four,acc);
	for(i = 0; i < MAX_NUM; i++)
		gmpfi_init2(x[i],acc);

	// set a initial interval
	gmpfi_set_str(x[0], "0.7501", 10);
	gmpfi_set_str(one, "1.0", 10);
	gmpfi_set_str(four, "4.0", 10);

	///実行スタート
    clock_gettime(CLOCK_REALTIME, &start);
	for(i = 0; i <= 100; i++)
	{
		/*
		if((i % 10) == 0)
		{
			printf("%5d, ", i);
			mpfi_out_str(stdout , 10, 0, x[i]->mpfi);
            //mpfi_out_str(stdout, 10, 0, x);
			//mpfi_out_str(stdout, 10, 17, x[i]);
			mpfi_diam(relerr, x[i]->mpfi);
			mpfr_printf("%10.3RNe\n", relerr);
		}*/
        //gmpfi_cal(1);
		//x[i] = 4 * x[i-1] * (1 - x[i-1]);
		gmpfi_sub(x[i+1], one, x[i]);
		gmpfi_mul(x[i+1], x[i+1], x[i]);
		gmpfi_mul(x[i+1], x[i+1], four);

	}
	//実行終了
    clock_gettime(CLOCK_REALTIME, &end);
    cmpfi_cal();
	// delete variables
	mpfr_clear(relerr);
	gmpfi_clear(one);
	gmpfi_clear(four);
	//時間結果
	double elasped_time = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) * 1e-9;

 printf("%lf,", elasped_time);

	for(i = 0; i < MAX_NUM; i++){
		gmpfi_clear(x[i]);
	}
	return 0;
}