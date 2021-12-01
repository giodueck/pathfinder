#include "olcConsoleGameEngine.h"
#include "Player.h"
#include "UnicodeCharacters.h"

/*
    Pathfinder (1974)
    6x6 board with borders around and between cells
    2 players

    Pathfinder is a game where players race to navigate through the maze that their opponent has
    set up on their side of the board.

    Each player gets a maze grid and a tracking grid. The former is used to create the maze the opponent
    has to navigate, and the latter to track the progress through their opponents maze.

    Each player uses 30 barrier pieces to create a maze placing them between squares.
    
    Then each player places a hidden pawn, this is their opponents objective. The objective must
    be placed so that at least one route can be traced to it from a square on the left-most column.

    The movement through the opponents maze is kept track of using a tall yellow pawn.

    At the start of the game the starting player asks if they can enter the board from a square, and
    if the answer is yes (that is if there is not a border on the left side of that square) they can place
    their tall pawn in the square. They can then ask if they can move up, down, left or right, and if the
    answer is yes they can move their pawn and ask again until the answer is no. At that point the turn ends.

    On the tracking grid, a second yellow pawn keeps track of the current position, red chips are placed where
    the players already been, and barriers when blocked.

    If necessary, players can move back off the board and re-enter from another square on the left-most column.

    First one to reach their opponents hidden pawn wins.
*/

class Pathfinder : public olcConsoleGameEngine
{
public:
    void DrawBox(int x1, int y1, int x2, int y2)
    {
        DrawLine(x1 + 1, y1, x2 - 1, y1, HORIZONTAL_L);
        DrawLine(x1 + 1, y2, x2 - 1, y2, HORIZONTAL_L);
        DrawLine(x1, y1 + 1, x1, y2 - 1, VERTICAL_L);
        DrawLine(x2, y1 + 1, x2, y2 - 1, VERTICAL_L);
        Draw(x1, y1, CORNER_L_DOWN_RIGHT);
        Draw(x1, y2, CORNER_L_UP_RIGHT);
        Draw(x2, y1, CORNER_L_DOWN_LEFT);
        Draw(x2, y2, CORNER_L_UP_LEFT);
    }

    void DrawWall(int i, int j, int offsetX, int offsetY, short c = 0, short col = FG_DARK_YELLOW)
    {
        // Vertical
        if (i % 2 == 0)
        {
            if (!c) DrawLine(offsetX + i * UIScale, offsetY + j * UIScale - UIScale / 2, offsetX + i * UIScale, offsetY + j * UIScale + UIScale / 2, VERTICAL_L, col);
            else DrawLine(offsetX + i * UIScale, offsetY + j * UIScale - UIScale / 2, offsetX + i * UIScale, offsetY + j * UIScale + UIScale / 2, c, col);
        }
        // Horizontal
        else
        {
            if (!c) DrawLine(offsetX + i * UIScale - UIScale / 2, offsetY + j * UIScale, offsetX + i * UIScale + UIScale / 2, offsetY + j * UIScale, HORIZONTAL_H, col);
            else DrawLine(offsetX + i * UIScale - UIScale / 2, offsetY + j * UIScale, offsetX + i * UIScale + UIScale / 2, offsetY + j * UIScale, c, col);
        }
    }

