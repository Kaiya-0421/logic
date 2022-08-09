#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpfi_io.h>
#include "mpfr.h"
#include "mpfi.h"
#include <time.h>
#define MAX_NUM 128

int main()
{
	struct timespec start, end;
	int i;
	unsigned long prec;
	mpfi_t x[MAX_NUM];
	mpfr_t relerr;
    prec = 1024; //変更する。仮数部長
	//printf("prec(bits) = "); while(scanf("%ld", &prec) < 1);
	mpfr_set_default_prec(prec);

	// initialize variables
	mpfr_init(relerr);
	for(i = 0; i < MAX_NUM; i++)
		mpfi_init(x[i]);

	// set a initial interval
	mpfi_set_str(x[0], "0.7501", 10);
	//実行スタート
    clock_gettime(CLOCK_REALTIME, &start);
	for(i = 0; i <= 100; i++)
	{
		/*if((i % 10) == 0)
		{
			printf("%5d, ", i);
			mpfi_out_str(stdout , 10, 0, x[i]);
            //mpfi_out_str(stdout, 10, 0, x);
			//mpfi_out_str(stdout, 10, 17, x[i]);
			mpfi_diam(relerr, x[i]);
			mpfr_printf("%10.3RNe\n", relerr);
		}*/

		//x[i + 1] = 4 * x[i] * (1 - x[i]);
		mpfi_ui_sub(x[i + 1], 1UL, x[i]);
		mpfi_mul(x[i + 1], x[i + 1], x[i]);
		mpfi_mul_ui(x[i + 1], x[i + 1], 4UL);

	}
    clock_gettime(CLOCK_REALTIME, &end);
	//実行終了
	// delete variables
	//時間結果
	double elasped_time = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) * 1e-9;
    printf("時間は%lf\n", elasped_time);
	mpfr_clear(relerr);
	for(i = 0; i < MAX_NUM; i++)
		mpfi_clear(x[i]);

	return 0;
}