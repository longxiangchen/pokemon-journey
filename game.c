#include <stdlib.h>
#include <stdio.h>
#include "game.h"
#include "gba.h"
#include "sprites.h"
#include "mode0.h"
#include "collisionMap1.h"
#include "collisionMap2.h"
#include "collisionMap3.h"
#include "mapOne.h"

// objects
PLAYER player;
ENEMY firstE;
ENEMY hypno;
ENEMY onyx;
ENEMY gengar;

// ShadowOAM Structure
OBJ_ATTR shadowOAM[128];

// states
enum {SPLASH, INSTRUCTIONS, GAME, PAUSE, BATTLE, WIN, LOSE};

// directions
enum {UP, DOWN, LEFT, RIGHT};

// 
enum {HYPNO, ONYX, GENGAR};
extern int enmy;
// maps
int currMap;
int stage;
enum {MAP1, MAP2, MAP3};

// unsigned char * collisionMap = (unsigned char *) collisionMap1Bitmap;
unsigned char *collisionM1;
unsigned char *collisionM2;
unsigned char *collisionM3;
// variables
int frame;
int collisionEnabled;
int hOff;
int vOff;
int battleState;
int palSeconds;
int hOffCount;
int hOffCount2;
int bg0hOff;
int bg1hOff;
int shadow;
extern int stage;

inline unsigned char colorAt(int x, int y) {
    if (currMap == MAP1) {
        return collisionM1[OFFSET(x, y, MAPWIDTH)];
    } else if (currMap == MAP2) {
        return collisionM2[OFFSET(x, y, MAPWIDTH)];
    } else if (currMap == MAP3) {
        return collisionM3[OFFSET(x, y, MAPWIDTH)];
    }
}

void initGame() {
    REG_DISPCTL = MODE(0) | BG_ENABLE(2) | SPRITE_ENABLE;
    REG_BG2CNT = BG_CHARBLOCK(2) | BG_SCREENBLOCK(20) | BG_SIZE_SMALL | BG_4BPP;
    DMANow(3, mapOnePal, PALETTE, mapOnePalLen/2); // loading palette (note: if recoloring, change this palette)
    DMANow(3, mapOneTiles, &CHARBLOCK[2], mapOneTilesLen/2); // loading tileset into charblock 0
    DMANow(3, mapOneMap, &SCREENBLOCK[20], mapOneMapLen / 2);
    stage = 1;
    shadow = 0;
    hOffCount = 0;
    hOffCount2 = 0;
    bg0hOff = 0;
    bg1hOff = 0;
    currMap = MAP1;
    palSeconds = 0;
    player.palRow = 0;
    player.move = 0;
    collisionEnabled = 1;
    battleState = 0;
   	hideSprites();
    initHypno();
    initOnyx();
    initPlayer();
    // initFirstEnemy();

    collisionM1 = malloc(collisionMap1BitmapLen);
    DMANow(3, collisionMap1Bitmap, collisionM1, collisionMap1BitmapLen / 2);

    collisionM2 = malloc(collisionMap2BitmapLen);
    DMANow(3, collisionMap2Bitmap, collisionM2, collisionMap2BitmapLen / 2);

    collisionM3 = malloc(collisionMap3BitmapLen);
    DMANow(3, collisionMap3Bitmap, collisionM3, collisionMap3BitmapLen / 2);
}

void initPlayer() {
    player.isMoving = 0;
    player.numOfFrames = 3;
    player.level = 1;
    player.health = 100;
    player.defense = 10;
    player.x = 40;
    player.y = 80;
    player.height = 24;
    player.width = 24;
    player.frameDelay = 10;
    player.direction = RIGHT;
    player.xVel = 1;
    player.yVel = 1;
    player.attack = 20;
}

// void initFirstEnemy() {
//     firstE.level = 1;
//     firstE.direction = DOWN;
//     firstE.health = 100;
//     firstE.defense = 7;
//     firstE.x = 97;
//     firstE.y = 45;
//     firstE.active = 0;
//     firstE.width = 16;
//     firstE.height = 16;
// }

int canMoveLeft() {

    if (!collisionEnabled)
        return 1;

    int left = player.x;
    int right = player.x + player.width - 1;
    int top = player.y + 1;
    int bottom = player.y + player.height - 8;

    return colorAt(left - player.xVel, top) && colorAt(left - player.xVel, bottom);
}

