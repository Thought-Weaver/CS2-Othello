#include "player.hpp"
#include <vector>
#include <tuple>

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish
 * within 30 seconds.
 */
Player::Player(Side side) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;
    player_side = side;
    board = new Board();

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
 * Destructor for the player.
 */
Player::~Player() {

    delete board;
}

Move *Player::getMove(Board *b, Side s)
{
    if (!b->hasMoves(s) || b->isDone())
    {
        return nullptr;
    }

    int max_points = -99999, max_x = -1, max_y = -1;

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (b->checkMove(new Move(i, j), s))
            {
                int sum = 0;

                if (b->onBoard(i + 1, j) && !b->isPlayerSide(s, i + 1, j))
                {
                    for (int q = i + 1; q < 8; q++)
                    {
                        if (b->isPlayerSide(s, q, j))
                        {
                            sum += q - i - 1;
                            break;
                        }
                    }
                }
                
                if (b->onBoard(i - 1, j) && !b->isPlayerSide(s, i - 1, j))
                {
                    for (int q = i - 1; q >= 0; q--)
                    {
                        if (b->isPlayerSide(s, q, j))
                        {
                            sum += i - q - 1;
                            break;
                        }
                    }
                }

                if (b->onBoard(i, j + 1) && !b->isPlayerSide(s, i, j + 1))
                {
                    for (int q = j + 1; q < 8; q++)
                    {
                        if (b->isPlayerSide(s, i, q))
                        {
                            sum += q - j - 1;
                            break;
                        }
                    }
                }
                
                if (b->onBoard(i, j - 1) && !b->isPlayerSide(s, i, j - 1))
                {
                    for (int q = j - 1; q >= 0; q--)
                    {
                        if (b->isPlayerSide(s, i, q))
                        {
                            sum += j - q - 1;
                            break;
                        }
                    }
                }

                if (b->onBoard(i + 1, j + 1) && !b->isPlayerSide(s, i + 1, j + 1))
                {
                    for (int q = 1; q < 8; q++)
                    {
                        if (b->onBoard(i + q, j + q))
                        {
                            break;
                        }

                        if (b->isPlayerSide(s, i + q, j + q))
                        {
                            sum += q - 1;
                            break;
                        }
                    }
                }

                if (b->onBoard(i - 1, j + 1) && !b->isPlayerSide(s, i - 1, j + 1))
                {
                    for (int q = 1; q < 8; q++)
                    {
                        if (b->onBoard(i - q, j + q))
                        {
                            break;
                        }

                        if (b->isPlayerSide(s, i - q, j + q))
                        {
                            sum += q - 1;
                            break;
                        }
                    }
                }

                if (b->onBoard(i + 1, j - 1) && !b->isPlayerSide(s, i + 1, j - 1))
                {
                    for (int q = 1; q < 8; q++)
                    {
                        if (b->onBoard(i + q, j - q))
                        {
                            break;
                        }

                        if (b->isPlayerSide(s, i + q, j - q))
                        {
                            sum += q - 1;
                            break;
                        }
                    }
                }

                if (b->onBoard(i - 1, j + 1) && !b->isPlayerSide(s, i + 1, j + 1))
                {
                    for (int q = 1; q < 8; q++)
                    {
                        if (b->onBoard(i - q, j + q))
                        {
                            break;
                        }

                        if (b->isPlayerSide(s, i - q, j + q))
                        {
                            sum += q - 1;
                            break;
                        }
                    }
                }

                /* 4 Directional Adjacent to Corners */
                if ((i == 0 && j == 1) || (i == 1 && j == 0) || (i == 7 && j == 1) || (i == 1 && j == 1) ||
                    (i == 0 && j == 6) || (i == 6 && j == 7) || (i == 6 && j == 0) || (i == 7 && j == 6))
                {
                    sum = 0;
                }

                /* Diagonal Adjacent to Corners */
                if ((i == 1 && j == 1) || (i == 6 && j == 6) || (i == 1 && j == 6) || (i == 6 && j == 1))
                {
                    sum = 0;
                }

                /* Corners */
                if ((i == 0 && j == 0) || (i == 7 && j == 7) || (i == 7 && j == 0) || (i == 0 && j == 7))
                {
                    sum *= 2;
                }

                if (sum > max_points)
                {
                    max_points = sum;
                    max_x = i;
                    max_y = j;
                }
            }
        }
    }

    if (max_x != -1 && max_y != -1)
    {
        return new Move(max_x, max_y);
    }

    return nullptr;
}

