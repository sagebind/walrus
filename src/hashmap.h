#ifndef WALRUS_HASHMAP_H
#define WALRUS_HASHMAP_H

typedef struct
   {
      char* table[100];
    } hashmap;

void hinsert(char* inp);

void hlookup(char* inp);

void hdelete(void);

#endif
