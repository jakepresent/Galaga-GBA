/*
 * Exported with nin10kit v1.7
 * Invocation command was nin10kit --mode=sprites --bpp=4 --for_bitmap --transparent=000000 sprites ship.png alien1.png alien2.png bullet.png 
 * Time-stamp: Thursday 04/04/2019, 03:41:02
 * 
 * Image Information
 * -----------------
 * ship.png 16@16
 * alien1.png 16@16
 * alien2.png 16@16
 * bullet.png 8@8
 * Transparent color: (0, 0, 0)
 * 
 * All bug reports / feature requests are to be filed here https://github.com/TricksterGuy/nin10kit/issues
 */

#ifndef SPRITES_H
#define SPRITES_H

#define SPRITES_PALETTE_TYPE (0 << 13)
#define SPRITES_DIMENSION_TYPE (1 << 6)

extern const unsigned short sprites_palette[256];
#define SPRITES_PALETTE_SIZE 512
#define SPRITES_PALETTE_LENGTH 256

extern const unsigned short sprites[208];
#define SPRITES_SIZE 416
#define SPRITES_LENGTH 208

#define SHIP_PALETTE_ID (0 << 12)
#define SHIP_SPRITE_SHAPE (0 << 14)
#define SHIP_SPRITE_SIZE (1 << 14)
#define SHIP_ID 512

#define ALIEN1_PALETTE_ID (0 << 12)
#define ALIEN1_SPRITE_SHAPE (0 << 14)
#define ALIEN1_SPRITE_SIZE (1 << 14)
#define ALIEN1_ID 516

#define ALIEN2_PALETTE_ID (0 << 12)
#define ALIEN2_SPRITE_SHAPE (0 << 14)
#define ALIEN2_SPRITE_SIZE (1 << 14)
#define ALIEN2_ID 520

#define BULLET_PALETTE_ID (0 << 12)
#define BULLET_SPRITE_SHAPE (0 << 14)
#define BULLET_SPRITE_SIZE (0 << 14)
#define BULLET_ID 524

#endif

