#include <stdlib.h>
#include <string.h>
#include "symboltable.h"
#include "hashmap.h"
int* scope = 0;

hashmap* activeTable;

Stack* theStack;



Stack* stack_create(){
	StackNode* tail;
	tail.previous = null; 
	tail.content = 0;
	theStack.tail = tail;
	theStack.current = tail;
}

void push(int* inp){
	StackNode* new;
	new.content = inp;
	new.previous = theStack.current;
	theStack.current = new;
	
}

int* pop(){
	int* x = theStack.current.content;
	theStack.current = theStack.previous;
}


void insert(char* element){
	
}

void lookup(char* inp){
	
}
