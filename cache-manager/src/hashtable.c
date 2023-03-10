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
    result->key = (char *)key;
    result->value = value;
    return result;
}

HashTable *hTableInit(const unsigned long size)
{
    HashTable *v = malloc(sizeof(HashTable));
    v->values = malloc(sizeof(ValueEntryNode) * size);
    v->maxsize = size;
    for (int i = 0; i < size; i++)
        v->values[i] = NULL;
    return v;
}

ValueEntry *hTableSearch(HashTable *table, const char *key)
{
    int hash = hashCode(key, table->maxsize);

    ValueEntryNode *node = table->values[hash];

    while (node != NULL)
    {
        int compare = strcmp(node->value->key, key);
        if (table->verbose)
            printf("Hash encontrado na posição [%d], comparando [%s] com [%s] resultando em [%d]\n", hash, node->value->key, key, compare);
        if (compare == 0)
        {
            return node->value;
        }
        else
        {
            node = node->next;
        }
    }

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
        int compare = strcmp(node->value->key, value->key);
        if (table->verbose)
            printf("Colisão de código, comparando [%s] com [%s] resultando em [%d]\n", node->value->key, value->key, compare);
        if (compare == 0)
        {
            if (table->verbose)
                printf("Chave já existe na tabela, ignorando inserção\n");
            return 0;
        }
        else
        {
            if (table->verbose)
                printf("Colisão do hash, inserindo na lista\n");
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
        int compare = strcmp(node->value->key, key);
        if (compare == 0)
        {
            if (node->prev != NULL)
                node->prev = node->next;
            if (node->next != NULL)
                node->next = node->prev;
            hTableReleaseNodeEntry(node);
            table->values[hash] = NULL;
            return 1;
        }
        else
        {
            node = node->next;
        }
    }

    return 0;
}
void hTableWriteEntry(ValueEntry *entry, FILE *file, int verbose)
{
    if (verbose)
        printf("Escrevendo [chave: %s | valor: %d]\n", entry->key, entry->value);

    int length = strlen(entry->key);
    fwrite(&length, sizeof(int), 1, file);
    fwrite((const void *)entry->key, sizeof(char), length, file);
    fwrite(&(entry->value), sizeof(int), 1, file);
}
void hTableSave(HashTable *table, const char *filename)
{
    if (table->verbose)
        printf("Abrindo arquivo %s para gravação\n", filename);

    FILE *file = fopen(filename, "w");
    if (file != NULL)
    {
        if (table->verbose)
            printf("Escrevendo tamanho [%d]\n", table->maxsize);

        long size = table->maxsize;
        fwrite(&size, sizeof(long), 1, file);

        for (int i = 0; i < size; i++)
        {
            ValueEntryNode *node = table->values[i];
            while (node != NULL)
            {
                if (table->verbose)
                    printf("Avaliando posição %d para gravação\n", i);
                ValueEntry *entry = node->value;
                hTableWriteEntry(entry, file, table->verbose);
                node = node->next;
            }
        }
        fclose(file);
        if (table->verbose)
            printf("Arquivo gravado com sucesso\n");
    }
    else
    {
        if (table->verbose)
            printf("Falha ao abrir o arquivo para escrita\n");
    }
}
HashTable *hTableLoad(const char *filename, const int verbose)
{
    if (verbose)
        printf("Abrindo arquivo %s para leitura\n", filename);

    FILE *file = fopen(filename, "r");
    if (file != NULL)
    {
        int size = 0;
        fread(&size, sizeof(long), 1, file);
        HashTable *table = hTableInit(size);
        table->verbose = verbose;
        while (!feof(file))
        {
            int keyLength = 0;
            fread(&keyLength, sizeof(int), 1, file);
            char *key = malloc(sizeof(char) * keyLength);
            fread(key, sizeof(char), keyLength, file);
            int value = 0;
            fread(&value, sizeof(int), 1, file);
            ValueEntry *entry = hTableInitEntry(key, value);

            if (strlen(entry->key) > 0)
            {
                if (table->verbose)
                    printf("Lido [chave: %s | valor: %d]\n", entry->key, entry->value);
                hTableInsert(table, entry);
            }
        }
        fclose(file);
        if (table->verbose)
            printf("Tabela carregada com sucesso\n");

        return table;
    }
    else
    {
        if (verbose)
            printf("Falha ao abrir o arquivo para leitura\n");
    }
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
    return value % size;
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