//
// Created by matthew on 3/13/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "database.h"

int main()
{
    clock_t t;
    t = clock();
    if (!populateDatabase("food_nutrient_db.csv"))
        return 1;
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Tree creation took %fs\n", time_taken);

    t = clock();
    findNumber(45001563);
    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Search took %fs\n", time_taken);

    char trash[5];
    scanf("%s", trash);

    destroyDatabase();
    return 0;
}