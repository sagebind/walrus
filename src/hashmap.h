#ifndef WALRUS_HASHMAP_H
#define WALRUS_HASHMAP_H

typedef struct
   {
      char* table[100];
    } hashmap;

void hinsert(void);

void hdelete(void);

#endif
