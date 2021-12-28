#include "olcConsoleGameEngine.h"
#include "Player.h"
#include "UnicodeCharacters.h"
#include "AStar.h"

#include <string>
#include <ctime>

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

    If necessary, players can move back off of the board and re-enter from another square on the left-most column.

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

    void PlaceRandWalls(Player& player)
    {
        int i, j;
        while (player.GetMazeWallCount() < player.maxWalls)
        {
            i = rand() % 6;
            j = rand() % 6;
            if (rand() % 2)
                player.SetMazeWall(i * 2 + 1, j * 2);
            else
                player.SetMazeWall(i * 2, j * 2 + 1);
        }
    }

    void PlaceRandObjective(Player& player)
    {
        int i, j;
        i = rand() % 3;
        j = rand() % 6;
        player.SetObjective((i + 3) * 2 + 1, j * 2 + 1);
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

        //srand(3);
        //PlaceRandWalls(player1);
        //PlaceRandObjective(player1);

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime)
    {
        static int enterLocation = 1;
        static bool startPressed = false;
        static int directionP1 = RIGHT;
        static int directionP2 = RIGHT;
        static bool retreat = false;
        static bool confirm = false;
        static int turn = 0;
        static bool turnOver = false;
        static float excTimeCtr = 0;
        static bool winner = false;
        static bool boardValid = false;
        static float delayP2 = 0.5f;

        // Timed messages
        static int timedMessageLen = 0;
        const float timedMessageTimeout = 5.0f;
        excTimeCtr += fElapsedTime;

        // INPUT
            // Setup
        if (gameState == 0 && m_mouse[0].bPressed)
        {
            // check if clicked on the "start!" button
            if (player1.GetMazeWallCount() == player1.maxWalls &&
                MouseInBox(trackingOffsetX, 2, trackingOffsetX + 7, 4))
            {
                AStar as = AStar(player1);
                boardValid = as.Pathfind();
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
                if ((m_keys['w'].bPressed || m_keys['W'].bPressed))
                    enterLocation += 10;
                if ((m_keys['s'].bPressed || m_keys['S'].bPressed))
                    enterLocation += 2;
                // this will cause wrapping when pressing 'up' at the top or 'down' at the bottom
                enterLocation = abs(enterLocation % 12);
            }
            else
            {
                if (m_keys['w'].bPressed || m_keys['W'].bPressed)
                    directionP1 = UP;
                if (m_keys['s'].bPressed || m_keys['S'].bPressed)
                    directionP1 = DOWN;
                if (m_keys['a'].bPressed || m_keys['A'].bPressed)
                    directionP1 = LEFT;
                if (m_keys['d'].bPressed || m_keys['D'].bPressed)
                    directionP1 = RIGHT;
                if (m_keys['r'].bPressed || m_keys['R'].bPressed)
                    retreat = !retreat;
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
                // Back off of the board
                if (retreat)
                {
                    player1.Retreat(player2);
                    retreat = false;
                }
                else
                {
                    // Move, if not blocked turn continues
                    try
                    {
                        if (!player1.Move(player2, directionP1))
                            turnOver = true;
                    }
                    catch (exception e)
                    {
                        DrawCString(trackingOffsetX, trackingOffsetY + UIScale * 13, e.what(), FG_RED);
                        timedMessageLen = strlen(e.what());
                        excTimeCtr = 0;
                    }
                    catch (...)
                    {
                        exit(143);
                    }
                }
            }
        }
            // Player 2's turn (AI)
        else if (gameState == 1 && turn == 1)
        {
            // turnOver = true;

            int enterLocations[6] = { 1, 3, 5, 7, 9, 11 };
            bool locationTried[6] = { false, false, false, false, false, false };
            int ind;

            // Small delay to keep player1 from seeing what the opponent does
            if (delayP2 > 0)
                delayP2 -= fElapsedTime;

            // Enter board
            else if (!player2.PawnOnBoard())
            {
                // Select entry point randomly
                do
                {
                    ind = rand() % 6;
                } while (locationTried[ind]);

                if (!player2.Enter(player1, enterLocations[ind]))
                    turnOver = true;
                else
                    directionP2 = RIGHT;
                delayP2 = 0.5f;
            }
            // Move
            else
            {
                if (!player2.NavigateMaze(directionP2, player1))
                    turnOver = true;
                delayP2 = 0.5f;
            }
        }

        // DRAWING

            // Text
        if (timedMessageLen && excTimeCtr > timedMessageTimeout)
        {
            DrawLine(trackingOffsetX, trackingOffsetY + UIScale * 13, trackingOffsetX + timedMessageLen, trackingOffsetY + UIScale * 13, ' ');
            timedMessageLen = 0;
        }

        if (gameState == 0)
        {
            DrawString(mazeOffsetX, 2, L"SET UP A MAZE IN THIS GRID");
            DrawString(mazeOffsetX, 3, L"WALLS: " + to_wstring(player1.maxWalls - player1.GetMazeWallCount())
                + L" ");
            if (startPressed && boardValid)
            {
                DrawString(mazeOffsetX, 2, L"                          ");
                DrawString(mazeOffsetX, 3, L"         ");
            }
            else if (startPressed)
            {
                const char* msg = "OBJECTIVE NOT REACHABLE";
                DrawCString(trackingOffsetX, trackingOffsetY + UIScale * 13, msg, FG_RED);
                timedMessageLen = strlen(msg);
                excTimeCtr = 0;
                startPressed = false;
            }
        }
        else if (gameState == 1)
        {
            if (turn == 0)
            {
                if (!player1.PawnOnBoard())
                {
                    DrawString(trackingOffsetX, 2, L"ENTER THE BOARD FROM THE LEFT");
                    DrawString(trackingOffsetX, 3, L"USE W AND S TO SELECT A SQUARE");
                }
                else if (retreat)
                {
                    DrawString(trackingOffsetX, 2, L"DO YOU WISH TO RETREAT?      ");
                    DrawString(trackingOffsetX, 3, L"CANCEL WITH R OR CONFIRM      ");
                }
                else
                {
                    DrawString(trackingOffsetX, 2, L"LOOK FOR THE MAZE EXIT       ");
                    DrawString(trackingOffsetX, 3, L"USE WASD TO SELECT A DIRECTION");
                }
            }
            else
            {
                DrawString(trackingOffsetX, 2, L"PLAYER 2'S TURN              ");
                DrawString(trackingOffsetX, 3, L"                              ");
            }
        }
        else if (gameState == 2)
        {
            DrawString(trackingOffsetX, 2, L"                             ");
            DrawString(trackingOffsetX, 3, L"                              ");
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
                        if (gameState == 0 && MouseOnBorder(j, i))
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

        // GAME OVER MESSAGE
        if (gameState == 2)
        {
            DrawBox(mazeOffsetX - 9, trackingOffsetY + UIScale * 13 / 2 - 1, mazeOffsetX + 6, trackingOffsetY + UIScale * 13 / 2 + 1, FG_GREEN);

            if (winner)
                DrawString(mazeOffsetX - 8, trackingOffsetY + UIScale * 13 / 2, L"PLAYER 1 WINS!");
            else
                DrawString(mazeOffsetX - 8, trackingOffsetY + UIScale * 13 / 2, L"PLAYER 2 WINS!");
        }

        // STATE UPDATES
        if (startPressed)
        {
            startPressed = false;
            gameState = 1;
            bool res;
            do 
            {
                player2.PlaceRandWalls();
                player2.PlaceRandObjective();
                AStar as = AStar(player2);
                res = as.Pathfind();
            } while (!res);
        }

        if (turnOver)
        {
            turn = (turn + 1) % 2;
            turnOver = false;
        }

            // gameover conditions
        if (player1.OpponentWon() || player2.OpponentWon())
        {
            winner = player2.OpponentWon();
            gameState = 2;
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