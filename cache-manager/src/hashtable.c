#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*Internals*/
int hashCode(const char *key, const long size);

ValueEntryNode *hTableInitNodeEntry(ValueEntry *entry);

void hTableReleaseNodeEntry(ValueEntryNode *entry);

ValueEntry *hTableInitEntry(const char *key, const unsigned long value)
{
    ValueEntry *result = malloc(sizeof(ValueEntry));
    result->key = key;
    result->value = value;
    return result;
}

HashTable *hTableInit(const unsigned long size)
{
    HashTable *v = malloc(sizeof(HashTable));
    v->values = malloc(sizeof(ValueEntryNode) * size);
    v->maxsize = size;
    return v;
}

ValueEntry *hTableSearch(HashTable *table, const char *key)
{
    return NULL;
}

int hTableInsert(HashTable *table, ValueEntry *value)
{
    int hash = hashCode(value->key, table->maxsize);

    ValueEntryNode *node = table->values[hash];

    if (node == NULL)
    {
        table->values[hash] = hTableInitNodeEntry(value);
    }
    else
    {
        while (node->next != NULL)
            node = node->next;
        node->next = hTableInitNodeEntry(value);
        node->next->prev = node;
    }

    return 0;
}
int hTableRemove(HashTable *table, const char *key)
{
    int hash = hashCode(key, table->maxsize);

    ValueEntryNode *node = table->values[hash];

    while (node != NULL)
    {
        if (strcmp(node->value->key, key))
        {
            if (node->prev != NULL)
                node->prev = node->next;
            if (node->next != NULL)
                node->next = node->prev;
            hTableReleaseNodeEntry(node);
            return 0;
        }
        else
        {
            node = node->next;
        }
    }

    return 1;
}

void hTableSave(HashTable *table, const char *filename)
{
}

HashTable *hTableLoad(const char *filename)
{
    return NULL;
}

int hashCode(const char *key, const long size)
{
    int length = strlen(key);
    int value = 0;
    for (int i = 0; i < length; i++)
    {
        value += key[i] - 65;
    }
    return key[0] % size;
}

ValueEntryNode *hTableInitNodeEntry(ValueEntry *entry)
{
    ValueEntryNode *result = malloc(sizeof(ValueEntryNode));
    result->value = entry;
    result->next = NULL;
    return result;
}

void hTableReleaseNodeEntry(ValueEntryNode *entry)
{
    if (entry->value != NULL)
        free(entry->value);
    free(entry);
}