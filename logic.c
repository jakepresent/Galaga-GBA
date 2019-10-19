#include "logic.h"
#include "draw.h"
#include <stdlib.h>
#include <stdio.h>

static Player *initializePlayer(void) {
    Player *p = (Player *) malloc(sizeof(Player));
    p->x = (WIDTH - SPRITE_WIDTH) / 2;
    p->y = HEIGHT - (SPRITE_HEIGHT * 3 / 2) - 3;
    return p;
}
static Alien *initializeAlien(int rowWidth, int amount, int index) {
    Alien *alien = (Alien *) malloc(sizeof(Alien));
    alien->y = 5 + SPRITE_HEIGHT * (index / rowWidth) * 6 / 5;
    alien->isType1 = randint(0, 2);
    int numFullRows = amount / rowWidth;
    int pos = index % rowWidth;
    if (index >= numFullRows * rowWidth) { //last row; not full
        int totalInLine = amount % rowWidth;
        alien->x = (WIDTH + (SPRITE_WIDTH * ((2 * pos) - totalInLine)) * 9 / 8) / 2;
    } else { //full row
        alien->x = (WIDTH + (SPRITE_WIDTH * ((2 * pos) - rowWidth)) * 9 / 8) / 2;
    }
    return alien;
}



static Alien **initializeAlienList(int amount, int rowWidth) {
    Alien **alienList = (Alien **) malloc(sizeof(Alien *) * amount);
    for (int i = 0; i < amount; i++) {
        alienList[i] = initializeAlien(rowWidth, amount, i);
    }
    return alienList;
}

static Bullet **initializeBulletList(int maxAmount) {
    Bullet** list = (Bullet **) malloc(sizeof(Bullet *) * maxAmount);
    for (int i = 0; i < maxAmount; i++) {
        list[i] = NULL;
    }
    return list;
}

static int initializeBullet(Bullet **list, int x, int y, int isPlayer) {
    for (int i = 0; i < 20; i++) {
        if (list[i] == NULL) {
            Bullet *b = (Bullet *) malloc(sizeof(Bullet));
            b->x = x;
            b->y = y;
            b->isPlayer = isPlayer;
            list[i] = b;
            return i;
        }
    }
    return -1;
}

void initializeAppState(AppState* appState) {
    // TA-TODO: Initialize everything that's part of this AppState struct here.
    // Suppose the struct contains random values, make sure everything gets
    // the value it should have when the app begins.
    appState->gameOver = 0;
    appState->nextLevel = 0;
    appState->level = 1;
    appState->lives = 3;
    appState->score = 0;
    appState->numAliens = 5;
    appState->alienSpeed = 60;
    appState->movingRight = 0; //start by moving left
    appState->bulletTimer = 0;
    appState->player = initializePlayer();
    appState->alienList = initializeAlienList(appState->numAliens, 8);
    appState->bulletList = initializeBulletList(20);
}

// TA-TODO: Add any process functions for sub-elements of your app here.
// For example, for a snake game, you could have a processSnake function
// or a createRandomFood function or a processFoods function.
//
// e.g.:
// static Snake processSnake(Snake* currentSnake);
// static void generateRandomFoods(AppState* currentAppState, AppState* nextAppState);

static void alienBulletCollision(AppState *state, int aIndex, int bIndex) {
    free(state->alienList[aIndex]);
    state->alienList[aIndex] = NULL;
    free(state->bulletList[bIndex]);
    state->bulletList[bIndex] = NULL;
    state->score += 100 * state->level;
}

static void playerBulletCollision(AppState *state, int bIndex) {
    free(state->player);
    state->player = NULL;
    free(state->bulletList[bIndex]);
    state->bulletList[bIndex] = NULL;
    state->lives -= 1;
    state->bulletTimer = vBlankCounter + 40; //set timer to respawn after .667 seconds
    if (state->lives == 0) {
        state->gameOver = 1;
    }
}

//returns true if a point is inside the player bounds
static int isInsidePlayer(Player *player, int x, int y) {
    int x1 = player->x;
    int x2 = player->x + 2;
    int x3 = player->x + 5;
    int x4 = player->x + 9;
    int x5 = player->x + 12;
    int x6 = player->x + 14;
    int y1 = player->y + 1;
    int y2 = player->y + 6;
    int y3 = player->y + 10;
    int y4 = player->y + 15; //lines to form around ship

    return (y <= y4 && //is bullet above ship
            ((x1 <= x && x < x2 && y3 <= y) || //first rect
            (x2 <= x && x < x3 && y2 <= y) || //second rect
            (x3 <= x && x <= x4 && y1 <= y) || //middle rect
            (x4 < x && x <= x5 && y2 <= y) ||
            (x5 < x && x <= x6 && y3 <= y)));
}

