#include <iostream>

#include "game.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    int rows = 10, cols = 10, bombs = 20;
    if (argc > 1 && sscanf(argv[1], "%d", &rows));
    if (argc > 2 && sscanf(argv[2], "%d", &cols));
    if (argc > 3 && sscanf(argv[3], "%d", &bombs));

    Game game(rows, cols, bombs);

    game.start(); // Blocking

    getchar();
}
