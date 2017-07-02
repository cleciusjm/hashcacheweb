#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *str2md5(const char *str);
HashTable *hTableinit(int size)
{
    HashTable *v = malloc(sizeof(HashTable));
    v->values = malloc(sizeof(ValueEntry) * size);
    v->size = size;
    return v;
}
ValueEntry *hTableSearch(HashTable *table, char *key)
{
    return NULL;
}
int hTableInsert(HashTable *table, ValueEntry *value)
{
    return 0;
}
int hTableRemove(HashTable *table, char *key)
{
    return 0;
}
