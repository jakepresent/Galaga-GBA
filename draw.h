#ifndef DRAW_H
#define DRAW_H

#include "logic.h"

// This function will be used to draw everything about the state of your app
// including the background and whatnot.
void fullDrawAppState(AppState *state);

// This function will be used to draw things that might have moved in a frame.
// For example, in a Snake game, draw the snake, the food, the score.
void drawAppState(AppState *state);

void drawLevelText(int level);

// If you have anything else you need accessible from outside the graphics.c
// file, you can add them here. You likely won't.

#endif
