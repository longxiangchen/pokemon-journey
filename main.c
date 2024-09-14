#include "gba.h"
#include "mode0.h"
#include "print.h"
#include "sprites.h"
#include "background.h"
#include "tileset.h"
#include "spritesheet.h"
#include "game.h"
#include "splash.h"
#include "instructions.h"
#include "battleSelect.h"
#include "pause.h"
#include "win.h"
#include "lose.h"
#include "sound.h"
#include <stdlib.h>
#include "littleRoot.h"
#include "bg2.h"
#include "collisionmap.h"
#include "mapOne.h"
#include "mapTwo.h"
#include "mapThree.h"
#include "mapTwobg0.h"
#include "mapTwobg1.h"
#include "instructions1.h"
#include "instructions2.h"
#include "instructions3.h"
#include "instructions4.h"
#include "instructions5.h"
#include "buttonpress.h"

void initialize();
void setupInterrupts();
void interruptHandler();

OBJ_ATTR shadowOAM[128];

unsigned short oldButtons;
unsigned short buttons;

enum {SPLASH, INSTRUCTIONS1, INSTRUCTIONS2, INSTRUCTIONS3, INSTRUCTIONS4, INSTRUCTIONS5, GAME, PAUSE, BATTLE, WIN, LOSE};

PLAYER player;
extern ENEMY curEnemy;
extern ENEMY onyx;
extern ENEMY gengar;
extern ENEMY hypno;
extern int currMap;
extern int enemyTurn;
enum {MAP1, MAP2, MAP3};
int won = 0;
int x;
int state;
int vOff;
int hOff;
int minute, second;
extern int stage;

extern int enmy;

enum {HYPNO, ONYX, GENGAR};

int main() {
    initialize();

    while (1) {
        oldButtons = buttons;
        buttons = REG_BUTTONS;

        switch(state) {
            case SPLASH:
                splash();
                break;
            case INSTRUCTIONS1:
                instructions1();
                if (BUTTON_PRESSED(BUTTON_START)) {
                    initGame();
                    goToGame();
                }
                break;
            case INSTRUCTIONS2:
                instructions2();
                if (BUTTON_PRESSED(BUTTON_START)) {
                    initGame();
                    goToGame();
                }
                break;
            case INSTRUCTIONS3:
                instructions3();
                if (BUTTON_PRESSED(BUTTON_START)) {
                    initGame();
                    goToGame();
                }
                break;
            case INSTRUCTIONS4:
                instructions4();
                if (BUTTON_PRESSED(BUTTON_START)) {
                    initGame();
                    goToGame();
                }
                break;
            case INSTRUCTIONS5:
                instructions5();
                if (BUTTON_PRESSED(BUTTON_START)) {
                    initGame();
                    goToGame();
                }
                break;
            case PAUSE:
                pause();
                break;
            case GAME:
                game();
                if (gengar.health <= 0 && !gengar.active && stage == 4 && currMap == MAP1) {
                    REG_DISPCTL = MODE(0) | BG_ENABLE(0) | SPRITE_ENABLE;
                    REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(26) | BG_SIZE_SMALL| BG_4BPP; 
                    won = 0;
                    REG_BG0VOFF = 0;
                    REG_BG0HOFF = 0;
                    DMANow(3, splashPal, PALETTE, splashPalLen /2); // loading palette (note: if recoloring, change this palette)
                    DMANow(3, splashTiles, &CHARBLOCK[0], splashTilesLen /2); // loading tileset into charblock 0
                    DMANow(3, splashMap, &SCREENBLOCK[26], splashMapLen / 2);
                    state = WIN;
                    goToWin();
                }
                if (battleState) {
                    goToBATTLE();
                }
                break;
            case BATTLE:
                if (BUTTON_PRESSED(BUTTON_A)) {
                    playSoundB(buttonpress_data, buttonpress_length, 0);
                }
                battle();
                if (player.health <= 0) {
                    goToLose();
                } 
                if (!battleState) {
                    goToGame();
                }
                break;
            case WIN:
                win();
                break;
            case LOSE:
                lose();
                if (BUTTON_PRESSED(BUTTON_START)) {
                    goToSplash();
                }
                break;
        }
    }
}

void splash() {
    waitForVBlank();

    if (BUTTON_PRESSED(BUTTON_START)) {
        initGame();
        goToGame();
    } else if (BUTTON_PRESSED(BUTTON_SELECT)) {
        goToInstructions1();
    }
}

