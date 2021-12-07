#include "Player.h"

Player::Player()
{
    mazeWallCount = 0;
    trackingWallCount = 0;
    for (int i = 0; i < 13; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            mazeGrid[i][j] = BLANK;
            trackingGrid[i][j] = UNKNOWN;
        }
    }
}

void Player::SetMazeWall(int i, int j)
{
    if (mazeGrid[i][j] != WALL && mazeWallCount < 30)
    {
        mazeGrid[i][j] = WALL;
        mazeWallCount++;
    }
}

void Player::SetTrackingWall(int i, int j)
{
    if (trackingGrid[i][j] != WALL && trackingWallCount < 30)
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

//void Player::SetTrackingObjective(int i, int j)
//{
//    trackingGrid[i][j] = OBJECTIVE;
//}

void Player::UnsetObjective()
{
    for (int i_ = 0; i_ < 13; i_++)
    {
        for (int j_ = 0; j_ < 13; j_++)
        {
            if (mazeGrid[i_][j_] == OBJECTIVE)
            {
                mazeGrid[i_][j_] = BLANK;
                break;
            }
        }
    }
}

void Player::ToggleObjective(int i, int j)
{
    if (mazeGrid[i][j] == OBJECTIVE)
        mazeGrid[i][j] = BLANK;
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
        mazeGrid[i][j] = BLANK;
        mazeWallCount--;
    }
    else if (mazeWallCount < maxWalls)
    {
        mazeGrid[i][j] = WALL;
        mazeWallCount++;
    }
}

bool Player::EnterOpponent(int j)
{
    if (j % 2 && !opponentOnBoard)
    {
        if (mazeGrid[0][j] != WALL)
        {
            opponentOnBoard = true;
            if (mazeGrid[1][j] == OBJECTIVE)
                opponentWon = true;
            mazeGrid[1][j] = PAWN;
            return true;
        }
        else
            return false;
    }
    else if (j % 2 == 0)
        throw std::exception("EnterOpp: Invalid row, has to be uneven");
    else
        throw std::exception("EnterOpp: Already entered");
}

bool Player::Enter(Player& opponent, int j)
{
    if (j % 2 && !onBoard)
    {
        if (trackingGrid[0][j] != WALL)
        {
            if (opponent.EnterOpponent(j))
            {
                trackingGrid[0][j] = BLANK;
                trackingGrid[1][j] = PAWN;
                onBoard = true;
                return true;
            }
            else
            {
                trackingGrid[0][j] = WALL;
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else if (j % 2 == 0)
        throw std::exception("Enter: Invalid row, has to be uneven");
    else
        throw std::exception("Enter: Already entered");
}

bool Player::GetOpponentPawn(int& i_, int& j_)
{
    for (int i = 1; i < 13; i += 2)
    {
        for (int j = 1; j < 13; j += 2)
        {
            if (mazeGrid[i][j] == PAWN)
            {
                i_ = i;
                j_ = j;
                return true;
            }
        }
    }

    i_ = 0;
    j_ = 0;
    return false;
}

bool Player::GetPawn(int& i_, int& j_)
{
    for (int i = 1; i < 13; i += 2)
    {
        for (int j = 1; j < 13; j += 2)
        {
            if (trackingGrid[i][j] == PAWN)
            {
                i_ = i;
                j_ = j;
                return true;
            }
        }
    }

    i_ = 0;
    j_ = 0;
    return false;
}

bool Player::MoveOpponent(int direction)
{
    int i, j;

    if (GetOpponentPawn(i, j))
    {
        switch (direction)
        {
        case LEFT:
            if (mazeGrid[i - 1][j] == WALL)
                return false;
            else if (i - 2 > 0)
            {
                if (mazeGrid[i - 2][j] == OBJECTIVE)
                    opponentWon = true;
                mazeGrid[i - 2][j] = PAWN;
                mazeGrid[i][j] = VISITED;
            }
            return true;
        case RIGHT:
            if (mazeGrid[i + 1][j] == WALL)
                return false;
            else if (i + 2 < 13)
            {
                if (mazeGrid[i + 2][j] == OBJECTIVE)
                    opponentWon = true;
                mazeGrid[i + 2][j] = PAWN;
                mazeGrid[i][j] = VISITED;
            }
            return true;
        case UP:
            if (mazeGrid[i][j - 1] == WALL)
                return false;
            else if (j - 2 > 0)
            {
                if (mazeGrid[i][j - 2] == OBJECTIVE)
                    opponentWon = true;
                mazeGrid[i][j - 2] = PAWN;
                mazeGrid[i][j] = VISITED;
            }
            return true;
        case DOWN:
            if (mazeGrid[i][j + 1] == WALL)
                return false;
            else if (j + 2 < 13)
            {
                if (mazeGrid[i][j + 2] == OBJECTIVE)
                    opponentWon = true;
                mazeGrid[i][j + 2] = PAWN;
                mazeGrid[i][j] = VISITED;
            }
            return true;

        default:
            throw std::exception("MoveOpp: Invalid direction");
        }
    }
    else
        throw std::exception("MoveOpp: Invalid action, pawn not on board");
}

bool Player::Move(Player& opponent, int direction)
{
    int i, j;

    if (GetPawn(i, j))
    {
        switch (direction)
        {
        case LEFT:
            if (opponent.MoveOpponent(direction))
            {
                if (i - 2 > 0)
                {
                    trackingGrid[i - 2][j] = PAWN;
                    trackingGrid[i][j] = VISITED;
                }
                return true;
            }
            else
            {
                SetTrackingWall(i - 1, j);
                return false;
            }
        case RIGHT:
            if (opponent.MoveOpponent(direction))
            {
                if (i + 2 < 13)
                {
                    trackingGrid[i + 2][j] = PAWN;
                    trackingGrid[i][j] = VISITED;
                }
                return true;
            }
            else
            {
                SetTrackingWall(i + 1, j);
                return false;
            }
        case UP:
            if (opponent.MoveOpponent(direction))
            {
                if (j - 2 > 0)
                {
                    trackingGrid[i][j - 2] = PAWN;
                    trackingGrid[i][j] = VISITED;
                }
                return true;
            }
            else
            {
                SetTrackingWall(i, j - 1);
                return false;
            }
        case DOWN:
            if (opponent.MoveOpponent(direction))
            {
                if (j + 2 < 13)
                {
                    trackingGrid[i][j + 2] = PAWN;
                    trackingGrid[i][j] = VISITED;
                }
                return true;
            }
            else
            {
                SetTrackingWall(i, j + 1);
                return false;
            }

        default:
            throw std::exception("Move: Invalid direction");
        }
    }
    else
        throw std::exception("Move: Invalid action, pawn not on board");
}

void Player::RetreatOpponent()
{
    int i, j;
    if (GetOpponentPawn(i, j))
        mazeGrid[i][j] = VISITED;
    opponentOnBoard = false;
}

void Player::Retreat(Player& opponent)
{
    int i, j;
    if (GetPawn(i, j))
        trackingGrid[i][j] = VISITED;
    opponent.RetreatOpponent();
    onBoard = false;
}

bool Player::PawnOnBoard()
{
    return onBoard;
}