#include "player.hpp"

#define HIGH 2147483647
#define LOW -2147483646

Player::Player(Side temp) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;
    side = temp;
    board = Board();

    if (testingMinimax)
    {
        depth = 2;
    }
    else
    {
        depth = 4;
    }
}

/*
 * Destructor for the player
 */
Player::~Player() {

}

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be
 * nullptr.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return nullptr.
 */
Move *Player::doMove(Move *opponentsMove, int msLeft) {

    if (opponentsMove != nullptr)
    {
        board.doMove(opponentsMove, (side == WHITE) ? BLACK : WHITE);
    }

    if (msLeft == -1 || !board.hasMoves(side) || board.isDone())
    {
        return nullptr;
    }

    Move *to_make = doABMinimaxMove();

    if (to_make != nullptr)
    {
        board.doMove(to_make, side);
    }

    return to_make;
} 

Move *Player::doNaiveMove() {

    Move *possible;
    Move *best_move = nullptr;

    int possible_score;
    int best_score;

    for (int i = 0; i < BOARDSIZE; ++i)
    {
        for (int j = 0; j < BOARDSIZE; ++j)
        {
            possible = new Move(i, j);
            if (board.checkMove(possible, side))
            {
                board.doMove(possible, side);
                possible_score = board.getDiffScore(side);
                board.undoMove(possible);

                if (best_move == nullptr)
                {
                    best_move = possible;
                    best_score = possible_score;
                }
                else if (possible_score > best_score)
                {
                    delete best_move;
                    best_move = possible;
                    best_score = possible_score;
                }
                else
                {
                    delete possible;
                }
            }
            else
            {
                delete possible;
            }
        }
    }

    return best_move;
}

Move *Player::doABMinimaxMove()
{
    Move *current_move;
    Move *best_move = nullptr;

    double current_score;
    double best_score;

    for (int i = 0; i < BOARDSIZE; ++i)
    {
        for (int j = 0; j < BOARDSIZE; ++j)
        {
            current_move = new Move(i, j);
            if (board.checkMove(current_move, side))
            {
                board.doMove(current_move, side);
                current_score = getABScore(depth, LOW, HIGH);
                board.undoMove(current_move);

                if (best_move == nullptr)
                {
                    best_move = current_move;
                    best_score = current_score;
                }
                else if (current_score >= best_score)
                {
                    delete best_move;
                    best_move = current_move;
                    best_score = current_score;
                }
                else
                {
                    delete current_move;
                }
            }
            else
            {
                delete current_move;
            }
        }
    }

    return best_move;
}

double Player::getABScore(int depth, double alpha, double beta)
{
    if (depth == 0)
    {
        return board.getBoardScore(side);
    }

    double value = 0;
    double best_value = (depth % 2 != 0) ? LOW : HIGH;
    bool played = false;

    Move *possible_move;
    Side opposite = (side == WHITE) ? BLACK : WHITE;

    if (depth % 2 != 0)
    {
        for (int i = 0; i < BOARDSIZE; ++i)
        {
            for (int j = 0; j < BOARDSIZE; ++j)
            {
                possible_move = new Move(i, j);
                if (board.checkMove(possible_move, side))
                {
                    played = true;

                    board.doMove(possible_move, side);
                    value = getABScore(depth - 1, alpha, beta);
                    board.undoMove(possible_move);

                    best_value = max(value, best_value);
                    alpha = max(alpha, best_value);

                    if (beta < alpha)
                    {
                        delete possible_move;
                        break;
                    }
                }

                delete possible_move;
            }
        }

        if (played)
        {
            return best_value;
        }
        else
        {
            return getABScore(depth - 1, alpha, beta);
        }
    }
    else
    {
        for (int i = 0; i < BOARDSIZE; ++i)
        {
            for (int j = 0; j < BOARDSIZE; ++j)
            {
                possible_move = new Move(i, j);
                if (board.checkMove(possible_move, opposite))
                {
                    played = true;
                    board.doMove(possible_move, opposite);
                    value = getABScore(depth - 1, alpha, beta);
                    board.undoMove(possible_move);

                    best_value = min(best_value, value);
                    beta = min(beta, best_value);

                    if (beta < alpha)
                    {
                        delete possible_move;
                        break;
                    }
                }

                delete possible_move;
            }
        }

        if (played)
        {
            return best_value;
        }
        else
        {
            return getABScore(depth - 1, alpha, beta);
        }
    }
}
