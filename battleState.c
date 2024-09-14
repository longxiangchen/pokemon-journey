
// // background, selection, moves, damage, health

// enum menuStates {MENU, MOVES};

// // list of moves that can change

#include "gba.h"
#include "mode0.h"
#include "print.h"
#include "sprites.h"
#include "game.h"
#include "battleSelect.h"

int menuState;
int enemyX, enemyY, pX, pY;
int sX, sY;
int enemyTurn = 0;
PLAYER player;
extern ENEMY hypno;
extern ENEMY gengar;
extern ENEMY onyx;
// stuff here
int health, level, defense, tileX, tileY;

int count = 0;
enum {HYPNO, ONYX, GENGAR};
enum {MAP1, MAP2, MAP3};

extern int currMap;
int enmy = HYPNO;
int won;

#define healthBars 13

pX = 70;
pY = 110;
enemyX = 180;
enemyY = 70;
enum {SCRATCH, FIREBALL, HEAL, DEFEND};

int move = SCRATCH;

int vOff;
int hOff;

void initEnemy(int en) {
    if (en == HYPNO) {
        health = hypno.health;
        defense = hypno.defense;
        level = hypno.level;
        tileX = hypno.battleTileX;
        tileY = hypno.battleTileY;
    } else if (en == ONYX) {
        health = onyx.health;
        defense = onyx.defense;
        level = onyx.level;
    } else if (en == GENGAR) {
        health = gengar.health;
        defense = gengar.defense;
        level = gengar.level;
        tileX = gengar.battleTileX;
        tileY = gengar.battleTileY;
    }
    mgba_printf("health is: %d", health);
}


void initPlayerHealthBar() {
    int pHx = 0;
    // draw the initial health bar (full health)
    for (int i = 0; i < healthBars; i++) {
        shadowOAM[i + 3].attr0 = ATTR0_Y(95 - vOff) | ATTR0_4BPP | ATTR0_SQUARE;
        shadowOAM[i + 3].attr1 = ATTR1_TINY | ATTR1_X(20 + pHx);
        shadowOAM[i + 3].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(3 + i, 13);
        pHx += 8;
        // ends at shadowOAM[14], so use shadowOAM[15] for next sprite
    }
    DMANow(3, shadowOAM, OAM, 512);
}

int checkPlayerHealthBar() {
    for (int i = 0; i < healthBars; i++) {
        if (player.health <= ((i + 1) * 100 / 13)) return i;
    }
}

void initBattle() {
    enemyTurn = 0;
}

void initEnemyHealthBar() {
    int eHx = 0;
    for (int i = 0; i < healthBars; i++) {
        shadowOAM[i + 16].attr0 = ATTR0_Y(44 - vOff) | ATTR0_4BPP | ATTR0_SQUARE;
        shadowOAM[i + 16].attr1 = ATTR1_TINY | ATTR1_X(125 + eHx);
        shadowOAM[i + 16].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(3 + i, 13);
        eHx += 8;
    }
    DMANow(3, shadowOAM, OAM, 512);
}

int checkEnemyHealthBar() {
    for (int i = 0; i < healthBars; i++) {

        if (enmy == HYPNO) {
            if (hypno.health <= ((i +1) * 100 / 13)) return i;
        } else if (enmy == ONYX) {
            if (onyx.health <= ((i +1) * 100 / 13)) return i;
        } else if (enmy == GENGAR) {
            if (gengar.health <= ((i +1) * 100 / 13)) return i;
        }
    }
}

void updateEnemyHealthBar() {
    int eHx = 0;
    // draw the empty health bar (0 health)
    for (int i = 0; i < healthBars; i++) {
        shadowOAM[i + 15].attr0 = ATTR0_Y(44 - vOff) | ATTR0_4BPP | ATTR0_SQUARE;
        shadowOAM[i + 15].attr1 = ATTR1_TINY | ATTR1_X(125 + eHx - vOff);
        shadowOAM[i + 15].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(3 + i, 12);
        eHx += 8;
    }
    for (int i = 0; i < checkEnemyHealthBar(); i++) {
        shadowOAM[i + 15].attr2 = ATTR2_TILEID(3 + i, 13);
    }
}
void updatePlayerHealthBar() {
    if (player.health >= 100) {
        initPlayerHealthBar();
    } else {
            int pHx = 0;
            // draw the empty health bar (0 health)
            for (int i = 0; i < healthBars; i++) {
                shadowOAM[i + 3].attr0 = ATTR0_Y(95 - vOff) | ATTR0_4BPP | ATTR0_SQUARE;
                shadowOAM[i + 3].attr1 = ATTR1_TINY | ATTR1_X(20 + pHx - vOff);
                shadowOAM[i + 3].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(3 + i, 12);
                pHx += 8;
            }
        for (int i = 0; i < checkPlayerHealthBar(); i++) {
            shadowOAM[i + 3].attr2 = ATTR2_TILEID(3 + i, 13);
        }
    }
}

