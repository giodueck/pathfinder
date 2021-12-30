#pragma once

#include <exception>
#include <cstdlib>
#include <ctime>

enum Pieces
{
    UNKNOWN = -1,   // for tracking grid
    BLANK = 0,
    OBJECTIVE = 1,
    PAWN = 2,
    VISITED = 3,
    WALL = 4
};

enum Direction
{
    UP = 0,
    LEFT = 1,
    DOWN = 2,
    RIGHT = 3
};

class Player
{
public:
    const int maxWalls = 30; // 6x5

    Player();

    ~Player() {}

    // For debugging/testing
    void SetMazeWall(int i, int j);
    void SetTrackingWall(int i, int j);
    void SetObjective(int i, int j);
    //void SetTrackingObjective(int i, int j);
    void SetPawn(int i, int j);
    void SetVisited(int i, int j);

    int GetMazeCell(int i, int j);

    int GetTrackingCell(int i, int j);

    int GetMazeWallCount();

    int GetTrackingWallCount();

    bool PawnOnBoard();

    void ToggleWall(int i, int j);

    void UnsetObjective();

    void ToggleObjective(int i, int j);

    bool EnterOpponent(int i);

    bool Enter(Player& opponent, int i);

    // Returns true if the turn continues, false otherwise
    bool MoveOpponent(int direction);

    // Returns true if the turn continues, false otherwise
    bool Move(Player& opponent);

    void RetreatOpponent();

    void Retreat(Player& opponent);

    bool OpponentWon() { return opponentWon; }

    void PlaceRandWalls();

    void PlaceRandObjective();

    // Performs Move() behind the scenes, true if turn continues, false otherwise
    // Intended for AI use
    bool NavigateMaze(Player& opponent);

    void SetDirection(int direction);

protected:
    int mazeGrid[13][13];
    int trackingGrid[13][13];
    int mazeWallCount, trackingWallCount;
    bool onBoard = false, opponentOnBoard = false;
    bool opponentWon = false;
    int direction = RIGHT;

    bool GetOpponentPawn(int& i, int& j);
    bool GetPawn(int& i, int& j);
    bool PathAheadClear(int i, int j);
};