#include "board.hpp"

#include <random>
#include <ncurses.h>

using namespace std;

Cell::Cell(Board &board, int row, int col, bool isBomb)
    : board(board),
      vicinityBombs{0},
      isBomb{isBomb},
      isExposed{false},
      isGuess{false},
      row{row},
      col{col}
{
}

void Cell::reveal()
{
    if (!isExposed) {
        isExposed = true;
        board.discovered++;
    }
}

void Cell::toggleGuess() {
    isGuess = !isGuess;
    board.guessed += isGuess ? 1 : -1;
}


CellState Cell::state()
{
    if (isGuess) return CellState::GUESS;
    if (!isExposed) return CellState::UNKNOWN;
    if (isBomb) return CellState::BOMB;
    if (vicinityBombs > 0) return CellState::VICINITY;
    return CellState::LAND;
}

int Cell::toChar()
{
    if (isGuess) return '?';
    if (!isExposed) return '#';
    if (isBomb) return '*';
    if (vicinityBombs > 0) return 0x30 + (vicinityBombs % 10);
    return '.';
}

/**
 * Traverse neighbours of a given cell.
 */
void Cell::neighbours(std::function<const void(Cell &cell)> callback)
{
    for (const int *d : ::neighbours) {
        int row = this->row + d[0], col = this->col + d[1];
        if (row >= board.nRows || row < 0) continue;
        if (col >= board.nCols || col < 0) continue;
        callback(board.cells[row][col]);
    }
}

Board::Board(int rows, int cols, int bombs)
    : nBombs{bombs}, nRows{rows}, nCols{cols}
{
    initialize();
    shuffleBombs();
    vicinityBombs();
    discovered = 0;
    guessed = 0;
}

Cell& Board::cell(int row, int col) {
    return cells[row][col];
}

int Board::remaining()
{
    return nRows * nCols - (discovered + guessed);
}

/**
 * Utility method to traverse the whole board cell by cell.
 */
void Board::traverse(std::function<const void(Cell &cell)> callback)
{
    for (int row = 0; row < nRows; row++)
        for (int col = 0; col < nCols; col++)
            callback(cells[row][col]);
}

/**
 * Reveal all the board.
 * Useful at the end of a game, or when a bomb is found.
 * @return Remaining undiscovered cells
 */
int Board::reveal()
{
    traverse([](Cell &cell) { cell.reveal(); });
    return remaining();
}

/**
 * Reveal a single cell on the board.
 * @return Remaining undiscovered cells
 */
int Board::reveal(Cell &cell)
{
    if (cell.isBomb) {
        reveal();
        return -1;  // Found a bomb :(
    };
    if (!cell.isExposed && !cell.isGuess)
        expandBlank(cell);
    return remaining();
}

/**
 * Make a guess on the board.
 * The undiscovered cell is marked as a guess for bomb.
 * @return Remaining undiscovered cells
 */
int Board::guess(Cell &cell)
{
    if (!cell.isExposed)
        cell.toggleGuess();
    return remaining();
}

/**
 * Initialize board by allocating cells
 */
void Board::initialize()
{
    cells.clear();
    int bombs = nBombs;
    for (int row = 0; row < nRows; row++) {
        cells.push_back(vector<Cell>());
        for (int col = 0; col < nCols; col++) {
            cells[row].push_back(Cell(*this, row, col, bombs-- > 0));
        }
    }
}

/**
 * Expand the land upon vicinity of bombs.
 */
void Board::expandBlank(Cell &cell)
{
    cell.neighbours([&](Cell &neighbour) {
        if (!neighbour.isExposed) {
            neighbour.reveal();
            if (!neighbour.vicinityBombs) expandBlank(neighbour);
        }
    });
}

/**
 * Shuffle the whole board bombs.
 * To be called on a new board, before computing the vicinity.
 */
void Board::shuffleBombs()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> r(0, nRows - 1);
    std::uniform_int_distribution<> c(0, nCols - 1);

    traverse([&](Cell &cell) {
        int row = r(gen), col = c(gen);  // Pick a random cell
        // Swap bomb state
        bool is_bomb = cell.isBomb;
        cell.isBomb = cells[row][col].isBomb;
        cells[row][col].isBomb = is_bomb;
    });
}

/**
 * Number all cells according to their proximity with bombs.
 * To be executed once the board is initialized.
 */
void Board::vicinityBombs()
{
    // Reset vicinity number
    traverse([](Cell &cell) { cell.vicinityBombs = 0; });

    // Update vicinity number
    traverse([](Cell &cell) {
        if (!cell.isBomb) return;
        cell.neighbours([](Cell &cell) { cell.vicinityBombs++; });
    });
}
