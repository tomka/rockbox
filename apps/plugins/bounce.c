/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2002 Daniel Stenberg
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 **************************************************************************/
#include "plugin.h"
#include "time.h"

#ifdef HAVE_LCD_BITMAP

PLUGIN_HEADER

#define SS_TITLE       "Bouncer"
#define SS_TITLE_FONT  2

#define LETTERS_ON_SCREEN (LCD_WIDTH/10)

#define YSPEED 2
#define XSPEED 3
#define YADD -4

/* variable button definitions */
#if CONFIG_KEYPAD == RECORDER_PAD
#define BOUNCE_UP   BUTTON_UP
#define BOUNCE_DOWN BUTTON_DOWN
#define BOUNCE_QUIT (BUTTON_OFF | BUTTON_REL)
#define BOUNCE_MODE (BUTTON_ON | BUTTON_REL)

#elif CONFIG_KEYPAD == ONDIO_PAD
#define BOUNCE_UP   BUTTON_UP
#define BOUNCE_DOWN BUTTON_DOWN
#define BOUNCE_QUIT (BUTTON_OFF | BUTTON_REL)
#define BOUNCE_MODE (BUTTON_MENU | BUTTON_REL)

#elif (CONFIG_KEYPAD == IRIVER_H100_PAD) || \
      (CONFIG_KEYPAD == IRIVER_H300_PAD)
#define BOUNCE_UP   BUTTON_UP
#define BOUNCE_DOWN BUTTON_DOWN
#define BOUNCE_QUIT (BUTTON_OFF | BUTTON_REL)
#define BOUNCE_MODE (BUTTON_SELECT | BUTTON_REL)

#define BOUNCE_RC_QUIT (BUTTON_RC_STOP | BUTTON_REL)

#elif (CONFIG_KEYPAD == IPOD_4G_PAD) || \
      (CONFIG_KEYPAD == IPOD_3G_PAD)
#define BOUNCE_UP   BUTTON_SCROLL_BACK
#define BOUNCE_DOWN BUTTON_SCROLL_FWD
#define BOUNCE_QUIT (BUTTON_MENU | BUTTON_REL)
#define BOUNCE_MODE (BUTTON_SELECT | BUTTON_REL)

#elif (CONFIG_KEYPAD == IAUDIO_X5_PAD)
#define BOUNCE_UP   BUTTON_UP
#define BOUNCE_DOWN BUTTON_DOWN
#define BOUNCE_QUIT BUTTON_POWER
#define BOUNCE_MODE BUTTON_PLAY

#elif (CONFIG_KEYPAD == GIGABEAT_PAD)
#define BOUNCE_UP   BUTTON_UP
#define BOUNCE_DOWN BUTTON_DOWN
#define BOUNCE_QUIT BUTTON_A
#define BOUNCE_MODE BUTTON_POWER

#elif CONFIG_KEYPAD == SANSA_E200_PAD
#define BOUNCE_UP   BUTTON_SCROLL_UP
#define BOUNCE_DOWN BUTTON_SCROLL_DOWN
#define BOUNCE_QUIT BUTTON_POWER
#define BOUNCE_MODE BUTTON_SELECT

#elif (CONFIG_KEYPAD == IRIVER_H10_PAD)
#define BOUNCE_UP   BUTTON_SCROLL_UP
#define BOUNCE_DOWN BUTTON_SCROLL_DOWN
#define BOUNCE_QUIT BUTTON_POWER
#define BOUNCE_MODE BUTTON_PLAY

#endif

static struct plugin_api* rb;

#define TABLE_SIZE (sizeof(table)/sizeof(table[0]))

#if LCD_HEIGHT < 128

static unsigned char table[]={
26,28,30,33,35,37,39,40,42,43,45,46,46,47,47,47,47,47,46,46,45,43,42,40,39,37,35,33,30,28,26,24,21,19,17,14,12,10,8,7,5,4,2,1,1,0,0,0,0,0,1,1,2,4,5,7,8,10,12,14,17,19,21,23,
};

static unsigned char xtable[]={
    50, 54, 59, 64, 69, 73, 77, 81, 85, 88, 91, 94, 96, 97, 99, 99, 99, 99, 
    99, 97, 96, 94, 91, 88, 85, 81, 77, 73, 69, 64, 59, 54, 50, 45, 40, 35, 
    30, 26, 22, 18, 14, 11, 8, 5, 3, 2, 0, 0, 0, 0, 0, 2, 3, 5, 8, 11, 14, 
    18, 22, 26, 30, 35, 40, 45, 
};
#else

