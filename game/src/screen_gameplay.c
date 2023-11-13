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
#include <time.h>
#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#include "levels.h"

#define DEBUG_SEED 1234 
#define MAX_BULLETS 640 //640 bullets ought to be enough for anyone
#define MAX_ENEMIES 30 
#define MIN_ENEMY_DISTANCE 192
#define BULLET_TYPE_PLAYER 0
#define BULLET_TYPE_ENEMY 1

typedef struct Bullets {
	Vector2 origin;
	Vector2 position;
	Vector2 targetPosition;
	float distance;
	float speed;
	Color color;
	int type;
	int damage;
} Bullet;


typedef struct Enemies {
	Vector2 position;
	int hitPoints;
	float direction;
	float speed;
	Color color;
	int lastFired;

} Enemy;

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static float elapsedTime = 0.0f;


static Vector2 cursorPosition;
static Vector2 playerPosition;
static int playerSize = 24;
static int playerGunLenght = 24;
static float playerSpeed = 150.0f;
static float playerProjectileSpeed = 300.0f;
static int playerBulletCounter = 0;
static int lastPlayerBulletSpawn = -1;
static int playerHitPoints = 3; 

static Enemy enemies[MAX_ENEMIES];
static int enemyCounter = 0;
static int gameClock = 0;
static int lastEnemySpawn = -1;
static int killCounter = 0;
static Level* currentLevel = &level1;
static Bullet bullets[MAX_BULLETS];
// Keep track of how many bullets are flying around
// Need to be decreased every time a bullet disappears!
static int bulletCounter = 0;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------
void Fire(Vector2 origin, float speed, Vector2 target, int type);
void DeleteEnemy(int enemyIndex);
/**
 * Calculate the coordinates of a point along a trajectory based on the distance
 * travelled from the origin.
 * Can go beyond the target.
**/
Vector2 GetPointOnTrajectory(Vector2 origin, Vector2 target, float distance, int direction)
{
	Vector2 point;
	double tanX = origin.x > target.x ? -(origin.x - target.x) : target.x - origin.x;
	double tanY = origin.y > target.y ? origin.y - target.y : target.y - origin.y;
	double angle = atan2(tanY, tanX);
	point.x = origin.x + cos(angle) * distance;
	point.y = origin.y + (sin(-angle) * distance) * direction;
	return point;
}

void SpawnEnemy()
{
	if (enemyCounter < MAX_ENEMIES)
	{
		Enemy newEnemy;
		newEnemy.color = BLUE;
		newEnemy.hitPoints = 1;
		newEnemy.speed = 150;
		newEnemy.position.x = (rand() % 2) > 0 ? GetScreenWidth() - 12 : 12;
		newEnemy.position.y = rand() % MIN_ENEMY_DISTANCE;
		newEnemy.direction = newEnemy.position.x == 12 ? 1 : -1;
		enemies[enemyCounter++] = newEnemy;
	}
}

void UpdateEnemies()
{
	for (int e = 0; e < enemyCounter; e++)
	{
		if (enemies[e].hitPoints <= 0)
		{
			DeleteEnemy(e);
			continue;
		}
		float newX = (enemies[e].speed * GetFrameTime() * enemies[e].direction);
		if (enemies[e].position.x + newX >= GetScreenWidth() - 12 || enemies[e].position.x + newX < 12) enemies[e].direction *= -1;
		enemies[e].position.x += newX;

		int enemyBulletClock = elapsedTime * 1000;
		if (enemyBulletClock - enemies[e].lastFired >= 800)
		{
			Fire(enemies[e].position, 400.0f, playerPosition, BULLET_TYPE_ENEMY);
			enemies[e].lastFired = enemyBulletClock;
		}

	}
}

void DrawEnemies()
{
	for (int e = 0; e < enemyCounter; e++)
	{
		DrawRectangle(enemies[e].position.x, enemies[e].position.y, 24, 12, enemies[e].color);
	}
}

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
		atan2 gives the angle of the point from the origin (in radians)
		the origin needs to be the player position
	*/

	int tanX = playerPosition.x > cursorPosition.x ? -(playerPosition.x - cursorPosition.x) : cursorPosition.x - playerPosition.x;
	int tanY = playerPosition.y > cursorPosition.y ? playerPosition.y - cursorPosition.y : 0;
	double ang = atan2(tanY, tanX);

	double gunX = playerPosition.x + cos(ang) * playerGunLenght;
	double gunY = playerPosition.y - sin(ang) * playerGunLenght;
	DrawCircle(playerPosition.x, playerPosition.y, playerSize / 2, RED);
	DrawLine(playerPosition.x, playerPosition.y, gunX, gunY, RED);
}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
	// seed the RNG 
	srand(DEBUG_SEED);
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

void DeleteEnemy(int enemyIndex)
{
	if (enemyIndex > enemyCounter) return; //this shouldn't be possible, right?
	for (int i = enemyIndex; i < enemyCounter - 1; i++)
	{
		enemies[i] = enemies[i + 1];
	}
	enemyCounter--;
	killCounter++;
	return;
}


int GetEnemyHitByBullet(Vector2 bulletPosition)
{
	for (int e = 0; e < enemyCounter; e++)
	{ //enemies[e].position.x, enemies[e].position.y, 24, 12,
		Rectangle enemyRect;
		enemyRect.x = enemies[e].position.x;
		enemyRect.y = enemies[e].position.y;
		enemyRect.width = 24;
		enemyRect.height = 12;

		if (CheckCollisionPointRec(bulletPosition, enemyRect))
		{
			return e;
		}
	}
	return -1;
}

