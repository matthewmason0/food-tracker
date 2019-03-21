//
// Created by matthew on 3/17/19.
//

#ifndef FOOD_TRACKER_DATABASE_H
#define FOOD_TRACKER_DATABASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entries.h"
#include "rbTree.h"

typedef struct list {
    struct list* next;
    Food* food;
} List;

Tree* numberTree;
Tree* nameTree;
Tree* manufacturerTree;
Tree* caloriesTree;
Tree* carbohydratesTree;
Tree* fatTree;
Tree* proteinTree;
Tree* servingSizeTree;
Tree* servingUnitsTree;
Tree* householdServingSizeTree;
Tree* householdServingUnitsTree;

void printEntryNumber(Node* node)
{
    printf("Number: %ld, Name: %s\n", node->food->number, node->food->name);
}

char* readLine(FILE* file)
{
    size_t size = 256;
    char* line = malloc(size);
    char c = (char)getc(file);
    size_t i;
    for (i = 0; c != EOF && c != '\n'; i++)
    {
        line[i] = c;
        if (i == size - 1)
        {
            size *= 2;
            line = realloc(line, size);
        }
        c = (char)getc(file);
    }
    line[i] = '\0';
    return realloc(line, i + 1);
}

List* createList(FILE* file)
{
    List* head = malloc(sizeof(List));
    head->next = NULL;

    List* node = head;
    char* line = readLine(file);
    while(line[0] != '\0')
    {
        long number          = atol(strsep(&line, "~"));
        char* name           = strsep(&line, "~");
        char* manufacturer   = strsep(&line, "~");
        double calories      = atof(strsep(&line, "~"));
        double carbohydrates = atof(strsep(&line, "~"));
        double fat           = atof(strsep(&line, "~"));
        double protein       = atof(strsep(&line, "~"));
        double servingSize   = atof(strsep(&line, "~"));
        char* servingUnits   = strsep(&line, "~");
        double householdServingSize = atof(strsep(&line, "~"));
        char* householdServingUnits = strsep(&line, "~");

        node->food = newFood(number,
                             name,
                             manufacturer,
                             calories,
                             carbohydrates,
                             fat,
                             protein,
                             servingSize,
                             servingUnits,
                             householdServingSize,
                             householdServingUnits);
        node->next = malloc(sizeof(List));

        node = node->next;
        node->next = NULL;

        line = readLine(file);
    }

    return head;
}

int populateDatabase(char* csvFilename)
{
    FILE* file = fopen(csvFilename, "r");
    if (file == NULL)
    {
        printf("CSV file \"%s\" not found.\n", csvFilename);
        return 0;
    }

    List* list = createList(file);

    fclose(file);

    List* node = list;
    while (node != NULL)
    {
        Node* rbNode = malloc(sizeof(Node));
        rbNode->food = node->food;
        rbInsert(numberTree, rbNode);
        node = node->next;
    }

    free(list);
    return 1;
}

void destroyDatabase()
{
    rbDeleteTree(numberTree);
}

void findNumber(long number)
{
    printEntryNumber(rbSearchLong(numberTree, number));
}

#endif //FOOD_TRACKER_DATABASE_H
