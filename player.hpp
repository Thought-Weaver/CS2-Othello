#ifndef __PLAYER_H__
#define __PLAYER_H__
#define NDEBUG

#include "common.hpp"
#include "board.hpp"
#include <iostream>

using namespace std;

class Player {
public:
    Player(Side side);
    ~Player();

    Move *doMove(Move *opponentsMove, int msLeft);
    Move *doNaiveMove();
    Move *doABMinimaxMove();

    double getABScore(int depth, double alpha, double beta);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
    int depth;
    Side side;
    Board board;
};

#endif