static void updateBullets(Bullet** list, AppState *state) {
    if (list == NULL) {
        return;
    }
    for (int i = 0; i < 20; i++) {
        if (list[i] != NULL) {
            if (list[i]->y <= -8 || list[i]->y >= HEIGHT + 8) { //out of bounds – destroy bullet
                free(list[i]);
                list[i] = NULL;
            } else { //still in bounds
                if (list[i]->isPlayer) {
                    Alien *alien;
                    for (int j = 0; j < state->numAliens; j++) {
                        alien = state->alienList[j];
                        if (alien != NULL) {
                            if (alien->isType1) {
                                if (list[i]->y <= alien->y + 9 &&
                                        list[i]->y >= alien->y + 2 &&
                                        list[i]->x <= alien->x + 12 &&
                                        list[i]->x >= alien->x + 2) { //collision
                                    alienBulletCollision(state, j, i);
                                }
                            } else {
                                if (alien->y <= list[i]->y && //top line
                                        ((list[i]->y <= alien->y + 15 && //bottom line
                                        ((alien->x <= list[i]->x &&
                                        list[i]->x <= alien->x + 3) || //left side
                                        (alien->x + 11 <= list[i]->x &&
                                        list[i]->x <= alien->x + 14))) || //right side
                                        (list[i]->y <= alien->y + 10 && //middle bottom line
                                        alien->x + 4 <= list[i]->x && //middle left line
                                        list[i]->x <= alien->x + 10))) { //middle right line
                                    alienBulletCollision(state, j, i);
                                }
                            }
                        }
                    }
                    list[i]->y -= 3;
                } else {
                    if (isInsidePlayer(state->player, list[i]->x, list[i]->y + 7)) {
                        playerBulletCollision(state, i);
                    }


                    list[i]->y += 3;
                }
            }
        }
    }
}



static int doEvery(int frames) { //returns 1 if should be done now, 0 otherwise
    return vBlankCounter % frames == 0;
}


void clearLevel(AppState *state, int goToNextLevel) {
    if (state->alienList != NULL) {
        for (int i = 0; i < state->numAliens; i++) {
            if (state->alienList[i] != NULL) {
                free(state->alienList[i]);
                state->alienList[i] = NULL;
            }
        }
        free(state->alienList);
        state->alienList = NULL;
    }
    if (state->player != NULL) {
        free(state->player);
        state->player = NULL;
    }
    if (state->bulletList != NULL) {
        for (int i = 0; i < 20; i++) {
            if (state->bulletList[i] != NULL) {
                free(state->bulletList[i]);
                state->bulletList[i] = NULL;
            }
        }
        free(state->bulletList);
        state->bulletList = NULL;
    }

    if (goToNextLevel) {
        state->level++;
        state->lives++;
        state->score += 10000 * (state->level - 1);;
        state->numAliens += 2;
        if (state->alienSpeed > 1) {
            state->alienSpeed *= 55;
            state->alienSpeed >>= 6; // /= 64
        }

        state->nextLevel = 1;
    }
}
void initializeNextLevel(AppState *state) {
    int rowWidth = 8;
    if (state->level >= 5) {
        rowWidth++;
    }
    if (state->level >= 10) {
        rowWidth++;
    }
    state->bulletList = initializeBulletList(20);
    state->alienList = initializeAlienList(state->numAliens, rowWidth);
    state->player = initializePlayer();
    state->nextLevel = 0;
}

