#include "brain.h"

Brain::Brain()
{
}

int Brain::getNextMove(GameState &gamestate)
{
    return chooseMove(gamestate.vision); // Determine next move based on vision
}

bool isWalkable(char tile)
{
    if (tile == ' ' || tile == '0' || tile == 'A' || tile == 'B')
    {
        return true; // Walkable tiles include empty space, 0, A, and B
    }
    return false;
}

std::string toStringVision(const std::vector<std::vector<char>> &vision)
{
    std::string result;
    for (const auto &row : vision)
    {
        for (const auto &tile : row)
        {
            result += tile;
        }
        result += '\n';
    }
    return result;
}

bool isWallNearby(const std::vector<std::vector<char>> &vision, int r, int c) // check for wall or door adjacent
{
    int rows = vision.size();    // number of rows
    int cols = vision[0].size(); // number of columns

    if (r > 0 && (vision[r - 1][c] == '+' || vision[r - 1][c] == 'D'))
        return true; // check up
    if (r < rows - 1 && (vision[r + 1][c] == '+' || vision[r + 1][c] == 'D'))
        return true; // check down
    if (c > 0 && (vision[r][c - 1] == '+' || vision[r][c - 1] == 'D'))
        return true; // check left
    if (c < cols - 1 && (vision[r][c + 1] == '+' || vision[r][c + 1] == 'D'))
        return true; // check right
    return false;
}

