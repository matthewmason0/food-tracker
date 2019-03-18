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

Tree* rbTree;

void printEntryNumber(Entry* node)
{
    printf("Number: %ld, Name: %s\n", node->number, node->name);
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

int populateDatabase(char* csvFilename)
{
    FILE* file = fopen(csvFilename, "r");
    if (file == NULL)
    {
        printf("CSV file \"%s\" not found.\n", csvFilename);
        return 0;
    }

    rbTree = rbNewTree();

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

        Entry* entry = newEntry(number,
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

        rbInsert(rbTree, entry);

        line = readLine(file);
    }
    fclose(file);
    return 1;
}

void destroyDatabase()
{
    rbDeleteTree(rbTree);
}

#endif //FOOD_TRACKER_DATABASE_H