static void updateAliens(Alien** list, AppState *state) {
    int flip = 0;
    static int allGone = 0;

    if (allGone) {
        allGone = 0;
        clearLevel(state, 1);
        return;
    }

    int offset = 5;
    if ((state->level + 2) / 7  < 5) {
        offset -= (state->level + 2) / 7;
    }

    if (doEvery(state->alienSpeed)) {
        for (int i = 0; i < state->numAliens; i++) {
            if (list[i] != NULL) {
                allGone = 0;
                if (state->movingRight) {
                    list[i]->x += offset;
                } else {
                    list[i]->x -= offset;
                }
                if (list[i]->x >= WIDTH - SPRITE_WIDTH - 10 || list[i]->x < 5) {
                    flip = 1;
                }
            }
        }
    }

    //reverses direction of aliens when one reaches the boundary
    if (flip) {
        state->movingRight = state->movingRight ? 0 : 1;
        for (int i = 0; i < state->numAliens; i++) {
            if (list[i] != NULL) {
                list[i]->y += 10;
                if (state->movingRight) {
                    list[i]->x += offset; //adjusting so aliens move down in a straight line
                } else {
                    list[i]->x -= offset;
                }
            }
        }
    }

    allGone = 1;

    for (int i = 0; i < state->numAliens; i++) {
        if (list[i] != NULL) {
            allGone = 0;
            if (list[i]-> y >= state->player->y - SPRITE_HEIGHT) {
                if (isInsidePlayer(state->player, list[i]->x + 1, list[i]->y + 1) ||
                        isInsidePlayer(state->player, list[i]->x + 1, list[i]->y + 12) ||
                        isInsidePlayer(state->player, list[i]->x + 14, list[i]->y + 1) ||
                        isInsidePlayer(state->player, list[i]->x + 14, list[i]->y + 12)) {
                    state->gameOver = 1;
                }
            }
        }
    }


    //randomly fire bullets
    if (randint(0, 64) == 0) {
        //pick a random alien that isn't null
        int i = randint(0, state->numAliens);
        int start = i;
        while (list[i] == NULL) {
            i++;
            if (i >= state->numAliens) {
                i = 0;
            }
            if (i == start) {
                return;
            }
        }
        initializeBullet(state->bulletList, list[i]->x + SPRITE_WIDTH / 2 - 1,
            list[i]->y + SPRITE_HEIGHT - 10, 0);
    }
}

// // This function processes your current app state and returns the new (i.e. next)
// state of your application.
AppState processAppState(AppState *currentAppState, u32 keysPressedBefore, u32 keysPressedNow) {
    /* TA-TODO: Do all of your app processing here. This function gets called
     * every frame.
     *
     * To check for key presses, use the KEY_JUST_PRESSED macro for cases where
     * you want to detect each key press once, or the KEY_DOWN macro for checking
     * if a button is still down.
     *
     * To count time, suppose that the GameBoy runs at a fixed FPS (60fps) and
     * that VBlank is processed once per frame. Use the vBlankCounter variable
     * and the modulus % operator to do things once every (n) frames. Note that
     * you want to process button every frame regardless (otherwise you will
     * miss inputs.)
     *
     * Do not do any drawing here.
     *
     * TA-TODO: VERY IMPORTANT! READ THIS PART.
     * You need to perform all calculations on the currentAppState passed to you,
     * and perform all state updates on the nextAppState state which we define below
     * and return at the end of the function. YOU SHOULD NOT MODIFY THE CURRENTSTATE.
     * Modifying the currentAppState will mean the undraw function will not be able
     * to undraw it later.
     */

    //AppState nextAppState = *currentAppState;

    

    Player *player = currentAppState->player;

    //left/right logic
    if (KEY_DOWN(BUTTON_LEFT, keysPressedNow)) {
        if (player->x > 10) {
            if (doEvery(3)) {
                player->x -= 2;
            } else {
                player->x -= 1; //average rate of 1.33px/frame
            }
        }
    } else if (KEY_DOWN(BUTTON_RIGHT, keysPressedNow)) {
        if (player->x < WIDTH - SPRITE_WIDTH - 10) {
            if (doEvery(3)) {
                player->x += 2;
            } else {
                player->x += 1; //average rate of 1.33px/frame
            }
        }
    }

    if (KEY_JUST_PRESSED(BUTTON_A, keysPressedNow, keysPressedBefore)) {
        if (currentAppState->bulletTimer <= vBlankCounter) {
            int index = initializeBullet(currentAppState->bulletList, 
                player->x + SPRITE_WIDTH / 2 - 1, player->y + 10, 1);
            if (index != -1) {
                currentAppState->bulletTimer = vBlankCounter + 25; // 0.33 second delay between shots
            }
        }
    }

    if (currentAppState->player == NULL) {
        if (currentAppState->bulletTimer <= vBlankCounter) {
            currentAppState->player = initializePlayer();
        }
    }

    updateAliens(currentAppState->alienList, currentAppState);
    updateBullets(currentAppState->bulletList, currentAppState);
    /*drawRectDMA(10, 10, 100, 40, BLACK);

    char *hello_world = (char*)malloc(20 * sizeof(char));
    sprintf(hello_world, "%d", vBlankCounter);
    drawString(10, 10, hello_world, BLUE);
    free(hello_world);*/

    return *currentAppState;
}
