#pragma once

enum Pieces
{
    OBJECTIVE = 1,
    PAWN = 2,
    VISITED = 3,
    WALL = 4
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
    void SetPawn(int i, int j);
    void SetVisited(int i, int j);

    int GetMazeCell(int i, int j);

    int GetTrackingCell(int i, int j);

    int GetMazeWallCount();

    int GetTrackingWallCount();

    void ToggleWall(int i, int j);

    void UnsetObjective();

    void ToggleObjective(int i, int j);

protected:
    int mazeGrid[13][13];
    int trackingGrid[13][13];
    int mazeWallCount, trackingWallCount;
};