//
// Created by matthew on 3/13/19.
//

#include <stdio.h>
#include <stdlib.h>
#include "database.h"

int main()
{
    if (!populateDatabase("food_nutrient_db.csv"))
        return 1;

    char trash[5];
    scanf("%s", trash);

    destroyDatabase();
    return 0;
}