int canMoveRight() {

    if (!collisionEnabled)
        return 1;

    int left = player.x;
    int right = player.x + player.width - 1;
    int top = player.y + 1;
    int bottom = player.y + player.height - 8;

    return colorAt(right + player.xVel, bottom) && colorAt(right + player.xVel, top);
}

int canMoveUp() {

    if (!collisionEnabled)
        return 1;

    int left = player.x;
    int right = player.x + player.width - 9;
    int top = player.y;
    int bottom = player.y + player.height - 1;

    return colorAt(left, top - player.yVel) && colorAt(right, top - player.yVel);
}

int canMoveDown() {
    if (!collisionEnabled)
        return 1;

    int left = player.x;
    int right = player.x + player.width - 9;
    int top = player.y;
    int bottom = player.y + player.height - 1;

    return colorAt(left, bottom + player.yVel) && colorAt(right, bottom + player.yVel);
}

void movePlayer() {
    // mgba_printf("x, y: (%d, %d)", player.x, player.y);
    if (BUTTON_HELD(BUTTON_LEFT) && canMoveLeft())
        player.x -= player.xVel;

    if (BUTTON_HELD(BUTTON_RIGHT) && canMoveRight())
        player.x += player.xVel;

    if (BUTTON_HELD(BUTTON_UP) && canMoveUp())
        player.y -= player.yVel;

    if (BUTTON_HELD(BUTTON_DOWN) && canMoveDown())
        player.y += player.yVel;

    // Set camera position based on new player position
    hOff = player.x - (SCREENWIDTH - player.width) / 2;
    vOff = player.y - (SCREENHEIGHT - player.height) / 2;

    // Restrict player movement to map
    if (player.x < 0)
        player.x = 0;
    if (player.y < 0)
        player.y = 0;
    if (player.x > MAPWIDTH - player.width)
        player.x = MAPWIDTH - player.width;
    if (player.y > MAPHEIGHT - player.height)
        player.y = MAPHEIGHT - player.height;

    // Restrict camera movement to map
    if (hOff < 0)
        hOff = 0;
    if (vOff < 0)
        vOff = 0;
    if (hOff > MAPWIDTH - SCREENWIDTH)
        hOff = MAPWIDTH - SCREENWIDTH;
    if (vOff > MAPHEIGHT - SCREENHEIGHT)
        vOff = MAPHEIGHT - SCREENHEIGHT;

    
}

void animatePlayer() {
    player.isMoving = 0;
    if (BUTTON_HELD(BUTTON_UP)) {
        player.isMoving = 1;
        player.direction = UP;
    }

    if (BUTTON_HELD(BUTTON_DOWN)) {
        player.isMoving = 1;
        player.direction = DOWN;
    }

    if (BUTTON_HELD(BUTTON_LEFT)) {
        player.isMoving = 1;
        player.direction = LEFT;
    }

    if (BUTTON_HELD(BUTTON_RIGHT)) {
        player.isMoving = 1;
        player.direction = RIGHT;
    }

    if (player.isMoving) {
        player.frameDelay--;
        if (player.frameDelay == 0) {
            player.frameDelay = 10;
            player.curFrame = (player.curFrame + 1) % player.numOfFrames;
        }
    } else {
        player.frameDelay = 10;
        player.curFrame = 0;
    }
}

void updatePlayer() {
    if (BUTTON_PRESSED(BUTTON_B)) {
        if (!player.palRow) {
            player.level = 5000;
            player.health = 5000;
            player.palRow = 3;
            collisionEnabled = 0;
        } else if (player.palRow == 3) {
            player.level = 10;
            player.health = 100;
            player.palRow = 0;
            collisionEnabled = 1;
        }
    }
    palSeconds++;
    if (collision(player.x, player.y, player.width, player.height, 26, 226, 3, 4) && player.level < 4500 && !shadow && palSeconds > 60 && player.level >= 5 && currMap == MAP1) {
        player.palRow = !player.palRow;
        collisionEnabled = !collisionEnabled;
        palSeconds = 0;
        shadow = 1;
    }

    movePlayer(&player);
    animatePlayer(&player);

    // mgba_printf("Position: (%d, %d)", player.x, player.y);
}

