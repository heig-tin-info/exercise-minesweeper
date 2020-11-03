#include <curses.h>
#include <stdlib.h>
#include <unistd.h>

#define ITERMAX 10000

int main(void)
{
    // initialize curses
    initscr();
    cbreak();
    noecho();

    clear();

    // Initialize triangle
    const int maxlines = LINES - 1;
    const int maxcols = COLS - 1;
    const int x[3] = {0, maxcols / 2, maxcols};
    const int y[3] = {0, maxlines, 0};
    mvaddch(y[0], x[0], '0');
    mvaddch(y[1], x[1], '1');
    mvaddch(y[2], x[2], '2');

    // Initialize yi,xi with random values
    int yi = rand() % maxlines;
    int xi = rand() % maxcols;
    mvaddch(yi, xi, '.');

    // Iterate the triangle 
    for (int iter = 0; iter < ITERMAX; iter++) {
        int index = rand() % 3;

        yi = (yi + y[index]) / 2;
        xi = (xi + x[index]) / 2;

        mvaddch(yi, xi, '*');
        refresh();
        usleep(100);
    }

    // Cleaning
    mvaddstr(maxlines, 0, "Press any key to quit");
    refresh();
    getch();
    endwin();
}
