#ifndef _THR_H
#define _THR_H

typedef struct thr_arg{
	char f[256];
	int id;
}thr_arg;

void *read_dir(void *targ);


#endif