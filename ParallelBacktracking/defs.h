#ifndef _DEFS_H
#define _DEFS_H

#define maxrules 5
#define maxword 10
#define hsize 50
#define args 10 
typedef struct bodynode{
	int no_args;
	//int body_type;
	char key[maxword];
	char (*arg)[maxword];
	int *is_const;
	struct bodynode* next;
}bodynode;

typedef struct rulenode{
	int no_args;
	int no_body;
	char (*arg)[maxword];
	int *is_const;
	bodynode* body;
}rulenode;

typedef struct hashnode{
	char key[maxword];
	int no_rules;
	int is_const;
	rulenode *rule[maxrules];
}hashnode;

typedef struct stackNode
{
	int is_const;
	char key[maxword];
	int no_args;
	char arg[args][maxword];
	int type_arg[args];
	struct stackNode* next;
}stackNode;

typedef struct varTablenode
{
	char key[maxword];
}varTablenode;

#endif
