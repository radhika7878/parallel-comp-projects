Compiling and running code-

i) 	For the compilation of pthread implementation of the problem run the command-
		gcc -o pt_imp thr.c queue.c cmp2.c cmp.c pt_imp.c -pthread -lcrypto -lm
	For running-
		./pt_imp

iI) For the compilation of OpenMP implementation of the problem run the command-
		gcc -o omp_imp queue.c cmp2.c cmp.c omp_imp.c -fopenmp -lcrypto -lm
	For running-
		./omp_imp

Input-

First give number of file systems to be compared(max=10).
Then give the pathnames of file systems relative to current path as input.(max length=256)

**calculation of SHA1 hash for a file requires openssl library