void updateGame() {
    if (shadow) {
        // remove the orb tile
        SCREENBLOCK[20].tilemap[OFFSET(3, 28, 32)] = TMAP_ENTRY_TILEID(1030);
    }
    if (stage == 3 && currMap == MAP1) {
        // swap the boulder tiles to path tiles
        SCREENBLOCK[20].tilemap[OFFSET(30, 21, 32)] = TMAP_ENTRY_TILEID(1150);
        SCREENBLOCK[20].tilemap[OFFSET(30, 22, 32)] = TMAP_ENTRY_TILEID(1150);
        SCREENBLOCK[20].tilemap[OFFSET(31, 21, 32)] = TMAP_ENTRY_TILEID(1150);
        SCREENBLOCK[20].tilemap[OFFSET(31, 22, 32)] = TMAP_ENTRY_TILEID(1150);
        initGengar();
        collisionEnabled = 1;
        // remove collision
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                collisionM1[OFFSET(240 + i, 168 + j, MAPWIDTH)] = 1;
            }
        }
    }
    hOffCount++;
    hOffCount2++;
    if (collision(player.x, player.y, player.width, player.height, 253, 172, 3, 5) && currMap == MAP1) {
        // change to map 2
        player.x = 15;
        player.y = 90;
        currMap = MAP2;
        onyx.active = 1;
    }
    if (collision(player.x, player.y, player.width, player.height, 0, 107, 8, 24) && currMap == MAP2) {
        // change to map 1
        player.x = 220;
        player.y = 160;
        currMap = MAP1;
    }
    // if (!firstE.active) {
    //     detectEnemyCollision(firstE, firstE.direction);
    // }
    updatePlayer();
    updateHypno();
    if (stage == 3) {
        updateGengar();

    }
    if (currMap == MAP2) {
        hypno.active = 0;
        if (hOffCount > 10) {
            bg0hOff = (bg0hOff + 2) % 256;
            hOffCount = 0;
        }
        if (hOffCount2 > 14) {
            bg1hOff = (bg1hOff + 1) % 256;
            hOffCount2 = 0;
        }
        updateOnyx();
    }
}

void drawGame() {

    shadowOAM[0].attr0 = ATTR0_Y(player.y - vOff) | ATTR0_4BPP | ATTR0_SQUARE;
    shadowOAM[0].attr1 = ATTR1_MEDIUM | ATTR1_X(player.x - hOff);
    shadowOAM[0].attr2 = ATTR2_PALROW(player.palRow) | ATTR2_TILEID(player.direction * 4, player.curFrame * 4) | ATTR2_PRIORITY(0);

    drawHypno();
    drawOnyx(onyx.x + hOff, onyx.y);
    drawGengar();
    if (currMap == MAP2) {
        REG_BG2HOFF = bg1hOff;
        REG_BG2VOFF = vOff;
    } else {
        REG_BG2HOFF = hOff;
        REG_BG2VOFF = vOff;
    }
    REG_BG1HOFF = bg1hOff;
    REG_BG0HOFF = bg0hOff;
    REG_BG1VOFF = vOff;
    REG_BG0VOFF = vOff;
    DMANow(3, shadowOAM, OAM, 512);
}

int detectEnemyCollision(ENEMY * e, int direction) {
    if (e->active) {
        if (direction == DOWN) {
            if (collision(player.x, player.y, player.width, player.height, e->x, e->y + e->height/2 - 2, e->width - 2, e->height + 2)) {
                battleState = 1;
                return 1;
            }
        } else if (direction == UP) {
            if (collision(player.x, player.y, player.width, player.height, e->x + e->width, e->y - e->height + 2, player.width + 2, player.height + 2)) {
                battleState = 1;
                return 1;
            }
        } else if (direction == LEFT) {
            if (collision(player.x, player.y, player.width, player.height, e->x - e->width, e->y, player.width + 2, player.height + 2)) {
                battleState = 1;
                return 1;
            }
        } else if (direction == RIGHT) {
            if (collision(player.x, player.y, player.width, player.height, e->x + e->width, e->y, player.width - 2, player.height + 2)) {
                battleState = 1;
                return 1;
            }
        } else {
            return 0;
        }
    }
}

void initOnyx() {
    onyx.level = 5;
    onyx.health = 200;
    onyx.defense = 20;
    onyx.active = 1;
    onyx.x = 200;
    onyx.y = 85;
}

