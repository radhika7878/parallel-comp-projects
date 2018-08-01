#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxword 10

typedef struct varTablenode
{
	char key[maxword];
}varTablenode;

void write(int varTable[26]){
	int x;
	for(x=0;x<26;x++){
		varTable[x] =1;
	}
	
	
	varTable[0] = 2;
	varTable[1] = varTable[0];

}

void print(int varTable[26]){
	int x;
	for(x=0;x<26;x++){
		printf("%d",varTable[x]);
	}

}

int main() {
	// varTablenode *varTable[26];
	// int x ;
	// for(x=0;x<26;x++){
	// 	varTable[x] = (varTablenode *)malloc(sizeof(varTablenode));
	// 	strcpy(varTable[x]->key,"");
	// }

	int varTable[26];

	write(varTable);
	print(varTable);

}