void goToSplash() {
    REG_DISPCTL = MODE(0) | BG_ENABLE(0) | SPRITE_ENABLE;
    REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(26) | BG_SIZE_SMALL| BG_4BPP; 
    won = 0;
    REG_BG0VOFF = 0;
    REG_BG0HOFF = 0;
    DMANow(3, splashPal, PALETTE, splashPalLen /2); // loading palette (note: if recoloring, change this palette)
    DMANow(3, splashTiles, &CHARBLOCK[0], splashTilesLen /2); // loading tileset into charblock 0
    DMANow(3, splashMap, &SCREENBLOCK[26], splashMapLen / 2);
    state = SPLASH;
}
void goToGame() {
    hideSprites();
    DMANow(3, shadowOAM, OAM, 512);
    if (currMap == MAP1) {
        REG_DISPCTL = MODE(0) | BG_ENABLE(2) | SPRITE_ENABLE;
        REG_BG2CNT = BG_CHARBLOCK(2) | BG_SCREENBLOCK(20) | BG_SIZE_SMALL | BG_4BPP;
        DMANow(3, mapOnePal, PALETTE, mapOnePalLen/2); // loading palette (note: if recoloring, change this palette)
        DMANow(3, mapOneTiles, &CHARBLOCK[2], mapOneTilesLen/2); // loading tileset into charblock 0
        DMANow(3, mapOneMap, &SCREENBLOCK[20], mapOneMapLen / 2);
    } else if (currMap == MAP2) {
        REG_DISPCTL |= BG_ENABLE(0) | BG_ENABLE(1) ;
        REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(26) | BG_SIZE_SMALL | BG_4BPP; 
        REG_BG1CNT = BG_CHARBLOCK(1) | BG_SCREENBLOCK(28) | BG_SIZE_SMALL | BG_4BPP;
        DMANow(3, mapTwoPal, PALETTE, mapTwoPalLen/2); // loading palette (note: if recoloring, change this palette)
        DMANow(3, mapTwoTiles, &CHARBLOCK[2], mapTwoTilesLen / 2); // loading tileset into charblock 0
        DMANow(3, mapTwoMap, &SCREENBLOCK[20], mapTwoMapLen / 2);
        DMANow(3, mapTwobg0Tiles, &CHARBLOCK[0], mapTwobg0TilesLen / 2);
        DMANow(3, mapTwobg1Tiles, &CHARBLOCK[1], mapTwobg1TilesLen / 2);
        DMANow(3, mapTwobg1Map, &SCREENBLOCK[28], mapTwobg1MapLen / 2);
        DMANow(3, mapTwobg0Map, &SCREENBLOCK[26], mapTwobg0MapLen / 2);
    }

    // DMANow(3, bg2Pal + 16, PALETTE + 16, 16);
    // DMANow(3, bg2Tiles, &CHARBLOCK[0], bg2TilesLen / 2);
    // DMANow(3, bg2Map, &SCREENBLOCK[26], bg2MapLen / 2);
    DMANow(3, spritesheetPal, SPRITEPALETTE, 256);
    
    state = GAME;
}

void goToInstructions1() {
    REG_DISPCTL = MODE(0) | BG_ENABLE(0) | SPRITE_ENABLE;
    REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(26) | BG_SIZE_SMALL | BG_4BPP; 
    REG_BG0VOFF = 0;
    REG_BG0HOFF = 0;
    DMANow(3, instructions1Pal, PALETTE, instructions1PalLen / 2);
    DMANow(3, instructions1Tiles, &CHARBLOCK[0], instructions1TilesLen / 2);
    DMANow(3, instructions1Map, &SCREENBLOCK[26], instructions1MapLen / 2);
    state = INSTRUCTIONS1;
}

void goToInstructions2() {
    REG_BG0VOFF = 0;
    REG_BG0HOFF = 0;
    DMANow(3, instructions2Tiles, &CHARBLOCK[0], instructions2TilesLen / 2);
    DMANow(3, instructions2Map, &SCREENBLOCK[26], instructions2MapLen / 2);
    state = INSTRUCTIONS2;
}

void goToInstructions3() {
    REG_BG0VOFF = 0;
    REG_BG0HOFF = 0;
    DMANow(3, instructions3Tiles, &CHARBLOCK[0], instructions3TilesLen / 2);
    DMANow(3, instructions3Map, &SCREENBLOCK[26], instructions3MapLen / 2);
    state = INSTRUCTIONS3;
}

void goToInstructions4() {
    REG_BG0VOFF = 0;
    REG_BG0HOFF = 0;
    DMANow(3, instructions4Tiles, &CHARBLOCK[0], instructions4TilesLen / 2);
    DMANow(3, instructions4Map, &SCREENBLOCK[26], instructions4MapLen / 2);
    state = INSTRUCTIONS4;
}

