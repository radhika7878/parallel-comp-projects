#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <omp.h>
#include <mpi.h>
#include "defs.h"

hashnode htable[hsize];
int found;


int hashfunction(char *str) {
	unsigned long hash = 5381;
	unsigned long hval;
	int c;
	int hashkey;
	while((c= *str++)!=0)
	{
		hash=hash*33+c;
	}	
	hval=hash%hsize;
	hashkey=(int)hval;
	return hashkey;
}

void push(stackNode** stackTop,int is_const,bodynode *body)
{
	stackNode* temp;
	temp=(stackNode*)malloc(sizeof(stackNode));
	temp->is_const=is_const;
	strcpy(temp->key,body->key);
	temp->next=NULL;
	temp->no_args=body->no_args;
	//temp->type_arg=(int *)malloc((body->no_args)*sizeof(int));
	//temp->arg=malloc((body->no_args)*maxword);
	int i;
	for(i=0;i<(body->no_args);i++)
	{
		temp->type_arg[i]=body->is_const[i];
		strcpy(temp->arg[i],body->arg[i]);
	}
	if((*stackTop)==NULL)
	{
		(*stackTop)=temp;
	}
	else
	{
		temp->next=(*stackTop);
		(*stackTop)=temp;
	}
}
stackNode* pop(stackNode** stackTop)
{
	if((*stackTop)==NULL)
	{
		printf("Stack is empty!No pop\n");
		return NULL;
	}
	stackNode* temp;
	temp=(*stackTop);
	(*stackTop)=(*stackTop)->next;
	return temp;
}


int unify(stackNode* stck,int rulenumber, varTablenode *varTable[26], int hash)
{
	
	if(stck->is_const==1){
		//it is constant
		if(htable[hash].is_const!=1)
			return 0;
		else{
			if(strcmp(htable[hash].key,stck->key)==0)
				return 1;
			else
				return 0;
		}

	}
	else if(stck->is_const==0){
		if(stck->no_args != htable[hash].rule[rulenumber]->no_args)
			return 0;
		else{
			rulenode *r = htable[hash].rule[rulenumber];
			int x;
			for(x=0;x<r->no_args;x++){
				if(stck->type_arg[x]==1 && r->is_const[x]==1){
					if(strcmp(stck->arg[x],r->arg[x])!=0)
						return 0;
				}
				else if(stck->type_arg[x]==2 && r->is_const[x]==2){
					int pos1 = stck->arg[x][0] - 'A';
					int pos2 = r->arg[x][0] - 'A';
					if(strcmp(varTable[pos1]->key,"") == 0 && strcmp(varTable[pos2]->key,"") == 0 )
						varTable[pos1] = varTable[pos2];
					else if(strcmp(varTable[pos1]->key,"") != 0 && strcmp(varTable[pos2]->key,"") == 0 )
						varTable[pos2] = varTable[pos1];
					else if(strcmp(varTable[pos1]->key,"") == 0 && strcmp(varTable[pos2]->key,"") != 0 )
						varTable[pos1] = varTable[pos2];
					else if(strcmp(varTable[pos1]->key,"") != 0 && strcmp(varTable[pos2]->key,"") != 0 ){
						if(strcmp(varTable[pos1]->key,varTable[pos2]->key) == 0)
							continue;
						else
							return 0;
					}
						
				}
				else if(stck->type_arg[x]==2 && r->is_const[x]==1){
					int pos1 = stck->arg[x][0] - 'A';
					if(strcmp(varTable[pos1]->key,"") == 0)
						strcpy(varTable[pos1]->key,r->arg[x]);
					else{
						if(strcmp(varTable[pos1]->key,r->arg[x]) == 0)
							continue;
						else
							return 0;
					}
				}

			}
			return 1;
		}

	}
	else 
		return 0;
}

void print(varTablenode* varTable[26],int i){
	int x;
	for(x=0;x<26;x++){
		if(strcmp(varTable[x]->key,"")==0)
			continue;
		else{
			printf("from %d: %c = %s\n",i,(char)(x + (int)'A'),varTable[x]->key);
		}
	}

}


