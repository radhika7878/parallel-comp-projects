#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#define maxrules 5
#define maxword 10
#define hsize 50

//body node: key of bodynode and number of arguments
//arg is an array of strings which stores the argument name for the functor 
//is_const array stores whether the argument is constant or variable
typedef struct bodynode{
	int no_args;
	char key[maxword];
	char (*arg)[maxword];
	int *is_const;
	struct bodynode* next;
}bodynode;

//rule node: number of arguments the functor has, no of body clauses it has
//arg is an array of strings which stores the argument name for the functor 
//is_const array stores whether the argument is constant or variable
//body : pointer to bodynode which is a linked list of body items
typedef struct rulenode{
	int no_args;
	int no_body;
	char (*arg)[maxword];
	int *is_const;
	bodynode* body;
}rulenode;


//main node: stores key,no_rules associated with it, whether it is a constant or a functor
//pointers to each rule, for now maximum number of rules per name is 5
typedef struct hashnode{
	char key[maxword];
	int no_rules;
	int is_const;
	rulenode *rule[maxrules];
}hashnode;


//basic hash function, can change if needed
int hashfunction(char value[]) {
	int i = 0;
	int sum = 0;
	while (value[i] != '\0') {
		sum += (int) value[i];
		i++;
	}
	return (sum % hsize);
}

// typedef struct stack{
// 	bodynode data;
// 	struct stack *n;
// }stack;

// void popstack(stack** top) {
// 	stack* temp;
// 	temp = *top;	
// 	(*top) = (*top)->n;
// 	free(temp);	
// }

// void pushstack(stack** top, bodynode val) {
// 	stack* temp;
// 	temp =(stack*)malloc(sizeof(stack));
// 	temp->data = val;
// 	temp->n = *top;
// 	*top = temp;
// }


int main(){
	hashnode htable[hsize];
	

	int l;
	for(l=0;l<hsize;l++){
		htable[l].is_const=-1;
	}

	FILE* fp = fopen("kb2.txt","r");
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




}