void goToInstructions5() {
    REG_BG0VOFF = 0;
    REG_BG0HOFF = 0;
    DMANow(3, instructions5Tiles, &CHARBLOCK[0], instructions5TilesLen / 2);
    DMANow(3, instructions5Map, &SCREENBLOCK[26], instructions5MapLen / 2);
    state = INSTRUCTIONS5;
}

void instructions1() {
    if (BUTTON_PRESSED(BUTTON_RIGHT)) {
        goToInstructions2();
    }
}

void instructions2() {
    if (BUTTON_PRESSED(BUTTON_LEFT)) {
        goToInstructions1();
    } else if (BUTTON_PRESSED(BUTTON_RIGHT)) {
        goToInstructions3();
    }
}

void instructions3() {
    if (BUTTON_PRESSED(BUTTON_LEFT)) {
        goToInstructions2();
    } else if (BUTTON_PRESSED(BUTTON_RIGHT)) {
        goToInstructions4();
    }
}

void instructions4() {
    if (BUTTON_PRESSED(BUTTON_LEFT)) {
        goToInstructions3();
    } else if (BUTTON_PRESSED(BUTTON_RIGHT)) {
        goToInstructions5();
    }
}

void instructions5() {
    if (BUTTON_PRESSED(BUTTON_LEFT)) {
        goToInstructions4();
    }
}

void goToBATTLE() {
    mgba_printf("the enemy is: %d", enmy);
    enemyTurn = 0;
    initEnemy(enmy);
    initPlayerHealthBar();
    initEnemyHealthBar();
    state = BATTLE;
}


void game() {
    // DMANow(3, bg2Pal + 16, PALETTE + 16, 16);
    if (currMap == MAP1) {
        REG_DISPCTL = MODE(0) | BG_ENABLE(2) | SPRITE_ENABLE;
        REG_BG2CNT = BG_CHARBLOCK(2) | BG_SCREENBLOCK(20) | BG_SIZE_SMALL | BG_4BPP;
        DMANow(3, mapOnePal, PALETTE, mapOnePalLen/2); // loading palette (note: if recoloring, change this palette)
        DMANow(3, mapOneTiles, &CHARBLOCK[2], mapOneTilesLen/2); // loading tileset into charblock 0
        DMANow(3, mapOneMap, &SCREENBLOCK[20], mapOneMapLen / 2);
    } else if (currMap == MAP2) {
        REG_DISPCTL = MODE(0) | BG_ENABLE(0) | BG_ENABLE(1) | BG_ENABLE(2) | SPRITE_ENABLE;
        REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(26) | BG_SIZE_SMALL | BG_4BPP; 
        REG_BG1CNT = BG_CHARBLOCK(1) | BG_SCREENBLOCK(28) | BG_SIZE_SMALL | BG_4BPP;
        REG_BG2CNT = BG_CHARBLOCK(2) | BG_SCREENBLOCK(20) | BG_SIZE_SMALL | BG_4BPP;
        DMANow(3, mapTwoPal, PALETTE, mapTwoPalLen/2); // loading palette (note: if recoloring, change this palette)
        DMANow(3, mapTwoTiles, &CHARBLOCK[2], mapTwoTilesLen/2); // loading tileset into charblock 0
        DMANow(3, mapTwoMap, &SCREENBLOCK[20], mapTwoMapLen / 2);
        DMANow(3, mapTwobg0Tiles, &CHARBLOCK[0], mapTwobg0TilesLen / 2);
        DMANow(3, mapTwobg1Tiles, &CHARBLOCK[1], mapTwobg1TilesLen / 2);
        DMANow(3, mapTwobg0Map, &SCREENBLOCK[26], mapTwobg0MapLen / 2);
        DMANow(3, mapTwobg1Map, &SCREENBLOCK[28], mapTwobg1MapLen / 2);
    }
    updateGame();
    if (BUTTON_PRESSED(BUTTON_SELECT)) {
        goToPause();
    }
    // DMANow(3, bg2Tiles, &CHARBLOCK[0], bg2TilesLen / 2);
    // DMANow(3, bg2Map, &SCREENBLOCK[26], bg2MapLen / 2);
    DMANow(3, spritesheetTiles, &CHARBLOCK[4], spritesheetTilesLen/2);
    DMANow(3, spritesheetPal, SPRITEPALETTE, 256);
    waitForVBlank();
    drawGame();
}

void goToPause() {
    state = PAUSE;
}

