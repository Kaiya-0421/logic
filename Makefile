logfi:logfi.c
	gcc  -O3  logfi.c -lmpfr -lgmp -lmpfi -o logfi
parallel: parallel.cpp
	g++ parallel.cpp -I /usr/local/include/eigen3 -std=c++14 -fopenmp -lpthread -O3 -o $@	

