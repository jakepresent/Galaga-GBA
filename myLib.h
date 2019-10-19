#ifndef MYLIB_H
#define MYLIB_H

// ---------------------------------------------------------------------------
//                       USEFUL TYPEDEFS
// ---------------------------------------------------------------------------
/** An unsigned 32-bit (4-byte) type */
typedef unsigned int u32;

/** An unsigned 16-bit (2-byte) type */
typedef unsigned short u16;

/** An unsigned 8-bit (1-byte) type. Note that this type cannot be written onto RAM directly. */
typedef unsigned char u8;


#define SPRITE_HEIGHT 16
#define SPRITE_WIDTH 16

// ---------------------------------------------------------------------------
//                       SPRITE INFO
// ---------------------------------------------------------------------------

#define OBJ_ENABLE (1<<12)

typedef struct {
   u16 attr0;
   u16 attr1;
   u16 attr2;
   u16 fill;
} OamEntry;

#define OAMMEM  ((OamEntry*)0x7000000)
#define SPRITEPAL ((u16 *)0x5000200)
typedef struct { u16 tileimg[8192]; } charblock;
#define CHARBLOCKBASE ((charblock*)0x6000000)
#define ATTR0_REG (0<<8) // Default
#define ATTR0_HIDE (2<<8) // If set the sprite is hidden, by default all sprites are SHOWN
#define ATTR0_MOSAIC (1<<12) // C controls Mosaic effect if set the sprite will appear pixelated.
#define ATTR1_NOFLIP 0
#define ATTR1_HFLIP (1<<12)
#define ATTR1_VFLIP (1<<13)
#define PRIORITY(pri) ((pri) << 10)

// This is initialized in gba.c
extern volatile unsigned short *videoBuffer;


typedef struct {
    int x;
    int y;
} Player;

typedef struct {
    int x;
    int y;
    int isType1;
} Alien;

typedef struct {
    int x;
    int y;
    int isPlayer;
} Bullet;

typedef struct {
    // Store whether or not the game is over in this member:
    int gameOver;
    int nextLevel;
    int level;
    int lives;
    int score;
    int numAliens;
    int alienSpeed; //how often to move aliens in frames - lower is faster
    int movingRight; //0 for left, 1 for right
    u32 bulletTimer; //stores current time + offset to check for successive A presses
    Player *player;
    Alien **alienList;
    Bullet **bulletList; //max of 20 on screen at once

} AppState;

void setUpSprites(void);
void clearSprites(void);
void drawGameOverScreen(AppState *state);

#endif