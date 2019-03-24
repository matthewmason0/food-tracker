//
// Created by matthew on 3/17/19.
//

#ifndef FOOD_TRACKER_FOOD_H
#define FOOD_TRACKER_FOOD_H

#include <stdlib.h>
#include <string.h>

typedef enum key
{
    NUMBER,
    NAME,
    MANUFACTURER,
    CALORIES,
    CARBOHYDRATES,
    FAT,
    PROTEIN,
    SERVING_SIZE,
    SERVING_UNITS,
    HOUSEHOLD_SERVING_SIZE,
    HOUSEHOLD_SERVING_UNITS
} Key;

typedef struct food
{
    long number;
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
} Food;

typedef enum color { RED, BLACK } Color;

typedef struct node
{
    // rb tree fields
    Color color;
    struct node* left;
    struct node* right;
    struct node* parent;
    // satellite data
    Food* food;
} Node;

typedef struct tree { Node* root; Key key; } Tree;

Food* newFood(long number,
              char *name,
              char *manufacturer,
              double calories,
              double carbohydrates,
              double fat,
              double protein,
              double servingSize,
              char *servingUnits,
              double householdServingSize,
              char *householdServingUnits)
{
    Food* food = malloc(sizeof(Food));
    food->number = number;
    food->name = malloc(strlen(name) + 1);
    strcpy(food->name, name);
    food->manufacturer = malloc(strlen(manufacturer) + 1);
    strcpy(food->manufacturer, manufacturer);
    food->calories = calories;
    food->carbohydrates = carbohydrates;
    food->fat = fat;
    food->protein = protein;
    food->servingSize = servingSize;
    food->servingUnits = malloc(strlen(servingUnits) + 1);
    strcpy(food->servingUnits, servingUnits);
    food->householdServingSize = householdServingSize;
    food->householdServingUnits = malloc(strlen(householdServingUnits) + 1);
    strcpy(food->householdServingUnits, householdServingUnits);
    return food;
}

void deleteFood(Food *food)
{
    free(food->name);
    free(food->manufacturer);
    free(food->servingUnits);
    free(food->householdServingUnits);
    free(food);
}

Node* newNode(Food* food)
{
    Node* node = malloc(sizeof(Node));
    node->food = food;
    return node;
}

void deleteNode(Node* node)
{
    free(node);
}

void printNode(Node* node)
{
    printf("Number: %ld, Name: %s\n", node->food->number, node->food->name);
}

#endif //FOOD_TRACKER_FOOD_H
