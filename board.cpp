#include "board.hpp"
#include <iostream>
#include <string>

// Adapted from:
// https://courses.cs.washington.edu/courses/cse573/04au/Project/mini1/RUSSIA/Final_Paper.pdf
const int static_scores[64] = 
{
       4, -3,  2,  2,  2,  2, -3,  4, 
      -3, -4, -1, -1, -1, -1, -4, -3, 
       2, -1,  1,  0,  0,  1, -1,  2,
       2, -1,  0,  1,  1,  0, -1,  2,
       2, -1,  0,  1,  1,  0, -1,  2, 
       2, -1,  1,  0,  0,  1, -1,  2, 
      -3, -4, -1, -1, -1, -1, -4, -3, 
       4, -3,  2,  2,  2,  2, -3,  4
};

/*
 * Make a standard BOARDSIZExBOARDSIZE othello board and initialize it to the standard setup.
 */
Board::Board() {
    taken.set(3 + BOARDSIZE * 3);
    taken.set(3 + BOARDSIZE * 4);
    taken.set(4 + BOARDSIZE * 3);
    taken.set(4 + BOARDSIZE * 4);
    black.set(4 + BOARDSIZE * 3);
    black.set(3 + BOARDSIZE * 4);
}

/*
 * Destructor for the board.
 */
Board::~Board() {
}

/*
 * Returns a copy of this board.
 */
Board *Board::copy() {
    Board *newBoard = new Board();
    newBoard->black = black;
    newBoard->taken = taken;
    return newBoard;
}

bool Board::occupied(int x, int y) {
    return taken[x + BOARDSIZE*y];
}

bool Board::get(Side side, int x, int y) {
    return occupied(x, y) && (black[x + BOARDSIZE*y] == (side == BLACK));
}

void Board::set(Side side, int x, int y) {
    taken.set(x + BOARDSIZE*y);
    black.set(x + BOARDSIZE*y, side == BLACK);
}

bool Board::onBoard(int x, int y) {
    return(0 <= x && x < BOARDSIZE && 0 <= y && y < BOARDSIZE);
}


/*
 * Returns true if the game is finished; false otherwise. The game is finished
 * if neither side has a legal move.
 */
bool Board::isDone() {
    return !(hasMoves(BLACK) || hasMoves(WHITE));
}

/*
 * Returns true if there are legal moves for the given side.
 */
bool Board::hasMoves(Side side) {
    for (int i = 0; i < BOARDSIZE; i++) {
        for (int j = 0; j < BOARDSIZE; j++) {
            Move move(i, j);
            if (checkMove(&move, side)) return true;
        }
    }
    return false;
}

/*
 * Returns true if a move is legal for the given side; false otherwise.
 */
bool Board::checkMove(Move *m, Side side) {
    // Passing is only legal if you have no moves.
    if (m == nullptr) return !hasMoves(side);

    int X = m->getX();
    int Y = m->getY();

    // Make sure the square hasn't already been taken.
    if (occupied(X, Y)) return false;

    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            // Is there a capture in that direction?
            int x = X + dx;
            int y = Y + dy;
            if (onBoard(x, y) && get(other, x, y)) {
                do {
                    x += dx;
                    y += dy;
                } while (onBoard(x, y) && get(other, x, y));

                if (onBoard(x, y) && get(side, x, y)) return true;
            }
        }
    }
    return false;
}

/*
 * Modifies the board to reflect the specified move.
 */
void Board::doMove(Move *m, Side side) {
    // A nullptr move means pass.
    if (m == nullptr) return;

    // Ignore if move is invalid.
    if (!checkMove(m, side)) return;

    int X = m->getX();
    int Y = m->getY();
    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            int x = X;
            int y = Y;
            do {
                x += dx;
                y += dy;
            } while (onBoard(x, y) && get(other, x, y));

            if (onBoard(x, y) && get(side, x, y)) {
                x = X;
                y = Y;
                x += dx;
                y += dy;
                while (onBoard(x, y) && get(other, x, y)) {
                    set(side, x, y);
                    m->flipped[m->num_flipped] = x + BOARDSIZE * y;
                    m->num_flipped += 1;
                    x += dx;
                    y += dy;
                }
            }
        }
    }
    set(side, X, Y);
}

void Board::undoMove(Move *m) {
    taken.set(m->getX() + m->getY() * BOARDSIZE, 0);
    black.set(m->getX() + m->getY() * BOARDSIZE, 0);
    for (int i = 0; i < m->num_flipped; ++i) {
        black.flip(m->flipped[i]);
    }
}

