#include "draw.h"
#include "gba.h"
#include "images/endScreen.h"
#include "images/sprites.h"
#include <stdlib.h>
#include <stdio.h>

// TA-TODO: Include any header files for images generated by nin10kit.
// Example for the provided garbage image:
// #include "images/garbage.h"

// TA-TODO: Add any draw/undraw functions for sub-elements of your app here.
// For example, for a snake game, you could have a drawSnake function
// or a drawFood function
//
// e.g.:
// static void drawSnake(Snake* snake);
// static void drawFood(Food* food);

volatile OamEntry shadow[128];
volatile OamEntry* shipSprite = &shadow[0];
volatile OamEntry* firstBulletSprite = &shadow[1];
volatile OamEntry* firstAlienSprite = &shadow[21];


static void drawSprites(void) {
	DMA[3].src = shadow;
   	DMA[3].dst = OAMMEM;
   	DMA[3].cnt = 128*4 | DMA_ON;
}

static void drawPlayer(Player *p) {
	//drawImageDMA(p->x, p->y, SPRITE_WIDTH, SPRITE_HEIGHT, ship);
	if (p != NULL) {
		shipSprite->attr0 = p->y | SPRITES_PALETTE_TYPE | SHIP_SPRITE_SHAPE;
		shipSprite->attr1 = p->x | SHIP_SPRITE_SIZE;
	} else {
		shipSprite->attr0 = ATTR0_HIDE;
	}
}


static void drawAlien(Alien *a, int index) {
	volatile OamEntry *alienSprite = firstAlienSprite + index;
	if (a->isType1) {
		alienSprite->attr0 = a->y | SPRITES_PALETTE_TYPE | ALIEN1_SPRITE_SHAPE;
		alienSprite->attr1 = a->x | ALIEN1_SPRITE_SIZE;
	} else {
		alienSprite->attr0 = a->y | SPRITES_PALETTE_TYPE | ALIEN2_SPRITE_SHAPE;
		alienSprite->attr1 = a->x | ALIEN2_SPRITE_SIZE;
	}
}

static void drawAlienList(Alien **list, int numAliens) {
	if (list == NULL) {
		return;
	}
	for (int i = 0; i < numAliens; i++) {
		if (list[i] == NULL) {
			(firstAlienSprite + i)->attr0 = ATTR0_HIDE;
		} else {
    		drawAlien(list[i], i);
    	}
    }
}

static void drawBullet(Bullet *b, int index) {
	volatile OamEntry *bulletSprite = firstBulletSprite + index;
	bulletSprite->attr0 = b->y | SPRITES_PALETTE_TYPE | BULLET_SPRITE_SHAPE;
	bulletSprite->attr1 = b->x | BULLET_SPRITE_SIZE;
	bulletSprite->attr2 = BULLET_PALETTE_ID | BULLET_ID;
}

static void drawBulletList(Bullet **list) {
	for (int i = 0; i < 20; i++) {
		if (list == NULL || list[i] == NULL) {
			(firstBulletSprite + i)->attr0 = ATTR0_HIDE;
		}
		else {
			drawBullet(list[i], i);
		}
	}
}
// static void eraseAlien(Alien *a) {
// 	drawRectDMA(a->x, a->y, SPRITE_WIDTH, SPRITE_HEIGHT, BLACK);
// }

// This function will be used to draw everything about the app
// including the background and whatnot.
void fullDrawAppState(AppState *state) {
    //set variable to refer to correct sprite

    fillScreenDMA(BLACK);

    shipSprite = &shadow[0];
    shipSprite->attr2 = SHIP_PALETTE_ID | SHIP_ID;

    firstBulletSprite = &shadow[1];

    firstAlienSprite = &shadow[21];
    for (int i = 0; i < state->numAliens; i++) {
    	//set correct sprites for aliens
    	if (state->alienList[i]->isType1) {
    		(firstAlienSprite + i)->attr2 = ALIEN1_PALETTE_ID | ALIEN1_ID;
    	} else {
    		(firstAlienSprite + i)->attr2 = ALIEN2_PALETTE_ID | ALIEN2_ID;
    	}
    	drawAlien(state->alienList[i], i);
    }


    // TA-TODO: IMPLEMENT.
    drawPlayer(state->player);
    drawSprites();
    /*char *hello_world = (char*)malloc(13 * sizeof(char));
    sprintf(hello_world, "%d", state->alienList[i]->isType1);
    drawString(10 * i, i / 20 * 10, hello_world, BLUE);
    free(hello_world);*/
}


void drawLevelText(int level) {
	for(int i = 0; i < 128; i++) {
           shadow[i].attr0 = ATTR0_HIDE;
    }
    drawRectDMA(0, 0, WIDTH, HEIGHT - 8, BLACK);
    char str[9];
    sprintf(str, "Level %d", level);
    drawCenteredString(WIDTH / 2, HEIGHT / 2, 0, 0, str, WHITE);
    u32 currentFrame = vBlankCounter;
    while(currentFrame + 80 > vBlankCounter) { //delay of 80 frames = 1.33 sec
    	waitForVBlank();
    	if (KEY_DOWN(BUTTON_SELECT, BUTTONS)) {
    		return;
    	}
    }
}

void drawScoreText(int score) {
	drawRectDMA(0, HEIGHT - 8, 100, 8, BLACK);
    char str[20];
    sprintf(str, "Score: %08d", score);
    drawString(0, HEIGHT - 8, str, WHITE);
}

void drawLivesText(int lives) {
	drawRectDMA(WIDTH - 100, HEIGHT - 8, 100, 8, BLACK);
    char str[10];
    sprintf(str, "Lives: %02d", lives);
    drawString(WIDTH - 55, HEIGHT - 8, str, WHITE);
}

void drawGameOverScreen(AppState *state) {
    static int isShown = 0;
    if (vBlankCounter % 40 == 0) {
    	if (isShown) {
    		drawRectDMA(60, 80, 130, 60, BLACK);
    		isShown = 0;
    	} else {
    		drawFullScreenImageDMA(endScreen);
    		isShown = 1;
    	}
    }

    char str[20];
    sprintf(str, "Score: %08d", state->score);
    drawString(0, HEIGHT - 8, str, WHITE);

    sprintf(str, "Level: %02d", state->level);
    drawString(WIDTH-55, HEIGHT - 8, str, WHITE);
}

// This function will be used to draw things that might have moved in a frame.
// For example, in a Snake game, draw the snake, the food, the score.
void drawAppState(AppState *state) {
    // TA-TODO: IMPLEMENT.
    drawScoreText(state->score);
    drawLivesText(state->lives);
    drawPlayer(state->player);
    drawAlienList(state->alienList, state->numAliens);
    drawBulletList(state->bulletList);
    drawSprites();
}

void setUpSprites(void) {
    //load sprite palette
    DMA[3].src = sprites_palette;
    DMA[3].dst = SPRITEPAL;
    DMA[3].cnt = SPRITES_PALETTE_LENGTH | DMA_ON;

    //copy sprite graphic data to where gba expects
    DMA[3].src = sprites;
    DMA[3].dst = &CHARBLOCKBASE[5];
    DMA[3].cnt = SPRITES_LENGTH | DMA_ON;

    //hide sprites
    for(int i = 0; i < 128; i++) {
        shadow[i].attr0 = ATTR0_HIDE;
    }
}

void clearSprites(void) {
	for(int i = 0; i < 128; i++) {
        shadow[i].attr0 = ATTR0_HIDE;
    }
    drawSprites();
}