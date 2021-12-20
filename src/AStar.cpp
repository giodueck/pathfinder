#include "AStar.h"

int _abs(int n)
{
    if (n < 0) return -n;
    else return n;
}

AStar::AStar(Player player)
{
    for (int i = 0; i < 13; i++)
        for (int j = 0; j < 13; j++)
        {
            mazeGrid[i][j] = player.GetMazeCell(i, j);
            if (mazeGrid[i][j] == OBJECTIVE)
            {
                obji = i;
                objj = j;
            }
        }

    // just in case. The pathfinding will take place on the own maze, so we use opponent methods
    RetreatOpponent();
}

bool AStar::SquareInList(int i, int j, std::vector<ASSquare>& list)
{
    ASSquare s;
    for (int k = 0; k < int(list.size()); k++)
    {
        s = list.at(k);
        if (s.i == i && s.j == j)
            return true;
    }
    return false;
}

ASSquare AStar::GetSquare(int i, int j, std::vector<ASSquare>& list)
{
    for (int k = 0; k < int(list.size()); k++)
    {
        if (list[k].i == i && list[k].j == j)
            return list[k];
    }
    throw "GetSquare: Square not found";
}

int AStar::GetSquareIndex(int i, int j, std::vector<ASSquare>& list)
{
    for (int k = 0; k < int(list.size()); k++)
    {
        if (list[k].i == i && list[k].j == j)
            return k;
    }
    return -1;
}

void AStar::AddAdjacentSquares(ASSquare& parent)
{
    ASSquare t;
    int i = parent.i, j = parent.j;
    
    // Left
    if (i - 2 >= 0 && !SquareInList(i - 2, j, closed) && mazeGrid[i - 1][j] != WALL)
    {
        if (!SquareInList(i - 2, j, open))
        {
            t.i = i - 2;
            t.j = j;
            t.pi = parent.i;
            t.pj = parent.j;
            t.F = _abs(obji - t.i) + _abs(objj - t.j);
            open.push_back(t);
        } else if (GetSquare(i - 2, j, open).F > _abs(obji - (i - 2)) + _abs(objj - j))
        {
            t = open[GetSquareIndex(i - 2, j, open)];
            open.erase(open.begin() + GetSquareIndex(i - 2, j, open));
            t.pi = parent.i;
            t.pj = parent.j;
            t.F = _abs(obji - t.i) + _abs(objj - t.j);
            open.push_back(t);
        }
    }

    // Right
    if (i + 2 < 13 && !SquareInList(i + 2, j, closed) && mazeGrid[i + 1][j] != WALL)
    {
        if (!SquareInList(i + 2, j, open))
        {
            t.i = i + 2;
            t.j = j;
            t.pi = parent.i;
            t.pj = parent.j;
            t.F = _abs(obji - t.i) + _abs(objj - t.j);
            open.push_back(t);
        } else if (GetSquare(i + 2, j, open).F > _abs(obji - (i + 2)) + _abs(objj - j))
        {
            t = open[GetSquareIndex(i + 2, j, open)];
            open.erase(open.begin() + GetSquareIndex(i + 2, j, open));
            t.pi = parent.i;
            t.pj = parent.j;
            t.F = _abs(obji - t.i) + _abs(objj - t.j);
            open.push_back(t);
        }
    }

    // Up
    if (j - 2 >= 0 && !SquareInList(i, j - 2, closed) && mazeGrid[i][j - 1] != WALL)
    {
        if (!SquareInList(i, j - 2, open))
        {
            t.i = i;
            t.j = j - 2;
            t.pi = parent.i;
            t.pj = parent.j;
            t.F = _abs(obji - t.i) + _abs(objj - t.j);
            open.push_back(t);
        } else if (GetSquare(i, j - 2, open).F > _abs(obji - i) + _abs(objj - (j - 2)))
        {
            t = open[GetSquareIndex(i, j - 2, open)];
            open.erase(open.begin() + GetSquareIndex(i, j - 2, open));
            t.pi = parent.i;
            t.pj = parent.j;
            t.F = _abs(obji - t.i) + _abs(objj - t.j);
            open.push_back(t);
        }
    }

    // Down
    if (j + 2 < 13 && !SquareInList(i, j + 2, closed) && mazeGrid[i][j + 1] != WALL)
    {
        if (!SquareInList(i, j + 2, open))
        {
            t.i = i;
            t.j = j + 2;
            t.pi = parent.i;
            t.pj = parent.j;
            t.F = _abs(obji - t.i) + _abs(objj - t.j);
            open.push_back(t);
        } else if (GetSquare(i, j + 2, open).F > _abs(obji - i) + _abs(objj - (j + 2)))
        {
            t = open[GetSquareIndex(i, j + 2, open)];
            open.erase(open.begin() + GetSquareIndex(i, j + 2, open));
            t.pi = parent.i;
            t.pj = parent.j;
            t.F = _abs(obji - t.i) + _abs(objj - t.j);
            open.push_back(t);
        }
    }
}

bool AStar::Pathfind()
{
    ASSquare s;
    int lowestFIndex;
    for (int l = 0; l < 6; l++)
    {
        // Attempt to enter the maze
        if (EnterOpponent(l * 2 + 1))
        {
            closed.clear();
            open.clear();

            // First square
            s.i = 1;
            s.j = l * 2 + 1;
            s.pi = -1;
            s.pj = -1;
            s.F = _abs(obji - s.i) + _abs(objj - s.j);
            closed.push_back(s);

            // Add adjacent squares to open list
            AddAdjacentSquares(closed.back());

            // Start algorithm
            while (!open.empty())
            {
                // Look for lowest cost sqaure on open list
                lowestFIndex = 0;
                for (int m = 1; m < int(open.size()); m++)
                {
                    if (open.at(m).F < open.at(lowestFIndex).F)
                        lowestFIndex = m;
                }

                // drop from open list and add to closed
                s = open.at(lowestFIndex);
                open.erase(open.begin() + lowestFIndex);
                closed.push_back(s);

                // add adjacent squares if objective not reached yet
                if (s.F != 0)
                    AddAdjacentSquares(closed.back());
                else
                    open.clear();
            }

            // Check if objective was found
            RetreatOpponent();
            if (closed.back().F == 0)
                return true;
            else
                closed.clear();
        }
    }

    return false;
}