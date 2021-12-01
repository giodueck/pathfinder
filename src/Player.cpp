#include "Player.h"

Player::Player()
{
    mazeWallCount = 0;
    trackingWallCount = 0;
    for (int i = 0; i < 13; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            mazeGrid[i][j] = 0;
            trackingGrid[i][j] = 0;
        }
    }
}

void Player::SetMazeWall(int i, int j)
{
    if (mazeWallCount < 30)
    {
        mazeGrid[i][j] = WALL;
        mazeWallCount++;
    }
}

void Player::SetTrackingWall(int i, int j)
{
    if (trackingWallCount < 30)
    {
        trackingGrid[i][j] = WALL;
        trackingWallCount++;
    }
}

void Player::SetObjective(int i, int j)
{
    mazeGrid[i][j] = OBJECTIVE;
}

void Player::SetPawn(int i, int j)
{
    mazeGrid[i][j] = PAWN;
}

void Player::SetVisited(int i, int j)
{
    mazeGrid[i][j] = VISITED;
}

int Player::GetMazeCell(int i, int j)
{
    return mazeGrid[i][j];
}

int Player::GetTrackingCell(int i, int j)
{
    return trackingGrid[i][j];
}

int Player::GetMazeWallCount()
{
    return mazeWallCount;
}

int Player::GetTrackingWallCount()
{
    return trackingWallCount;
}