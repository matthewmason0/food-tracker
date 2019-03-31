//
// Created by matthew on 3/13/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>
#include "database.h"

typedef enum control { QUIT, ERROR, MENU, SEARCH } Control;

typedef enum colors { NORMAL, HIGHLIGHTED_ACTIVE, HIGHLIGHTED_INACTIVE, BAD, GOOD } Colors;

typedef enum type { T_NAME, T_MANUFACTURER, T_UPC, T_NDB } Type;

FILE* userFile = NULL;
bool fileLoaded = false;
char* userName;

Control search(WINDOW* main);
Control createFile(WINDOW* main);
Control loadFile(WINDOW* main);
Control searchUserFile(WINDOW* main);

Control selectResult(WINDOW* main, char* query, int type, int selection);
Control selectUserResult(WINDOW* main, char* query, int type, int selection);
char* addEllipses(char* string, int length, char* workingString);

int main(int argc, char** argv)
{
/*
    clock_t t;
    t = clock();
    if (!populateDatabase("food_database.csv"))
        return 1;
    t = clock() - t;
    double time_taken = ((double) t) / CLOCKS_PER_SEC;
    printf("Tree creation took %fs\n", time_taken);
    t = clock();

//    findNumber(45001534);
//    findName(argv[1]);
//    findManufacturer("International Commodity Distributors, Inc.");
    Node** results = NULL;
    results = rbSearchString(databaseTrees[NAME], argv[1], 10);
    if (results == NULL)
        return ERROR;
    char temp[12];
    for (int item = 0; item < 10 && results[item] != NIL; item++)
        printf("%s\n", addEllipses(results[item]->food->name, 10, &temp));
    free(results);

    t = clock() - t;
    time_taken = ((double) t) / CLOCKS_PER_SEC * 1000000;
    printf("Search took %0.fus\n", time_taken);
//    char trash[5];
//    scanf("%s", trash);
    destroyDatabase();
*/

    initscr();
    noecho();
    curs_set(0);
    start_color();
    init_pair(NORMAL, COLOR_WHITE, COLOR_BLACK);
    init_pair(HIGHLIGHTED_ACTIVE, COLOR_WHITE, COLOR_BLUE);
    init_pair(HIGHLIGHTED_INACTIVE, COLOR_BLACK, COLOR_WHITE);
    init_pair(BAD, COLOR_RED, COLOR_BLACK);
    init_pair(GOOD, COLOR_GREEN, COLOR_BLACK);

    int row, col;
    getmaxyx(stdscr, row, col);
    char loading[] = "Loading...";
    char tip[] = "Tip: press \"q\" at any time to quit";
    mvwaddstr(stdscr, row / 2 - 2, (col - (int)strlen(loading)) / 2, loading);
    wattron(stdscr, A_BOLD);
    mvwaddstr(stdscr, row / 2, (col - (int)strlen(tip)) / 2, tip);
    wattroff(stdscr, A_BOLD);
    wrefresh(stdscr);

    if (!populateDatabase("food_database.csv"))
    {
        endwin();
        printf("CSV file \"food_database.csv\" not found.\n");
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

    const int menuOptions = 4;
    char list[4][18] = { "Search for a food", "Create user file", "Load user file", "Search user file" };
    int i = 0;

    wattron(main, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
    mvwaddstr(main, 0, 0, list[0]);
    wattroff(main, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
    mvwaddstr(main, 1, 0, list[1]);
    mvwaddstr(main, 2, 0, list[2]);
    wattron(main, A_DIM);
    mvwaddstr(main, 3, 0, list[3]);
    wattroff(main, A_DIM);

    wrefresh(main);

    i = 0;

    int ch = wgetch(main);
    while (ch != 'q')
    {
        switch (ch)
        {
        case KEY_UP:
            i -= (i == 0) ? 0 : 1;
            break;
        case KEY_DOWN:
            if (fileLoaded)
                i += (i == menuOptions - 1) ? 0 : 1;
            else
                i += (i == menuOptions - 2) ? 0 : 1;
            break;
        case 10: // ENTER
            {
                Control c = MENU;
                switch (i)
                {
                case 0: // Search for a food
                    c = search(main); break;
                case 1: // Create user file
                    c = createFile(main); break;
                case 2: // Load user file
                    c = loadFile(main); break;
                case 3: // Search user file
                    c = searchUserFile(main); break;
                }
                switch (c)
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
                    i = 0;
                    break;
                }
            }
        }

        for (int option = 0; option < menuOptions; option++)
        {
            if (option == i)
                wattron(main, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
            if (option == 3 && !fileLoaded)
                wattron(main, A_DIM);
            mvwaddstr(main, option, 0, list[option]);
            if (option == 3 && fileLoaded)
                wprintw(main, " | User: %s", userName);
            wattroff(main, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
            wattroff(main, A_DIM);
        }

        ch = wgetch(main);
    }

    endwin();
    if (fileLoaded)
    {
        fclose(userFile);
        free(userName);
    }
    return QUIT;
}

char* addEllipses(char* string, int length, char* workingString)
{
    size_t len = strlen(string);
    if (len > length)
    {
        strncpy(workingString, string, length);
        strncpy(workingString + length - 3, "...\0", 4);
        return workingString;
    }
    return string;
}

Control search(WINDOW* main)
{
    wclear(main);
    mvwprintw(main, 0, 0, "Use TAB to change search type, UP/DOWN to highlight results, and ENTER to select. Press ESC to return to menu.");
    char types[4][20] = { "Name", "Manufacturer", "UPC", "NDB No." };
    Type selectedType = T_NAME;
    wmove(main, 3, 0);
    for (Type type = T_NAME; type <= T_NDB; type++)
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
            selectedType = (selectedType + 1) % (T_NDB + 1);
            wmove(main, 3, 0);
            for (Type type = T_NAME; type <= T_NDB; type++)
            {
                if (type == selectedType)
                    wattron(main, COLOR_PAIR(HIGHLIGHTED_INACTIVE));
                waddstr(main, types[type]);
                wattroff(main, COLOR_PAIR(HIGHLIGHTED_INACTIVE));
                waddstr(main, "  ");
            }
            wrefresh(main);
            i = 0;
            query[0] = '\0';
            wclear(queryWin);
            wclear(resultsWin);
            box(queryWin, 0, 0);
            box(resultsWin, 0, 0);
            wrefresh(queryWin);
            wrefresh(resultsWin);
            wmove(queryWin, 1, 1);
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
            if (i > 0)
            {
                if (selectResult(main, query, selectedType, highlightedItem) == QUIT)
                    return QUIT;
                curs_set(1);
                wclear(main);
                mvwprintw(main, 0, 0, "Use TAB to change search type, UP/DOWN to highlight results, and ENTER to select. Press ESC to return to menu.");
                wmove(main, 3, 0);
                for (Type type = T_NAME; type <= T_NDB; type++)
                {
                    if (type == selectedType)
                        wattron(main, COLOR_PAIR(HIGHLIGHTED_INACTIVE));
                    waddstr(main, types[type]);
                    wattroff(main, COLOR_PAIR(HIGHLIGHTED_INACTIVE));
                    waddstr(main, "  ");
                }
                wrefresh(main);
                box(queryWin, 0, 0);
                box(resultsWin, 0, 0);
                wrefresh(queryWin);
                wrefresh(resultsWin);
            }
            break;
        default:
            switch (selectedType)
            {
            case T_NAME:
            case T_MANUFACTURER:
                if (i < getmaxx(queryWin) - 3 && isprint(ch))
                {
                    waddch(queryWin, ch);
                    query[i] = (char) ch;
                    query[i + 1] = '\0';
                    i++;
                }
                break;
            case T_UPC:
            case T_NDB:
                if (i < getmaxx(queryWin) - 3 && isdigit(ch))
                {
                    waddch(queryWin, ch);
                    query[i] = (char) ch;
                    query[i + 1] = '\0';
                    i++;
                }
                break;
            }
            break;
        }

        if (i > 0) // print results
        {
            wclear(resultsWin);
            box(resultsWin, 0, 0);
            Node** results = NULL;
            switch (selectedType)
            {
            case T_NAME:
                results = rbSearchString(databaseTrees[NAME], query, 10); break;
            case T_MANUFACTURER:
                results = rbSearchString(databaseTrees[MANUFACTURER], query, 10); break;
            case T_UPC:
                results = rbSearchString(databaseTrees[UPC], query, 10); break;
            case T_NDB:
                results = rbSearchString(databaseTrees[NUMBER], query, 10); break;
            }
            if (results == NULL)
                mvwprintw(resultsWin, 1, 1, "No results");
            else
            {
                int width = getmaxx(resultsWin) - 2;
                char* workingString = malloc(getmaxx(resultsWin));
                for (int item = 0; item < 10 && results[item] != NIL; item++)
                {
                    if (item == highlightedItem)
                        wattron(resultsWin, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
                    switch (selectedType)
                    {
                    case T_NAME:
                        mvwprintw(resultsWin, item + 1, 1, addEllipses(results[item]->food->name, width, workingString));
                        break;
                    case T_MANUFACTURER:
                        {
                            char temp[strlen(results[item]->food->manufacturer) + strlen(results[item]->food->name) + 4];
                            sprintf(temp, "%s | %s", results[item]->food->manufacturer, results[item]->food->name);
                            mvwprintw(resultsWin, item + 1, 1, addEllipses(temp, width, workingString));
                        }
                        break;
                    case T_UPC:
                        {
                            char temp[strlen(results[item]->food->upc) + strlen(results[item]->food->name) + 4];
                            sprintf(temp, "%s | %s", results[item]->food->upc, results[item]->food->name);
                            mvwprintw(resultsWin, item + 1, 1, addEllipses(temp, width, workingString));
                        }
                        break;
                    case T_NDB:
                        {
                            char temp[strlen(results[item]->food->number) + strlen(results[item]->food->name) + 4];
                            sprintf(temp, "%s | %s", results[item]->food->number, results[item]->food->name);
                            mvwprintw(resultsWin, item + 1, 1, addEllipses(temp, width, workingString));
                        }
                        break;
                    }
                    wattroff(resultsWin, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
                }
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

char* promptFilename(WINDOW* main)
{
    wclear(main);
    mvwaddstr(main, 0, 0, "Please enter your name: .log");
    curs_set(1);

    int i = 0;
    wmove(main, 0, i + 24);
    char name[getmaxx(main)];
    int ch = wgetch(main);
    while (ch != 10 || i == 0)
    {
        if (i + 25 < getmaxx(main) - 3 && isalpha(ch))
        {
            wprintw(main, "%c.log", ch);
            wmove(main, 0, i + 25);
            name[i] = (char) ch;
            name[i + 1] = '\0';
            i++;
        }
        else if (i > 0 && ch == KEY_BACKSPACE)
        {

            i--;
            mvwaddstr(main, 0, i + 24, ".log ");
            wmove(main, 0, i + 24);
            name[i] = '\0';
        }
        wrefresh(main);
        ch = wgetch(main);
    }
    if (fileLoaded)
        free(userName);
    userName = malloc(strlen(name) + 1);
    userName[0] = '\0';
    strcpy(userName, name);
    char* filename = malloc(strlen(name) + 5);
    filename[0] = '\0';
    sprintf(filename, "%s.log", name);
    return filename;
}

Control createFile(WINDOW* main)
{
    char* filename = promptFilename(main);
    curs_set(0);
    if (fileLoaded)
        fclose(userFile);
    fileLoaded = false;
    userFile = fopen(filename, "r");
    if (userFile != NULL)
    {
        wattron(main, COLOR_PAIR(BAD));
        mvwprintw(main, 2, 0, "The file \"%s\" already exists, overwrite? (y/n) ", filename);
        wattroff(main, COLOR_PAIR(BAD));
        curs_set(1);
        int ch = wgetch(main);
        while (1)
        {
            if (ch == 'y')
            {
                waddch(main, ch);
                curs_set(0);
                break;
            }
            else if (ch == 'n')
            {
                free(filename);
                return MENU;
            }
            ch = wgetch(main);
        }
    }
    userFile = fopen(filename, "w");
    if (userFile == NULL)
        return ERROR;
    fileLoaded = true;
    wattron(main, COLOR_PAIR(GOOD));
    mvwprintw(main, 4, 0, "User file \"%s\" created successfully!", filename);
    wattroff(main, COLOR_PAIR(GOOD));
    wrefresh(main);
    sleep(2);
    free(filename);
    return MENU;
}

Control loadFile(WINDOW* main)
{
    char* filename = promptFilename(main);
    curs_set(0);
    if (fileLoaded)
        fclose(userFile);
    fileLoaded = false;
    userFile = fopen(filename, "r");
    if (userFile == NULL)
    {
        wattron(main, COLOR_PAIR(BAD));
        mvwprintw(main, 2, 0, "Sorry, user file \"%s\" does not exist!", filename);
        wattroff(main, COLOR_PAIR(BAD));
    }
    else
    {
        userFile = fopen(filename, "a");
        if (userFile == NULL)
            return ERROR;
        fileLoaded = true;
        loadUserDatabase(userFile);
        wattron(main, COLOR_PAIR(GOOD));
        mvwprintw(main, 2, 0, "Welcome back, %s!", userName);
        wattroff(main, COLOR_PAIR(GOOD));
    }
    wrefresh(main);
    sleep(2);
    free(filename);
    return MENU;
}

Control selectResult(WINDOW* main, char* query, int type, int selected)
{
    Node** results = NULL;
    switch (type)
    {
    case T_NAME:
        results = rbSearchString(databaseTrees[NAME], query, 10);
        break;
    case T_MANUFACTURER:
        results = rbSearchString(databaseTrees[MANUFACTURER], query, 10);
        break;
    case T_UPC:
        results = rbSearchString(databaseTrees[UPC], query, 10);
        break;
    case T_NDB:
        results = rbSearchString(databaseTrees[NUMBER], query, 10);
        break;
    }
    if (results == NULL)
    {
        free(results);
        return SEARCH;
    }
    Food* result = results[selected]->food;
    free(results);
    curs_set(0);
    wclear(main);
    wattron(main, A_UNDERLINE | A_BOLD);
    mvwprintw(main, 0, 0, result->name);
    wattroff(main, A_UNDERLINE | A_BOLD);
    mvwprintw(main, 1, 0, result->manufacturer);
    mvwprintw(main, 2, 0, "NDB No. %s", result->number);
    // m means mL
    if (strcasecmp(result->servingUnits, "m") == 0)
        mvwprintw(main, 4, 0, "Serving Size:  %0.2lf mL,", result->servingSize);
    else
        mvwprintw(main, 4, 0, "Serving Size:  %0.2lf %s,", result->servingSize, result->servingUnits);
    // only display decimal values if needed
    if (result->householdServingSize - (int)(result->householdServingSize))
        mvwprintw(main, 5, 0, "               %0.2lf %s", result->householdServingSize, result->householdServingUnits);
    else
        mvwprintw(main, 5, 0, "               %0.lf %s", result->householdServingSize, result->householdServingUnits);
    mvwprintw(main, 6, 0, "Calories:      %0.2lf kcal", result->calories);
    mvwprintw(main, 7, 0, "Carbohydrates: %0.2lf g", result->carbohydrates);
    mvwprintw(main, 8, 0, "Fat:           %0.2lf g", result->fat);
    mvwprintw(main, 9, 0, "Protein:       %0.2lf g", result->protein);


    char list[2][18] = { "Back to search", "Save to user file" };
    int i = 0;

    wattron(main, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
    mvwaddstr(main, 11, 0, list[0]);
    wattroff(main, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
    if (!fileLoaded)
        wattron(main, A_DIM);
    mvwaddstr(main, 12, 0, list[1]);
    wattroff(main, A_DIM);
    if (fileLoaded)
        wprintw(main, " | User: %s", userName);

    wrefresh(main);

    i = 0;

    int ch = wgetch(main);
    while (ch != 'q')
    {
        switch (ch)
        {
        case KEY_UP:
            i -= (i == 0) ? 0 : 1;
            break;
        case KEY_DOWN:
            if (fileLoaded)
                i += (i == 1) ? 0 : 1;
            else
                i += (i == 0) ? 0 : 1;
            break;
        case 10: // ENTER
            if (i == 0) // Back to search
                return SEARCH;
            // Save to user file
            fprintf(userFile, "%s~%s~%s~%lf~%lf~%lf~%lf~%lf~%s~%lf~%s\n", result->number,
                                                                          result->name,
                                                                          result->manufacturer,
                                                                          result->calories,
                                                                          result->carbohydrates,
                                                                          result->fat,
                                                                          result->protein,
                                                                          result->servingSize,
                                                                          result->servingUnits,
                                                                          result->householdServingSize,
                                                                          result->householdServingUnits);
            wattron(main, COLOR_PAIR(GOOD));
            mvwprintw(main, 14, 0, "Food saved!");
            wattroff(main, COLOR_PAIR(GOOD));
            wrefresh(main);
            sleep(1);
            return SEARCH;
        }

        for (int option = 0; option < 2; option++)
        {
            if (option == i)
                wattron(main, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
            if (option == 1 && !fileLoaded)
                wattron(main, A_DIM);
            mvwaddstr(main, option + 11, 0, list[option]);
            if (option == 1 && fileLoaded)
                wprintw(main, " | User: %s", userName);
            wattroff(main, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
            wattroff(main, A_DIM);
        }

        ch = wgetch(main);
    }
    return QUIT;
}

Control searchUserFile(WINDOW* main)
{
    wclear(main);
    mvwprintw(main, 0, 0, "Use TAB to change search type, UP/DOWN to highlight results, and ENTER to select. Press ESC to return to menu.");
    char types[4][20] = { "Name", "Manufacturer", "UPC", "NDB No." };
    Type selectedType = T_NAME;
    wmove(main, 3, 0);
    for (Type type = T_NAME; type <= T_NDB; type++)
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
            selectedType = (selectedType + 1) % (T_NDB + 1);
            wmove(main, 3, 0);
            for (Type type = T_NAME; type <= T_NDB; type++)
            {
                if (type == selectedType)
                    wattron(main, COLOR_PAIR(HIGHLIGHTED_INACTIVE));
                waddstr(main, types[type]);
                wattroff(main, COLOR_PAIR(HIGHLIGHTED_INACTIVE));
                waddstr(main, "  ");
            }
            wrefresh(main);
            i = 0;
            query[0] = '\0';
            wclear(queryWin);
            wclear(resultsWin);
            box(queryWin, 0, 0);
            box(resultsWin, 0, 0);
            wrefresh(queryWin);
            wrefresh(resultsWin);
            wmove(queryWin, 1, 1);
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
            if (i > 0)
            {
                if (selectUserResult(main, query, selectedType, highlightedItem) == QUIT)
                    return QUIT;
                curs_set(1);
                wclear(main);
                mvwprintw(main, 0, 0, "Use TAB to change search type, UP/DOWN to highlight results, and ENTER to select. Press ESC to return to menu.");
                wmove(main, 3, 0);
                for (Type type = T_NAME; type <= T_NDB; type++)
                {
                    if (type == selectedType)
                        wattron(main, COLOR_PAIR(HIGHLIGHTED_INACTIVE));
                    waddstr(main, types[type]);
                    wattroff(main, COLOR_PAIR(HIGHLIGHTED_INACTIVE));
                    waddstr(main, "  ");
                }
                wrefresh(main);
                box(queryWin, 0, 0);
                box(resultsWin, 0, 0);
                wrefresh(queryWin);
                wrefresh(resultsWin);
            }
            break;
        default:
            switch (selectedType)
            {
            case T_NAME:
            case T_MANUFACTURER:
                if (i < getmaxx(queryWin) - 3 && isprint(ch))
                {
                    waddch(queryWin, ch);
                    query[i] = (char) ch;
                    query[i + 1] = '\0';
                    i++;
                }
                break;
            case T_UPC:
            case T_NDB:
                if (i < getmaxx(queryWin) - 3 && isdigit(ch))
                {
                    waddch(queryWin, ch);
                    query[i] = (char) ch;
                    query[i + 1] = '\0';
                    i++;
                }
                break;
            }
            break;
        }

        if (i > 0) // print results
        {
            wclear(resultsWin);
            box(resultsWin, 0, 0);
            Node** results = NULL;
            switch (selectedType)
            {
            case T_NAME:
                results = rbSearchString(userTrees[NAME], query, 10); break;
            case T_MANUFACTURER:
                results = rbSearchString(userTrees[MANUFACTURER], query, 10); break;
            case T_UPC:
                results = rbSearchString(userTrees[UPC], query, 10); break;
            case T_NDB:
                results = rbSearchString(userTrees[NUMBER], query, 10); break;
            }
            if (results == NULL)
                mvwprintw(resultsWin, 1, 1, "No results");
            else
            {
                int width = getmaxx(resultsWin) - 2;
                char* workingString = malloc(getmaxx(resultsWin));
                for (int item = 0; item < 10 && results[item] != NIL; item++)
                {
                    if (item == highlightedItem)
                        wattron(resultsWin, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
                    switch (selectedType)
                    {
                    case T_NAME:
                        mvwprintw(resultsWin, item + 1, 1, addEllipses(results[item]->food->name, width, workingString));
                        break;
                    case T_MANUFACTURER:
                        {
                            char temp[strlen(results[item]->food->manufacturer) + strlen(results[item]->food->name) + 4];
                            sprintf(temp, "%s | %s", results[item]->food->manufacturer, results[item]->food->name);
                            mvwprintw(resultsWin, item + 1, 1, addEllipses(temp, width, workingString));
                        }
                        break;
                    case T_UPC:
                        {
                            char temp[strlen(results[item]->food->upc) + strlen(results[item]->food->name) + 4];
                            sprintf(temp, "%s | %s", results[item]->food->upc, results[item]->food->name);
                            mvwprintw(resultsWin, item + 1, 1, addEllipses(temp, width, workingString));
                        }
                        break;
                    case T_NDB:
                        {
                            char temp[strlen(results[item]->food->number) + strlen(results[item]->food->name) + 4];
                            sprintf(temp, "%s | %s", results[item]->food->number, results[item]->food->name);
                            mvwprintw(resultsWin, item + 1, 1, addEllipses(temp, width, workingString));
                        }
                        break;
                    }
                    wattroff(resultsWin, COLOR_PAIR(HIGHLIGHTED_ACTIVE));
                }
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

Control selectUserResult(WINDOW* main, char* query, int type, int selected)
{

}