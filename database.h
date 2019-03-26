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

    numberTree = rbNewTree(NUMBER);
    nameTree = rbNewTree(NAME);
    manufacturerTree = rbNewTree(MANUFACTURER);
    caloriesTree = rbNewTree(CALORIES);
    carbohydratesTree = rbNewTree(CARBOHYDRATES);
    fatTree = rbNewTree(FAT);
    proteinTree = rbNewTree(PROTEIN);
    servingSizeTree = rbNewTree(SERVING_SIZE);
    servingUnitsTree = rbNewTree(SERVING_UNITS);
    householdServingSizeTree = rbNewTree(HOUSEHOLD_SERVING_SIZE);
    householdServingUnitsTree = rbNewTree(HOUSEHOLD_SERVING_UNITS);

//    int num = 1;
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

        Food* foodItem = newFood(number,
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

        rbInsert(numberTree, newNode(foodItem));
        rbInsert(nameTree, newNode(foodItem));
        rbInsert(manufacturerTree, newNode(foodItem));
//        rbInsert(caloriesTree, newNode(foodItem));
//        rbInsert(carbohydratesTree, newNode(foodItem));
//        rbInsert(fatTree, newNode(foodItem));
//        rbInsert(proteinTree, newNode(foodItem));
//        rbInsert(servingSizeTree, newNode(foodItem));
//        rbInsert(servingUnitsTree, newNode(foodItem));
//        rbInsert(householdServingSizeTree, newNode(foodItem));
//        rbInsert(householdServingUnitsTree, newNode(foodItem));

//        printf("Nodes inserted: %d\n", num++);

        line = readLine(file);
    }
    fclose(file);
    return 1;
}

void destroyDatabase()
{
    rbDeleteTree(numberTree, false);
    rbDeleteTree(nameTree, false);
    rbDeleteTree(manufacturerTree, true);
//    rbDeleteTree(caloriesTree, false);
//    rbDeleteTree(carbohydratesTree, false);
//    rbDeleteTree(fatTree, false);
//    rbDeleteTree(proteinTree, false);
//    rbDeleteTree(servingSizeTree, false);
//    rbDeleteTree(servingUnitsTree, false);
//    rbDeleteTree(householdServingSizeTree, false);
//    rbDeleteTree(householdServingUnitsTree, true);

    free(NIL);
}

void findNumber(long number)
{
    Node** results = rbSearchLong(numberTree, number, 10);
    for (int i = 0; i < 10 && results[i] != NIL; i++)
        printNode(results[i]);
    free(results);
}

void findName(char* name)
{
    Node** results = rbSearchString(nameTree, name, 10);
    for (int i = 0; i < 10 && results[i] != NIL; i++)
        printNode(results[i]);
    free(results);
}
void findManufacturer(char* manufacturer)
{
    Node** results = rbSearchString(manufacturerTree, manufacturer, 10);
    for (int i = 0; i < 10 && results[i] != NIL; i++)
        printNode(results[i]);
    free(results);
}

#endif //FOOD_TRACKER_DATABASE_H
