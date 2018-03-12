#include "player.hpp"

#define HIGH 2147483647
#define LOW -2147483646

Player::Player(Side temp) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;
    side = temp;
    board = Board();
    made_moves = "";

    if (testingMinimax)
    {
        depth = 2;
    }
    else if (!testingMinimax && side == BLACK)
    {
        depth = 6;
    }
    else if (!testingMinimax && side == WHITE)
    {
        depth = 5;
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
    vector<Board*> current_boards;
    vector<Move*> current_moves;
    vector<double> result_scores;
    vector<double> original_scores;
    Move *current_move;

    for (int i = 0; i < BOARDSIZE; ++i)
    {
        for (int j = 0; j < BOARDSIZE; ++j)
        {
            current_move = new Move(i, j);
            if (board.checkMove(current_move, side))
            {
                Board *b = board.copy();
                b->doMove(current_move, side);
                current_moves.push_back(current_move);
                current_boards.push_back(b);
            }
            else
            {
                delete current_move;
            }
        }
    }

    for (uint i = 0; i < current_moves.size(); ++i)
    {
        int not_taken = BOARDSIZE * BOARDSIZE - (current_boards[i]->count(WHITE) + current_boards[i]->count(BLACK));
        int d = depth;
        if (side == BLACK && not_taken > 30 && not_taken < 50)
        {
            d -= 1;
        }

        auto fut = async(launch::async, &Player::getABScore, this, *current_boards[i], d, LOW, HIGH);
        result_scores.push_back(fut.get());
    }

    reverse(result_scores.begin(), result_scores.end());
    Move *best_move = current_moves[current_moves.size() - 1 - distance(result_scores.begin(), max_element(result_scores.begin(), result_scores.end()))]->copy();

    for (uint i = 0; i < current_moves.size(); ++i)
    {
        delete current_moves[i];
        delete current_boards[i];
    }

    cerr << "I chose (" << best_move->getX() << "," << best_move->getY() << ")" << endl;

    return best_move;
}

double Player::getABScore(Board b, int d, double alpha, double beta)
{
    if (d == 0)
    {
        return b.getBoardScore(side);
    }

    double value = 0;
    double best_value = (d % 2 != 0) ? LOW : HIGH;
    bool played = false;

    Move *possible_move;
    Side opposite = (side == WHITE) ? BLACK : WHITE;

    if (d % 2 != 0)
    {
        for (int i = 0; i < BOARDSIZE; ++i)
        {
            for (int j = 0; j < BOARDSIZE; ++j)
            {
                possible_move = new Move(i, j);
                if (b.checkMove(possible_move, side))
                {
                    played = true;

                    b.doMove(possible_move, side);
                    value = getABScore(b, d - 1, alpha, beta);
                    b.undoMove(possible_move);

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
            return getABScore(b, d - 1, alpha, beta);
        }
    }
    else
    {
        for (int i = 0; i < BOARDSIZE; ++i)
        {
            for (int j = 0; j < BOARDSIZE; ++j)
            {
                possible_move = new Move(i, j);
                if (b.checkMove(possible_move, opposite))
                {
                    played = true;

                    b.doMove(possible_move, opposite);
                    value = getABScore(b, d - 1, alpha, beta);
                    b.undoMove(possible_move);

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
            return getABScore(b, d - 1, alpha, beta);
        }
    }
}

void Player::LoadOpeningMoves()
{
    ifstream file("opening_moves");
    string str; 
    while (getline(file, str))
    {
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        opening_moves.push_back(str);
    }
    file.close();
}