/*
 * Current count of given side's stones.
 */
int Board::count(Side side) {
    return (side == BLACK) ? countBlack() : countWhite();
}

/*
 * Current count of black stones.
 */
int Board::countBlack() {
    return black.count();
}

/*
 * Current count of white stones.
 */
int Board::countWhite() {
    return taken.count() - black.count();
}

/*
 * Sets the board state given an BOARDSIZExBOARDSIZE char array where 'w' indicates a white
 * piece and 'b' indicates a black piece. Mainly for testing purposes.
 */
void Board::setBoard(char data[]) {
    taken.reset();
    black.reset();
    for (int i = 0; i < BOARDSIZE*BOARDSIZE; i++) {
        if (data[i] == 'b') {
            taken.set(i);
            black.set(i);
        } if (data[i] == 'w') {
            taken.set(i);
        }
    }
}

int Board::getDiffScore(Side side)
{
    return count(side) - count((side == WHITE) ? BLACK : WHITE);
}


double Board::getBoardScore(Side side)
{
    Move* possible;
    double white_count = 0;
    double black_count = 0;
    for (int i = 0; i < BOARDSIZE; ++i)
    {
        for (int j = 0; j < BOARDSIZE; ++j)
        {
            possible = new Move(i, j);
            if (checkMove(possible, BLACK))
            {
                ++black_count;
            }
            if (checkMove(possible, WHITE))
            {
                ++white_count;
            }
            delete possible;
        }
    }

    double move_diff_val = 0;
    if (black_count + white_count != 0)
    {
        if (side == BLACK)
        {
            move_diff_val = 100 * (black_count - white_count) / (black_count + white_count);
        }
        else
        {  
            move_diff_val = 100 * (white_count - black_count) / (black_count + white_count);
        }
    }
        
    double white_ms = 0;
    double black_ms = 0;
    for (int i = 0; i < BOARDSIZE * BOARDSIZE; ++i)
    {
        if (taken[i])
        {
            if (black[i])
            {
                black_ms += static_scores[i];
            }
            else
            {
                white_ms += static_scores[i];
            }
        }
    }
    
    double mob_diff_val = 0;
    if (black_ms + white_ms != 0)
    {
        if (side == BLACK)
        {
            mob_diff_val = 100 * (black_ms - white_ms) / (black_ms + white_ms);
        }
        else
        {
            mob_diff_val = 100 * (white_ms - black_ms) / (black_ms + white_ms);
        }
    }
    
    double piece_diff_val;
    if (side == BLACK)
    {
        piece_diff_val = 100 * (double) (count(BLACK) - count(WHITE)) / (count(BLACK) + count(WHITE));
    }
    else
    {
        piece_diff_val = 100 * (double) (count(WHITE) - count(BLACK)) / (count(BLACK) + count(WHITE));
    }
    
    double black_corners = 0;
    double white_corners = 0;
    double black_cc = 0;
    double white_cc = 0;
    double initial[4] = {0, 7, 56, 63};
    double to_check[4][3] = {{1, 8, 9}, {6, 14, 15}, {48, 49, 57}, {54, 55, 62}};
    for (int i = 0; i < 4; ++i)
    {
        if (taken[initial[i]])
        {
            if (black[initial[i]])
            {
                ++black_corners;
            }
            else
            {
                ++white_corners;
            }
        }
        else
        {
            for (int j = 0; j < 3; ++j)
            {
                if (taken[to_check[i][j]])
                {
                    if (black[to_check[i][j]])
                    {
                        ++black_cc;
                    }
                    else
                    {
                        ++white_cc;
                    }
                }
            }
        }
    }
    
    double cc_val = 0;
    if (white_cc + black_cc != 0)
    {
        if (side == BLACK)
        {
            cc_val = 100 * (white_cc - black_cc) / (black_cc + white_cc);
        }
        else
        {
            cc_val = 100 * (black_cc - white_cc) / (black_cc + white_cc);
        }
    }
    
    double corner_diff_val = 0;
    if (black_corners + white_corners != 0)
    {
        if (side == BLACK)
        {
            corner_diff_val = 100 * (black_corners - white_corners) / (black_corners + white_corners);
        }
        else
        {
            corner_diff_val = 100 * (white_corners - black_corners) / (black_corners + white_corners);
        }
    }
    
    return piece_diff_val / 10.0 + (mob_diff_val + move_diff_val) + 2.0 * cc_val + 10.0 * corner_diff_val;
}
