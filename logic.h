#ifndef LOGIC_H
#define LOGIC_H

#include "gba.h"


// This function can initialize an unused AppState struct.
void initializeAppState(AppState *appState);
void initializeNextLevel(AppState *state);
void clearLevel(AppState *state, int goToNextLevel);

// This function will be used to process app frames.
AppState processAppState(AppState *currentAppState, u32 keysPressedBefore, u32 keysPressedNow);

// If you have anything else you need accessible from outside the logic.c
// file, you can add them here. You likely won't.

#endif
