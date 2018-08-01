#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <omp.h>
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

void parapro(stackNode* temp, varTablenode *vTable[26]){

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
				//varTable[x] = vTable[x];
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
					if(htable[hash].rule[i]->no_body != 0){
						bodynode* b  = htable[hash].rule[i]->body;
						while(b){
							push(&temp,0,b);
							b=b->next;
						}	
					}			
					if(temp){
						//print(varTable,i);
						//print1(temp,i);
						//printf("\n");
						parapro(temp,varTable);
					}
					else{
						found=1;
						printf("YES\n");
						print(varTable,i);
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
	//printf("%s",buff);
	strcpy((*st)->key,buff);
	fscanf(fp, "%d", &buf);
	//printf("%d",buf);
	(*st)->is_const=buf;
	fscanf(fp, "%d", &buf);
	//printf("%d",buf);
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
	
int main(){
	
	found=0;
	int l;
	for(l=0;l<hsize;l++){
		htable[l].is_const=-1;
	}

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
	// for(l=0;l<hsize;l++){
	// 	if(htable[l].is_const!=-1){
	// 		printf("%s ",htable[l].key);
	// 		printf("%d ",htable[l].no_rules);
	// 		printf("%d\n",htable[l].is_const);
	// 		int m;
	// 		for(m=0;m<htable[l].no_rules;m++){
	// 			rulenode* temp = htable[l].rule[m];
	// 			printf("%d ",temp->no_args);
	// 			printf("%d\n",temp->no_body);
	// 			int n;
	// 			for(n=0;n<temp->no_args;n++){
	// 				printf("%s ",temp->arg[n]);
	// 				printf("%d\n",temp->is_const[n]);
	// 			}
	// 			bodynode* b = temp->body;
	// 			for(n=0;n<temp->no_body;n++){

	// 				printf("%d ",b->no_args);
	// 				printf("%s ",b->key);
	// 				int o;
	// 				for(o=0;o<b->no_args;o++){
	// 					printf("%s ",b->arg[o]);
	// 					printf("%d ", b->is_const[o]);
	// 				}
	// 				if(b->next)
	// 					b=b->next;
	// 				printf("\n");
	// 			}


	// 		}

	// 	}
	// }
	
	
	stackNode *st = (stackNode *)malloc(sizeof(stackNode));
	// st->is_const=0;
	// strcpy(st->key,"p");
	// st->no_args=1;
	// st->next=NULL;
	// st->type_arg[0] = 2;
	// strcpy(st->arg[0],"X");

	//if variable is pushed find constant in rule table
	//if found print yes+ variable else print no
	//exit()

	readquery(&st,"query1.txt");
	//print1(st,5);

	

	//stackNode *temp = st;
	varTablenode *varTable[26];
	int x = 26;
	while(x--){
		varTable[x] = (varTablenode *)malloc(sizeof(varTablenode));
		strcpy(varTable[x]->key,"");
	}

	//int flag=1;

	parapro(st,varTable);

	// while(temp){
	// 	int hash = hashfunction(temp->key);
	// 	if(htable[hash].is_const==-1){
	// 		flag=0;
	// 		break;//add flag
	// 	}
	// 	else{
	// 		omp_set_num_threads(htable[hash].no_rules);
	// 		int id=0;
	// 		#pragma omp parallel private(id,varTable,temp,flag,hash)
	// 		{
	// 			id = omp_get_thread_num();
	// 			int ret = unify(temp,id,varTable,hash);
	// 			if(ret==0){
	// 				flag=0;
	// 				//break;//add flag
	// 			}
	// 			else{
	// 				stackNode* t = pop(&temp);
	// 				bodynode* b  = htable[hash].rule[id]->body;
	// 				while(b){
	// 					push(&temp,0,b);
	// 					b=b->next;
	// 				}
	// 			}

	// 		}
	// 	}
	// }

	// if(flag){
	// 	printf("YES\n");
	// 	print(varTable);
	// }
	if(found==0){
		printf("NO\n");
	}

}

