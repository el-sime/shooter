/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "screens.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static Vector2 cursorPosition;
static Vector2 playerPosition;
static int playerSize = 24;
static int playerGunLenght = 12;
static float playerSpeed = 0.3f;
//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------
void DrawCursor()
{
    DrawRectangle(cursorPosition.x - 15, cursorPosition.y - 3, 12, 6, RED);
    DrawRectangle(cursorPosition.x + 3, cursorPosition.y - 3, 12, 6, RED);
    DrawRectangle(cursorPosition.x - 3, cursorPosition.y - 15, 6, 12, RED);
    DrawRectangle(cursorPosition.x - 3, cursorPosition.y + 3, 6, 12, RED);
}

void DrawPlayer()
{
    /*
        Calculate the angle of the gun
        atan2 gives the tangent of the angle of the point from the origin
        the origin needs to be the player position
    */
    
    int tanX = playerPosition.x > cursorPosition.x ? -(playerPosition.x - cursorPosition.x) : cursorPosition.x - playerPosition.x;
    int tanY = playerPosition.y > cursorPosition.y ? playerPosition.y - cursorPosition.y : 0;
    double ang = atan2(tanY, tanX);
    double gunX = playerPosition.x + sin(ang)  * playerGunLenght;
    double gunY = playerPosition.y - cos(ang) * playerGunLenght;    
    DrawText(TextFormat("tanX : %d; cos: %d", tanX, tanY), 10, 10, 10, WHITE);
    DrawText(TextFormat("ang : %f; rad: %f ",ang * 180 / PI, ang), 10, 20, 10, WHITE);
    DrawLine(playerPosition.x, playerPosition.y, gunX, gunY, RED);
    
    
    DrawCircle(playerPosition.x, playerPosition.y, playerSize / 2, RED);
    
    DrawLine(playerPosition.x, playerPosition.y, cursorPosition.x, cursorPosition.y, GRAY);
    DrawLine(playerPosition.x, playerPosition.y, cursorPosition.x, playerPosition.y, GRAY);
    DrawLine(cursorPosition.x, cursorPosition.y, cursorPosition.x, playerPosition.y, GRAY);
    DrawCircle(gunY, gunX, 1, GREEN);

}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;
    cursorPosition.x = (GetScreenWidth() / 2);
    cursorPosition.y = (GetScreenHeight() / 2);
    playerPosition.x = GetScreenWidth() / 2;
    playerPosition.y = GetScreenHeight() - playerSize / 2;
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // TODO: Update GAMEPLAY screen variables here!
    cursorPosition.x = GetMouseX();
    cursorPosition.y = GetMouseY();
    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER) )
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }

    if (IsKeyDown(KEY_A))
    {
        int newX = playerPosition.x - playerSpeed * GetFrameTime();
        if (newX >= playerSize / 2) playerPosition.x = newX;
    }
    if (IsKeyDown(KEY_D))
    {
        int newX = playerPosition.x + playerSpeed * GetFrameTime();
        if (newX <= GetScreenWidth()) playerPosition.x = newX;
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    DrawCursor();
    DrawPlayer();
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}