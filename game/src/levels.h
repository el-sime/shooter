#ifndef LEVELS_H
#define LEVELS_H

#define ROWS 16
#define COLS 25
#define TILE_SIZE 32

typedef struct Levels {
	int background[ROWS][COLS];
	int foreground[ROWS][COLS];
	int items[ROWS][COLS];
} Level;

Level level1 = {
	.background = {0},
	.foreground = {
		{0},
		{0},
		{0},
		{1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0},
		{1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0},
		{1,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{0}
	},
	.items = {0}
};
#endif