/* 160 - 12 = 148
   148 / 2 = 74 (radius)
*/

static unsigned char xtable[]={
74, 77, 81, 84, 88, 91, 95, 98, 102, 105, 108, 112, 115, 118, 120, 123, 
126, 128, 131, 133, 135, 137, 139, 140, 142, 143, 144, 145, 146, 147, 147, 
147, 147, 147, 147, 147, 146, 145, 144, 143, 142, 140, 139, 137, 135, 133, 
131, 128, 126, 123, 120, 118, 115, 112, 108, 105, 102, 98, 95, 91, 88, 
84, 81, 77, 74, 70, 66, 63, 59, 56, 52, 49, 45, 42, 39, 35, 32, 29, 27, 
24, 21, 19, 16, 14, 12, 10, 8, 7, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 
2, 3, 4, 5, 7, 8, 10, 12, 14, 16, 19, 21, 24, 27, 29, 32, 35, 39, 42, 45, 
49, 52, 56, 59, 63, 66, 70, 

};

/* 128 - 16 = 116
   112 / 2 = 56 (radius)
*/
static unsigned char table[]={
56, 58, 61, 64, 66, 69, 72, 74, 77, 79, 82, 84, 87, 89, 91, 93, 95, 97, 
99, 100, 102, 104, 105, 106, 107, 108, 109, 110, 110, 111, 111, 111, 111, 
111, 111, 111, 110, 110, 109, 108, 107, 106, 105, 104, 102, 100, 99, 97, 
95, 93, 91, 89, 87, 84, 82, 79, 77, 74, 72, 69, 66, 64, 61, 58, 56, 53, 
50, 47, 45, 42, 39, 37, 34, 32, 29, 27, 24, 22, 20, 18, 16, 14, 12, 11, 
9, 7, 6, 5, 4, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 4, 5, 6, 7, 
9, 11, 12, 14, 16, 18, 20, 22, 24, 27, 29, 32, 34, 37, 39, 42, 45, 47, 
50, 53, 

};

#endif

static signed char speed[]={
  1,2,3,3,3,2,1,0,-1,-2,-2,-2,-1,0,0,1,
};

#define LETTER_WIDTH 12 /* pixels wide */

