//
// Created by matthew on 3/17/19.
//

#ifndef FOOD_TRACKER_DATABASE_H
#define FOOD_TRACKER_DATABASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "food.h"
#include "rbTree.h"

Tree* databaseTrees[HOUSEHOLD_SERVING_UNITS + 1];
Tree* userTrees[HOUSEHOLD_SERVING_UNITS + 1];

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
    NIL = malloc(sizeof(Node));
    NIL->color = BLACK;

    FILE* file = fopen(csvFilename, "r");
    if (file == NULL)
        return 0;

    for (Key key = NUMBER; key <= MANUFACTURER; key++)
        databaseTrees[key] = rbNewTree(key);

    char* line = readLine(file);
    while(line[0] != '\0')
    {
        char* number         = strsep(&line, "~");
        char* name           = strsep(&line, "~");
        char* upc            = strsep(&line, "~");
        char* manufacturer   = strsep(&line, "~");
        double calories      = atof(strsep(&line, "~"));
        double carbohydrates = atof(strsep(&line, "~"));
        double fat           = atof(strsep(&line, "~"));
        double protein       = atof(strsep(&line, "~"));
        double servingSize   = atof(strsep(&line, "~"));
        char* servingUnits   = strsep(&line, "~");
        double householdServingSize = atof(strsep(&line, "~"));
        char* householdServingUnits = strsep(&line, "~");

        Food* foodItem = newFood(number,
                                 name,
                                 upc,
                                 manufacturer,
                                 calories,
                                 carbohydrates,
                                 fat,
                                 protein,
                                 servingSize,
                                 servingUnits,
                                 householdServingSize,
                                 householdServingUnits);

        for (Key key = NUMBER; key <= MANUFACTURER; key++)
            rbInsert(databaseTrees[key], newNode(foodItem));

        line = readLine(file);
    }
    fclose(file);
    return 1;
}

void destroyDatabase()
{
    for (Key key = NUMBER; key <= MANUFACTURER; key++)
        rbDeleteTree(databaseTrees[key], key == MANUFACTURER);
    free(NIL);
}

void findName(char* name)
{
    Node** results = rbSearchString(databaseTrees[UPC], name, 10);
    for (int i = 0; i < 10 && results[i] != NIL; i++)
        printNode(results[i]);
    free(results);
}
void findManufacturer(char* manufacturer)
{
    Node** results = rbSearchString(databaseTrees[MANUFACTURER], manufacturer, 10);
    for (int i = 0; i < 10 && results[i] != NIL; i++)
        printNode(results[i]);
    free(results);
}

int loadUserDatabase(FILE* userFile)
{
    for (Key key = NUMBER; key <= HOUSEHOLD_SERVING_UNITS; key++)
        userTrees[key] = rbNewTree(key);

    char* line = readLine(userFile);
    while(line[0] != '\0')
    {
        char* number         = strsep(&line, "~");
        char* name           = strsep(&line, "~");
        char* upc            = strsep(&line, "~");
        char* manufacturer   = strsep(&line, "~");
        double calories      = atof(strsep(&line, "~"));
        double carbohydrates = atof(strsep(&line, "~"));
        double fat           = atof(strsep(&line, "~"));
        double protein       = atof(strsep(&line, "~"));
        double servingSize   = atof(strsep(&line, "~"));
        char* servingUnits   = strsep(&line, "~");
        double householdServingSize = atof(strsep(&line, "~"));
        char* householdServingUnits = strsep(&line, "~");

        Food* foodItem = newFood(number,
                                 name,
                                 upc,
                                 manufacturer,
                                 calories,
                                 carbohydrates,
                                 fat,
                                 protein,
                                 servingSize,
                                 servingUnits,
                                 householdServingSize,
                                 householdServingUnits);

        for (Key key = NUMBER; key <= HOUSEHOLD_SERVING_UNITS; key++)
            rbInsert(userTrees[key], newNode(foodItem));

        line = readLine(userFile);
    }
}

#endif //FOOD_TRACKER_DATABASE_H
