#pragma once

#include <functional>
#include <vector>

static int constexpr neighbours[][2] = {{-1, -1}, {-1, 0}, {-1, 1},
                                        {0, -1},  {0, 1},  {0, 0},
                                        {1, -1},  {1, 0},  {1, 1}};

enum CellState { UNKNOWN = 0, LAND, BOMB, GUESS, VICINITY };

class Board;  // Forward

class Cell
{
    friend Board;  // Allow the board to access all cells

    Board &board;

    int vicinityBombs;
    bool isBomb;
    bool isExposed;
    bool isGuess;

public:
    const int row;
    const int col;

    Cell(Board &board, int row, int col, bool isBomb = false);
    void reveal();
    void toggleGuess();

    CellState state();
    int toChar();

    void neighbours(std::function<const void(Cell &cell)> callback);
};

class Board
{
    friend Cell;  // Allow the board to access all cells

    std::vector<std::vector<Cell>> cells;

    void initialize();
    void expandBlank(Cell &cell);
    void shuffleBombs();
    void vicinityBombs();
    int remaining();

protected:
    int nBombs;
    int discovered, guessed;  // How many cells

public:
    int nRows, nCols;  // Board size
    Board(int rows, int cols, int bombs);

    Cell& cell(int row, int col);

    int reveal();
    int reveal(Cell &cell); /* Typically left click */
    int guess(Cell &cell); /* Typically right click */
    void traverse(std::function<const void(Cell &cell)>callback);
};