vector<vector<tuple<Move *, int, int>>> Player::createDecisionTree(uint depth)
{
    /* Okay, so the current problem is that it's selecting the best
    board regardless of what depth it was found. We need to select
    the board that leads to the best board from the first layer of
    depth. To do that, we should return a vector of tuples of the 
    move, the points gained, the current depth, and the index from
    which it came. */
    vector<vector<tuple<Move *, int, int>>> moves;
    vector<vector<Board *>> branches;

    branches.push_back(vector<Board *>());
    moves.push_back(vector<tuple<Move *, int, int>>());
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (board->checkMove(new Move(i, j), player_side))
            {
                Board *b = board->copy();
                b->doMove(new Move(i, j), player_side);
                branches[0].push_back(b);
                moves[0].push_back(make_tuple(new Move(i, j),
                                               b->countDiff(player_side),
                                               -1));
            }
        }
    }

    if (moves[0].size() <= 0)
    {
        return moves;
    }

    for (uint i = 0; i < depth; ++i)
    {
        vector<Board *> branch;
        vector<tuple<Move *, int, int>> current_moves;
        for (uint j = 0; j < branches[i].size(); ++j)
        {
            Side current_side;
            if (i % 2 == 0)
            {
                current_side = (player_side == WHITE ? BLACK : WHITE);
            }
            else
            {
                current_side = (player_side == WHITE ? WHITE : BLACK);
            }

            for (int n = 0; n < 8; ++n)
            {
                for (int m = 0; m < 8; ++m)
                {
                    Board *current_board = branches[i][j]->copy();
                    if (current_board->checkMove(new Move(n, m), current_side))
                    {
                        Move *move = new Move(n, m);
                        current_board->doMove(move, current_side);
                        branch.push_back(current_board);
                        current_moves.push_back(make_tuple(move, get<1>(moves[i][j]) + current_board->countDiff(current_side), j));
                    }
                }
            }
        }

        if (current_moves.size() > 0)
        {
            branches.push_back(branch);
            moves.push_back(current_moves);
        }

        if (i > 0)
        {
            for (uint j = 0; j < branches[i - 1].size(); ++j)
            {
                delete branches[i - 1][j];
            }
        }
    }

    return moves;
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
        board->doMove(opponentsMove, player_side == WHITE ? BLACK : WHITE);
    }

    if (msLeft == -1 || !board->hasMoves(player_side) || board->isDone())
    {
        return nullptr;
    }

    // Below is the simple, non-branching method.
    /*Move * move = getMove(board, player_side);
    if (move != nullptr)
    {
        board->doMove(move, player_side);
        return move;
    }*/

    vector<vector<tuple<Move *, int, int>>> moves = createDecisionTree(depth);

    int max_val = 0, max_index = 0;

    for (uint i = 0; i < moves[moves.size() - 1].size(); ++i)
    {
        if (get<1>(moves[moves.size() - 1][i]) > max_val)
        {
            max_val = get<1>(moves[moves.size() - 1][i]);
            max_index = i;
        }
    }

    int current_depth = moves.size() - 1;
    while (current_depth != 0)
    {
        max_index = get<2>(moves[current_depth][max_index]);

        for (uint i = 0; i < moves[current_depth].size(); ++i)
        {
            delete get<0>(moves[current_depth][i]);
        }

        current_depth -= 1;
    }

    Move *to_make = get<0>(moves[current_depth][max_index]);

    if (to_make != nullptr)
    {
        board->doMove(to_make, player_side);
        return to_make;
    }

    return nullptr;
}

void Player::setBoard(Board *b)
{
    if (board != nullptr)
    {
        delete board;
    }

    board = b;
}
