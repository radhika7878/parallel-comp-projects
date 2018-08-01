BNPS:

compile: mpicc bnps.c -o execname
run: mpirun -n <no_processes> -host name1,name2,etc ./execname
no_processes: maximum 12 because only 11 text files with news have been provided here, for more processes mpre text files have to be created

BNPM:
compile: mpicc bnpm.c -o execname
run: mpirun -n <no_processes> -host name1,name2,etc ./execname
no_processes: maximum 12 because only 11 text files with news have been provided here, for more processes mpre text files have to be created
partitions: by default each partition is of size 4. To change that change the value of EDITOR_SIZE at line number 6(to any divisor of number of processes)in bnpm.c and recompile.