void drawPlayers() {
    shadowOAM[0].attr0 = ATTR0_Y(pY - vOff) | ATTR0_4BPP | ATTR0_SQUARE;
    shadowOAM[0].attr1 = ATTR1_MEDIUM | ATTR1_X(pX - hOff);
    shadowOAM[0].attr2 = ATTR2_PALROW(player.palRow) | ATTR2_TILEID(16, 0);

    if (enmy == HYPNO) {
        shadowOAM[1].attr0 = ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(enemyY - vOff);
        shadowOAM[1].attr1 = ATTR1_MEDIUM | ATTR1_X(enemyX - hOff);
        shadowOAM[1].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(5, 15) | ATTR2_PRIORITY(0);
    } else if (enmy == ONYX) {
        drawOnyx(enemyX - 5, enemyY - 15);
    } else if (enmy == GENGAR) {
        shadowOAM[31].attr0 = ATTR0_Y(enemyY - vOff) | ATTR0_4BPP | ATTR0_SQUARE;
        shadowOAM[31].attr1 = ATTR1_MEDIUM | ATTR1_X(enemyX - hOff);
        shadowOAM[31].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(gengar.battleTileX, gengar.battleTileY) | ATTR2_PRIORITY(0);
    }

    waitForVBlank();
    DMANow(3, shadowOAM, OAM, 512);
}

void updateBattle() {
    count++;
    moves();
    if (enemyTurn && player.health > 0 && count > 25) {
        count = 0;
        if (enmy == HYPNO) {
            player.health -= 15 - player.defense;
        } else if (enmy == ONYX) {
            player.health -= 20 - player.defense;
        } else if (enmy == GENGAR) {
            player.health -= gengar.level;
        }
        mgba_printf("enemy attacks you for 20. your health is now: %d", player.health);
        enemyTurn = 0;
    }
    updatePlayerHealthBar();
    updateEnemyHealthBar();
    DMANow(3, shadowOAM, OAM, 512);
}

