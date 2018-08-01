#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <limits.h>
#include "cmp.h"
#include "queue.h"

extern Queue *q;
extern struct dirent ***f_list;
extern int *map;
extern char *dir_name;
extern int *no_files;


int index_left(int arr1[],int arr2[],int n){
	int i,t=0;
	for(i=0;i<n;i++){
		if(arr1[i]<arr2[i])
			t=t+1;
	}
	if(t>=ceil(n/2))
		return 1;
	else 
		return 0;
}

int compare(int num){
	int index[num];
	int i,j,exist;
	exist=0;
	int total;
	for(i=0;i<num;i++)
		index[i]=2;
	int ret;
	int min;
	while(index_left(index,no_files,num)){
		i=0;
		
		while(index[i]>no_files[i])
			i++;
		min=i;
		for(i=1;i<num;i++){
			if(index[i]<no_files[i]){
				ret = strcmp(f_list[min][index[min]]->d_name,f_list[i][index[i]]->d_name);
				if(ret>0)
					min=i;
			}
		}
		total=1;
		map[min]=1;
		for(i=0;i<num;i++){
			if(index[i]<no_files[i] && i!=min){
				ret = strcmp(f_list[i][index[i]]->d_name,f_list[min][index[min]]->d_name);
				if(ret==0){
					total = total +1;
					map[i]=1;
					index[i] = index[i] +1;
				}
			}
		}
		if(total>=ceil(num/2)){
			exist=1;
			break;
			}
		else
			index[min]++;
	}
	
	if(exist==0)
		return exist;
	else{
		if(f_list[min][index[min]]->d_type == DT_DIR){
			char* temp;
			if(dir_name != NULL){
				temp = (char *)malloc(strlen(dir_name)+strlen(f_list[min][index[min]]->d_name)+2);
				strcpy(temp,dir_name);
				strcat(temp,"/");
				strcat(temp,f_list[min][index[min]]->d_name);
			}
			else{
				temp = (char *)malloc(strlen(f_list[min][index[min]]->d_name)+2);
				strcpy(temp,"/");
				strcat(temp,f_list[min][index[min]]->d_name);
			}
			enQueue(q, temp);
			printf("%s added to queue\n",f_list[min][index[min]]->d_name);
		}
		else{
			printf("%s in subtree\n",f_list[min][index[min]]->d_name);
		}

		int min1;
		int f=INT_MAX;
		for(i=0;i<num;i++){
			if(map[i]==1 && no_files[i]<f){
				f = no_files[i];
				min1=i;
			}
		}

		int arr[no_files[min1]];

		for(i=0;i<no_files[min1];i++)
			arr[i]=1;

		for(i=0;i<num;i++){
			if(map[i]==1 &&  i!=min1){
				int ind_min1 = index[min1];
				int r;
				while(arr[ind_min1]!=1){
					ind_min1++;
				}
				while(index[i]<no_files[i] && ind_min1<no_files[min1]){
					r = strcmp(f_list[i][index[i]]->d_name,f_list[min1][ind_min1]->d_name);
					if(r==0){
						index[i] = index[i]+1;
						ind_min1 = ind_min1+1;
					}
					if(r>0){
						arr[ind_min1]=0;
						ind_min1++;
					}
					else{
						index[i]++;
					}
				}
				while(ind_min1 < no_files[min1]){
				arr[ind_min1]=0;
				ind_min1++;
				}	
			}
		}

		for(i=index[min1];i<no_files[min1];i++){
			if(arr[i]==1){
				if(f_list[min1][i]->d_type == DT_DIR){
					char* temp;
					if(dir_name != NULL){
						temp = (char *)malloc(strlen(dir_name)+strlen(f_list[min1][i]->d_name)+2);
						strcpy(temp,dir_name);
						strcat(temp,"/");
						strcat(temp,f_list[min1][i]->d_name);
					}
					else{
						temp = (char *)malloc(strlen(f_list[min1][i]->d_name)+2);
						strcpy(temp,"/");
						strcat(temp,f_list[min1][i]->d_name);
					}
					enQueue(q, temp);
					printf("%s added to queue\n",f_list[min1][i]->d_name);
				}
				else{
					printf("%s in subtree\n",f_list[min1][i]->d_name);
				}

			}
		}
		return exist;

	}
	
}

void comparenext(int num){
	int index[num];
	int i;
	for(i=0;i<num;i++)
		index[i]=2;

	int min1;
	int f=INT_MAX;
	for(i=0;i<num;i++){
		if(map[i]==1 && no_files[i]<f){
			f = no_files[i];
			min1=i;
		}
	}

	int arr[no_files[min1]];
	for(i=0;i<no_files[min1];i++)
		arr[i]=1;

	for(i=0;i<num;i++){
		if(map[i]==1 &&  i!=min1){
			int ind_min1 = index[min1];
			int r;
			while(arr[ind_min1]!=1){
				ind_min1++;
			}
			while(index[i]<no_files[i] && ind_min1<no_files[min1]){
				r = strcmp(f_list[i][index[i]]->d_name,f_list[min1][ind_min1]->d_name);
				if(r==0){
					index[i] = index[i]+1;
					ind_min1 = ind_min1+1;
				}
				if(r>0){
					arr[ind_min1]=0;
					ind_min1++;
				}
				else{
					index[i]++;
				}
			}
			while(ind_min1 < no_files[min1]){
				arr[ind_min1]=0;
				ind_min1++;
			}
		}
	}

	for(i=2;i<no_files[min1];i++){
		if(arr[i]==1){
			if(f_list[min1][i]->d_type == DT_DIR){
				char* temp;
				if(dir_name != NULL){
					temp = (char *)malloc(strlen(dir_name)+strlen(f_list[min1][i]->d_name)+2);
					strcpy(temp,dir_name);
					strcat(temp,"/");
					strcat(temp,f_list[min1][i]->d_name);
				}
				else{
					temp = (char *)malloc(strlen(f_list[min1][i]->d_name)+2);
					strcpy(temp,"/");
					strcat(temp,f_list[min1][i]->d_name);
				}
				enQueue(q, temp);
				printf("%s added to queue\n",f_list[min1][i]->d_name);
			}
			else{
				printf("%s in subtree\n",f_list[min1][i]->d_name);
			}

		}
	}

}