    virtual bool OnUserCreate()
    {
        m_sAppName = L"Pathfinder";

        gameState = 0;

        UIScale = 2;

        trackingOffsetX = 1;
        trackingOffsetY = 2 * UIScale + 1;
        mazeOffsetX = 13 * UIScale + 5 - UIScale;
        mazeOffsetY = 2 * UIScale + 1;

        // move to OnUserUpdate if adding a start or endscreen
        DrawBox(trackingOffsetX - 1, trackingOffsetY - 1, trackingOffsetX - 1 + 13 * UIScale + 2 - UIScale, trackingOffsetY - 1 + 13 * UIScale + 2 - UIScale);
        DrawBox(mazeOffsetX - 1, mazeOffsetY - 1, mazeOffsetX - 1 + 13 * UIScale + 2 - UIScale, mazeOffsetY - 1 + 13 * UIScale + 2 - UIScale);

        // For debugging/testing
        for (int i = 0; i < 13; i++)
        {
            for (int j = 0; j < 13; j++)
            {
                if (i % 2 != j % 2 && (i % 3 == 1 || j % 4 == 3))
                    player1.SetMazeWall(i, j);
            }
        }
        player1.SetTrackingWall(0, 1);
        player1.SetTrackingWall(1, 0);
        player1.SetTrackingWall(11, 12);
        player1.SetTrackingWall(12, 11);
        player1.SetObjective(11, 11);
        player1.SetPawn(3, 1);
        player1.SetVisited(1, 1);

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime)
    {
        // testing mouse coordinates
        /*int x = GetMouseX(), y = GetMouseY();
        Draw(x, y, (m_mouse[0].bReleased) ? PIXEL_SOLID : PIXEL_HALF);
        return true;*/

        int mouseX = GetMouseX(), mouseY = GetMouseY();
        if (m_keys[' '].bPressed) gameState = !gameState;

        // Setup
        if (gameState == 0)
        {
            if (player1.GetMazeWallCount() < 30)
            {
                if (m_mouse[0].bReleased)
                {
                    

                    // check if clicked on a valid wall spot

                }
            }
        }

        // Gameplay
        else if (gameState == 1)
        {
            // game
        }

        // Drawing
        int piece;
        for (int i = 0; i < 13; i++)
        {
            for (int j = 0; j < 13; j++)
            {
                // Maze Grid
                if (piece = player1.GetMazeCell(i, j))
                {
                    if (i % 2 != 0 && j % 2 != 0)
                    {
                        switch (piece)
                        {
                        case OBJECTIVE:
                            Draw(mazeOffsetX + i * UIScale, mazeOffsetY + j * UIScale, 'O', FG_GREEN);
                            break;
                        case PAWN:
                            Draw(mazeOffsetX + i * UIScale, mazeOffsetY + j * UIScale, 'O', FG_YELLOW);
                            break;
                        case VISITED:
                            Draw(mazeOffsetX + i * UIScale, mazeOffsetY + j * UIScale, 'O', FG_RED);
                            break;
                        }
                    }
                    else if (i % 2 != j % 2 && piece == Pieces::WALL)
                        DrawWall(i, j, mazeOffsetX, mazeOffsetY);
                }
                // Setup phase border highlighting for wall placement
                else if (i % 2 != j % 2)
                {
                    if (gameState == 0 && (
                        i % 2 == 0
                        && mouseX == mazeOffsetX + i * UIScale
                        && (mouseY >= mazeOffsetY + j * UIScale - UIScale / 2 && mouseY <= mazeOffsetY + j * UIScale + UIScale / 2)
                        ||
                        i % 2 == 1
                        && (mouseX >= mazeOffsetX + i * UIScale - UIScale / 2 && mouseX <= mazeOffsetX + i * UIScale + UIScale / 2)
                        && mouseY == mazeOffsetY + j * UIScale
                        ))
                        DrawWall(i, j, mazeOffsetX, mazeOffsetY, PIXEL_QUARTER);
                    else
                        DrawWall(i, j, mazeOffsetX, mazeOffsetY, PIXEL_SOLID, FG_BLACK);
                }

                // Tracking Grid
                if (piece = player1.GetTrackingCell(i, j))
                {
                    if (i % 2 != 0 && j % 2 != 0)
                    {
                        switch (piece)
                        {
                        case OBJECTIVE:
                            Draw(trackingOffsetX + i * UIScale, trackingOffsetY + j * UIScale, 'O', FG_GREEN);
                            break;
                        case PAWN:
                            Draw(trackingOffsetX + i * UIScale, trackingOffsetY + j * UIScale, 'O', FG_YELLOW);
                            break;
                        case VISITED:
                            Draw(trackingOffsetX + i * UIScale, trackingOffsetY + j * UIScale, 'O', FG_RED);
                            break;
                        }
                    }
                    else if (i % 2 != j % 2 && piece == Pieces::WALL)
                        DrawWall(i, j, trackingOffsetX, trackingOffsetY);
                }
            }
        }

        return true;
    }

private:
    Player player1, player2;
    int gameState;

    int UIScale;
    int trackingOffsetX, trackingOffsetY;
    int mazeOffsetX, mazeOffsetY;
};

int main()
{
    Pathfinder game;
    game.ConstructConsole(60, 40, 12, 12);
    game.Start();

    return 0;
}