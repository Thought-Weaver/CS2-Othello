#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include <vector>
#include "common.hpp"
#include "board.hpp"
using namespace std;

class Player {

public:
    Player(Side side);
    ~Player();

    Move *doMove(Move *opponentsMove, int msLeft);
    Move *getMove(Board *b, Side s);
    void setBoard(Board *b);

    vector<vector<tuple<Move *, int, int>>> createDecisionTree(uint depth);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
    int depth;
    Board * board;
    Side player_side;
};

#endif
