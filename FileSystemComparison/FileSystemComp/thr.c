#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <memory.h>
#include "thr.h"

extern struct dirent ***f_list;
extern int *wait;
extern char *dir_name;
extern int *no_files;


void *read_dir(void *targ){
	thr_arg *arguments = (thr_arg *)targ; 
	int i;
	int id = arguments->id;
	char* f = arguments->f;
	no_files[id] = scandir(f, &f_list[id],0,alphasort);
	if(no_files[id]<0){
		perror("Could not scan directory: ");
		printf("%s\n",f);
	}
 	wait[id]=1;
	while (wait[id]==1){

	}
	char *temp;
	while(strcmp( dir_name,">>>>>>>>")){
		temp = (char *)malloc(strlen( dir_name)+strlen(f)+1);
		strcpy(temp,f);
		strcat(temp, dir_name);
		no_files[id] = scandir(temp, &f_list[id],0,alphasort);
		if( no_files[id]<0){
			perror("Could not scan directory: ");
			printf("%s\n",temp);
		}			

		wait[id]=1;
		while( wait[id]==1){

		}
	}
	 wait[id]=1;

}