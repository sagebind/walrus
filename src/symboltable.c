#include <stdlib.h>
#include <string.h>
#include "symboltable.h"
#include "hashmap.h"
int scope = 0;

hashmap scopes[50];
hashmap activeTable;
void initializeScope(){
	scopes[scope] = activeTable;
	scope++;
	if(scope <= 50){
	activeTable = scopes[scope];
	}
	/*else allocate more space, but who needs more than 50 scope in decaf?*/
}

void finalizeScope(){
	scopes[scope] = activeTable;
	if(scope >= 0){
	scope--;
	activeTable = scopes[scope];
	}
	/*else probably some kind of error*/
}

void insert(char* element){
	
}

void lookup(char* inp){
	
}
