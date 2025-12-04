#ifndef BRAIN_H
#define BRAIN_H

#include <string>
#include "game.h"
#include <cstdlib>
#include <ctime>

class Brain
{
public:
    Brain();                               // Constructor
    int getNextMove(GameState &gamestate); // Returns the next move for the AI
private:
    int lastMove = 0; // 0 = none, 1 = up, 2 = left, 3 = down, 4 = right
};

bool isWalkable(char tile);

std::string toStringVision(const std::vector<std::vector<char>> &vision);

bool isWallNearby(const std::vector<std::vector<char>> &vision, int r, int c); // check for wall or door adjacent

int chooseMove(const std::vector<std::vector<char>> &vision);
#endif // BRAIN_H