bool IsWallCollision(Vector2 bulletPosition)
{
	Rectangle rec;
	for (int y = 0; y < ROWS; y++)
	{
		for (int x = 0; x < COLS; x++)
		{

			if (currentLevel->foreground[y][x] > 0)
			{
				rec.x = x * TILE_SIZE;
				rec.y = y * TILE_SIZE;
				rec.width = TILE_SIZE;
				rec.height = TILE_SIZE;
				if (CheckCollisionPointRec(bulletPosition, rec))
					return true;
			}
		}
	}
	return false;
}


void UpdateBullets()
{
	for (int b = 0; b < bulletCounter; b++)
	{
		bullets[b].distance += playerProjectileSpeed * GetFrameTime();
		int bulletDirection = bullets[b].type == BULLET_TYPE_PLAYER ? 1 : -1;
		Vector2 newBulletPosition = GetPointOnTrajectory(bullets[b].origin, bullets[b].targetPosition, bullets[b].distance, bulletDirection);

		// check out of screen
		if (
			newBulletPosition.x < 0 ||
			newBulletPosition.x > GetScreenWidth() ||
			newBulletPosition.y < 0 ||
			newBulletPosition.y > GetScreenHeight()
			)
		{
			DeleteBullet(b);
			continue;
		}
		// check structures collision
		if (IsWallCollision(newBulletPosition))
		{
			DeleteBullet(b);
			continue;
		}



		if (bullets[b].type == BULLET_TYPE_PLAYER)
		{
			int enemyHit = GetEnemyHitByBullet(newBulletPosition);
			if (enemyHit > -1) {
				enemies[enemyHit].hitPoints -= bullets[b].damage;
				DeleteBullet(b);
				continue;
			}
		}
		else
		{
			if (CheckCollisionCircles(bullets[b].position, 4, playerPosition, playerSize))
			{
				playerHitPoints -= bullets[b].damage;
				DeleteBullet(b);
			}
		}


		//if(CheckCollisionPointRec(newBulletPosition, enemyRect)
		bullets[b].position = newBulletPosition;
	}
	return;
}

void Fire(Vector2 origin, float speed, Vector2 target, int type)
{
	if (bulletCounter < MAX_BULLETS)
	{
		Bullet newBullet;
		newBullet.origin = origin;
		newBullet.position = origin;
		newBullet.targetPosition = target;
		newBullet.distance = 0;
		newBullet.speed = speed;
		newBullet.type = type;
		newBullet.damage = 1;
		bullets[bulletCounter++] = newBullet;
	}
	return;
}

void DrawBackground()
{
	for (int y = 0; y < ROWS; y++)
	{
		for (int x = 0; x < COLS; x++)
		{
			if (currentLevel->background[y][x] == 1)
			{
				DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, DARKGREEN);
			}
			if (currentLevel->background[y][x] == 2)
			{
				DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, DARKGRAY);
			}

		}
	}
	return;
}

void DrawStructures()
{
	for (int y = 0; y < ROWS; y++)
	{
		for (int x = 0; x < COLS; x++)
		{

			if (currentLevel->foreground[y][x] == 1)
			{
				DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, DARKBROWN);
			}
		}
	}
	return;
}


// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
	float dt = GetFrameTime();
	elapsedTime += dt;
	gameClock = elapsedTime;
	cursorPosition.x = GetMouseX();
	cursorPosition.y = GetMouseY();

	if (playerHitPoints <= 0)
	{
		finishScreen = 1;
	}

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

	if (IsMouseButtonDown(0)) // IsMouseButtonPressed = 1 shot per click; IsMouseButtonDown = continuous fire
	{
		// fire!
		int playerBulletClock = (elapsedTime * 1000);
		if (playerBulletClock - lastPlayerBulletSpawn >= 300)
		{
			lastPlayerBulletSpawn = playerBulletClock;
			Fire(playerPosition, playerProjectileSpeed, cursorPosition, BULLET_TYPE_PLAYER);
		}
	}

	if (enemyCounter < 3)
	{
		int enemyClock = elapsedTime * 1000;
		if (enemyClock - lastEnemySpawn >= 3000)
		{
			lastEnemySpawn = enemyClock;
			SpawnEnemy();
		}
	}
	UpdateEnemies();
	UpdateBullets();
}

void DrawBullets()
{
	for (int b = 0; b < bulletCounter; b++)
	{
		DrawCircle(bullets[b].position.x, bullets[b].position.y, 4, WHITE);
		if (bullets[b].type == BULLET_TYPE_ENEMY)
		{
			DrawCircle(bullets[b].position.x, bullets[b].position.y, 3, RED);
		}
	}
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
	// TODO: Draw GAMEPLAY screen here!
	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
	DrawBackground();
	DrawCursor();
	DrawPlayer();
	DrawEnemies();
	DrawBullets();
	DrawStructures();
	DrawText(
		TextFormat("Elapsed time: %d", gameClock),
		600, 24,
		24,
		RAYWHITE
	);
	DrawText(
		TextFormat("Enemies killed: %d", killCounter),
		300, 24,
		24,
		RAYWHITE
	);
	DrawText(
		TextFormat("HP: %d", playerHitPoints),
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