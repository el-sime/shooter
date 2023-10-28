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

#define MAX_BULLETS  640 //640 bullets ought to be enough for anyone

typedef struct Bullets {
    Vector2 origin;
    Vector2 position;
    Vector2 targetPosition;
    float distance;
    float speed;
} Bullet;

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static Vector2 cursorPosition;
static Vector2 playerPosition;
static int playerSize = 24;
static int playerGunLenght = 24;
static float playerSpeed = 150.0f;
static float playerProjectileSpeed = 300.0f;
static Bullet bullets[MAX_BULLETS];
// Keep track of how many bullets are flying around
// Need to be decreased every time a bullet disappears!
static int bulletCounter = 0;

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

Vector2 GetPointOnTrajectory(Vector2 origin, Vector2 target, float distance)
{
    Vector2 point;
    double tanX = origin.x > target.x ? -(origin.x - target.x) : target.x - origin.x;
    double tanY = origin.y > target.y ? origin.y - target.y : target.y - origin.y;
    double angle = atan2(tanY, tanX);
    point.x = origin.x + cos(angle) * distance;
    point.y = origin.y + sin(-angle) * distance;
    return point;
}


void DrawPlayer()
{
    /*
        Calculate the angle of the gun
        atan2 gives the angle of the point from the origin (in radians)
        the origin needs to be the player position
    */
    
    int tanX = playerPosition.x > cursorPosition.x ? -(playerPosition.x - cursorPosition.x) : cursorPosition.x - playerPosition.x;
    int tanY = playerPosition.y > cursorPosition.y ? playerPosition.y - cursorPosition.y : 0;
    double ang = atan2(tanY, tanX);

    double gunX = playerPosition.x + cos(ang)  * playerGunLenght;
    double gunY = playerPosition.y - sin(ang) * playerGunLenght;    
    DrawCircle(playerPosition.x, playerPosition.y, playerSize / 2, RED);
    DrawLine(playerPosition.x, playerPosition.y, gunX, gunY, RED);
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

void DeleteBullet(int bulletIndex)
{
    if (bulletIndex > bulletCounter) return; //this shouldn't be possible, right?
    for (int i = bulletIndex; i < bulletCounter - 1; i++)
    {
        bullets[i] = bullets[i + 1];
    }
    bulletCounter--;
    return;
}

void UpdateBullets()
{
    for (int b = 0; b < bulletCounter; b++)
    {
        bullets[b].distance += playerProjectileSpeed * GetFrameTime();
        Vector2 newBulletPosition = GetPointOnTrajectory(bullets[b].origin, bullets[b].targetPosition, bullets[b].distance);
        // check collisions
        // check out of screen
        if (
            newBulletPosition.x < 0 ||
            newBulletPosition.x > GetScreenWidth() ||
            newBulletPosition.y < 0 ||
            newBulletPosition.y > GetScreenHeight()
            )
        {
            DeleteBullet(b);
        }
        else 
        {
            bullets[b].position = newBulletPosition;
        }
    }
    return;
}

void Fire(Vector2 origin, float speed, Vector2 target)
{
    if(bulletCounter < MAX_BULLETS)
    {
        Bullet newBullet;
        newBullet.origin = origin;
        newBullet.position = origin;
        newBullet.targetPosition = target;
        newBullet.distance = 0;
        newBullet.speed = playerProjectileSpeed;
        bullets[bulletCounter++] = newBullet;
    }
    return;
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    float dt = GetFrameTime();
    cursorPosition.x = GetMouseX();
    cursorPosition.y = GetMouseY();
    
    if (IsKeyDown(KEY_A))
    {
        float newX = playerPosition.x - playerSpeed * dt;
        if (newX >= playerSize / 2) playerPosition.x = newX;
    }
    
    if (IsKeyDown(KEY_D))
    {
        float newX = playerPosition.x + playerSpeed * dt;
        if (newX <= GetScreenWidth()) playerPosition.x = newX;
    }

    if (IsMouseButtonPressed(0))
    {
        // fire!
        Fire(playerPosition, playerProjectileSpeed, cursorPosition);
    }
    UpdateBullets();
}

void DrawBullets()
{
    for (int b = 0; b < bulletCounter; b++)
    {
        DrawCircle(bullets[b].position.x, bullets[b].position.y, 4, WHITE);
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    DrawCursor();
    DrawPlayer();
    DrawBullets();

    DrawText(
        TextFormat("Bullets count:%d", bulletCounter), 
        12, 24, 
        24, 
        RAYWHITE
    );
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