int chooseMove(const std::vector<std::vector<char>> &vision) // main decision-making function
{
    // learned how to use static variables from ChatGPT
    static int mode = 0;           // 0 = wall hug (maze level)
                                   // 1 = go right (coin level)
                                   // 2 = go up (coin level)
                                   // 3 = zig-zag (coin level)
                                   // 4 = A/B (capture the flag level)
    static bool zigRight = true;   // zig-zag direction
    static int lastMove = 0;       // 0 = none, 1 = up, 2 = left, 3 = down, 4 = right
    static bool triggered = false; // only trigger mode 1 once
    static int level4Stage = 0;    // 0 = go A, 1 = go B

    // Find player position
    int pr = -1, pc = -1;                      // player row and column (defult to -1 since impossible value)
    for (size_t r = 0; r < vision.size(); r++) // scan rows
    {
        for (size_t c = 0; c < vision[r].size(); c++) // scan columns
        {
            if (vision[r][c] == 'v' || vision[r][c] == '^' ||
                vision[r][c] == '<' || vision[r][c] == '>') // found player
            {
                pr = r; // player row
                pc = c; // player column
                break;
            }
        }
        if (pr != -1) // found player
            break;
    }

    bool up = isWalkable(vision[pr - 1][pc]);    // check up
    bool down = isWalkable(vision[pr + 1][pc]);  // check down
    bool left = isWalkable(vision[pr][pc - 1]);  // check left
    bool right = isWalkable(vision[pr][pc + 1]); // check right

    static bool triggeredLevel4 = false; // checks if we have triggered level 4

    if (!triggeredLevel4) // look for 'B' to trigger level 4
    {
        if (vision[pr - 1][pc] == 'B')
        {
            mode = 4;
            triggeredLevel4 = true;
        }
        else if (vision[pr + 1][pc] == 'B')
        {
            mode = 4;
            triggeredLevel4 = true;
        }
        else if (vision[pr][pc - 1] == 'B')
        {
            mode = 4;
            triggeredLevel4 = true;
        }
        else if (vision[pr][pc + 1] == 'B')
        {
            mode = 4;
            triggeredLevel4 = true;
        }
    }

    // MODE 4: Level 4 (capture the flag level)

    if (mode == 4)
    {
        char targetTile = ' '; // tile to go towards

        // Decide target
        if (level4Stage == 0)
            targetTile = 'A';
        else if (level4Stage == 1)
            targetTile = 'B';

        // Check if adjacent tile is target
        if ((up && vision[pr - 1][pc] == targetTile) ||
            (down && vision[pr + 1][pc] == targetTile) ||
            (left && vision[pr][pc - 1] == targetTile) ||
            (right && vision[pr][pc + 1] == targetTile))
        {
            if (level4Stage == 0) // picked up A
            {
                level4Stage = 1; // go to B next
            }
            else if (level4Stage == 1) // after visitng B
            {
                level4Stage = 0; // reset for next time
                mode = 0;        // back to wall hug
            }
        }

        // Move to target if adjacent
        if (up && vision[pr - 1][pc] == targetTile)
        {
            return lastMove = 1;
        }
        if (down && vision[pr + 1][pc] == targetTile)
        {
            return lastMove = 3;
        }
        if (left && vision[pr][pc - 1] == targetTile)
        {
            return lastMove = 2;
        }
        if (right && vision[pr][pc + 1] == targetTile)
        {
            return lastMove = 4;
        }

        // Wall hugging to find target
        if (up && isWallNearby(vision, pr - 1, pc) && lastMove != 3 && !left) // not left to avoid a loop
        {
            return lastMove = 1;
        }
        if (right && isWallNearby(vision, pr, pc + 1) && lastMove != 2)
        {
            return lastMove = 4;
        }
        if (down && isWallNearby(vision, pr + 1, pc) && lastMove != 1)
        {
            return lastMove = 3;
        }
        if (left && isWallNearby(vision, pr, pc - 1) && lastMove != 4)
        {
            return lastMove = 2;
        }

        // move to any walkable tile (last resort)
        if (up)
            return lastMove = 1;
        if (right)
            return lastMove = 4;
        if (down)
            return lastMove = 3;
        if (left)
            return lastMove = 2;

        return 0;
    }

    // Mode 1 trigger if coin is adjacent
    if (!triggered && mode == 0)
    {
        if (up && vision[pr - 1][pc] == '0')
        {
            mode = 1;
            triggered = true;
        }
        if (right && vision[pr][pc + 1] == '0')
        {
            mode = 1;
            triggered = true;
        }
        if (down && vision[pr + 1][pc] == '0')
        {
            mode = 1;
            triggered = true;
        }
        if (left && vision[pr][pc - 1] == '0')
        {
            mode = 1;
            triggered = true;
        }
    }

    // MODE 0: Wall hugging right priority (maze levels)
    if (mode == 0)
    {
        if (right && lastMove != 2)
            return lastMove = 4; // right
        if (down && lastMove != 1)
            return lastMove = 3; // down
        if (up && lastMove != 3)
            return lastMove = 1; // up
        if (left && lastMove != 4)
            return lastMove = 2; // left

        // fallback
        if (right)
            return lastMove = 4;
        if (down)
            return lastMove = 3;
        if (up)
            return lastMove = 1;
        if (left)
            return lastMove = 2;
        return 0;
    }

    //  MODE 1: Move right until you hit wall (coin level)
    if (mode == 1)
    {
        if (right)
            return lastMove = 4;
        else
            mode = 2;
    }

    //  MODE 2: Move up until you hit wall (coin level)
    if (mode == 2)
    {
        if (up)
            return lastMove = 1;
        else
            mode = 3;
    }

    //  MODE 3: Full zig-zag sweep (coin level)
    static bool firstZig = true; // so that it sweeps the first row correctly

    if (mode == 3)
    {
        if (firstZig)
        {
            if (pc == 0) // on left edge
            {
                zigRight = true;
            }
            else // on right edge
            {
                zigRight = false;
            }
            firstZig = false; // only do this once
        }

        if (zigRight)
        {
            if (right) // can go right
                return lastMove = 4;
            if (down) // cannot go right, try down
            {
                zigRight = false;
                return lastMove = 3;
            }
        }
        else
        {
            if (left) // can go left
                return lastMove = 2;
            if (down) // cannot go left, try down
            {
                zigRight = true;
                return lastMove = 3;
            }
        }
        if (!down || !isWalkable(vision[pr + 1][pc])) // cannot go down meaning zigzag complete
        {
            mode = 0;        // switch back to wall-following
            zigRight = true; // reset zigzag direction
            firstZig = true; // reset firstZig for next zigzag if applicable
        }

        if (up) // if can go up (should rarely happen)
            return lastMove = 1;
        return 0; // no move possible
    }

    return 0; // no move possible
}