void print1(stackNode* temp,int i){
	while(temp){
		printf("from %d: %s\n",i,temp->key);
		temp=temp->next;
	}

}

void parapro(stackNode* temp, varTablenode *vTable[26],int id){

	int hash = hashfunction(temp->key);
	if(htable[hash].is_const==-1){
		return;//add flag
	}
	else{
		//omp_set_num_threads(htable[hash].no_rules);
		//int id=0;
		int i;
		for(i=0;i<htable[hash].no_rules;i++){
			// int ret = unify(temp,i,varTable,hash);
			// if(ret==0){
			// 	return;
			// }
			varTablenode *varTable[26];
			int x;
			for(x=0;x<26;x++){
				varTable[x] = (varTablenode *)malloc(sizeof(varTablenode));
				strcpy(varTable[x]->key,vTable[x]->key);
			}

			int a,b;
			for(a=0;a<26;a++){
				for(b=a+1;b<26;b++){
					if(vTable[a]==vTable[b]){
						varTable[b]=varTable[a];
					}
				}

			}

			#pragma omp task firstprivate(i,varTable,temp,hash)
			{	
				int ret = unify(temp,i,varTable,hash);
				if(ret==1){
					stackNode* t = pop(&temp);
					// if(htable[hash].rule[i]->no_body != 0){
					// 	bodynode* b  = htable[hash].rule[i]->body;
					// 	while(b){
					// 		push(&temp,0,b);
					// 		b=b->next;
					// 	}	
					// }	
					int j;
					bodynode* b;
					if(htable[hash].rule[i]->no_body != 0)
						b  = htable[hash].rule[i]->body;
					for(j=0;j<htable[hash].rule[i]->no_body;j++){
						push(&temp,0,b);
						b=b->next;

					}		
					if(temp){
						//print(varTable,i);
						//print1(temp,i);
						//printf("\n");
						parapro(temp,varTable,id);
					}
					else{
						found=1;
						printf("YES\n");
						print(varTable,id);
						printf("\n");
					}
				}			
				

			}

		}
		return;
	}
}

