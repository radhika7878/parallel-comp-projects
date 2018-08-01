#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <memory.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include "cmp.h"
#include "queue.h"

extern Queue *q;
extern struct dirent ***f_list;
extern char *dir_name;
extern int *no_files;
extern identical;

void calsha(char* fname, char* const hashstr){
	SHA_CTX ctx;
    SHA1_Init(&ctx);
    FILE *fp;
    char b[100];

    fp = fopen(fname,"r");
    while(fgets(b,100,fp)!=NULL)
        SHA1_Update(&ctx,b,(int)strlen(b));
    fclose(fp);

    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1_Final(hash, &ctx);

    int i;
    for(i=0;i<20;i++)
    	sprintf(&hashstr[i*2], "%02X", hash[i]);
    hashstr[40]=0;

}

void compare2(){
	int i,j;
	i=2;
	j=2;
	int ret;
	while(i<no_files[0] && j<no_files[1]){
		ret = strcmp(f_list[0][i]->d_name,f_list[1][j]->d_name);
		if(ret==0){
			if(f_list[0][i]->d_type == f_list[1][j]->d_type){
				if(f_list[1][j]->d_type == DT_DIR){
					char* temp;
					if(dir_name != NULL){
						temp = (char *)malloc(strlen(dir_name)+strlen(f_list[0][i]->d_name)+2);
						strcpy(temp,dir_name);
						strcat(temp,"/");
						strcat(temp,f_list[0][i]->d_name);
					}
					else{
						temp = (char *)malloc(strlen(f_list[0][i]->d_name)+2);
						strcpy(temp,"/");
						strcat(temp,f_list[0][i]->d_name);
					}
					enQueue(q, temp);
					printf("%s added to queue\n",f_list[1][j]->d_name);
				}
				else{
					struct stat st1,st2;
					stat(f_list[0][i]->d_name,&st1);
					stat(f_list[1][j]->d_name,&st2);
					off_t size1 = st1.st_size;
					off_t size2 = st2.st_size;
					if(size1 == size2){
						char hash1[41],hash2[41];
						calsha(f_list[0][i]->d_name,hash1);
						calsha(f_list[1][j]->d_name,hash2);
						if(strcmp(hash1,hash2)){
							printf("%s has been edited in file system's: %s\n",f_list[0][i]->d_name,dir_name);
							identical=1;
						}
						else{
							printf("%s is identical in both file systems\n",f_list[0][i]->d_name);
						}
					}
					else{
						printf("%s has been edited in file system's: %s\n",f_list[0][i]->d_name,dir_name); 
						identical=1;
					}
				}
			}
			else{
				printf("%s is added in first file system's: %s\n",f_list[0][i]->d_name,dir_name);
				printf("%s is added in second file systems's: %s\n",f_list[1][j]->d_name,dir_name);
				identical=1;
			}
			i++;
			j++;
		}
		else if(ret<0){
			printf("%s is added in first file systems: %s\n", f_list[0][i]->d_name,dir_name);
			identical=1;
			i++;
		}
		else{
			printf("%s is added in second file systems: %s\n", f_list[1][j]->d_name,dir_name);
			identical=1;
			j++;
		}			
	}
	while(i<no_files[0]){
		printf("%s is added in first file systems: %s\n", f_list[0][i]->d_name,dir_name);
		identical=1;
		i++;
	}
	
	while(j<no_files[1]){
		printf("%s is added in second file systems: %s\n", f_list[1][j]->d_name,dir_name);
		identical=1;
		j++;
	}
}