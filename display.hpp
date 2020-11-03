#pragma once
#include "board.hpp"

#include <curses.h>
#include <iostream>

struct Mouse {
    int col, row;      // Position
    bool left, right;  // Clicks
};

class Display
{
    int offsetX, offsetY;
    std::string title;

public:
    void init(std::string title);

    void initColors();
    Mouse waitMouseClick(Board &board);
    void board(Board &board);

    ~Display();
};