void readquery(stackNode **st, char fname[]){
	FILE *fp = fopen(fname, "r");
	char buff[255];
	int buf;
	fscanf(fp, "%s", buff);
	strcpy((*st)->key,buff);
	fscanf(fp, "%d", &buf);
	(*st)->is_const=buf;
	fscanf(fp, "%d", &buf);
	(*st)->no_args=buf;

	int i;
	for(i=0;i<buf;i++){
		fscanf(fp, "%s", buff);
		strcpy((*st)->arg[i],buff);
		fscanf(fp, "%d", &buf);
		(*st)->type_arg[i]=buf;
	}

	(*st)->next = NULL;

}
		
	
int main(int argc, char **argv){

	int my_id, num_procs, ierr, i;
	found=0;
	int l;
	for(l=0;l<hsize;l++){
		htable[l].is_const=-1;
	}

	MPI_Datatype mpi_string;
	MPI_Datatype mpi_rowint;

	ierr = MPI_Init(&argc, &argv);

	/* Find out MY process ID, and how many processes were started. */

	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	MPI_Type_contiguous(maxword, MPI_CHAR, &mpi_string);
	MPI_Type_commit(&mpi_string);

	MPI_Type_contiguous(args, MPI_INT, &mpi_rowint);
	MPI_Type_commit(&mpi_rowint);


	FILE* fp = fopen("kb1.txt","r");
	char buff[255];
	int buf;
	while (1)
	{
	    fscanf(fp, "%s", buff);
	    if(!strcmp(buff,"."))
	    	break;
	    int bin = hashfunction(buff);
	    strcpy(htable[bin].key,buff);
	    fscanf(fp, "%d", &buf);
	    int r = buf;
	    htable[bin].no_rules=r;
	    fscanf(fp, "%d", &buf);
	    htable[bin].is_const=buf;
	    int i;
	    for(i=0;i<r;i++){
	    	htable[bin].rule[i] = (rulenode *)malloc(sizeof(rulenode));
	    	fscanf(fp, "%d", &buf);
	    	htable[bin].rule[i]->no_args = buf;
	    	if(buf!=0)
	    	{
	    		htable[bin].rule[i]->arg = malloc(buf*maxword);
	    		htable[bin].rule[i]->is_const= (int *)malloc(buf*sizeof(int));
	    	}
	    	fscanf(fp, "%d", &buf);
	    	htable[bin].rule[i]->no_body = buf;
	    	int j;
	    	for(j=0;j<htable[bin].rule[i]->no_args;j++){
	    		fscanf(fp, "%s", buff);
	    		strcpy(htable[bin].rule[i]->arg[j],buff);
	    		fscanf(fp, "%d", &buf);
	    		htable[bin].rule[i]->is_const[j]=buf;
	    	}
	    	bodynode* cur;
	    	for(j=0;j<htable[bin].rule[i]->no_body;j++){
	    		bodynode* temp = (bodynode *)malloc(sizeof(bodynode));
	    		fscanf(fp, "%d", &buf);
	    		temp->no_args = buf;
	    		if(buf!=0)
		    	{
		    		temp->arg = malloc(buf*maxword);
		    		temp->is_const= (int *)malloc(buf*sizeof(int));
		    	}
		    	fscanf(fp, "%s", buff);
	    		strcpy(temp->key,buff);
	    		int k;
	    		for(k=0;k<temp->no_args;k++){
		    		fscanf(fp, "%s", buff);
		    		strcpy(temp->arg[k],buff);
		    		fscanf(fp, "%d", &buf);
		    		temp->is_const[k]=buf;
		    	}
		    	if(j==0){
		    		htable[bin].rule[i]->body = temp;
		    		cur = temp;
		    	}
		    	else{
		    		cur->next = temp;
		    		cur =temp;
		    	}
	    	}
	    }
	    //printf("%s ",buff);
	}
	fclose(fp);

	varTablenode *varTable[26];
	int x = 26;
	while(x--){
		varTable[x] = (varTablenode *)malloc(sizeof(varTablenode));
		strcpy(varTable[x]->key,"");
	}

	int num = num_procs;

	//root process
	if(my_id==0){
		stackNode *st = (stackNode *)malloc(sizeof(stackNode));
		// st->is_const=0;
		// strcpy(st->key,"p");
		// st->no_args=1;
		// st->type_arg[0] = 2;
		// strcpy(st->arg[0],"X");
		// st->next=NULL;

		readquery(&st,"query1.txt");
		int flag=1;

		while(st && num>1){
			int hash = hashfunction(st->key);
			if(htable[hash].is_const==-1){
				//send boolean
				int i;
				i=num_procs-num+1;
				for(;i<num_procs;i++){
					int k=0;
					MPI_Send(&k,1,MPI_INT, i,1,MPI_COMM_WORLD);
				}
					
				flag=0;
				break;
			}
			else{
				int norules = htable[hash].no_rules;
				if(norules>1){
					if(norules>num){						
						break;
					}
					else{
						
						int i;
						for(i=1;i<norules;i++){
							int k=1;
							MPI_Send(&k,1,MPI_INT,num_procs-num+1,1,MPI_COMM_WORLD);
							MPI_Send(&i,1,MPI_INT,num_procs-num+1,1,MPI_COMM_WORLD);
							int j;
							char string[maxword];
							for(j=0;j<26;j++){
								strcpy(string,varTable[j]->key);
								MPI_Send(string, 1,mpi_string,num_procs-num+1,1,MPI_COMM_WORLD);
							}
							stackNode* temp=st;
							int size=0;
							while(temp){
								size++;
								temp=temp->next;
							}
							MPI_Send(&size,1,MPI_INT,num_procs-num+1,1,MPI_COMM_WORLD);
							
							temp=st;
							while(temp){
								int a = temp->is_const;
								MPI_Send(&a,1,MPI_INT,num_procs-num+1,1,MPI_COMM_WORLD);
								a = temp->no_args;
								MPI_Send(&a,1,MPI_INT,num_procs-num+1,1,MPI_COMM_WORLD);
								char key[maxword];
								strcpy(key,temp->key);

								MPI_Send(key,1,mpi_string,num_procs-num+1,1,MPI_COMM_WORLD);
								for(j=0;j<temp->no_args;j++){								
									strcpy(key,temp->arg[j]);
									MPI_Send(key,1,mpi_string,num_procs-num+1,1,MPI_COMM_WORLD);
									a = temp->type_arg[j];
									MPI_Send(&a,1,MPI_INT,num_procs-num+1,1,MPI_COMM_WORLD);

								}
								temp=temp->next;
							}
							num--;
						}

					}
					
				}
				int ret = unify(st,0,varTable,hash);
				if(ret==1){
					stackNode* t = pop(&st);
					// if(htable[hash].rule[0]->no_body != 0){
					// 	bodynode* b  = htable[hash].rule[0]->body;
					// 	while(b){
					// 		push(&st,0,b);
					// 		b=b->next;
					// 	}	
					// }
					int j;
					bodynode* b;
					if(htable[hash].rule[i]->no_body != 0)
						b  = htable[hash].rule[i]->body;
					for(j=0;j<htable[hash].rule[i]->no_body;j++){
						push(&st,0,b);
						b=b->next;

					}
				}
				else{
					int i;
					i=num_procs-num+1;
					for(;i<num_procs;i++){
						int k=0;
						MPI_Send(&k,1,MPI_INT, i,1,MPI_COMM_WORLD);
					}
						
					flag=0;
					break;
				}
				

			}
		}
		if(st && flag==1){
			int i;
			i=num_procs-num+1;
			for(;i<num_procs;i++){
				int k=0;
				MPI_Send(&k,1,MPI_INT, i,1,MPI_COMM_WORLD);
			}


			parapro(st,varTable,my_id);
			
		}

	}

	else{
		int k;
		MPI_Status status;
		MPI_Recv(&k,1,MPI_INT, 0,1,MPI_COMM_WORLD,&status);
		if(k==1){
			int ruletouse;
			MPI_Recv(&ruletouse,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
			int j;
			char string[maxword];
			for(j=0;j<26;j++){
				MPI_Recv(string,maxword,MPI_CHAR,0,1,MPI_COMM_WORLD,&status);
				strcpy(varTable[j]->key,string);
			}

			//recv stack

			stackNode* st = NULL;
			stackNode* temp;
			stackNode* temp1;

			int size;
			MPI_Recv(&size,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
			while(size--){
				temp=(stackNode *)malloc(sizeof(stackNode));
				int a,b;
				MPI_Recv(&a,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
				temp->is_const=a;
				MPI_Recv(&a,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
				temp->no_args=a;
				char key[maxword];
				MPI_Recv(key,maxword,MPI_CHAR,0,1,MPI_COMM_WORLD,&status);
				
				strcpy(temp->key,key);
				for(j=0;j<a;j++)
				{
					MPI_Recv(key,maxword,MPI_CHAR,0,1,MPI_COMM_WORLD,&status);
					strcpy(temp->arg[j],key);
					MPI_Recv(&b,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
					temp->type_arg[j]=b;
				}
				temp->next=NULL;
				if(st==NULL)
				{
					st=temp;
				}
				else
				{
					temp1=st;
					while(temp1->next!=NULL)
					{
						temp1=temp1->next;
					}
					temp1->next=temp;
				}

			}

			

			int hash = hashfunction(st->key);
			int ret = unify(st,ruletouse,varTable,hash);
			


			if(ret==1){
				stackNode* t = pop(&st);
				

				// if(htable[hash].rule[ruletouse]->no_body != 0){
				// 	bodynode* b  = htable[hash].rule[ruletouse]->body;
				// 	while(b){
				// 		push(&st,0,b);
				// 		b=b->next;
				// 	}	
				// }
				int j;
				bodynode* b;
				if(htable[hash].rule[ruletouse]->no_body != 0)
					b  = htable[hash].rule[ruletouse]->body;
				for(j=0;j<htable[hash].rule[ruletouse]->no_body;j++){
					push(&st,0,b);
					b=b->next;

				}
				

				parapro(st,varTable,my_id);
			}
			
		}
	}

	//MPI_Barrier(MPI_COMM_WORLD);

	MPI_Type_free(&mpi_string);
	MPI_Type_free(&mpi_rowint);

	MPI_Finalize();

}

