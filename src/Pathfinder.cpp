#include "olcConsoleGameEngine.h"
#include "Player.h"
#include "UnicodeCharacters.h"

#include <string>

using namespace std;

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
    void DrawBox(int x1, int y1, int x2, int y2, short col = FG_WHITE)
    {
        DrawLine(x1 + 1, y1, x2 - 1, y1, HORIZONTAL_L, col);
        DrawLine(x1 + 1, y2, x2 - 1, y2, HORIZONTAL_L, col);
        DrawLine(x1, y1 + 1, x1, y2 - 1, VERTICAL_L, col);
        DrawLine(x2, y1 + 1, x2, y2 - 1, VERTICAL_L, col);
        Draw(x1, y1, CORNER_L_DOWN_RIGHT, col);
        Draw(x1, y2, CORNER_L_UP_RIGHT, col);
        Draw(x2, y1, CORNER_L_DOWN_LEFT, col);
        Draw(x2, y2, CORNER_L_UP_LEFT, col);
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

    bool MouseOnBorder(int i, int j)
    {
        int mouseX = GetMouseX(), mouseY = GetMouseY();
        return (
            i % 2 == 0
            && mouseX == mazeOffsetX + i * UIScale
            && (mouseY >= mazeOffsetY + j * UIScale - UIScale / 2 && mouseY <= mazeOffsetY + j * UIScale + UIScale / 2)
            ||
            i % 2 == 1
            && (mouseX >= mazeOffsetX + i * UIScale - UIScale / 2 && mouseX <= mazeOffsetX + i * UIScale + UIScale / 2)
            && mouseY == mazeOffsetY + j * UIScale
            );
    }

    bool MouseOnSquare(int i, int j, int offsetX, int offsetY)
    {
        int mouseX = GetMouseX(), mouseY = GetMouseY();
        //return (mouseX == offsetX + i * UIScale && mouseY == offsetY + j * UIScale);
        return (
            mouseX >= offsetX + i * UIScale - UIScale / 2 && mouseX <= offsetX + i * UIScale + UIScale / 2 &&
            mouseY >= offsetY + j * UIScale - UIScale / 2 && mouseY <= offsetY + j * UIScale + UIScale / 2);
    }

    bool MouseInBox(int x1, int y1, int x2, int y2)
    {
        int mouseX = GetMouseX(), mouseY = GetMouseY();
        return (mouseX >= x1 && mouseX <= x2 && mouseY >= y1 && mouseY <= y2);
    }

    void DrawCString(int x, int y, const char* c, short col = 0x000F)
    {
        for (size_t i = 0; c[i] != '\0'; i++)
        {
            Draw(x + i, y, c[i], col);
        }
    }

    virtual bool OnUserCreate()
    {
        m_sAppName = L"Pathfinder";

        gameState = 0;

        trackingOffsetX = 2;
        trackingOffsetY = 2 * UIScale + 3;
        mazeOffsetX = 13 * UIScale + 6 - UIScale;
        mazeOffsetY = 2 * UIScale + 3;

        // move to OnUserUpdate if adding a start or endscreen
        DrawString(mazeOffsetX - 6, 0, L"PATHFINDER");
        DrawBox(trackingOffsetX - 1, trackingOffsetY - 1, trackingOffsetX - 1 + 13 * UIScale + 2 - UIScale, trackingOffsetY - 1 + 13 * UIScale + 2 - UIScale);
        DrawBox(mazeOffsetX - 1, mazeOffsetY - 1, mazeOffsetX - 1 + 13 * UIScale + 2 - UIScale, mazeOffsetY - 1 + 13 * UIScale + 2 - UIScale);

        // For debugging/testing
        //for (int i = 0; i < 13; i++)
        //{
        //    for (int j = 0; j < 13; j++)
        //    {
        //        if (i % 2 != j % 2 && (i % 3 == 1 || j % 4 == 3))
        //            player1.SetMazeWall(i, j);
        //    }
        //}
        //player1.SetTrackingWall(0, 1);
        //player1.SetTrackingWall(1, 0);
        //player1.SetTrackingWall(11, 12);
        //player1.SetTrackingWall(12, 11);
        //player1.SetObjective(11, 11);
        //player1.SetPawn(3, 1);
        //player1.SetVisited(1, 1);
        player2.SetMazeWall(2, 1);
        player2.SetMazeWall(2, 3);
        player2.SetMazeWall(2, 7);
        player2.SetMazeWall(2, 9);
        player2.SetMazeWall(3, 0);
        player2.SetMazeWall(3, 2);
        player2.SetMazeWall(3, 8);
        player2.SetMazeWall(3, 10);
        player2.SetMazeWall(4, 5);
        player2.SetMazeWall(5, 2);
        player2.SetMazeWall(5, 4);
        player2.SetMazeWall(5, 8);
        player2.SetMazeWall(5, 10);
        player2.SetMazeWall(6, 1);
        player2.SetMazeWall(6, 7);
        player2.SetMazeWall(7, 0);
        player2.SetMazeWall(7, 4);
        player2.SetMazeWall(7, 6);
        player2.SetMazeWall(7, 10);
        player2.SetMazeWall(8, 5);
        player2.SetMazeWall(8, 9);
        player2.SetMazeWall(8, 11);
        player2.SetMazeWall(9, 2);
        player2.SetMazeWall(9, 8);
        player2.SetMazeWall(10, 1);
        player2.SetMazeWall(10, 3);
        player2.SetMazeWall(10, 7);
        player2.SetMazeWall(11, 2);
        player2.SetMazeWall(11, 4);
        player2.SetMazeWall(11, 10);
        player2.SetObjective(3, 9);

        player1.SetMazeWall(2, 1);
        player1.SetMazeWall(2, 3);
        player1.SetMazeWall(2, 7);
        player1.SetMazeWall(2, 9);
        player1.SetMazeWall(3, 0);
        player1.SetMazeWall(3, 2);
        player1.SetMazeWall(3, 8);
        player1.SetMazeWall(3, 10);
        player1.SetMazeWall(4, 5);
        player1.SetMazeWall(5, 2);
        player1.SetMazeWall(5, 4);
        player1.SetMazeWall(5, 8);
        player1.SetMazeWall(5, 10);
        player1.SetMazeWall(6, 1);
        player1.SetMazeWall(6, 7);
        player1.SetMazeWall(7, 0);
        player1.SetMazeWall(7, 4);
        player1.SetMazeWall(7, 6);
        player1.SetMazeWall(7, 10);
        player1.SetMazeWall(8, 5);
        player1.SetMazeWall(8, 9);
        player1.SetMazeWall(8, 11);
        player1.SetMazeWall(9, 2);
        player1.SetMazeWall(9, 8);
        player1.SetMazeWall(10, 1);
        player1.SetMazeWall(10, 3);
        player1.SetMazeWall(10, 7);
        player1.SetMazeWall(11, 2);
        player1.SetMazeWall(11, 4);
        player1.SetMazeWall(11, 10);
        player1.SetObjective(3, 9);

        //player1.SetTrackingObjective(5, 5);

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime)
    {
        static int enterLocation = 1;
        static bool startPressed = false;
        static int direction = -1;
        static bool confirm = false;
        static int turn = 0;
        static bool turnOver = false;
        static float timeCtr = 0;

        // Timed messages
        static int timedMessageLen = 0;
        const float timedMessageTimeout = 5.0;
        timeCtr += fElapsedTime;

        // INPUT
            // Setup
        if (gameState == 0 && m_mouse[0].bPressed)
        {
            // check if clicked on the "start!" button
            if (player1.GetMazeWallCount() == player1.maxWalls &&
                MouseInBox(trackingOffsetX, 2, trackingOffsetX + 7, 4))
            {
                // TODO: Verify the maze is solvable
                startPressed = true;
            }
            else
            {
                // check if clicked on a valid wall or objective spot
                for (int i = 0; i < 13; i++)
                {
                    for (int j = 0; j < 13; j++)
                    {
                        if (i % 2 != j % 2 && MouseOnBorder(i, j))
                        {
                            player1.ToggleWall(i, j);
                        }
                        else if (i % 2 && j % 2 && MouseOnSquare(i, j, mazeOffsetX, mazeOffsetY))
                        {
                            player1.ToggleObjective(i, j);
                        }
                    }
                }
            }
        }
        else if (gameState == 1)
        {
            // Direction
            if (!player1.PawnOnBoard())
            {
                if ((m_keys['w'].bPressed || m_keys['W'].bPressed) && enterLocation > 1)
                    enterLocation -= 2;
                if ((m_keys['s'].bPressed || m_keys['S'].bPressed) && enterLocation < 11)
                    enterLocation += 2;
            }
            else
            {
                if (m_keys['w'].bPressed || m_keys['W'].bPressed)
                    direction = UP;
                if (m_keys['s'].bPressed || m_keys['S'].bPressed)
                    direction = DOWN;
                if (m_keys['a'].bPressed || m_keys['A'].bPressed)
                    direction = LEFT;
                if (m_keys['d'].bPressed || m_keys['D'].bPressed)
                    direction = RIGHT;
            }

            // Confirm
            if (m_keys[' '].bPressed || m_keys['\r'].bPressed /*|| m_mouse[0].bPressed*/)
                confirm = true;
            else
                confirm = false;
        }

        // LOGIC
            // Player 1's turn
        if (gameState == 1 && turn == 0)
        {
            // Enter board
            if (!player1.PawnOnBoard() && confirm)
            {
                if (!player1.Enter(player2, enterLocation))
                    turnOver = true;
            }
            // Move
            else if (confirm)
            {
                // Move, if not blocked turn continues
                try
                {
                    if (!player1.Move(player2, direction))
                        turnOver = true;
                }
                catch (exception e)
                {
                    DrawCString(trackingOffsetX, trackingOffsetY + UIScale * 13, e.what(), FG_RED);
                    timedMessageLen = strlen(e.what());
                    timeCtr = 0;
                }
                catch (...)
                {
                    exit(143);
                }
            }

        }
            // Player 2's turn (AI)
        else if (gameState == 1 && turn == 1)
        {
            // Skip turn
            turnOver = true;
        }

        // DRAWING

            // Text
        if (timedMessageLen && timeCtr > timedMessageTimeout)
        {
            DrawLine(trackingOffsetX, trackingOffsetY + UIScale * 13, trackingOffsetX + timedMessageLen, trackingOffsetY + UIScale * 13, ' ');
            timedMessageLen = 0;
        }

        if (gameState == 0)
        {
            DrawString(mazeOffsetX, 2, L"SET UP A MAZE IN THIS GRID");
            DrawString(mazeOffsetX, 3, L"WALLS: " + to_wstring(player1.maxWalls - player1.GetMazeWallCount())
                + L" ");
            if (startPressed)
            {
                DrawString(mazeOffsetX, 2, L"                          ");
                DrawString(mazeOffsetX, 3, L"         ");
            }
        }
        else if (gameState == 1)
        {

            if (!player1.PawnOnBoard())
            {
                DrawString(trackingOffsetX, 2, L"ENTER THE BOARD FROM THE LEFT");
                DrawString(trackingOffsetX, 3, L"USE W AND S TO SELECT A SQUARE");
            }
            else
            {
                DrawString(trackingOffsetX, 2, L"LOOK FOR THE MAZE EXIT       ");
                DrawString(trackingOffsetX, 3, L"USE WASD TO SELECT A DIRECTION");
            }
        }

            // Buttons
        if (gameState == 0 && player1.GetMazeWallCount() == player1.maxWalls && !startPressed)
        {
            DrawBox(trackingOffsetX, 2, trackingOffsetX + 7, 4, FG_BLUE);
            DrawString(trackingOffsetX + 1, 3, L"START!");
        } else if (gameState == 0)
        {
            DrawBox(trackingOffsetX, 2, trackingOffsetX + 7, 4, FG_BLACK);
            DrawString(trackingOffsetX + 1, 3, L"      ");
        }

            // Boards
        int piece;
        for (int i = 0; i < 13; i++)
        {
            for (int j = 0; j < 13; j++)
            {
                // Maze Grid
                if (piece = player1.GetMazeCell(j, i))
                {
                    if (i % 2 != 0 && j % 2 != 0)
                    {
                        switch (piece)
                        {
                        case OBJECTIVE:
                            Draw(mazeOffsetX + j * UIScale, mazeOffsetY + i * UIScale, 'O', FG_GREEN);
                            break;
                        case PAWN:
                            Draw(mazeOffsetX + j * UIScale, mazeOffsetY + i * UIScale, 'O', FG_YELLOW);
                            break;
                        case VISITED:
                            Draw(mazeOffsetX + j * UIScale, mazeOffsetY + i * UIScale, 'O', FG_RED);
                            break;
                        }
                    }
                    else if (i % 2 != j % 2 && piece == Pieces::WALL)
                    {
                        // Checks if the mouse is hovering over the wall
                        if (gameState == 0 && MouseOnBorder(i, j))
                            DrawWall(j, i, mazeOffsetX, mazeOffsetY, 0, FG_RED);
                        else
                            DrawWall(j, i, mazeOffsetX, mazeOffsetY);
                    }
                }
                // Setup phase border highlighting for wall placement
                else if (i % 2 != j % 2)
                {
                    // Checks if the mouse is hovering over the border
                    if (gameState == 0 && MouseOnBorder(j, i) && player1.GetMazeWallCount() < player1.maxWalls)
                        DrawWall(j, i, mazeOffsetX, mazeOffsetY, PIXEL_QUARTER);
                    else
                        DrawWall(j, i, mazeOffsetX, mazeOffsetY, ' ');
                }
                // Setup square highlighting for objective placement
                else if (i % 2 && j % 2)
                {
                    if (gameState == 0 && MouseOnSquare(j, i, mazeOffsetX, mazeOffsetY))
                        Draw(mazeOffsetX + j * UIScale, mazeOffsetY + i * UIScale, PIXEL_QUARTER, FG_GREEN);
                    else
                        Draw(mazeOffsetX + j * UIScale, mazeOffsetY + i * UIScale, ' ');
                }

                // Tracking Grid
                if ((piece = player1.GetTrackingCell(j, i)) > 0)
                {
                    if (i % 2 != 0 && j % 2 != 0)
                    {
                        switch (piece)
                        {
                        case OBJECTIVE:
                            Draw(trackingOffsetX + j * UIScale, trackingOffsetY + i * UIScale, 'O', FG_GREEN);
                            break;
                        case PAWN:
                            Draw(trackingOffsetX + j * UIScale, trackingOffsetY + i * UIScale, 'O', FG_YELLOW);
                            break;
                        case VISITED:
                            Draw(trackingOffsetX + j * UIScale, trackingOffsetY + i * UIScale, 'O', FG_RED);
                            break;
                        }
                    }
                    else if (i % 2 != j % 2 && piece == Pieces::WALL)
                        DrawWall(j, i, trackingOffsetX, trackingOffsetY);
                }
            }

            // Enter board indicator
            if (gameState == 1 && i == enterLocation && !player1.PawnOnBoard())
            {
                Draw(trackingOffsetX - 2, trackingOffsetY + i * UIScale, '>');
            }
            else
                Draw(trackingOffsetX - 2, trackingOffsetY + i * UIScale, ' ');
        }

        // STATE UPDATES
        if (startPressed)
        {
            startPressed = false;
            gameState = 1;
        }

        if (turnOver)
        {
            turn = !turn;
            turnOver = false;
        }

        return true;
    }

private:
    Player player1, player2;
    int gameState;

    const int UIScale = 2;
    int trackingOffsetX, trackingOffsetY;
    int mazeOffsetX, mazeOffsetY;
};

int main()
{
    Pathfinder game;
    game.ConstructConsole(57, 40, 12, 12);
    game.Start();

    return 0;
}