void pause() {
    REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(26) | BG_SIZE_SMALL | BG_4BPP; 
    REG_BG0HOFF = 0;
    REG_BG0VOFF = 0;
    hideSprites();
    DMANow(3, shadowOAM, OAM, 512);
    waitForVBlank();
    DMANow(3, pausePal, PALETTE, pausePalLen / 2); // loading palette (note: if recoloring, change this palette)
    DMANow(3, pauseTiles, &CHARBLOCK[0], pauseTilesLen /2); // loading tileset into charblock 0
    DMANow(3, pauseMap, &SCREENBLOCK[26], pauseMapLen / 2);
    hideSprites(); 
    if (BUTTON_PRESSED(BUTTON_START)) {
        goToGame();
    }
    if (BUTTON_PRESSED(BUTTON_SELECT)) {
        goToLose();
    }
}

void goToLose() {
    REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(26) | BG_SIZE_SMALL | BG_4BPP; 
    REG_BG0HOFF = 0;
    REG_BG0VOFF = 0;
    hideSprites();
    DMANow(3, shadowOAM, OAM, 512);
    waitForVBlank();
    DMANow(3, losePal, PALETTE, losePalLen /2); // loading palette (note: if recoloring, change this palette)
    DMANow(3, loseTiles, &CHARBLOCK[0], loseTilesLen /2); // loading tileset into charblock 0
    DMANow(3, loseMap, &SCREENBLOCK[26], loseMapLen / 2);
    state = LOSE;
}
void lose() {
    if (BUTTON_PRESSED(BUTTON_START)) {
        goToSplash();
    }
}

void goToWin() {
    REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(26) | BG_SIZE_SMALL | BG_4BPP; 
    REG_BG0HOFF = 0;
    REG_BG0VOFF = 0;
    hideSprites();
    DMANow(3, shadowOAM, OAM, 512);
    waitForVBlank();
    DMANow(3, winPal, PALETTE, winPalLen /2); // loading palette (note: if recoloring, change this palette)
    DMANow(3, winTiles, &CHARBLOCK[0], winTilesLen /2); // loading tileset into charblock 0
    DMANow(3, winMap, &SCREENBLOCK[26], winMapLen / 2);
    state = WIN;
}

void win() {
    if (BUTTON_PRESSED(BUTTON_START)) {
        goToSplash();
    }
}

// start the entire game thing (go to start screen)
void initialize() {
    REG_DISPCTL = MODE(0) | BG_ENABLE(0) | SPRITE_ENABLE;
    REG_BG0CNT = BG_CHARBLOCK(0) | BG_SCREENBLOCK(26) | BG_SIZE_SMALL| BG_4BPP; 
    second = 0;
    minute = 0;
    playSoundA(littleRoot_data, littleRoot_length, 1);
    mgba_open();
    hideSprites();
    DMANow(3, shadowOAM, OAM, 512);
    goToSplash();
    // setup sounds
    setupSounds();
    setupInterrupts();
}

void setupInterrupts() {

	REG_IME = 0;

    REG_IE = IRQ_VBLANK | IRQ_TIMER(2) | IRQ_TIMER(3);
    REG_DISPSTAT = DISPSTAT_VBLANK_IRQ;

    REG_INTERRUPT = &interruptHandler;

	REG_IME = 1;

}

void interruptHandler() {

	REG_IME = 0;

	if (REG_IF & IRQ_VBLANK) {
		if (soundA.isPlaying) {
			soundA.vBlankCount++;
			if (soundA.vBlankCount > soundA.durationInVBlanks) {
				 if (soundA.looping) {  //loops sound
                    playSoundA(soundA.data, soundA.dataLength, soundA.looping);
                } else { //stop sound
                    dma[1].cnt = 0; //to stop the sound
                    soundA.isPlaying = 0;
                    REG_TM0CNT = 0;
                }
			}
		}

        if (soundB.isPlaying) {
            soundB.vBlankCount++;
            if (soundB.vBlankCount > soundB.durationInVBlanks) { // stop or loop if sound ended
                if (soundB.looping) {  //loops sound
                    playSoundB(soundB.data, soundB.dataLength, soundB.looping);
                } else { //stop sound
                    dma[2].cnt = 0; //to stop the sound
                    soundB.isPlaying = 0;
                    REG_TM1CNT = 0;
                }
            }
		}
	}

    if (REG_IF & IRQ_TIMER(2)) {
        second = (second + 1) % 60;
    } 

    if (REG_IF & IRQ_TIMER(3)) {
        minute = (minute + 1) % 100;
    }

    REG_IF = REG_IF;
    REG_IME = 1;

}
