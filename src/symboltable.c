#include <stdlib.h>
#include <string.h>
#include "symboltable.h"
#include "hashmap.h"

//hashmap* activeTable;

Stack* theStack;

hashmap* scopes[100];

int* counter, pos;

pos = 0;


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
	
	scopes[theStack.current.content].hinsert(element);
	
}

void lookup(char* inp){
	
		scopes[theStack.current.content].hlookup(element);
	
}

void initiateScope(){
	pos++;
	theStack.push(pos);
	
}

void closeScope(){
	
	pos=theStack.pop();
	
}
