//
// Created by matthew on 3/17/19.
//

#ifndef FOOD_TRACKER_ENTRIES_H
#define FOOD_TRACKER_ENTRIES_H

#include <stdlib.h>
#include <string.h>

typedef enum color { RED, BLACK } Color;

typedef struct entry
{
    // rb tree fields
    Color color;
    struct entry* left;
    struct entry* right;
    struct entry* parent;
    // data fields
    long number; // key
    char* name;
    char* manufacturer;
    double calories;
    double carbohydrates;
    double fat;
    double protein;
    double servingSize;
    char* servingUnits;
    double householdServingSize;
    char* householdServingUnits;
} Entry;

typedef struct tree { Entry* root; } Tree;

Entry* newEntry(long number,
                char* name,
                char* manufacturer,
                double calories,
                double carbohydrates,
                double fat,
                double protein,
                double servingSize,
                char* servingUnits,
                double householdServingSize,
                char* householdServingUnits)
{
    Entry* entry = malloc(sizeof(Entry));
    entry->number = number;
    entry->name = malloc(sizeof(char) * strlen(name));
    strcpy(entry->name, name);
    entry->manufacturer = malloc(sizeof(char) * strlen(manufacturer));
    strcpy(entry->manufacturer, manufacturer);
    entry->calories = calories;
    entry->carbohydrates = carbohydrates;
    entry->fat = fat;
    entry->protein = protein;
    entry->servingSize = servingSize;
    entry->servingUnits = malloc(sizeof(char) * strlen(servingUnits));
    strcpy(entry->servingUnits, servingUnits);
    entry->householdServingSize = householdServingSize;
    entry->householdServingUnits = malloc(sizeof(char) * strlen(householdServingUnits));
    strcpy(entry->householdServingUnits, householdServingUnits);
    return entry;
}

void deleteEntry(Entry* entry)
{
    free(entry->name);
    free(entry->manufacturer);
    free(entry->servingUnits);
    free(entry->householdServingUnits);
    free(entry);
}

#endif //FOOD_TRACKER_ENTRIES_H
