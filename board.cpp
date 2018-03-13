#include "board.hpp"
#include <iostream>
#include <string>

// Adapted from:
// https://courses.cs.washington.edu/courses/cse573/04au/Project/mini1/RUSSIA/Final_Paper.pdf
const int static_scores[64] = 
{
       20, -3, 11, 8, 8, 11, -3, 20,
      -3, -7, -4, 1, 1, -4, -7, -3,
       11, -4, 2, 2, 2, 2, -4, 11,
       8, 1, 2, -3, -3, 2, 1, 8,
       8, 1, 2, -3, -3, 2, 1, 8,
       11, -4, 2, 2, 2, 2, -4, 11,
      -3, -7, -4, 1, 1, -4, -7, -3,
       20, -3, 11, 8, 8, 11, -3, 20
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

int Board::numValidMoves(Side side)
{
    int total = 0;
    for (int i = 0; i < BOARDSIZE; ++i)
    {
        for (int j = 0; j < BOARDSIZE; ++j)
        {
            Move *current_move = new Move(i, j);
            if (checkMove(current_move, side))
            {
                ++total;
            }
            delete current_move;
        }
    }

    return total;
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
    
    double white_move_score = 0;
    double black_move_score = 0;
    for (int i = 0; i < BOARDSIZE * BOARDSIZE; ++i)
    {
        if (taken[i])
        {
            if (black[i])
            {
                black_move_score += static_scores[i];
            }
            else
            {
                white_move_score += static_scores[i];
            }
        }
    }
    
    double mob_diff_val = 0;
    if (black_move_score + white_move_score != 0)
    {
        if (side == BLACK)
        {
            mob_diff_val = 10 * (black_move_score - white_move_score) / (black_move_score + white_move_score);
        }
        else
        {
            mob_diff_val = 10 * (white_move_score - black_move_score) / (black_move_score + white_move_score);
        }
    }
    
    double piece_diff_val;
    if (side == BLACK)
    {
        piece_diff_val = 10 * (double) (count(BLACK) - count(WHITE)) / (count(BLACK) + count(WHITE));
    }
    else
    {
        piece_diff_val = 10 * (double) (count(WHITE) - count(BLACK)) / (count(BLACK) + count(WHITE));
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
            cc_val = 12.5 * (white_cc - black_cc);
        }
        else
        {
            cc_val = 12.5 * (black_cc - white_cc);
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
    
    return piece_diff_val / 10.0 + (mob_diff_val + 2.0 * move_diff_val) + 5.0 * cc_val + 8.0 * corner_diff_val;
}

double Board::getBlackBoardScore()
{
    int black_score = 0, white_score = 0, black_frontiers = 0, white_frontiers = 0;
    double diff = 0, corners = 0, corner_diff = 0, mobility = 0, frontiers = 0, state = 0;

    int X1[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int Y1[] = {0, 1, 1, 1, 0, -1, -1, -1};

    for(int i = 0; i < BOARDSIZE; ++i)
    {
        for(int j = 0; j < BOARDSIZE; ++j)  
        {
            if(get(BLACK, i, j))  
            {
                state += static_scores[i + BOARDSIZE * j];
                black_score++;
            } 
            else if(get(WHITE, i, j))  
            {
                state -= static_scores[i + BOARDSIZE * j];
                white_score++;
            }

            if(occupied(i, j))   
            {
                for(int k = 0; k < BOARDSIZE; ++k)  
                {
                    int x = i + X1[k];
                    int y = j + Y1[k];
                    if(x >= 0 && x < 8 && y >= 0 && y < 8 && !occupied(x, y)) 
                    {
                        if(get(BLACK, i, j))
                        {
                            black_frontiers++;
                        }
                        else
                        {
                            white_frontiers++;
                        }
                        break;
                    }
                }
            }
        }
    }

    if(black_score > white_score)
    {
        diff = (100.0 * black_score) / (black_score + white_score);
    }
    else if(black_score < white_score)
    {
        diff = -(100.0 * white_score) / (black_score + white_score);
    }

    if(black_frontiers > white_frontiers)
    {
        frontiers = -(100.0 * black_frontiers) / (black_frontiers + white_frontiers);
    }
    else if(black_frontiers < white_frontiers)
    {
        frontiers = (100.0 * white_frontiers) / (black_frontiers + white_frontiers);
    }

    int X2[] = {0, 0, 7, 7};
    int Y2[] = {0, 7, 0, 7};

    black_score = white_score = 0;
    for (int i = 0; i < 4; ++i)
    {
        if(get(BLACK, X2[i], Y2[i]))
        {
            ++black_score;
        }
        else if (get(WHITE, X2[i], Y2[i]))
        {
            ++white_score;
        }
    }
    corners = 25 * (black_score - white_score);

    int X3[4][3] = {{0, 1, 1}, {0, 1, 1}, {7, 6, 6}, {6, 6, 7}};
    int Y3[4][3] = {{1, 1, 0}, {6, 6, 7}, {1, 1, 0}, {7, 6, 6}};
    black_score = white_score = 0;

    for (int i = 0; i < 4; ++i)
    {
        if (!occupied(X2[i], Y2[i]))
        {
            for (int j = 0; j < 3; ++j)
            {
                if (get(BLACK, X3[i][j], Y3[i][j]))
                {
                    ++black_score;
                }
                else if (get(WHITE, X3[i][j], Y3[i][j]))
                {
                    ++white_score;
                }
            }
        }
    }
    corner_diff = -12.5 * (black_score - white_score);

    black_score = numValidMoves(BLACK);
    white_score = numValidMoves(WHITE);
    if(black_score > white_score)
    {
        mobility = (100.0 * black_score)/(black_score + white_score);
    }
    else if(black_score < white_score)
    {
        mobility = -(100.0 * white_score)/(black_score + white_score);
    }

    return (10.0 * diff) + (801.724 * corners) + (382.026 * corner_diff) + (78.922 * mobility) + (74.396 * frontiers) + (10 * state);
}
