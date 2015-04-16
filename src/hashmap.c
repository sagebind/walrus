#include <stdlib.h>
#include <string.h>
#include "hashmap.h"

int scope = 0;
void insert(){

}

void initializeScope(){
	scope++;
}

void finalizeScope(){
	scope--;
}
