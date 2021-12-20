#pragma once

#include "Player.h"
#include <vector>

// Structure to be used by the A* algorithm in the open and closed lists
struct ASSquare
{
    int i, j;       // position
    int F; // cost
    int pi, pj;     // parent position for path reconstruction
};

class AStar : public Player
{
public:

    AStar(Player player);
    ~AStar() { }

    // Executes the algorithm. Returns true if at least one path is found to the objective, false otherwise.
    bool Pathfind();

private:

    // vectors keep track of open and visited squares
    std::vector<ASSquare> open, closed;

    // Objective position
    int obji, objj;

    // Adds all squares adjacent to parent to the open list if they are not on any list and are reachable,
    // or if they are in the open list and reachable update them
    void AddAdjacentSquares(ASSquare& parent);

    bool SquareInList(int i, int j, std::vector<ASSquare>& list);

    // Use only if square is for sure in list, otherwise return is an uninitialized square
    ASSquare GetSquare(int i, int j, std::vector<ASSquare>& list);

    // Returns index if found, -1 if not
    int GetSquareIndex(int i, int j, std::vector<ASSquare>& list);
};