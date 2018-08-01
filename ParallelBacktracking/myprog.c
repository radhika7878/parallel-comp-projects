#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "defs.h"
stackNode* stackTop;
//int varTable[hsize];

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

void push(int is_const,bodynode *body)
{
	stackNode* temp;
	temp=(stackNode*)malloc(sizeof(stackNode));
	temp->is_const=is_const;
	strcpy(temp->key,body->key);
	temp->next=NULL;
	temp->no_args=body->no_args;
	temp->type_arg=(int *)malloc((body->no_args)*sizeof(int));
	temp->arg=malloc((body->no_args)*maxword);
	int i;
	for(i=0;i<(body->no_args);i++)
	{
		temp->type_arg[i]=body->is_const[i];
		strcpy(temp->arg[i],body->arg[i]);
	}
	if(stackTop==NULL)
	{
		stackTop=temp;
	}
	else
	{
		temp->next=stackTop;
		stackTop=temp;
	}
}
stackNode* pop()
{
	if(stackTop==NULL)
	{
		printf("Stack is empty!No pop\n");
		return NULL;
	}
	stackNode* temp;
	temp=stackTop;
	stackTop=stackTop->next;
	return temp;
}
void readQuery()
{
	bodynode *body=(bodynode*)malloc(sizeof(bodynode));
	stackTop=NULL;
	FILE* fp = fopen("query.txt","r");
	char buff[255];
	int is_const,buf;
	fscanf(fp, "%s", buff);
	strcpy(body->key,buff);
	fscanf(fp, "%d",&is_const);
	fscanf(fp, "%d",&buf);
	body->no_args=buf;
	body->next=NULL;
	body->arg=NULL;
	body->is_const=NULL;
	push(is_const,body);
}

char** unify(stackNode* stck,rulenode *rule)//for functors only
{
	char **varTable;
	vartable = (char **)malloc(hsize*sizeof(char*));
	int i,var1,var2;
	for(i=0;i<hsize;i++)
	{
		vartable[i] = (char *)malloc(maxword*sizeof(char));
		strcpy(varTable[i],"\0");
	}
	if(stck->no_args==rule->no_args)
	{
		//compare arguments
		for(i=0;i<stck->no_args;i++)
		{
			if(stck->type_arg[i]==2 && rule->is_const[i]==2)
			{
				//both variables
				var1=hashfunction(stck->arg[i]);
				var2=hashfunction(rule->arg[i]);
				strcpy(varTable[var1],rule->arg[i]);
				strcpy(varTable[var2],rule->arg[i]);

			}
			else if(stck->type_arg[i]==2 && rule->is_const[i]==1)
			{
				//rule is const
				var1=hashfunction(stck->arg[i]);
				strcpy(varTable[var1],rule->arg[i]);
			}
			else if(stck->type_arg[i]==1 && rule->is_const[i]==1){
				if(strcmp(stck->arg[i],rule->arg[i]))
					return NULL;
			}
		}

	}
	else
	{
		return NULL;
	}
	return varTable;
}

// void solve(hashnode *htable)
// {
// 	//query read and pushed already
// 	struct stackNode *temp;
// 	int var,i;
// 	char varTable[hsize][maxword];
// 	while(stackTop!=NULL)
// 	{
// 		temp=pop();
// 		var=hashfunction(temp->key);
// 		if(temp->is_const==1 && htable[var]->is_const==1)
// 		{
// 			//both are constants and match
// 			continue;
// 		}
// 		else if(temp->is_const==2)
// 		{
// 			//it is variable then match with all rules and unify variable with constant
// 			for(i=0;i<hsize;i++)
// 			{
// 				if(htable[i]->is_const==1)
// 				{
// 					strcpy(varTable[var],htable[i]->key);
// 					break;
// 				}
// 			}	
// 		}
// 		else if(temp->is_const==0 && htable[var]->is_const==0)
// 		{
// 			//functor and head matches
// 			int no_rules=htable[var]->no_rules;
// 			for(i=0;i<no_rules;i++)
// 			{
// 				//unify etc
// 			}
// 		}
// 	}
	
// }

int main(){
	hashnode htable[hsize];

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

	
	for(l=0;l<hsize;l++){
		if(htable[l].is_const!=-1){
			printf("%s ",htable[l].key);
			printf("%d ",htable[l].no_rules);
			printf("%d\n",htable[l].is_const);
			int m;
			for(m=0;m<htable[l].no_rules;m++){
				rulenode* temp = htable[l].rule[m];
				printf("%d ",temp->no_args);
				printf("%d\n",temp->no_body);
				int n;
				for(n=0;n<temp->no_args;n++){
					printf("%s ",temp->arg[n]);
					printf("%d\n",temp->is_const[n]);
				}
				bodynode* b = temp->body;
				for(n=0;n<temp->no_body;n++){

					printf("%d ",b->no_args);
					printf("%s ",b->key);
					int o;
					for(o=0;o<b->no_args;o++){
						printf("%s ",b->arg[o]);
						printf("%d ", b->is_const[o]);
					}
					if(b->next)
						b=b->next;
					printf("\n");
				}


			}

		}
	}
	readQuery();
	stackNode *temp=stackTop;
	while(temp!=NULL)
	{
		printf("%s  %d  %d \n",temp->key,temp->is_const,temp->no_args);
		temp=temp->next;
	}
	for(l=0;l<hsize;l++){
	if(htable[l].is_const!=-1){
			int m;
			for(m=0;m<htable[l].no_rules;m++){
				rulenode* temp = htable[l].rule[m];
				bodynode* b = temp->body;
				while(b!=NULL)
				{
					push(0,b);
					printf("pushed\n");
					b=b->next;
				}
			}
		}
	}
	temp=stackTop;
	while(temp!=NULL)
	{
		printf("%s  %d  %d \n",temp->key,temp->is_const,temp->no_args);
		temp=temp->next;
	}
	struct stackNode *tp=pop();
	temp=stackTop;
	while(temp!=NULL)
	{
		printf("\n%s  %d  %d \n",temp->key,temp->is_const,temp->no_args);
		temp=temp->next;
	}

	


}


