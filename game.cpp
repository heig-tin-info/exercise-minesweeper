#include "game.hpp"
#include "ncurses.h"

Game::Game(int rows, int cols, int bombs)
    : board{rows, cols, bombs}, state{GameState::IDLE}
{
    display.init("Labo 03 - Minesweeper");
}

GameState Game::start()
{
    state = GameState::PLAYING;

    int remaining = 1;
    do {
        display.board(board);
        Mouse m = display.waitMouseClick(board);
        Cell &cell = board.cell(m.row, m.col);
        if (m.right) remaining = board.guess(cell);
        if (m.left) remaining = board.reveal(cell);
    } while (remaining > 0);

    display.board(board);
    return remaining < 0 ? GameState::LOST : GameState::WON;
}
