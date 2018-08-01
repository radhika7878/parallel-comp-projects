#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include "queue.h"
#include "thr.h"
#include "cmp.h"
#include "cmp2.h"


Queue *q;
struct dirent ***f_list;
int *wait;
char *dir_name = NULL;
int *no_files;
char f_name[10][256];
int identical;
int *map;


int zero_check(int arr[], int n){
	int flag,i;
	flag=0;
	for(i=0;i<n;i++)
		if(arr[i]==0) flag=1;
	return flag;
}

int zero_check1(int arr[], int n){
	int flag,i;
	flag=0;
	for(i=0;i<n;i++)
		if(arr[i]==0 && map[i]==1) flag=1;
	return flag;
}


int main(){

	identical=0;

	int num,i,j;
	printf("No. of file systems to be compared: ");
	scanf("%d",&num);

	wait = (int *)malloc(sizeof(int)*num);
	map = (int *)malloc(sizeof(int)*num);
	for(i=0;i<num;i++){
		wait[i]=0;
		map[i]=0;
	}

	pthread_t th[num];
	thr_arg targ[num];

	f_list = (struct dirent***)malloc(sizeof(**f_list)*num);
	no_files = (int *)malloc(sizeof(int)*num);


	for(i=0;i<num;i++){
		printf("\nName of file system %d: ", i+1);
		scanf("%s",f_name[i]);
		strcpy(targ[i].f, f_name[i]);
		targ[i].id = i;
		pthread_create(&th[i],NULL,read_dir,(void *)&targ[i]);
	}

	q = createQueue();

	if(num==2){

		while(1){

			while(zero_check(wait,num)){

			}

			compare2();

			for(i=0;i<num;i++){
				for(j=0;j<no_files[i];j++)
					free(f_list[i][j]);
				free(f_list[i]);
			}

			QNode *temp = q->front;
			while(temp != NULL){
				printf("%s\n", temp->key);
				temp = temp->next;
			}

			if(q->rear != NULL){
				QNode *temp = deQueue(q);
				dir_name = (char *)malloc(strlen(temp->key)+1);
				strcpy(dir_name,temp->key);
			}
			else{
				dir_name = (char *)malloc(9);
				strcpy(dir_name,">>>>>>>>");
				break;
			}
			for(i=0;i<num;i++)
				wait[i]=0;

		}

		for(i=0;i<num;i++)
			wait[i]=0;

		for(i=0;i<num;i++)
			pthread_join(th[i],NULL);

		if(identical==0)
			printf("GIVEN DIRECTORIES ARE IDENTICAL\n");

	}

	else{
		int x=zero_check(wait,num);
		while(x==1){
			x=zero_check(wait,num);
		}
		int e = compare(num);
		if(e==0)
			printf("Completely divergent");
		else{
			int i;
			for(i=0;i<num;i++){
				if(map[i]!=1){
					dir_name = (char *)malloc(9);
					strcpy(dir_name,">>>>>>>>");
					wait[i]=0;
					pthread_join(th[i],NULL);
					int j;
					for(j=0;j<no_files[i];j++)
						free(f_list[i][j]);
					free(f_list[i]);
				}
			}
			while(1){
				for(i=0;i<num;i++){
					if(map[i]==1){
						for(j=0;j<no_files[i];j++)
							free(f_list[i][j]);
						free(f_list[i]);
					}
				}

				if(q->rear != NULL){
					QNode *temp = deQueue(q);
					dir_name = (char *)malloc(strlen(temp->key)+1);
					strcpy(dir_name,temp->key);
				}
				else{
					dir_name = (char *)malloc(9);
					strcpy(dir_name,">>>>>>>>");
					break;
				}
				for(i=0;i<num;i++)
					wait[i]=0;

				while(zero_check1(wait,num)){

				}

				comparenext(num);

			}
		}

		

	}
}



