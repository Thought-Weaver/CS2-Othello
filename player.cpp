#include "player.hpp"

#define HIGH 2147483647
#define LOW -2147483646

Player::Player(Side temp) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;
    side = temp;
    board = Board();
    turns_taken = 0;
    curr_time = 0;
    made_moves = "";

    //LoadOpeningMoves();

    if (testingMinimax)
    {
        depth = 2;
    }
    
    depth = 6;
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

    curr_time = msLeft;

    if (opponentsMove != nullptr)
    {
        board.doMove(opponentsMove, (side == WHITE) ? BLACK : WHITE);
        char c_arr[] = {'a' + opponentsMove->getX(), '1' + opponentsMove->getY(), '\0'};
        made_moves += string(c_arr);
    }

    if (msLeft == -1 || !board.hasMoves(side) || board.isDone())
    {
        return nullptr;
    }

    Move *to_make = nullptr;
    string to_make_str = "";
    bool pattern_found = false;

    for (uint i = 0; i < opening_moves.size(); ++i)
    {
        if (opening_moves[i].substr(0, made_moves.length()) != "")
        {
            to_make = new Move(opening_moves[i][made_moves.length()] - 'a', opening_moves[i][made_moves.length() + 1] - '1');

            if (board.checkMove(to_make, side))
            {
                char c_arr[] = {opening_moves[i][made_moves.length()], opening_moves[i][made_moves.length() + 1], '\0'};
                to_make_str = string(c_arr);
                pattern_found = true;
                break;
            }
        }
    }

    if (!pattern_found)
    {
        Move *to_make = doABMinimaxMove();

        if (to_make != nullptr)
        {
            board.doMove(to_make, side);
            char c_arr[] = {'a' + to_make->getX(), '1' + to_make->getY(), '\0'};
            made_moves += string(c_arr);
        }

        ++turns_taken;

        return to_make;
    }

    if (to_make != nullptr)
    {
        board.doMove(to_make, side);
        made_moves += to_make_str;
    }

    ++turns_taken;

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
    double best_value = LOW;
    Move *best_move = nullptr;

    for (int i = 0; i < BOARDSIZE; ++i)
    {
        for (int j = 0; j < BOARDSIZE; ++j)
        {
            current_move = new Move(i, j);
            if (board.checkMove(current_move, side))
            {
                board.doMove(current_move, side);
                int d = depth;
                if (turns_taken != 0 && (TOURNEYTIME - curr_time) / turns_taken > TIMELIMIT)
                {
                    --d;
                }
                if (turns_taken != 0 && (TOURNEYTIME - curr_time) / turns_taken > 2 * TIMELIMIT)
                {
                    --d;
                }
                if (turns_taken != 0 && (TOURNEYTIME - curr_time) / turns_taken < TIMELIMIT / depth)
                {
                    ++d;
                }
                double value = getABScore(board, d, LOW, HIGH);
                board.undoMove(current_move);

                if (value > best_value)
                {
                    best_value = value;
                    best_move = current_move->copy();
                }
            }
            
            delete current_move;
        }
    }

    return best_move;
}

double Player::getABScore(Board b, int d, double alpha, double beta)
{
    if (d == 0)
    {
        return (side == WHITE) ? b.getBoardScore(side) : b.getBlackBoardScore();
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

