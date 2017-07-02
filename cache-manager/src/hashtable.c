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
    if (table->verbose)
        printf("Inserção de chave [%s] selecionada posição [%d]\n", value->key, hash);

    ValueEntryNode *node = table->values[hash];

    if (node == NULL)
    {
        table->values[hash] = hTableInitNodeEntry(value);
    }
    else
    {
        if (strcmp(node->value->key, value->key))
        {
            if (table->verbose)
                printf("Chave já existe na tabela, ignorando inserção");
            return 0;
        }
        else
        {
            if (table->verbose)
                printf("Colisão do hash, inserindo na lista");
            while (node->next != NULL)
                node = node->next;
            node->next = hTableInitNodeEntry(value);
            node->next->prev = node;
        }
    }

    return 1;
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
            return 1;
        }
        else
        {
            node = node->next;
        }
    }

    return 0;
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