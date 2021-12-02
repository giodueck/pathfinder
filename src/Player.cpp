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
    UnsetObjective();
    mazeGrid[i][j] = OBJECTIVE;
}

void Player::UnsetObjective()
{
    for (int i_ = 0; i_ < 13; i_++)
    {
        for (int j_ = 0; j_ < 13; j_++)
        {
            if (mazeGrid[i_][j_] == OBJECTIVE)
            {
                mazeGrid[i_][j_] = 0;
                break;
            }
        }
    }
}

void Player::ToggleObjective(int i, int j)
{
    if (mazeGrid[i][j] == OBJECTIVE)
        mazeGrid[i][j] = 0;
    else
        SetObjective(i, j);
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

void Player::ToggleWall(int i, int j)
{
    if (mazeGrid[i][j])
    {
        mazeGrid[i][j] = 0;
        mazeWallCount--;
    }
    else if (mazeWallCount < maxWalls)
    {
        mazeGrid[i][j] = WALL;
        mazeWallCount++;
    }
}