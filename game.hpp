#pragma once

#include "board.hpp"
#include "display.hpp"

enum GameState { IDLE, PLAYING, WON, LOST };

class Game
{
    Board board;
    GameState state;
    Display display;

public:
    Game(int bombs);
    Game(int rows, int cols, int bombs);
    GameState start();
};
