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

typedef enum control { QUIT, ERROR, MENU } Control;

typedef enum colors { NORMAL, HIGHLIGHTED_ACTIVE, HIGHLIGHTED_INACTIVE } Colors;

Control search(WINDOW* main);

int main(int argc, char** argv)
{
    initscr();
    noecho();
    curs_set(0);
    start_color();
    init_pair(NORMAL, COLOR_WHITE, COLOR_BLACK);
    init_pair(HIGHLIGHTED_ACTIVE, COLOR_WHITE, COLOR_BLUE);
    init_pair(HIGHLIGHTED_INACTIVE, COLOR_BLACK, COLOR_WHITE);

    int row, col;
    getmaxyx(stdscr, row, col);
    mvwaddstr(stdscr, row / 2, (col - strlen("Loading...")) / 2, "Loading...");
    wrefresh(stdscr);

    if (!populateDatabase("food_nutrient_db.csv"))
    {
        endwin();
        printf("CSV file \"food_nutrient_db.csv\" not found.\n");
        return ERROR;
    }

    wclear(stdscr);
    wattron(stdscr, A_BOLD);
    mvwprintw(stdscr, 1, 2, "Food Tracker v0.1");
    wattroff(stdscr, A_BOLD);
    wrefresh(stdscr);

    WINDOW* main = newwin(getmaxy(stdscr) - 2, getmaxx(stdscr) - 4, 3, 2);
    keypad(main, TRUE);

    wbkgd(main, COLOR_PAIR(NORMAL));

    char list[3][18] = { "Search for a food", "Load user file", "Save user file" };
    int i = 0;
    bool fileLoaded = false;

    while (1) // main menu
    {
        wattron(main, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
        mvwaddstr(main, 0, 0, list[0]);
        wattroff(main, COLOR_PAIR(HIGHLIGHTED_ACTIVE));

        mvwaddstr(main, 1, 0, list[1]);

        if (!fileLoaded)
            wattron(main, A_DIM);
        mvwaddstr(main, 2, 0, list[2]);
        wattroff(main, A_DIM);

        wrefresh(main);

        i = 0;

        int ch = wgetch(main);
        while (ch != 10)
        {
            if (ch == 'q')
            {
                endwin();
                return 0;
            }

            // right pad with spaces to make the items appear with even width.
            mvwaddstr(main, i, 0, list[i]);
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

            wattron(main, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
            mvwaddstr(main, i, 0, list[i]);
            wattroff(main, COLOR_PAIR(HIGHLIGHTED_ACTIVE));

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
    typedef enum type { NAME, MANUFACTURER, UPC, NDB_NUMBER } Type;
    wclear(main);
    mvwprintw(main, 0, 0, "Use TAB to change search type, UP/DOWN to highlight results, and ENTER to select. Press ESC to return to menu.");
    char types[4][20] = { "Name", "Manufacturer", "UPC", "NDB Number" };
    Type selectedType = 0;
    wmove(main, 3, 0);
    for (int type = 0; type < 4; type++)
    {
        if (type == selectedType)
            wattron(main, COLOR_PAIR(HIGHLIGHTED_INACTIVE));
        waddstr(main, types[type]);
        wattroff(main, COLOR_PAIR(HIGHLIGHTED_INACTIVE));
        waddstr(main, "  ");
    }
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
    char query[getmaxx(queryWin)];
    int highlightedItem = 0;
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
                mvwaddch(queryWin, y, x - 1, ' ');
                wmove(queryWin, y, x - 1);
                i--;
                query[i] = '\0';
            }
            if (i == 0)
            {
                wclear(resultsWin);
                box(resultsWin, 0, 0);
                wmove(queryWin, 1, 1);
            }
            break;
        case '\t':
            selectedType = (selectedType + 1) % 4;
            wmove(main, 3, 0);
            for (int type = 0; type < 4; type++)
            {
                if (type == selectedType)
                    wattron(main, COLOR_PAIR(HIGHLIGHTED_INACTIVE));
                waddstr(main, types[type]);
                wattroff(main, COLOR_PAIR(HIGHLIGHTED_INACTIVE));
                waddstr(main, "  ");
            }
            wrefresh(main);
            break;
        case KEY_UP:
            if (highlightedItem != 0)
                highlightedItem--;
            break;
        case KEY_DOWN:
            if (highlightedItem != 9)
                highlightedItem++;
            break;
        case 27: // ESC
            return MENU;
        case 10: // ENTER
            break;
        default:
            if (isprint(ch))
            {
                waddch(queryWin, ch);
                query[i] = (char) ch;
                query[i + 1] = '\0';
                i++;
            }
            break;
        }

        if (i > 0) // print results
        {
            wclear(resultsWin);
            box(resultsWin, 0, 0);
            Node** results;
            switch (selectedType)
            {
            case NAME:
                results = rbSearchString(nameTree, query, 10);
                break;
            case MANUFACTURER:
                results = rbSearchString(manufacturerTree, query, 10);
                break;
            case UPC:
                results = rbSearchString(nameTree, query, 10);
                break;
            case NDB_NUMBER:
                results = rbSearchString(numberTree, query, 10);
                break;
            }
            for (int item = 0; item < 10 && results[item] != NIL; item++)
            {
                if (item == highlightedItem)
                    wattron(resultsWin, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
                mvwprintw(resultsWin, item + 1, 1, results[item]->food->name);
                wattroff(resultsWin, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
            }
            free(results);
        }
        else
            highlightedItem = 0;

        wrefresh(resultsWin);
        wrefresh(queryWin);

        ch = wgetch(queryWin);
    }
    return QUIT;
}