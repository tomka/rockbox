/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2002 Itai Shaked
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

/*
Snake!

by Itai Shaked

ok, a little explanation - 
board holds the snake and apple position - 1+ - snake body (the number
represents the age [1 is the snake's head]).
-1 is an apple, and 0 is a clear spot.
dir is the current direction of the snake - 0=up, 1=right, 2=down, 3=left;

*/

#include "plugin.h"
#ifdef HAVE_LCD_BITMAP

static int board[28][16],snakelength;
static unsigned int score,hiscore=0;
static short dir,frames,apple,level=1,dead=0;
static struct plugin_api* rb;

void die (void)
{
    char pscore[5],hscore[17];
    rb->lcd_clear_display();
    rb->snprintf(pscore,sizeof(pscore),"%d",score);
    rb->lcd_putsxy(3,12,"oops...");
    rb->lcd_putsxy(3,22,"Your score :");
    rb->lcd_putsxy(3,32, pscore);
    if (score>hiscore) {
        hiscore=score;
        rb->lcd_putsxy(3,42,"New High Score!");
    }
    else {
        rb->snprintf(hscore,sizeof(hscore),"High Score: %d",hiscore);
        rb->lcd_putsxy(3,42,hscore);
    }
    rb->lcd_update();
    rb->sleep(3*HZ);
    dead=1;
}

void colission (short x, short y)
{
    switch (board[x][y]) {
        case 0:
            break; 
        case -1:
            snakelength+=2;
            score+=level;
            apple=0;
            break;
        default:
            die();
            break;
    }
    if (x==28 || x<0 || y==16 || y<0) 
        die();
}

void move_head (short x, short y)
{
    switch (dir) {
        case 0:
            y-=1;
            break;
        case 1:
            x+=1; 
            break;
        case 2:
            y+=1;
            break;
        case 3:
            x-=1;
            break;
    }
    colission (x,y);
    if (dead)
        return;
    board[x][y]=1;
    rb->lcd_fillrect(x*4,y*4,4,4);
}

void frame (void)
{
    short x,y,head=0;
    for (x=0; x<28; x++) {
        for (y=0; y<16; y++) {
            switch (board[x][y]) {
                case 1:
                    if (!head) {
                        move_head(x,y);
                        if (dead)
                            return;
                        board[x][y]++;
                        head=1;
                    }
                    break;
                case 0:
                    break;
                case -1:
                    break;
                default:
                    if (board[x][y]==snakelength) {
                        board[x][y]=0;
                        rb->lcd_clearrect(x*4,y*4,4,4);
                    }
                    else 
                        board[x][y]++;
                    break;
            }
        }
    }
    rb->lcd_update();
}

void redraw (void)
{
    short x,y;
    rb->lcd_clear_display();
    for (x=0; x<28; x++) {
        for (y=0; y<16; y++) {
            switch (board[x][y]) {
                case -1:
                    rb->lcd_fillrect((x*4)+1,y*4,2,4);
                    rb->lcd_fillrect(x*4,(y*4)+1,4,2);
                    break;
                case 0:
                    break;
                default:
                    rb->lcd_fillrect(x*4,y*4,4,4);
                    break;
            }
        }
    }
    rb->lcd_update();
}

void game_pause (void) {
    rb->lcd_clear_display();
    rb->lcd_putsxy(3,12,"Game Paused");
    rb->lcd_putsxy(3,22,"[Play] to resume");
    rb->lcd_putsxy(3,32,"[Off] to quit");
    rb->lcd_update();
    while (1) {
        switch (rb->button_get(true)) {
            case BUTTON_OFF:
                dead=1;
                return;
            case BUTTON_PLAY:
                redraw();
                rb->sleep(HZ/2);
                return;
        }
    }
}


void game (void) {
    short x,y;
    while (1) {
        frame();
        if (dead)
            return;
	frames++;
        if (frames==10) {
            frames=0;
            if (!apple) {
                do {
                    x=rb->rand() % 28;
                    y=rb->rand() % 16;
                } while (board[x][y]);
                apple=1;
                board[x][y]=-1;
                rb->lcd_fillrect((x*4)+1,y*4,2,4);
                rb->lcd_fillrect(x*4,(y*4)+1,4,2);
            }
        }

        rb->sleep(HZ/level);

        switch (rb->button_get(false)) {
             case BUTTON_UP:
                 if (dir!=2) dir=0;
                 break;
             case BUTTON_RIGHT:
                 if (dir!=3) dir=1;
                 break;
             case BUTTON_DOWN:
                 if (dir!=0) dir=2;
                 break;
             case BUTTON_LEFT:
                 if (dir!=1) dir=3;
                 break;
             case BUTTON_OFF:
                 dead=1;
                 return;
             case BUTTON_PLAY:
                 game_pause();
                 break;
        }      
    }
}

void game_init(void) {
    short x,y;
    char plevel[10],phscore[20];

    for (x=0; x<28; x++) {
        for (y=0; y<16; y++) {
            board[x][y]=0;
        }
    }
    dead=0;
    apple=0;
    snakelength=4;
    score=0;
    board[11][7]=1;   


    rb->lcd_clear_display();
    rb->lcd_setfont(FONT_SYSFIXED);
    rb->snprintf(plevel,sizeof(plevel),"Level - %d",level);
    rb->snprintf(phscore,sizeof(phscore),"High Score: %d",hiscore);
    rb->lcd_puts(0,0, plevel);
    rb->lcd_puts(0,1, "(1-slow, 9-fast)");
    rb->lcd_puts(0,2, "OFF - quit");
    rb->lcd_puts(0,3, "PLAY - start/pause");
    rb->lcd_puts(0,4, phscore);
    rb->lcd_update();

    while (1) {    
        switch (rb->button_get(true)) {
            case BUTTON_RIGHT:
            case BUTTON_UP:
                if (level<9) 
                    level++;
                break;
            case BUTTON_LEFT:
            case BUTTON_DOWN:
                if (level>1)
                    level--;
                break;
            case BUTTON_OFF:
                dead=1;
                return;
                break;
            case BUTTON_PLAY:
                return;
                break;
        }
        rb->snprintf(plevel,sizeof(plevel),"Level - %d",level);
        rb->lcd_puts(0,0, plevel);
        rb->lcd_update();
    }
     
}

enum plugin_status plugin_start(struct plugin_api* api, void* parameter)
{
    TEST_PLUGIN_API(api);
    (void)(parameter);
    rb = api;

    game_init(); 
    rb->lcd_clear_display();
    game();    
    return false;
}

#endif
