#include "display.hpp"

#include <curses.h>
#include <locale.h>

#include <iostream>

void Display::init(std::string title)
{
    title = title;
    offsetX = 5;
    offsetY = 5;

    // Allow UTF-8 cchars
    setlocale(LC_ALL, "");

    // Init screen
    initscr();
    cbreak();
    noecho();
    initColors();

    // Enables keypad mode. This makes mouse events getting
    // reported as KEY_MOUSE, instead as of random letters.
    keypad(stdscr, TRUE);

    // Only report mouse clicks
    mousemask(BUTTON1_CLICKED | BUTTON3_CLICKED, NULL);

    // Hide cursor
    curs_set(0);

    // Display title
    mvhline(0, 0, ' ', COLS);
    printw(" %s", title.c_str());
    mvchgat(0, 0, -1, A_REVERSE, 0, 0);
}

void Display::initColors()
{
    if (!has_colors()) return;

    start_color();

    init_pair(CellState::LAND, COLOR_GREEN, COLOR_BLACK);
    init_pair(CellState::BOMB, COLOR_RED, COLOR_BLACK);
    init_pair(CellState::GUESS, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(CellState::VICINITY, COLOR_CYAN, COLOR_BLACK);
    init_pair(CellState::UNKNOWN, COLOR_WHITE, COLOR_BLACK);
}

Mouse Display::waitMouseClick(Board &board)
{
    MEVENT event;
    int row, col;
    do {
        while (wgetch(stdscr) != KEY_MOUSE || getmouse(&event) != OK) {
        }
        col = event.x - offsetX;
        row = event.y - offsetY;
    } while (row < 0 || row >= board.nRows || col < 0 || col >= board.nCols);
    return Mouse{col, row, static_cast<bool>(event.bstate & BUTTON1_CLICKED),
                 static_cast<bool>(event.bstate & BUTTON3_CLICKED)};
}

void Display::board(Board &board)
{
    board.traverse([&](Cell &cell) {
        attron(COLOR_PAIR(cell.state()));
        mvaddch(cell.row + offsetY, cell.col + offsetX, cell.toChar());
        attroff(COLOR_PAIR(cell.state()));
    });
    refresh();
}

Display::~Display()
{
    keypad(stdscr, FALSE);
    endwin();
}