const unsigned char char_gen_12x16[][22] =
{
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
    { 0x00,0x00,0x00,0x7c,0xff,0xff,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x33,0x00,0x00,0x00,0x00,0x00 },
    { 0x00,0x00,0x3c,0x3c,0x00,0x00,0x3c,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
    { 0x00,0x10,0x90,0xf0,0x7e,0x1e,0x90,0xf0,0x7e,0x1e,0x10,0x02,0x1e,0x1f,0x03,0x02,0x1e,0x1f,0x03,0x02,0x00,0x00 },
    { 0x00,0x78,0xfc,0xcc,0xff,0xff,0xcc,0xcc,0x88,0x00,0x00,0x00,0x04,0x0c,0x0c,0x3f,0x3f,0x0c,0x0f,0x07,0x00,0x00 },
    { 0x00,0x38,0x38,0x38,0x00,0x80,0xc0,0xe0,0x70,0x38,0x1c,0x30,0x38,0x1c,0x0e,0x07,0x03,0x01,0x38,0x38,0x38,0x00 },
    { 0x00,0x00,0xb8,0xfc,0xc6,0xe2,0x3e,0x1c,0x00,0x00,0x00,0x00,0x1f,0x3f,0x31,0x21,0x37,0x1e,0x1c,0x36,0x22,0x00 },
    { 0x00,0x00,0x00,0x27,0x3f,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
    { 0x00,0x00,0xf0,0xfc,0xfe,0x07,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x03,0x0f,0x1f,0x38,0x20,0x20,0x00,0x00,0x00 },
    { 0x00,0x00,0x01,0x01,0x07,0xfe,0xfc,0xf0,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x38,0x1f,0x0f,0x03,0x00,0x00,0x00 },
    { 0x00,0x98,0xb8,0xe0,0xf8,0xf8,0xe0,0xb8,0x98,0x00,0x00,0x00,0x0c,0x0e,0x03,0x0f,0x0f,0x03,0x0e,0x0c,0x00,0x00 },
    { 0x00,0x80,0x80,0x80,0xf0,0xf0,0x80,0x80,0x80,0x00,0x00,0x00,0x01,0x01,0x01,0x0f,0x0f,0x01,0x01,0x01,0x00,0x00 },
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xb8,0xf8,0x78,0x00,0x00,0x00,0x00,0x00 },
    { 0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00 },
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x38,0x38,0x00,0x00,0x00,0x00,0x00 },
    { 0x00,0x00,0x00,0x00,0x80,0xc0,0xe0,0x70,0x38,0x1c,0x0e,0x18,0x1c,0x0e,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00 },
    { 0xf8,0xfe,0x06,0x03,0x83,0xc3,0x63,0x33,0x1e,0xfe,0xf8,0x07,0x1f,0x1e,0x33,0x31,0x30,0x30,0x30,0x18,0x1f,0x07 },
    { 0x00,0x00,0x0c,0x0c,0x0e,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x30,0x3f,0x3f,0x30,0x30,0x30,0x00 },
    { 0x1c,0x1e,0x07,0x03,0x03,0x83,0xc3,0xe3,0x77,0x3e,0x1c,0x30,0x38,0x3c,0x3e,0x37,0x33,0x31,0x30,0x30,0x30,0x30 },
    { 0x0c,0x0e,0x07,0xc3,0xc3,0xc3,0xc3,0xc3,0xe7,0x7e,0x3c,0x0c,0x1c,0x38,0x30,0x30,0x30,0x30,0x30,0x39,0x1f,0x0e },
    { 0xc0,0xe0,0x70,0x38,0x1c,0x0e,0x07,0xff,0xff,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x3f,0x3f,0x03,0x03 },
    { 0x3f,0x7f,0x63,0x63,0x63,0x63,0x63,0x63,0xe3,0xc3,0x83,0x0c,0x1c,0x38,0x30,0x30,0x30,0x30,0x30,0x38,0x1f,0x0f },
    { 0xc0,0xf0,0xf8,0xdc,0xce,0xc7,0xc3,0xc3,0xc3,0x80,0x00,0x0f,0x1f,0x39,0x30,0x30,0x30,0x30,0x30,0x39,0x1f,0x0f },
    { 0x03,0x03,0x03,0x03,0x03,0x03,0xc3,0xf3,0x3f,0x0f,0x03,0x00,0x00,0x00,0x30,0x3c,0x0f,0x03,0x00,0x00,0x00,0x00 },
    { 0x00,0xbc,0xfe,0xe7,0xc3,0xc3,0xc3,0xe7,0xfe,0xbc,0x00,0x0f,0x1f,0x39,0x30,0x30,0x30,0x30,0x30,0x39,0x1f,0x0f },
    { 0x3c,0x7e,0xe7,0xc3,0xc3,0xc3,0xc3,0xc3,0xe7,0xfe,0xfc,0x00,0x00,0x30,0x30,0x30,0x38,0x1c,0x0e,0x07,0x03,0x00 },
    { 0x00,0x00,0x00,0x70,0x70,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0x1c,0x1c,0x00,0x00,0x00,0x00,0x00 },
    { 0x00,0x00,0x00,0x70,0x70,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9c,0xfc,0x7c,0x00,0x00,0x00,0x00,0x00 },
    { 0x00,0xc0,0xe0,0xf0,0x38,0x1c,0x0e,0x07,0x03,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x0e,0x1c,0x38,0x30,0x00,0x00 },
    { 0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x00 },
    { 0x00,0x03,0x07,0x0e,0x1c,0x38,0xf0,0xe0,0xc0,0x00,0x00,0x00,0x30,0x38,0x1c,0x0e,0x07,0x03,0x01,0x00,0x00,0x00 },
    { 0x1c,0x1e,0x07,0x03,0x83,0xc3,0xe3,0x77,0x3e,0x1c,0x00,0x00,0x00,0x00,0x00,0x37,0x37,0x00,0x00,0x00,0x00,0x00 },
    { 0xf8,0xfe,0x07,0xf3,0xfb,0x1b,0xfb,0xfb,0x07,0xfe,0xf8,0x0f,0x1f,0x18,0x33,0x37,0x36,0x37,0x37,0x36,0x03,0x01 },
    { 0x00,0x00,0xe0,0xfc,0x1f,0x1f,0xfc,0xe0,0x00,0x00,0x00,0x38,0x3f,0x07,0x06,0x06,0x06,0x06,0x07,0x3f,0x38,0x00 },
    { 0xff,0xff,0xc3,0xc3,0xc3,0xc3,0xe7,0xfe,0xbc,0x00,0x00,0x3f,0x3f,0x30,0x30,0x30,0x30,0x30,0x39,0x1f,0x0f,0x00 },
    { 0xf0,0xfc,0x0e,0x07,0x03,0x03,0x03,0x07,0x0e,0x0c,0x00,0x03,0x0f,0x1c,0x38,0x30,0x30,0x30,0x38,0x1c,0x0c,0x00 },
    { 0xff,0xff,0x03,0x03,0x03,0x03,0x07,0x0e,0xfc,0xf0,0x00,0x3f,0x3f,0x30,0x30,0x30,0x30,0x38,0x1c,0x0f,0x03,0x00 },
    { 0xff,0xff,0xc3,0xc3,0xc3,0xc3,0xc3,0xc3,0x03,0x03,0x00,0x3f,0x3f,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x00 },
    { 0xff,0xff,0xc3,0xc3,0xc3,0xc3,0xc3,0xc3,0x03,0x03,0x00,0x3f,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
    { 0xf0,0xfc,0x0e,0x07,0x03,0xc3,0xc3,0xc3,0xc7,0xc6,0x00,0x03,0x0f,0x1c,0x38,0x30,0x30,0x30,0x30,0x3f,0x3f,0x00 },
    { 0xff,0xff,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xff,0xff,0x00,0x3f,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x00 },
    { 0x00,0x00,0x03,0x03,0xff,0xff,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x3f,0x3f,0x30,0x30,0x00,0x00,0x00 },
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x0e,0x1e,0x38,0x30,0x30,0x30,0x30,0x38,0x1f,0x07,0x00 },
    { 0xff,0xff,0xc0,0xe0,0xf0,0x38,0x1c,0x0e,0x07,0x03,0x00,0x3f,0x3f,0x00,0x01,0x03,0x07,0x0e,0x1c,0x38,0x30,0x00 },
    { 0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x00 },
    { 0xff,0xff,0x1e,0x78,0xe0,0xe0,0x78,0x1e,0xff,0xff,0x00,0x3f,0x3f,0x00,0x00,0x01,0x01,0x00,0x00,0x3f,0x3f,0x00 },
    { 0xff,0xff,0x0e,0x38,0xf0,0xc0,0x00,0x00,0xff,0xff,0x00,0x3f,0x3f,0x00,0x00,0x00,0x03,0x07,0x1c,0x3f,0x3f,0x00 },
    { 0xf0,0xfc,0x0e,0x07,0x03,0x03,0x07,0x0e,0xfc,0xf0,0x00,0x03,0x0f,0x1c,0x38,0x30,0x30,0x38,0x1c,0x0f,0x03,0x00 },
    { 0xff,0xff,0x83,0x83,0x83,0x83,0x83,0xc7,0xfe,0x7c,0x00,0x3f,0x3f,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00 },
    { 0xf0,0xfc,0x0e,0x07,0x03,0x03,0x07,0x0e,0xfc,0xf0,0x00,0x03,0x0f,0x1c,0x38,0x30,0x36,0x3e,0x1c,0x3f,0x33,0x00 },
    { 0xff,0xff,0x83,0x83,0x83,0x83,0x83,0xc7,0xfe,0x7c,0x00,0x3f,0x3f,0x01,0x01,0x03,0x07,0x0f,0x1d,0x38,0x30,0x00 },
    { 0x3c,0x7e,0xe7,0xc3,0xc3,0xc3,0xc3,0xc7,0x8e,0x0c,0x00,0x0c,0x1c,0x38,0x30,0x30,0x30,0x30,0x39,0x1f,0x0f,0x00 },
    { 0x00,0x03,0x03,0x03,0xff,0xff,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x00,0x00,0x00,0x00,0x00 },
    { 0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x07,0x1f,0x38,0x30,0x30,0x30,0x30,0x38,0x1f,0x07,0x00 },
    { 0x07,0x3f,0xf8,0xc0,0x00,0x00,0xc0,0xf8,0x3f,0x07,0x00,0x00,0x00,0x01,0x0f,0x3e,0x3e,0x0f,0x01,0x00,0x00,0x00 },
    { 0xff,0xff,0x00,0x00,0x80,0x80,0x00,0x00,0xff,0xff,0x00,0x3f,0x3f,0x1c,0x06,0x03,0x03,0x06,0x1c,0x3f,0x3f,0x00 },
    { 0x03,0x0f,0x1c,0x30,0xe0,0xe0,0x30,0x1c,0x0f,0x03,0x00,0x30,0x3c,0x0e,0x03,0x01,0x01,0x03,0x0e,0x3c,0x30,0x00 },
    { 0x03,0x0f,0x3c,0xf0,0xc0,0xc0,0xf0,0x3c,0x0f,0x03,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x00,0x00,0x00,0x00,0x00 },
    { 0x03,0x03,0x03,0x03,0xc3,0xe3,0x33,0x1f,0x0f,0x03,0x00,0x30,0x3c,0x3e,0x33,0x31,0x30,0x30,0x30,0x30,0x30,0x00 },
    { 0x00,0x00,0xff,0xff,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x30,0x30,0x30,0x30,0x00,0x00,0x00 },
    { 0x0e,0x1c,0x38,0x70,0xe0,0xc0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x0e,0x1c,0x18 },
    { 0x00,0x00,0x03,0x03,0x03,0x03,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x30,0x30,0x3f,0x3f,0x00,0x00,0x00 },
    { 0x60,0x70,0x38,0x1c,0x0e,0x07,0x0e,0x1c,0x38,0x70,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0 },
    { 0x00,0x00,0x00,0x00,0x3e,0x7e,0x4e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
    { 0x00,0x40,0x60,0x60,0x60,0x60,0x60,0x60,0xe0,0xc0,0x00,0x1c,0x3e,0x33,0x33,0x33,0x33,0x33,0x33,0x3f,0x3f,0x00 },
    { 0xff,0xff,0xc0,0x60,0x60,0x60,0x60,0xe0,0xc0,0x80,0x00,0x3f,0x3f,0x30,0x30,0x30,0x30,0x30,0x38,0x1f,0x0f,0x00 },
    { 0x80,0xc0,0xe0,0x60,0x60,0x60,0x60,0x60,0xc0,0x80,0x00,0x0f,0x1f,0x38,0x30,0x30,0x30,0x30,0x30,0x18,0x08,0x00 },
    { 0x80,0xc0,0xe0,0x60,0x60,0x60,0xe0,0xc0,0xff,0xff,0x00,0x0f,0x1f,0x38,0x30,0x30,0x30,0x30,0x30,0x3f,0x3f,0x00 },
    { 0x80,0xc0,0xe0,0x60,0x60,0x60,0x60,0x60,0xc0,0x80,0x00,0x0f,0x1f,0x3b,0x33,0x33,0x33,0x33,0x33,0x13,0x01,0x00 },
    { 0xc0,0xc0,0xfc,0xfe,0xc7,0xc3,0xc3,0x03,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
    { 0x80,0xc0,0xe0,0x60,0x60,0x60,0x60,0x60,0xe0,0xe0,0x00,0x03,0xc7,0xce,0xcc,0xcc,0xcc,0xcc,0xe6,0x7f,0x3f,0x00 },
    { 0xff,0xff,0xc0,0x60,0x60,0x60,0xe0,0xc0,0x80,0x00,0x00,0x3f,0x3f,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x00,0x00 },
    { 0x00,0x00,0x00,0x60,0xec,0xec,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x3f,0x3f,0x30,0x30,0x00,0x00,0x00 },
    { 0x00,0x00,0x00,0x00,0x00,0x60,0xec,0xec,0x00,0x00,0x00,0x00,0x00,0x60,0xe0,0xc0,0xc0,0xff,0x7f,0x00,0x00,0x00 },
    { 0x00,0xff,0xff,0x00,0x80,0xc0,0xe0,0x60,0x00,0x00,0x00,0x00,0x3f,0x3f,0x03,0x07,0x0f,0x1c,0x38,0x30,0x00,0x00 },
    { 0x00,0x00,0x00,0x03,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x3f,0x3f,0x30,0x30,0x00,0x00,0x00 },
    { 0xe0,0xc0,0xe0,0xe0,0xc0,0xc0,0xe0,0xe0,0xc0,0x80,0x00,0x3f,0x3f,0x00,0x00,0x3f,0x3f,0x00,0x00,0x3f,0x3f,0x00 },
    { 0x00,0xe0,0xe0,0x60,0x60,0x60,0x60,0xe0,0xc0,0x80,0x00,0x00,0x3f,0x3f,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x00 },
    { 0x80,0xc0,0xe0,0x60,0x60,0x60,0x60,0xe0,0xc0,0x80,0x00,0x0f,0x1f,0x38,0x30,0x30,0x30,0x30,0x38,0x1f,0x0f,0x00 },
    { 0xe0,0xe0,0x60,0x60,0x60,0x60,0x60,0xe0,0xc0,0x80,0x00,0xff,0xff,0x0c,0x18,0x18,0x18,0x18,0x1c,0x0f,0x07,0x00 },
    { 0x80,0xc0,0xe0,0x60,0x60,0x60,0x60,0x60,0xe0,0xe0,0x00,0x07,0x0f,0x1c,0x18,0x18,0x18,0x18,0x0c,0xff,0xff,0x00 },
    { 0x00,0xe0,0xe0,0xc0,0x60,0x60,0x60,0x60,0xe0,0xc0,0x00,0x00,0x3f,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
    { 0xc0,0xe0,0x60,0x60,0x60,0x60,0x60,0x40,0x00,0x00,0x00,0x11,0x33,0x33,0x33,0x33,0x33,0x3f,0x1e,0x00,0x00,0x00 },
    { 0x60,0x60,0xfe,0xfe,0x60,0x60,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x3f,0x30,0x30,0x30,0x30,0x00,0x00,0x00 },
    { 0xe0,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0xe0,0x00,0x0f,0x1f,0x38,0x30,0x30,0x30,0x30,0x18,0x3f,0x3f,0x00 },
    { 0x60,0xe0,0x80,0x00,0x00,0x00,0x00,0x80,0xe0,0x60,0x00,0x00,0x01,0x07,0x1e,0x38,0x38,0x1e,0x07,0x01,0x00,0x00 },
    { 0xe0,0xe0,0x00,0x00,0xe0,0xe0,0x00,0x00,0xe0,0xe0,0x00,0x07,0x1f,0x38,0x1c,0x0f,0x0f,0x1c,0x38,0x1f,0x07,0x00 },
    { 0x60,0xe0,0xc0,0x80,0x00,0x80,0xc0,0xe0,0x60,0x00,0x00,0x30,0x38,0x1d,0x0f,0x07,0x0f,0x1d,0x38,0x30,0x00,0x00 },
    { 0x00,0x60,0xe0,0x80,0x00,0x00,0x80,0xe0,0x60,0x00,0x00,0x00,0x00,0x81,0xe7,0x7e,0x1e,0x07,0x01,0x00,0x00,0x00 },
    { 0x60,0x60,0x60,0x60,0x60,0xe0,0xe0,0x60,0x20,0x00,0x00,0x30,0x38,0x3c,0x36,0x33,0x31,0x30,0x30,0x30,0x00,0x00 },
    { 0x00,0x80,0xc0,0xfc,0x7e,0x07,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x01,0x1f,0x3f,0x70,0x60,0x60,0x60,0x00,0x00 },
    { 0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x00,0x00,0x00,0x00,0x00 },
    { 0x00,0x03,0x03,0x03,0x07,0x7e,0xfc,0xc0,0x80,0x00,0x00,0x00,0x60,0x60,0x60,0x70,0x3f,0x1f,0x01,0x00,0x00,0x00 },
    { 0x10,0x18,0x0c,0x04,0x0c,0x18,0x10,0x18,0x0c,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
    { 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x00 }
};

#define XDIFF -4
#define YDIFF -6

enum {
  NUM_XSANKE,
  NUM_YSANKE,
  NUM_XADD,
  NUM_YADD,
  NUM_XDIST,
  NUM_YDIST,

  NUM_LAST
};

struct counter {
  char *what;
  int num;
};

struct counter values[]={
  {"xsanke", 1},
  {"ysanke", 1},
  {"xadd", 1},
  {"yadd", 2},
  {"xdist", -4},
  {"ydist", -6},
};

#ifdef CONFIG_RTC
static unsigned char yminute[]={
53,53,52,52,51,50,49,47,46,44,42,40,38,36,34,32,29,27,25,23,21,19,17,16,14,13,12,11,11,10,10,10,11,11,12,13,14,16,17,19,21,23,25,27,29,31,34,36,38,40,42,44,46,47,49,50,51,52,52,53,
};
static unsigned char yhour[]={
42,42,42,42,41,41,40,39,39,38,37,36,35,34,33,32,30,29,28,27,26,25,24,24,23,22,22,21,21,21,21,21,21,21,22,22,23,24,24,25,26,27,28,29,30,31,33,34,35,36,37,38,39,39,40,41,41,42,42,42,
};

static unsigned char xminute[]={
56,59,63,67,71,74,77,80,83,86,88,90,91,92,93,93,93,92,91,90,88,86,83,80,77,74,71,67,63,59,56,52,48,44,40,37,34,31,28,25,23,21,20,19,18,18,18,19,20,21,23,25,28,31,34,37,40,44,48,52,
};
static unsigned char xhour[]={
56,57,59,61,63,65,66,68,69,71,72,73,73,74,74,74,74,74,73,73,72,71,69,68,66,65,63,61,59,57,56,54,52,50,48,46,45,43,42,40,39,38,38,37,37,37,37,37,38,38,39,40,42,43,45,46,48,50,52,54,
};

static void addclock(void)
{
    int i;
    int hour;
    int minute;
    int pos;

    struct tm* current_time = rb->get_time();
    hour = current_time->tm_hour;
    minute = current_time->tm_min;

    pos = 90-minute;
    if(pos >= 60)
        pos -= 60;

    rb->lcd_drawline(LCD_WIDTH/2, LCD_HEIGHT/2, xminute[pos], yminute[pos]);

    hour = hour*5 + minute/12;
    pos = 90-hour;
    if(pos >= 60)
        pos -= 60;

    rb->lcd_drawline(LCD_WIDTH/2, LCD_HEIGHT/2, xhour[pos], yhour[pos]);

    /* draw a circle */
    for(i=0; i < 60; i+=3) {
        rb->lcd_drawline( xminute[i],
                          yminute[i],
                          xminute[(i+1)%60],
                          yminute[(i+1)%60]);
    }
}
#endif /* CONFIG_RTC */

#define DRAW_WIDTH (LCD_WIDTH + LETTER_WIDTH*2)

#if LCD_DEPTH > 1
static const unsigned face_colors[] =
{
#ifdef HAVE_LCD_COLOR
    LCD_BLACK, LCD_RGBPACK(0, 0, 255), LCD_RGBPACK(255, 0, 0)
#else
    LCD_BLACK, LCD_LIGHTGRAY, LCD_DARKGRAY
#endif
};
#endif

static int scrollit(void)
{
    int b;
    unsigned int y=100;
    int x=LCD_WIDTH;
    unsigned int yy,xx;
    unsigned int i;
    int textpos=0;

    char* rock="Rockbox! Pure pleasure. Pure fun. Oooh. What fun! ;-) ";
    unsigned int rocklen = rb->strlen(rock);
    int letter;

    rb->lcd_clear_display();
    while(1)
    {
        b = rb->button_get_w_tmo(HZ/10);
        switch(b)
        {
#ifdef BOUNCE_RC_QUIT
            case BOUNCE_RC_QUIT :
#endif
            case BOUNCE_QUIT :
                return 0;
            case BOUNCE_MODE : 
                return 1;
            default:
                if ( rb->default_event_handler(b) == SYS_USB_CONNECTED )
                    return -1;
        }
        rb->lcd_clear_display();

        for(i=0, yy=y, xx=x; i< LETTERS_ON_SCREEN; i++) {
            letter = rock[(i+textpos) % rocklen ];
#if LCD_DEPTH > 1
            rb->lcd_set_foreground(face_colors[ letter % 3] );
#endif
            rb->lcd_mono_bitmap(char_gen_12x16[letter-0x20],
                                xx, table[yy&(TABLE_SIZE-1)], 11, 16);
            yy += YADD;
            xx+= DRAW_WIDTH/LETTERS_ON_SCREEN;
        }
#ifdef CONFIG_RTC
        addclock();
#endif
        rb->lcd_update();

        x-= XSPEED;
        
        if(x < -LETTER_WIDTH) {
            x += DRAW_WIDTH/LETTERS_ON_SCREEN;
            y += YADD;
            textpos++;
        }

        y+=YSPEED;

    }
}

static int loopit(void)
{
    int b;
    unsigned int y=100;
    unsigned int x=100;
    unsigned int yy,xx;
    unsigned int i;
    unsigned int ysanke=0;
    unsigned int xsanke=0;

    char* rock="ROCKbox";
    unsigned int rocklen = rb->strlen(rock);

    int show=0;
    int timeout=0;
    char buffer[30];

    rb->lcd_clear_display();
    while(1)
    {
        b = rb->button_get_w_tmo(HZ/10);
        if ( b == BOUNCE_QUIT )
            return 0;

        if ( b == BOUNCE_MODE )
            return 1;        

        if ( rb->default_event_handler(b) == SYS_USB_CONNECTED )
            return -1;

        if ( b != BUTTON_NONE )
            timeout=20;

        y+= speed[ysanke&15] + values[NUM_YADD].num;
        x+= speed[xsanke&15] + values[NUM_XADD].num;

        rb->lcd_clear_display();
#ifdef CONFIG_RTC
        addclock();
#endif
        if(timeout) {
            switch(b) {
                case BUTTON_LEFT:
                  values[show].num--;
                  break;
                case BUTTON_RIGHT:
                  values[show].num++;
                  break;
                case BOUNCE_UP:
                  if(++show == NUM_LAST)
                      show=0;
                  break;
                case BOUNCE_DOWN:
                  if(--show < 0)
                      show=NUM_LAST-1;
                  break;
            }
            rb->snprintf(buffer, 30, "%s: %d",
                         values[show].what, values[show].num);
            rb->lcd_putsxy(0, LCD_HEIGHT -  8, (unsigned char *)buffer);
            timeout--;
        }
        for(i=0, yy=y, xx=x;
            i<rocklen;
            i++, yy+=values[NUM_YDIST].num, xx+=values[NUM_XDIST].num)
            rb->lcd_mono_bitmap(char_gen_12x16[rock[i]-0x20],
                                xtable[xx&(TABLE_SIZE-1)],
                                table[yy&(TABLE_SIZE-1)], 11, 16);
        rb->lcd_update();

        ysanke+= values[NUM_YSANKE].num;
        xsanke+= values[NUM_XSANKE].num;
    }
}


enum plugin_status plugin_start(struct plugin_api* api, void* parameter)
{
    int w, h;
    char *off = "[Off] to stop";
    int len;

    (void)(parameter);
    rb = api;

    len = rb->strlen(SS_TITLE);
#if LCD_DEPTH > 1
    rb->lcd_set_backdrop(NULL);
#endif
    rb->lcd_setfont(FONT_SYSFIXED);
    rb->lcd_getstringsize((unsigned char *)SS_TITLE, &w, &h);

    /* Get horizontel centering for text */
    len *= w;
    if (len%2 != 0)
        len = ((len+1)/2)+(w/2);
    else
        len /= 2;
    
    if (h%2 != 0)
        h = (h/2)+1;
    else
        h /= 2;
    
    rb->lcd_clear_display();
    rb->lcd_putsxy(LCD_WIDTH/2-len, (LCD_HEIGHT/2)-h, (unsigned char *)SS_TITLE);
    
    len = 1;
    rb->lcd_getstringsize((unsigned char *)off, &w, &h);

    /* Get horizontel centering for text */
    len *= w;
    if (len%2 != 0)
        len = ((len+1)/2)+(w/2);
    else
        len /= 2;
    
    if (h%2 != 0)
        h = (h/2)+1;
    else
        h /= 2;
    
    rb->lcd_putsxy(LCD_WIDTH/2-len, LCD_HEIGHT-(2*h), (unsigned char *)off);
    rb->lcd_update();
    rb->sleep(HZ);
    rb->lcd_set_drawmode(DRMODE_FG);

    do {
        h = loopit();
        if (h > 0)
            h = scrollit();
#if LCD_DEPTH > 1
        rb->lcd_set_foreground(LCD_BLACK);
#endif
    } while(h > 0);
    
    rb->lcd_set_drawmode(DRMODE_SOLID);
    rb->lcd_setfont(FONT_UI);
    
    return (h == 0) ? PLUGIN_OK : PLUGIN_USB_CONNECTED;
}

#endif