void moves() {
    // move selected
    switch(move) {
            case SCRATCH:
                shadowOAM[2].attr0 = ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(107 - vOff);
                shadowOAM[2].attr1 = ATTR1_TINY | ATTR1_X(197 - hOff);
                shadowOAM[2].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(0, 12) | ATTR2_PRIORITY(0);

                if (BUTTON_PRESSED(BUTTON_DOWN)) {
                    shadowOAM[2].attr0 = ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(117 - vOff);
                    shadowOAM[2].attr1 = ATTR1_TINY | ATTR1_X(197 - hOff);
                    shadowOAM[2].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(0, 12) | ATTR2_PRIORITY(0);
                    move = FIREBALL;
                }
                if (BUTTON_PRESSED(BUTTON_A) && !enemyTurn) {
                    if (enmy == HYPNO) {
                        hypno.health -= (player.level + player.attack - hypno.defense);
                        mgba_printf("You used SCRATCH. The enemy has %d health left.", hypno.health);
                    } else if (enmy == ONYX) {
                        mgba_printf("onyx health: %d", onyx.health);
                        mgba_printf("player level: %d", player.level);
                        mgba_printf("onyx defense: %d", onyx.defense);
                        onyx.health -= (player.level + player.attack - onyx.defense);
                        mgba_printf("You used SCRATCH. The enemy has %d health left.", onyx.health);
                    } else if (enmy == GENGAR) {
                        gengar.health -= (player.level + player.attack - gengar.defense);
                        mgba_printf("You used SCRATCH. The enemy has %d health left.", gengar.health);
                    }
                    enemyTurn = 1;
                }
                break;
            case FIREBALL:
                if (BUTTON_PRESSED(BUTTON_DOWN)) {
                    move = HEAL;
                    shadowOAM[2].attr0 = ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(128 - vOff);
                    shadowOAM[2].attr1 = ATTR1_TINY | ATTR1_X(197 - hOff);
                    shadowOAM[2].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(0, 12) | ATTR2_PRIORITY(0);
                } else if (BUTTON_PRESSED(BUTTON_UP)) {
                    move = SCRATCH;
                    shadowOAM[2].attr0 = ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(107 - vOff);
                    shadowOAM[2].attr1 = ATTR1_TINY | ATTR1_X(197 - hOff);
                    shadowOAM[2].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(0, 12) | ATTR2_PRIORITY(0);
                }
                if (BUTTON_PRESSED(BUTTON_A) && !enemyTurn) {
                    if (enmy == HYPNO) {
                        hypno.health -= (player.level + player.attack - hypno.defense);
                        mgba_printf("You used FIREBALL. The enemy has %d health left.", hypno.health);
                    } else if (enmy == ONYX) {
                        onyx.health -= (player.level + player.attack - onyx.defense);
                        mgba_printf("You used FIREBALL. The enemy has %d health left.", onyx.health);
                    } else if (enmy == GENGAR) {
                        gengar.health -= (player.level + player.attack - gengar.defense);
                        mgba_printf("You used FIREBALL. The enemy has %d health left.", gengar.health);
                    }
                    enemyTurn = 1;
                }
                break;
            case HEAL:
                if (BUTTON_PRESSED(BUTTON_DOWN)) {
                    move = DEFEND;
                    shadowOAM[2].attr0 = ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(140 - vOff);
                    shadowOAM[2].attr1 = ATTR1_TINY | ATTR1_X(197 - hOff);
                    shadowOAM[2].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(0, 12) | ATTR2_PRIORITY(0);
                } else if (BUTTON_PRESSED(BUTTON_UP)) {
                    move = FIREBALL;
                    shadowOAM[2].attr0 = ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(117 - vOff);
                    shadowOAM[2].attr1 = ATTR1_TINY | ATTR1_X(197 - hOff);
                    shadowOAM[2].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(0, 12) | ATTR2_PRIORITY(0);
                }
                if (BUTTON_PRESSED(BUTTON_A) && player.health < 100 && !enemyTurn) {
                    if (player.health >= 80) {
                        player.health = 100;
                    } else {
                        player.health += 20 + (player.level);
                    }
                    mgba_printf("You healed! Your health is now %d.", player.health);
                    enemyTurn = 1;
                }
                break;
            case DEFEND:
                if (BUTTON_PRESSED(BUTTON_UP)) {
                    shadowOAM[2].attr0 = ATTR0_4BPP | ATTR0_SQUARE | ATTR0_Y(128 - vOff);
                    shadowOAM[2].attr1 = ATTR1_TINY | ATTR1_X(197 - hOff);
                    shadowOAM[2].attr2 = ATTR2_PALROW(0) | ATTR2_TILEID(0, 12) | ATTR2_PRIORITY(0);
                    move = HEAL;
                }
                if (BUTTON_PRESSED(BUTTON_A) && player.defense < 15 && !enemyTurn) {
                    if (player.defense < 10) player.defense += 2;
                    mgba_printf("You used DEFEND. Your defense is now %d.", player.defense);
                    enemyTurn = 1;
                }
                break;
    }
    
}

void battle() {
    REG_DISPCTL |= BG_ENABLE (0);
    REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(26) | BG_SIZE_SMALL | BG_4BPP; 
    vOff = 0;
    hOff = 0;
    REG_BG0HOFF = hOff;
    REG_BG0VOFF = vOff;;
    DMANow(3, shadowOAM, OAM, 512);
    DMANow(3, battleSelectPal, PALETTE, battleSelectPalLen /2); // loading palette (note: if recoloring, change this palette)
    DMANow(3, battleSelectTiles, &CHARBLOCK[0], battleSelectTilesLen /2); // loading tileset into charblock 0
    DMANow(3, battleSelectMap, &SCREENBLOCK[26], battleSelectMapLen / 2);
    updateHypno();
    updateGengar();
    if (currMap == MAP2) {
        updateOnyx();
    }
    updateBattle();
    waitForVBlank();
    drawPlayers();
    // dma the battle background 
}