void updateOnyx() {
    if (!battleState) {
        if (onyx.active) {
            if (detectEnemyCollision(&onyx, LEFT)) {
                enmy = ONYX;
            }
        }
    }
    if (onyx.health <= 0 && onyx.active) {
        battleState = 0;
        onyx.active = 0;
        player.level += 5;
        stage = 3;
        enmy = GENGAR;
    }
}
void drawOnyx(int oX, int oY) {
    if (onyx.active && currMap == MAP2) {
        shadowOAM[28].attr0 = ATTR0_Y(oY - vOff) | ATTR0_4BPP | ATTR0_SQUARE;
        shadowOAM[28].attr1 = ATTR1_MEDIUM | ATTR1_X(oX - hOff);
        shadowOAM[28].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(22, 2);
        shadowOAM[29].attr0 = ATTR0_Y(oY + 32 - vOff) | ATTR0_4BPP | ATTR0_WIDE;
        shadowOAM[29].attr1 = ATTR1_SMALL | ATTR1_X(oX- hOff);
        shadowOAM[29].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(22, 6);
        shadowOAM[30].attr0 = ATTR0_Y(oY + 16 - vOff) | ATTR0_4BPP | ATTR0_TALL;
        shadowOAM[30].attr1 = ATTR1_TINY | ATTR1_X(oX + 32 - hOff);
        shadowOAM[30].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(26, 4);
    } else {
        shadowOAM[28].attr0 = ATTR0_HIDE;
        shadowOAM[29].attr0 = ATTR0_HIDE;
        shadowOAM[30].attr0 = ATTR0_HIDE;
    }
}

void initHypno() {
    hypno.level = 1;
    hypno.health = 100;
    hypno.defense = 7;
    hypno.x = 136;
    hypno.y = 135;
    hypno.active = 1;
    hypno.frameDelay = 20;
    hypno.curFrame = 0;
    hypno.numOfFrames = 3;
    hypno.width = 26;
    hypno.height = 26;
    hypno.battleTileX = 5;
    hypno.battleTileY = 15;
}

void updateHypno() {
    if (!battleState) {
        hypno.frameDelay--;
        if (hypno.frameDelay == 0) {
            hypno.frameDelay = 20;
            hypno.curFrame = (hypno.curFrame + 1) % hypno.numOfFrames;
        }
        if (hypno.active) {
            // detectEnemyCollision(&hypno, DOWN);
            if (detectEnemyCollision(&hypno, DOWN)) {
                enmy = HYPNO;
            }
        }
    }
    if (hypno.health <= 0 && hypno.active) {
        battleState = 0;
        hypno.active = 0;
        player.level += 10;
        player.attack += 5;
        stage = 2;
        enmy = ONYX;
    }
}

void drawHypno() {
    if (hypno.active) {
        shadowOAM[1].attr0 = ATTR0_Y(hypno.y - vOff) | ATTR0_4BPP | ATTR0_SQUARE;
        shadowOAM[1].attr1 = ATTR1_MEDIUM | ATTR1_X(hypno.x - hOff);
        shadowOAM[1].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(0, 20 + 4 * hypno.curFrame) | ATTR2_PRIORITY(0);
    } else {
        shadowOAM[1].attr0 = ATTR0_HIDE;
    }
}


void initGengar() {
    gengar.level = 30;
    gengar.health = 200;
    gengar.defense = 10;
    gengar.x = 24;
    gengar.y = 138;
    gengar.active = 1;
    gengar.frameDelay = 20;
    gengar.curFrame = 0;
    gengar.numOfFrames = 3;
    gengar.width = 26;
    gengar.height = 26;
    gengar.battleTileX = 16;
    gengar.battleTileY = 3;
}

void updateGengar() {
    if (!battleState) {
        gengar.frameDelay--;
        if (gengar.frameDelay == 0) {
            gengar.frameDelay = 20;
            gengar.curFrame = (gengar.curFrame + 1) % gengar.numOfFrames;
        }
        if (gengar.active) {
            // detectEnemyCollision(&gengar, DOWN);
            if (detectEnemyCollision(&gengar, DOWN)) {
                enmy = GENGAR;
            }
        }
    }
    if (gengar.health <= 0 && gengar.active) {
        battleState = 0;
        gengar.active = 0;
        player.level += 10;
        player.attack += 15;
        stage = 4;
    }
}

void drawGengar() {
    if (gengar.active && currMap == MAP1) {
        shadowOAM[31].attr0 = ATTR0_Y(gengar.y - vOff) | ATTR0_4BPP | ATTR0_SQUARE;
        shadowOAM[31].attr1 = ATTR1_MEDIUM | ATTR1_X(gengar.x - hOff);
        shadowOAM[31].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(16, 8 * gengar.curFrame) | ATTR2_PRIORITY(0);
    } else {
        shadowOAM[31].attr0 = ATTR0_HIDE;
    }
}