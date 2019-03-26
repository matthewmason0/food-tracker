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

typedef enum control {QUIT, ERROR, MENU} Control;

Control search(WINDOW* main);

int main(int argc, char** argv)
{
    initscr();
    noecho();
    curs_set(0);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_WHITE);

    WINDOW* main = newwin(getmaxy(stdscr) - 2, getmaxx(stdscr) - 4, 1, 2);

    wbkgd(main, COLOR_PAIR(1));

    int row, col;
    getmaxyx(stdscr, row, col);
    mvaddstr(row / 2, (col - strlen("Loading...")) / 2, "Loading...");
    refresh();

    if (!populateDatabase("food_nutrient_db.csv"))
    {
        endwin();
        printf("CSV file \"food_nutrient_db.csv\" not found.\n");
        return ERROR;
    }

    char list[3][18] = { "Search for a food", "Load user file", "Save user file" };
    char item[18];
    int i = 0;
    bool fileLoaded = false;

    while (1) // main menu
    {
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

        if (!fileLoaded)
            wattron(main, A_DIM);
        sprintf(item, "%-18s", list[2]);
        mvwprintw(main, 4, 0, "%s", item);
        wattroff(main, A_DIM);

        wrefresh(main); // update the terminal screen

        i = 0;
        keypad(main, TRUE);

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
           switch (search(main))
           {
           case QUIT:
               endwin();
               return QUIT;
           case ERROR:
               endwin();
               return ERROR;
           case MENU:
               curs_set(0);
               wclear(main);
               wrefresh(main);
               break;
           }
        }
    }
}

Control search(WINDOW* main)
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

    WINDOW* queryWin = newwin(3, getmaxx(main), 7, 2);
    box(queryWin, 0, 0);
    WINDOW* resultsWin = newwin(12, getmaxx(main), 9, 2);
    box(resultsWin, 0, 0);
    wrefresh(queryWin);
    wrefresh(resultsWin);
    wmove(queryWin, 1, 1);
    curs_set(1);
    keypad(queryWin, TRUE);

    int i = 0;
    char query[100];
    int ch = wgetch(queryWin);
    while (ch != 'q')
    {
        switch (ch)
        {
        case KEY_BACKSPACE:
            if (i > 0)
            {
                int y, x;
                getyx(queryWin, y, x);
                wmove(queryWin, y, x - 1);
                wdelch(queryWin);
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
        case '\t':
            wprintw(queryWin, "Tab received");
            break;
        case KEY_UP:
            // move selected result up
            break;
        case KEY_DOWN:
            // move down
            break;
        case 27: // ESC
            return MENU;
        case 10: // ENTER
            break;
        default:
            waddch(queryWin, ch);
            query[i] = (char) ch;
            query[i + 1] = '\0';
            i++;
            break;
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
    return QUIT;
}