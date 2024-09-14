#ifndef GAME_H
#define GAME_H

#include "gba.h"

#define MAPWIDTH 256
#define MAPHEIGHT 256

// player
typedef struct {
    int level;
    int health;
    int defense;
    int x;
    int y;
    int width;
    int height;
    int dead;
    int direction;
    int frameDelay;
    int xVel;
    int yVel;
    int curFrame;
    int numOfFrames;
    int isMoving;
    int move;
    int palRow;
    int attack;
} PLAYER;

// enemy 
typedef struct {
    int level;
    int health;
    int defense;
    int x;
    int y;
    int width;
    int height;
    int enemyID;
    int active;
    int direction;
    int curFrame;
    int numOfFrames;
    int frameDelay;
    int battleTileX;
    int battleTileY;
} ENEMY;

// Variables
extern int state;
extern int frame;

extern PLAYER player;
extern ENEMY hypno;
extern ENEMY gengar;
extern ENEMY onyx;
extern int hOff;
extern int vOff;
extern int battleState;
extern int won;

// function prototypes
extern inline unsigned char colorAt(int x, int y);

#endif

