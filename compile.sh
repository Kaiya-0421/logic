#!/bin/bash

main="hilf_pmpfi.c"
file="cmpfi.c nodelist.c gmpfi.c store_mpfi.c thread.c" 

filename="jikkou"
comp_option="-lmpfi -pthread -lgmp -lmpfr"

# gcc -O3 -o $filename $main $file $comp_option
#gcc -O3 -o logic_pmpfi logic_pmpfi.c $file $comp_option
gcc -O3 -o loggfi loggfi.c $file $comp_option
# gcc -O3 -o test test.c $file $comp_option