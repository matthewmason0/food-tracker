//
// Created by matthew on 3/13/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <ncurses.h>
#include "database.h"

int main(int argc, char** argv)
{
    if (!populateDatabase("food_nutrient_db.csv"))
        return 1;

    char list[3][18] = { "Search for a food", "Load user file", "Save user file" };
    char item[18];
    int i = 0;
    bool fileLoaded = false;

    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_WHITE);

    WINDOW* main = newwin(getmaxy(stdscr) - 1, getmaxx(stdscr) - 3, 1, 2);

    wbkgd(main, COLOR_PAIR(1));

    wattron(main, A_BOLD);
    mvwprintw(main, 0, 0, "Food Tracker v0.1");
    wattroff(main, A_BOLD);

    wattron(main, A_STANDOUT);
    wattron(main, COLOR_PAIR(2));
    sprintf(item, "%-18s", list[0]);
    mvwprintw(main, 2, 0, "%s", item);
    wattroff(main, A_STANDOUT);
    wattroff(main, COLOR_PAIR(2));

    sprintf(item, "%-18s", list[1]);
    mvwprintw(main, 3, 0, "%s", item);

    wattron(main, A_DIM);
    sprintf(item, "%-18s", list[2]);
    mvwprintw(main, 4, 0, "%s", item);
    wattroff(main, A_DIM);

    wrefresh(main); // update the terminal screen

    i = 0;
    noecho();
    keypad(main, TRUE);
    curs_set(0);

    // get the input
    int ch = wgetch(main);
    while (ch != 10)
    {
        if (ch == 'q')
        {
            endwin();
            return 0;
        }

        // right pad with spaces to make the items appear with even width.
        sprintf(item, "%-18s",  list[i]);
        mvwprintw(main, i+2, 0, "%s", item);
        // use a variable to increment or decrement the value based on the input.
        switch (ch)
        {
        case KEY_UP:
            i -= (i == 0) ? 0 : 1;
            break;
        case KEY_DOWN:
            if (fileLoaded)
                i += (i == 2) ? 0 : 1;
            else
                i += (i == 1) ? 0 : 1;
            break;
        }

        wattron(main, A_STANDOUT);
        wattron(main, COLOR_PAIR(2));
        sprintf(item, "%-18s",  list[i]);
        mvwprintw(main, i+2, 0, "%s", item);
        wattroff(main, A_STANDOUT);
        wattroff(main, COLOR_PAIR(2));

        ch = wgetch(main);
    }

    if (i == 0) // Search for a food
    {
        wclear(main);
        wattron(main, A_BOLD);
        mvwprintw(main, 0, 0, "Food Tracker v0.1");
        wattroff(main, A_BOLD);
        mvwprintw(main, 2, 0, "Use tab to change search type, up/down to highlight results, and enter to select.");
        wattron(main, A_STANDOUT);
        wattron(main, COLOR_PAIR(2));
        mvwprintw(main, 5, 0, "Name");
        wattroff(main, A_STANDOUT);
        wattroff(main, COLOR_PAIR(2));
        wrefresh(main);

        WINDOW* queryWin = newwin(3, 50, 7, 2);
        box(queryWin, 0, 0);
        WINDOW* resultsWin = newwin(12, 50, 9, 2);
        box(resultsWin, 0, 0);
        wrefresh(queryWin);
        wrefresh(resultsWin);
        wmove(queryWin, 1, 1);
        curs_set(1);
        echo();
        keypad(queryWin, TRUE);

        i = 0;
        char query[100];
        ch = wgetch(queryWin);
        while (ch != 10)
        {
            if (ch == 'q')
            {
                endwin();
                return 0;
            }
            if (!isprint(ch))
            {
                switch (ch)
                {
                case KEY_BACKSPACE:
                    if (i > 0)
                    {
                        i--;
                        query[i] = '\0';
                    }
                    if (i == 0)
                    {
                        wclear(resultsWin);
                        box(resultsWin, 0, 0);
                        wrefresh(resultsWin);
                        wmove(queryWin, 1, 1);
                        wrefresh(queryWin);
                        ch = wgetch(queryWin);
                        continue;
                    }
                    break;
                case KEY_STAB:
                    // toggle search type focus
                    break;
                case KEY_UP:
                    // move selected result up
                    break;
                case KEY_DOWN:
                    // move down
                    break;
                default:
                    continue;
                }
            }
            else
            {
                query[i] = (char) ch;
                query[i + 1] = '\0';
                i++;
            }

            wclear(resultsWin);
            box(resultsWin, 0, 0);
            Node** results = rbSearchString(nameTree, query, 10);
            for (int j = 0; j < 10 && results[j] != NIL; j++)
                mvwprintw(resultsWin, j+1, 1, results[j]->food->name);
            free(results);

            wrefresh(resultsWin);
            wrefresh(queryWin);

            ch = wgetch(queryWin);
        }
    }


/*
    clock_t t;
    t = clock();
    if (!populateDatabase("food_nutrient_db.csv"))
        return 1;
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Tree creation took %fs\n", time_taken);

    t = clock();
//    findNumber(45001534);
//    findName(argv[1]);
    findManufacturer("International Commodity Distributors, Inc.");
    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC*1000000;
    printf("Search took %0.fus\n", time_taken);

//    char trash[5];
//    scanf("%s", trash);

    destroyDatabase();
*/
    endwin();
    return 0;
}