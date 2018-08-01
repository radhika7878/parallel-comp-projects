
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include <omp.h>
#include "queue.h"
#include "cmp.h"
#include "cmp2.h"

Queue *q;
struct dirent ***f_list;
char *dir_name;
int *no_files;
int identical;
int *map;

int main(){

	int multi=0;

	dir_name = NULL;
	q = createQueue();
	identical=0;
	int num,i;
	
	printf("No. of file systems to be compared: ");
	scanf("%d",&num);

	map = (int *)malloc(sizeof(int)*num);
	for(i=0;i<num;i++){
		map[i]=0;
	}

	f_list = (struct dirent***)malloc(sizeof(**f_list)*num);
	no_files = (int *)malloc(sizeof(int)*num);

	char f_name[10][256];

	for(i=0;i<num;i++){
		printf("\nName of file system %d: ", i+1);
		scanf("%s",f_name[i]);
	}

	omp_set_num_threads(num);
	int id=0;


	#pragma omp parallel private(id,i)
	{
		while(1){
			id = omp_get_thread_num();
			if(multi!=1 || map[id]!=0)
			{
				if(dir_name==NULL){
					no_files[id]=scandir(f_name[id],&f_list[id],0,alphasort);
					if(no_files[id]<0){
						perror("Could not scan directory: ");
						printf("%s\n",f_name[id]);
					}
					else{
						for(i=0;i<no_files[id];i++)
							printf("%s\n",f_list[id][i]->d_name);
					}
				}
				else if(!(strcmp(dir_name,">>>>>>>>")))
					break;
				else{
					char *temp;
					temp = (char *)malloc(strlen(dir_name)+strlen(f_name[id])+1);
					strcpy(temp,f_name[id]);
		 			strcat(temp,dir_name);
		 			no_files[id] = scandir(temp,&f_list[id],0,alphasort);
		 			if(no_files[id]<0){
		 				perror("Could not scan directory: ");
		 				printf("%s\n",temp);
		 			}			
		 			else{
		 				for(i=0;i<no_files[id];i++)
		 					printf("%s\n",f_list[id][i]->d_name);
					}	
				}
			}	

			#pragma omp barrier

			#pragma omp master
			{
				if(num==2){
					compare2();
					int j;
					for(i=0;i<num;i++){
						for(j=0;j<no_files[i];j++)
							free(f_list[i][j]);
						free(f_list[i]);
					}

					if(q->rear != NULL){
						QNode *temp = deQueue(q);
						dir_name = (char *)malloc(strlen(temp->key)+1);
						strcpy(dir_name,temp->key);
					}
					else{
						dir_name = (char *)malloc(9);
						strcpy(dir_name,">>>>>>>>");
						if(identical==0)
							printf("GIVEN DIRECTORIES ARE IDENTICAL\n");
					}
				}
				else
				{
					if(multi==1){
						comparenext(num);
						int j;
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
						}
					}
					if(multi==0){
						int e = compare(num);
						if(e==0){
							printf("COMPLETELY DIVERGENT DIRECTORIES\n");
							dir_name = (char *)malloc(9);
							strcpy(dir_name,">>>>>>>>");
						}
						else{
							int j;
							for(i=0;i<num;i++){
								for(j=0;j<no_files[i];j++)
									free(f_list[i][j]);
								free(f_list[i]);
							}

							if(q->rear != NULL){
								QNode *temp = deQueue(q);
								dir_name = (char *)malloc(strlen(temp->key)+1);
								strcpy(dir_name,temp->key);
							}
							else{
								dir_name = (char *)malloc(9);
								strcpy(dir_name,">>>>>>>>");
							}
							multi=1;
						}
					}	
				}
			}
			#pragma omp barrier
		}

	}
}



