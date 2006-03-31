/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2006 by Mat Holton
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#include "plugin.h"
#include "math.h"
#include "stdio.h"
PLUGIN_HEADER

/******************************* Globals ***********************************/
static struct plugin_api* rb; /* global api struct pointer */
/* variable button definitions */
#if CONFIG_KEYPAD == RECORDER_PAD
#define AST_PAUSE BUTTON_ON
#define AST_QUIT BUTTON_OFF
#define AST_THRUST_REP BUTTON_UP | BUTTON_REPEAT
#define AST_THRUST BUTTON_UP
#define AST_HYPERSPACE BUTTON_DOWN
#define AST_LEFT BUTTON_LEFT 
#define AST_LEFT_REP BUTTON_LEFT | BUTTON_REPEAT
#define AST_RIGHT BUTTON_RIGHT
#define AST_RIGHT_REP (BUTTON_RIGHT | BUTTON_REPEAT)
#define AST_FIRE BUTTON_PLAY
#define AST_FIRE_REP BUTTON_PLAY | BUTTON_REPEAT

#elif CONFIG_KEYPAD == ONDIO_PAD
#define AST_PAUSE BUTTON_ON
#define AST_QUIT BUTTON_OFF
#define AST_THRUST BUTTON_UP
#define AST_HYPERSPACE BUTTON_DOWN
#define AST_LEFT BUTTON_LEFT
#define AST_RIGHT BUTTON_RIGHT
#define AST_FIRE BUTTON_MENU

#elif (CONFIG_KEYPAD == IRIVER_H100_PAD) || \
      (CONFIG_KEYPAD == IRIVER_H300_PAD)
#define AST_PAUSE BUTTON_REC
#define AST_QUIT BUTTON_OFF
#define AST_THRUST_REP BUTTON_UP | BUTTON_REPEAT
#define AST_THRUST BUTTON_UP
#define AST_HYPERSPACE BUTTON_DOWN
#define AST_LEFT BUTTON_LEFT 
#define AST_LEFT_REP BUTTON_LEFT | BUTTON_REPEAT
#define AST_RIGHT BUTTON_RIGHT
#define AST_RIGHT_REP (BUTTON_RIGHT | BUTTON_REPEAT)
#define AST_FIRE BUTTON_SELECT
#define AST_FIRE_REP BUTTON_SELECT | BUTTON_REPEAT

#elif (CONFIG_KEYPAD == IAUDIO_X5_PAD)
#define AST_PAUSE BUTTON_PLAY
#define AST_QUIT BUTTON_POWER
#define AST_THRUST_REP BUTTON_UP | BUTTON_REPEAT
#define AST_THRUST BUTTON_UP
#define AST_HYPERSPACE BUTTON_DOWN
#define AST_LEFT BUTTON_LEFT 
#define AST_LEFT_REP BUTTON_LEFT | BUTTON_REPEAT
#define AST_RIGHT BUTTON_RIGHT
#define AST_RIGHT_REP (BUTTON_RIGHT | BUTTON_REPEAT)
#define AST_FIRE BUTTON_SELECT
#define AST_FIRE_REP BUTTON_SELECT | BUTTON_REPEAT

#elif (CONFIG_KEYPAD == IPOD_4G_PAD)
#define AST_PAUSE (BUTTON_SELECT | BUTTON_PLAY)
#define AST_QUIT (BUTTON_SELECT | BUTTON_MENU)
#define AST_THRUST BUTTON_MENU
#define AST_THRUST_REP (BUTTON_MENU | BUTTON_REPEAT)
#define AST_HYPERSPACE BUTTON_PLAY
#define AST_LEFT BUTTON_SCROLL_BACK
#define AST_LEFT_REP (BUTTON_SCROLL_BACK | BUTTON_REPEAT)
#define AST_RIGHT BUTTON_SCROLL_FWD
#define AST_RIGHT_REP (BUTTON_SCROLL_FWD | BUTTON_REPEAT)
#define AST_FIRE BUTTON_SELECT
#define AST_FIRE_REP (BUTTON_SELECT | BUTTON_REPEAT)
#endif

#define SHOW_COL 0
#define HISCORE_FILE PLUGIN_DIR "/astrorocks.hs"
#define POINT_SIZE 2
#define MAX_NUM_ASTEROIDS 25
#define MAX_NUM_MISSILES 6
#define ABS(x) ((x)>0?(x):-(x))

#define SCALE 5000
#define MISSILE_SCALE 5000
#define WRAP_GAP                12
#define EXPLOSION_LENGTH        35

#if CONFIG_KEYPAD == RECORDER_PAD || CONFIG_KEYPAD == ONDIO_PAD
  #define ENEMY_MISSILE_SURVIVAL_LENGTH 65
  #define MISSILE_SURVIVAL_LENGTH 40
#elif (CONFIG_KEYPAD == IRIVER_H100_PAD) || \
      (CONFIG_KEYPAD == IRIVER_H300_PAD) || \
      (CONFIG_KEYPAD == IAUDIO_X5_PAD) || \
      (CONFIG_KEYPAD == IPOD_4G_PAD)
  #define ENEMY_MISSILE_SURVIVAL_LENGTH 120
  #define MISSILE_SURVIVAL_LENGTH 80
#endif
      
#define SHOW_GAME_OVER_TIME     100
#define SHOW_LEVEL_TIME         50
#define START_LIVES             3
#define START_LEVEL             1
#define NUM_ASTEROID_VERTICES   10
#define NUM_SHIP_VERTICES       4
#define NUM_ENEMY_VERTICES      6
#define MAX_LEVEL               MAX_NUM_ASTEROIDS
#define ENEMY_SPEED             4
#define ENEMY_START_X           0
#define ENEMY_START_Y           0
#define SIZE_ENEMY_COLLISION    5*SCALE
#define ATTRACT_FLIP_TIME       100
#define NUM_STARS               50
#define NUM_ROTATIONS           16

#define SIN_COS_SCALE           10000
#define SHIP_ROT_CW_SIN         3827
#define SHIP_ROT_CW_COS         9239
#define SHIP_ROT_ACW_SIN        -3827 
#define SHIP_ROT_ACW_COS        9239 

#define FAST_ROT_CW_SIN         873
#define FAST_ROT_CW_COS         9963
#define FAST_ROT_ACW_SIN        -873
#define FAST_ROT_ACW_COS        9963

#define MEDIUM_ROT_CW_SIN       350
#define MEDIUM_ROT_CW_COS       9994
#define MEDIUM_ROT_ACW_SIN      -350
#define MEDIUM_ROT_ACW_COS      9994

#define SLOW_ROT_CW_SIN         350
#define SLOW_ROT_CW_COS         9994
#define SLOW_ROT_ACW_SIN -      350
#define SLOW_ROT_ACW_COS        9994

#define SCALED_WIDTH (LCD_WIDTH*SCALE)
#define SCALED_HEIGHT (LCD_HEIGHT*SCALE)
#define CENTER_LCD_X (LCD_WIDTH/2)
#define CENTER_LCD_Y (LCD_HEIGHT/2)

enum asteroid_type
{
    SMALL =  1,
    MEDIUM = 2,
    LARGE =  3,
};

enum game_state
{
    GAME_OVER,
    ATTRACT_MODE,
    SHOW_LEVEL,
    PLAY_MODE, 
    PAUSE_MODE
};

struct Point
{
    int x;
    int y;
    int dx;
    int dy;
};

/* Asteroid structure, contains an array of points */
struct Asteroid
{
    enum asteroid_type type; 
    bool exists; 
    struct Point  position; 
    struct Point  vertices[NUM_ASTEROID_VERTICES];
    int radius;
    long speed_cos;
    long speed_sin;
    int explode_countdown;
};

struct Ship
{
    struct Point vertices[NUM_SHIP_VERTICES];
    struct Point position;
    bool waiting_for_space;
    int explode_countdown;
};

struct Enemy
{
    struct Point vertices[NUM_ENEMY_VERTICES];
    struct Point position;
    int explode_countdown;
};

struct Missile
{
    struct Point position;
    struct Point oldpoint;
    int survived;
};

static enum game_state game_state;
static int asteroid_count;
static int next_missile_count;
static int next_thrust_count;
static int num_lives;
static int show_level_timeout;
static int attract_flip_timeout;
static int show_game_over;
static int current_level;
static int current_score;
static int high_score;
static int space_check_size = 20*SCALE;

static bool enemy_on_screen;
static char phscore[30];
static struct Ship ship;
static struct Point stars[NUM_STARS];
static struct Asteroid asteroids_array[MAX_NUM_ASTEROIDS];
static struct Missile missiles_array[MAX_NUM_MISSILES];
static struct Missile enemy_missile;
static struct Enemy enemy;
static struct Point lives_points[NUM_SHIP_VERTICES];

void draw_and_move_asteroids(void);
void initialise_game(int nStartNum);

bool is_asteroid_near_ship(struct Asteroid* asteroid);
bool is_point_within_asteroid(struct Asteroid* asteroid, struct Point* point);

void initialise_asteroid(struct Asteroid* asteroid, enum asteroid_type eType);
void draw_polygon(struct Point* vertices, int px, int py, int num_vertices);
void rotate_asteroid(struct Asteroid* asteroid);
void create_asteroid(enum asteroid_type type, int x, int y);
void create_stars(void);

void initialise_ship(void);
void draw_and_move_ship(void);
void rotate_ship(int s, int c);
void thrust_ship(void);

void initialise_missile(struct Missile* missile);
void draw_and_move_missiles(void);
void fire_missile(void);

void animate_and_draw_explosion(struct Point* point, int num_points, int xoffset, int yoffset);
void initialise_explosion(struct Point* point, int num_points);

void move_point(struct Point* point);
void hyperspace(void);
void check_collisions(void);
void initialise_enemy(void);
void draw_and_move_enemy(void);
void draw_lives(void);
void drawstars(void);
bool is_ship_within_asteroid(struct Asteroid* asteroid);

/* The array of points that make up an asteroid */
static const short asteroid_one[NUM_ASTEROID_VERTICES*2] =
{ 
    -1, -6, 
    2, -4, 
    4, -7, 
    8, -4, 
    7, 0,   
    10,  1,  
    6,  7, 
    -2,  7, 
    -5,  3,  
    -5, -4    
};

/* The array of points that make up an asteroid */
static const short asteroid_two[NUM_ASTEROID_VERTICES*2] =
{ 
    -1, -6, 
    2, -8,
    3, -7,
    8, -4,
    7, 0,
    10,  1,
    6,  7,
    -2,  7,
    -5,  3,
    -5, -4
};

/* The array of points that make up an asteroid */
static const short asteroid_three[NUM_ASTEROID_VERTICES*2] =
{ 
    -1, -6, 
    2, -8,
    3, -7,
    1, -4,
    7, 0,
    10,  1,
    6,  7,
    -2,  7,
    -8,  3,
    -5, -4
};

/* The array od points the make up the ship */
static const short ship_vertices[NUM_SHIP_VERTICES*2] =
{ 
    0,-4, 
    3, 4,
    0, 1,
    -3, 4
};

/* The array of points the make up the bad spaceship */
static const short enemy_vertices[NUM_ENEMY_VERTICES*2] =
{ 
    -5,  0, 
    -2,  2,
    2,  2,
    5,  0,
    2, -2,
    -2, -2
};

/*Hi-Score reading and writing to file - this needs moving to the hi-score plugin lib as
a 3rd function */
void iohiscore(void)
{
    int fd;
    int compare;

    /* clear the buffer we're about to load the highscore data into */
    rb->memset(phscore, 0, sizeof(phscore));

    fd = rb->open(HISCORE_FILE,O_RDWR | O_CREAT);

    /* highscore used to %d, is now %d\n
       Deal with no file or bad file */
    rb->read(fd,phscore, sizeof(phscore));

    compare = rb->atoi(phscore);

    if(high_score > compare){
        rb->lseek(fd,0,SEEK_SET);
        rb->fdprintf(fd, "%d\n", high_score);
    }
    else
        high_score = compare;   
    
    rb->close(fd);
}

bool point_in_poly(struct Point* _point, int num_vertices, int x, int y)
{
    struct Point* pi;
    struct Point* pj;
    int n;
    bool c = false;
  
    pi = _point;
    pj = _point;
    pj += num_vertices-1;
    
    n = num_vertices;
    while(n--)
    {
        if((((pi->y <= y) && (y < pj->y)) || ((pj->y <= y) && (y < pi->y))) &&
           (x < (pj->x - pi->x) * (y - pi->y) / (pj->y - pi->y) + pi->x))
            c = !c;
        
        if(n == num_vertices - 1)
            pj = _point;
        else
            pj++;
        
        pi++;
    }
    
    return c;
}

void move_point(struct Point* point)
{
    point->x += point->dx;
    point->y += point->dy;
    
    /*check bounds on the x-axis:*/
    if(point->x >= SCALED_WIDTH)
        point->x = 0;
    else if(point->x <= 0) 
        point->x = SCALED_WIDTH;
    
    /*Check bounds on the y-axis:*/
    if(point->y >= SCALED_HEIGHT) 
        point->y = 0;
    else if(point->y <= 0) 
        point->y = SCALED_HEIGHT; 
}

/*Check if point is within a rectangle*/
bool is_point_within_rectangle(struct Point* rect, struct Point* p, int size)
{
#if SHOW_COL
    int aTLx = rect->x - size;
    int aTLy = rect->y - size;
    int aBRx = rect->x + size;
    int aBRy = rect->y + size;
    rb->lcd_drawline( aTLx/SCALE, aTLy/SCALE, aBRx/SCALE, aTLy/SCALE);
    rb->lcd_drawline( aTLx/SCALE, aTLy/SCALE, aTLx/SCALE, aBRy/SCALE);
    rb->lcd_drawline( aTLx/SCALE, aBRy/SCALE, aBRx/SCALE, aBRy/SCALE);
    rb->lcd_drawline( aBRx/SCALE, aBRy/SCALE, aBRx/SCALE, aTLy/SCALE);
    return (p->x > aTLx && p->x < aBRx && p->y > aTLy && p->y < aBRy);
#else
    return (p->x > rect->x - size && p->x < rect->x + size &&
            p->y > rect->y - size && p->y < rect->y + size);
#endif
}

/* Draw polygon */
void draw_polygon(struct Point* vertices, int px, int py, int num_vertices)
{
    int n, t1, t2, oldX, oldY;
    struct Point *p;
    bool bDrawAll = px < WRAP_GAP || LCD_WIDTH - px < WRAP_GAP ||
        py < WRAP_GAP || LCD_HEIGHT - py < WRAP_GAP;
    
    p = vertices;
    p += num_vertices-1;
    oldX = p->x/SCALE + px;
    oldY = p->y/SCALE + py;
    p = vertices;
    for(n = num_vertices+1; --n;)
    {
        t1 = p->x/SCALE + px;
        t2 = p->y/SCALE + py;
        
        rb->lcd_drawline(oldX, oldY, t1, t2);
        
        if(bDrawAll)
        {    
            rb->lcd_drawline(oldX - LCD_WIDTH, oldY, t1 - LCD_WIDTH, t2);
            rb->lcd_drawline(oldX + LCD_WIDTH, oldY, t1 + LCD_WIDTH, t2);
            rb->lcd_drawline(oldX - LCD_WIDTH, oldY + LCD_HEIGHT,
                             t1 - LCD_WIDTH, t2 + LCD_HEIGHT);
            rb->lcd_drawline(oldX + LCD_WIDTH, oldY + LCD_HEIGHT,
                             t1 + LCD_WIDTH, t2 + LCD_HEIGHT);
            
            rb->lcd_drawline(oldX, oldY - LCD_HEIGHT, t1, t2 - LCD_HEIGHT);
            rb->lcd_drawline(oldX, oldY + LCD_HEIGHT, t1, t2 + LCD_HEIGHT);
            rb->lcd_drawline(oldX - LCD_WIDTH, oldY - LCD_HEIGHT,
                             t1 - LCD_WIDTH, t2 - LCD_HEIGHT);
            rb->lcd_drawline(oldX + LCD_WIDTH, oldY - LCD_HEIGHT,
                             t1 + LCD_WIDTH, t2 - LCD_HEIGHT);
        }
        oldX = t1;
        oldY = t2;
        p++;
    }
}

void animate_and_draw_explosion(struct Point* point, int num_points,
                                int xoffset, int yoffset)
{
    int n;
    for(n = num_points; --n;)
    {
        if(game_state != PAUSE_MODE)
        {
            point->x += point->dx; 
            point->y += point->dy;
        }
        rb->lcd_fillrect( point->x/SCALE + xoffset, point->y/SCALE + yoffset,
                          POINT_SIZE, POINT_SIZE);
        point++;
    }    
}

/*stop movement of ship, 'cos that's what happens when you go into hyperspace.*/
void hyperspace(void)
{
    ship.position.dx = ship.position.dy = 0;
    ship.position.x = (rb->rand()%SCALED_WIDTH);
    ship.position.y = (rb->rand()%SCALED_HEIGHT);
}

void initialise_enemy(void)
{
    struct Point* point;
    int n;
    
    enemy_missile.survived = 0;
    enemy_on_screen = true;
    enemy.explode_countdown = 0;
    
    point = enemy.vertices;
    for(n = 0; n < NUM_ENEMY_VERTICES+NUM_ENEMY_VERTICES; n+=2)
    {
        point->x  = enemy_vertices[n];
        point->y  = enemy_vertices[n+1];
        point->x *= SCALE;
        point->y *= SCALE;
        point++;
    }
    
    if(ship.position.x >= SCALED_WIDTH/2)
    {
        enemy.position.dx  = ENEMY_SPEED;
        enemy.position.x   = 0;
    }
    else
    {
        enemy.position.dx  = -ENEMY_SPEED;
        enemy.position.x   = SCALED_WIDTH;
    }
    
    if(ship.position.y >= SCALED_HEIGHT/2)
    {
        enemy.position.dy  = ENEMY_SPEED;
        enemy.position.y   = 0;
    }
    else
    {
        enemy.position.dy  = -ENEMY_SPEED;
        enemy.position.y   = SCALED_HEIGHT;
    }
    
    enemy.position.dx *= SCALE/10;
    enemy.position.dy *= SCALE/10;
}

void draw_and_move_enemy(void)
{
    int enemy_x, enemy_y;
    struct Point *point;
    
    if(enemy_on_screen)
    {
        enemy_x = enemy.position.x/SCALE;
        enemy_y = enemy.position.y/SCALE;
        if(!enemy.explode_countdown)
        {
            point = enemy.vertices;
            draw_polygon(enemy.vertices, enemy_x, enemy_y, NUM_ENEMY_VERTICES);
            rb->lcd_drawline(enemy.vertices[0].x/SCALE + enemy_x,
                             enemy.vertices[0].y/SCALE + enemy_y, 
                             enemy.vertices[3].x/SCALE  + enemy_x,
                             enemy.vertices[3].y/SCALE + enemy_y);

            if(game_state != PAUSE_MODE)
            {
                enemy.position.x += enemy.position.dx;
                enemy.position.y += enemy.position.dy;
            }
            
            if(enemy.position.x > SCALED_WIDTH || enemy.position.x < 0)
                enemy_on_screen = false;
            
            if(enemy.position.y > SCALED_HEIGHT)
                enemy.position.y = 0; 
            else if(enemy.position.y < 0)
                enemy.position.y = SCALED_HEIGHT;
            
            if( (rb->rand()%1000) < 10)
                enemy.position.dy = -enemy.position.dy;
        }
        else
        {
            
            animate_and_draw_explosion(enemy.vertices, NUM_ENEMY_VERTICES,
                                       enemy_x, enemy.position.y/SCALE);
            if(game_state != PAUSE_MODE)
            {
                enemy.explode_countdown--;
                if(!enemy.explode_countdown)
                    enemy_on_screen = false;
            }
        }
    }
    else
    {
        if( (rb->rand()%1000) < 2 )
            initialise_enemy();
    }
    
    if(!enemy_missile.survived && game_state != GAME_OVER)
    {
        /*if no missile and the enemy is here and not exploding..then shoot baby!*/
        if( !enemy.explode_countdown && enemy_on_screen &&
            !ship.waiting_for_space && (rb->rand()%10) > 5 )
        {
            enemy_missile.position.x  = enemy.position.x;
            enemy_missile.position.y  = enemy.position.y;
            
            /*lame, needs to be sorted - it's trying to shoot at the ship*/
            if(ABS(enemy.position.y - ship.position.y) <= 5*SCALE)
            {
                enemy_missile.position.dy = 0;
            }
            else
            {  
                if( enemy.position.y < ship.position.y)
                    enemy_missile.position.dy = 1;
                else 
                    enemy_missile.position.dy = -1;  
            }
            
            if(ABS(enemy.position.x - ship.position.x) <= 5*SCALE)
                enemy_missile.position.dx = 0;
            else
            {   
                if( enemy.position.x < ship.position.x)
                    enemy_missile.position.dx = 1;
                else 
                    enemy_missile.position.dx = -1;	  
            }
            
            if(enemy_missile.position.dx == 0 &&
               enemy_missile.position.dy == 0)
                enemy_missile.position.dx = enemy_missile.position.dy = -1;
  
            enemy_missile.position.dx *= SCALE;
            enemy_missile.position.dy *= SCALE;
            enemy_missile.survived = ENEMY_MISSILE_SURVIVAL_LENGTH;
            
        }
    }
    else
    {
        rb->lcd_fillrect( enemy_missile.position.x/SCALE,
                          enemy_missile.position.y/SCALE,
                          POINT_SIZE, POINT_SIZE);
        if(game_state != PAUSE_MODE)
        {
            move_point(&enemy_missile.position);
            enemy_missile.survived--;
        }
    }
}

/******************
* Lame method of collision
* detection. It's checking for collision
* between point and a big rectangle around the asteroid...
*******************/
bool is_point_within_asteroid(struct Asteroid* asteroid, struct Point* point)
{
    if( !is_point_within_rectangle(&asteroid->position, point,
                                   asteroid->radius+4*SCALE) )
        return false;
  
    if(point_in_poly(asteroid->vertices, NUM_ASTEROID_VERTICES,
                     point->x - asteroid->position.x,
                     point->y - asteroid->position.y))
    {
        switch(asteroid->type)
        { 
        case(SMALL):
            asteroid->explode_countdown = EXPLOSION_LENGTH; 
            initialise_explosion(asteroid->vertices, NUM_ASTEROID_VERTICES); 
            break;
            
        case(LARGE):
            create_asteroid(MEDIUM, asteroid->position.x,
                            asteroid->position.y);
            create_asteroid(MEDIUM, asteroid->position.x,
                            asteroid->position.y);
            break;
            
        case(MEDIUM):
            create_asteroid(SMALL, asteroid->position.x, asteroid->position.y);
            create_asteroid(SMALL, asteroid->position.x, asteroid->position.y);
            break;
        }
        
        current_score++;
        asteroid_count--;
        asteroid->exists = false;
        return true;
    }
    else
        return false;
}

bool is_point_within_enemy(struct Point* point)
{
    if( is_point_within_rectangle(&enemy.position, point, 5*SCALE) )
    {
        current_score += 5;
        /*enemy_missile.survived = 0;*/
        enemy.explode_countdown = EXPLOSION_LENGTH;
        initialise_explosion(enemy.vertices, NUM_ENEMY_VERTICES);
        return true;
    }
    else
        return false;
}

bool is_ship_within_asteroid(struct Asteroid* asteroid)
{
    bool hit = false;
    struct Point p;
    
    p.x = ship.position.x + ship.vertices[0].x;
    p.y = ship.position.y + ship.vertices[0].y;
    hit |= is_point_within_asteroid(asteroid, &p);
    
    if(!hit)
    {
        p.x = ship.position.x + ship.vertices[1].x;
        p.y = ship.position.y + ship.vertices[1].y;
        hit |= is_point_within_asteroid(asteroid, &p);
        if(!hit)
        {
            p.x = ship.position.x + ship.vertices[3].x;
            p.y = ship.position.y + ship.vertices[3].y;
            hit |= is_point_within_asteroid(asteroid, &p);  
        }
    }
    
    return hit;
}

void initialise_explosion(struct Point* point, int num_points)
{
    int n;
    
    point->x += point->dx; 
    point->y += point->dy;
    for(n = num_points; --n;)
    {
        point->dx = point->x;
        point->dy = point->y;
        point++;
    }
}

/* Check for collsions between the missiles and the asteroids and the ship */
void check_collisions(void)
{
    int m, n;
    bool asteroids_onscreen = false;
    struct Missile* missile;
    struct Asteroid* asteroid;
    bool ship_cant_be_placed = false;  
    
    asteroid = asteroids_array;
    m = MAX_NUM_ASTEROIDS;
    while(--m)
    {
        /*if the asteroids exists then test missile collision:*/
        if(asteroid->exists)
        {
            missile = missiles_array;
            n = MAX_NUM_MISSILES;
            while(--n)
            {
                /*if the missiles exists:*/
                if(missile->survived > 0)
                {
                    /*has the missile hit the asteroid?*/
                    if(is_point_within_asteroid(asteroid, &missile->position)
                       || is_point_within_asteroid(asteroid,
                                                   &missile->oldpoint))
                    {
                        missile->survived = 0;
                        break;
                    }
                }
                missile++;
            }
        } 
        
        /*If it exists now, check ship collision:*/
        if(asteroid->exists)
        {
            /*now check collision with ship:*/
            if(!ship.waiting_for_space && !ship.explode_countdown)
            {
                if(is_ship_within_asteroid(asteroid))
                {
                    /*blow up ship*/
                    ship.explode_countdown = EXPLOSION_LENGTH;
                    initialise_explosion(ship.vertices, NUM_SHIP_VERTICES);
                }
            }
        }  
        
        /*has the enemy missile blown something up?*/
        if(asteroid->exists && enemy_missile.survived)
        {
            if(is_point_within_asteroid(asteroid, &enemy_missile.position))
            {
                /*take that score back then:*/
                if(current_score > 0) current_score--;
                enemy_missile.survived = 0;
            }
        }
        
        /*if it still exists, check if ship is waiting for space:*/
        if(asteroid->exists && ship.waiting_for_space)
            ship_cant_be_placed |=
                is_point_within_rectangle(&ship.position,
                                          &asteroid->position,
                                          space_check_size);
        
        /*is an asteroid still exploding?*/
        if(asteroid->explode_countdown)
            asteroids_onscreen = true;  
        
        asteroid++;    
    }
    
    /*now check collision between ship and enemy*/
    if(enemy_on_screen && !ship.waiting_for_space &&
       !ship.explode_countdown && !enemy.explode_countdown)
    {
        /*has the enemy collided with the ship?*/
        if(is_point_within_enemy(&ship.position))
        {
            ship.explode_countdown = EXPLOSION_LENGTH;
            initialise_explosion(ship.vertices, NUM_SHIP_VERTICES);
        }
        
        /*Now see if the enemy has been shot at by the ships missiles:*/
        missile = missiles_array;
        n = MAX_NUM_MISSILES;
        while(--n)
        {
            if(missile->survived > 0 &&
               is_point_within_enemy(&missile->position))
            {
                missile->survived = 0;
                break;
            }
        }
    }
    
    /*test collision with enemy missile and ship:*/
    if(!ship_cant_be_placed && enemy_missile.survived > 0 && 
       point_in_poly(ship.vertices, NUM_SHIP_VERTICES,
                     enemy_missile.position.x - ship.position.x,
                     enemy_missile.position.y - ship.position.y))
    {
        ship.explode_countdown = EXPLOSION_LENGTH;
        initialise_explosion(ship.vertices, NUM_SHIP_VERTICES);
        enemy_missile.survived = 0;
        enemy_missile.position.x = enemy_missile.position.y = 0;
    }   
    
    if(!ship_cant_be_placed)
        ship.waiting_for_space = false;
    
    /*if all asteroids cleared then start again:*/
    if(asteroid_count == 0 && !enemy_on_screen && !asteroids_onscreen)
    {
        current_level++;
        game_state = SHOW_LEVEL;
        show_level_timeout = SHOW_LEVEL_TIME;
    }
}

/*************************************************
** Creates a new asteroid of the given 4type (size)
** and at the given location.
*************************************************/
void create_asteroid(enum asteroid_type type, int x, int y)
{
    struct Asteroid* asteroid;
    int n;
    
    asteroid = asteroids_array;
    n = MAX_NUM_ASTEROIDS;
    while(--n)
    {
        if(!asteroid->exists && !asteroid->explode_countdown)
        {
            initialise_asteroid(asteroid, type);
            asteroid->position.x = x;
            asteroid->position.y = y;
            break;
        }
        asteroid++; 
    }
}

/* Initialise a missile */
void initialise_missile(struct Missile* missile)
{
    missile->position.x = ship.position.x;
    missile->position.y = ship.position.y;
    missile->position.dx = (ship.vertices[0].x - ship.vertices[2].x)/2;
    missile->position.dy = (ship.vertices[0].y - ship.vertices[2].y)/2;
    missile->survived = MISSILE_SURVIVAL_LENGTH;
    missile->oldpoint.x =   missile->position.x;
    missile->oldpoint.y =   missile->position.y;
}

/* Draw and Move all the missiles */
void draw_and_move_missiles(void)
{
    int n;
    int p1x, p1y;
    int p2x, p2y;
    
    struct Missile* missile;
    missile = missiles_array;
    n = MAX_NUM_MISSILES;
    while(--n)
    {
        if(missile->survived)
        {
            if(missile->position.dx > 0)
            {
                if(missile->position.x >= missile->oldpoint.x)
                {
                    p1x = missile->oldpoint.x;
                    p2x = missile->position.x;
                } 
                else
                {
                    p1x = 0;
                    p2x = missile->position.x;
                }
            }
            else
            {
                if(missile->oldpoint.x >= missile->position.x)
                {
                    p1x = missile->oldpoint.x;
                    p2x = missile->position.x;
                } 
                else
                {
                    p1x = missile->oldpoint.x;
                    p2x = LCD_WIDTH;
                }
            }
            
            if(missile->position.dy > 0)
            {
                if(missile->position.y >= missile->oldpoint.y)
                {
                    p1y = missile->oldpoint.y;
                    p2y = missile->position.y;
                } 
                else
                {
                    p1y = 0;
                    p2y = missile->position.y;
                }
            }
            else
            {
                if(missile->oldpoint.y >= missile->position.y)
                {
                    p1y = missile->oldpoint.y;
                    p2y = missile->position.y;
                } 
                else
                {
                    p1y = missile->oldpoint.y;
                    p2y = LCD_HEIGHT;
                }
            } 
            
            rb->lcd_drawline( p1x/SCALE, p1y/SCALE, p2x/SCALE, p2y/SCALE);
            
            if(game_state != PAUSE_MODE)
            {
                missile->oldpoint.x = missile->position.x;
                missile->oldpoint.y = missile->position.y;
                move_point(&missile->position);
                missile->survived--;
            }
        }
        missile++;
    }
}

void draw_lives(void)
{
    int n;
    int px = (LCD_WIDTH - num_lives*4 - 1);
    int py = (LCD_HEIGHT-4);
    
    n = num_lives;
    while(--n)
    {
        draw_polygon(lives_points, px, py, NUM_SHIP_VERTICES);
        px += 6;
    }
}

/*Fire the next missile*/
void fire_missile(void)
{
    int n;
    struct Missile* missile;
    
    if(!ship.explode_countdown && !ship.waiting_for_space)
    {
        missile = missiles_array;
        n = MAX_NUM_MISSILES;
        while(--n)
        {
            if(!missile->survived)
            {
                initialise_missile(missile); 
                break;
            }
            missile++;
        }
    }
}

/* Initialise the passed Asteroid */
void initialise_asteroid(struct Asteroid* asteroid, enum asteroid_type type)
{
    int n;
    bool b,b2;
    struct Point* point;
    asteroid->exists = true;
    asteroid->type = type;
    asteroid->explode_countdown = 0;
    
    /*Set the radius of the asteroid:*/
    asteroid->radius = (int)type; 
    
    /*shall we move Clockwise and Fast*/
    if((rb->rand()%100)>75)
    {
        asteroid->speed_cos = FAST_ROT_CW_COS;
        asteroid->speed_sin = FAST_ROT_CW_SIN;
    }
    else if((rb->rand()%100)>75)
    {
        asteroid->speed_cos = FAST_ROT_ACW_COS;
        asteroid->speed_sin = FAST_ROT_ACW_SIN;
    }
    else if((rb->rand()%100)>75)
    {
        asteroid->speed_cos = SLOW_ROT_ACW_COS;
        asteroid->speed_sin = SLOW_ROT_ACW_SIN;
    }
    else
    {
        asteroid->speed_cos = SLOW_ROT_CW_COS;
        asteroid->speed_sin = SLOW_ROT_CW_SIN;
    }
    
    b = (rb->rand()%100)>66;
    b2 = (rb->rand()%100)>66;
    point = asteroid->vertices;
    for(n = 0; n < NUM_ASTEROID_VERTICES*2; n+=2)
    {
        if(b)
        {
            point->x = asteroid_one[n]*asteroid->radius/2;
            point->y = asteroid_one[n+1]*asteroid->radius/2;
        }
        else if( b2 )
        {
            point->x = asteroid_two[n]*asteroid->radius/2;
            point->y = asteroid_two[n+1]*asteroid->radius/2;
        }
        else
        {
            point->x = asteroid_three[n]*asteroid->radius/2;
            point->y = asteroid_three[n+1]*asteroid->radius/2;
        }
        point->x *= SCALE;
        point->y *= SCALE;
        point++;
    }
    
    asteroid->radius *= SCALE/2;  
    
    if(asteroid->type == LARGE)
        asteroid->radius *= 7;
    else if(asteroid->type == MEDIUM)
        asteroid->radius *= 4;
    else if(asteroid->type == SMALL)
        asteroid->radius /= 2;
    
    b = true;
    while(b)
    {
        /*Set the position randomly:*/
        asteroid->position.x = (rb->rand()%SCALED_WIDTH);
        asteroid->position.y = (rb->rand()%SCALED_HEIGHT);
        
        asteroid->position.dx = 0;
        while(asteroid->position.dx == 0)
            asteroid->position.dx = (rb->rand()%10)-5;
        
        asteroid->position.dy = 0;
        while(asteroid->position.dy == 0)
            asteroid->position.dy = (rb->rand()%10)-5;
        
        asteroid->position.dx *= SCALE/10;
        asteroid->position.dy *= SCALE/10;
        
        b = is_point_within_rectangle(&ship.position, &asteroid->position,
                                      space_check_size);
    }
    
    /*Now rotate the asteroid a bit, so they all look a bit different*/
    for(n=(rb->rand()%30) + 2;--n;)
        rotate_asteroid(asteroid);
    
    /*great, we've created an asteroid, don't forget to increment the total:*/
    asteroid_count++;
}

/*Initialise the ship*/
void initialise_ship(void)
{
    struct Point* point;
    struct Point* lives_point;
    int n;
    
    ship.position.x = CENTER_LCD_X;
    ship.position.y = CENTER_LCD_Y;
    ship.position.x *= SCALE;
    ship.position.y *= SCALE;
    ship.position.dx = ship.position.dy = 0;
    
    point = ship.vertices;
    lives_point = lives_points;
    for(n = 0; n < NUM_SHIP_VERTICES*2; n+=2)
    {
        point->x = ship_vertices[n];
        point->y = ship_vertices[n+1];
        point->x *= SCALE;
        point->y *= SCALE;
        point++;
        lives_point++;
    }
    
    ship.position.dx = 0;
    ship.position.dy = 0;
    ship.explode_countdown  = 0;
    
    /*hack-o-rama-city-arizona, take it out to see what happens:*/
    for(n=17;--n;)
        rotate_ship(SHIP_ROT_ACW_COS, SHIP_ROT_ACW_SIN);
    
    /*grab a copy of the ships points for the lives display:*/
    point = ship.vertices;
    lives_point = lives_points;
    for(n = 0; n < NUM_SHIP_VERTICES*2; n+=2)
    {
        lives_point->x = point->x; 
        lives_point->y = point->y;
        lives_point++;
        point++;
    }
}

void rotate_asteroid(struct Asteroid* asteroid)
{
    struct Point* point;
    int n;
    long xtemp;
    
    point = asteroid->vertices;
    for(n = NUM_ASTEROID_VERTICES+1; --n;)
    {
        xtemp = point->x;
        point->x = xtemp*asteroid->speed_cos/SIN_COS_SCALE -
            point->y*asteroid->speed_sin/SIN_COS_SCALE;
        point->y = point->y*asteroid->speed_cos/SIN_COS_SCALE +
            xtemp*asteroid->speed_sin/SIN_COS_SCALE;
        point++;
    }
}

/*************************************************
** Draws the ship, moves the ship and creates a new 
** one if it's finished exploding.
**************************************************/
void draw_and_move_ship(void)
{
    int nxoffset = ship.position.x/SCALE;
    int nyoffset = ship.position.y/SCALE;
    if(!ship.explode_countdown)
    {
        if(!ship.waiting_for_space)
            draw_polygon(ship.vertices, nxoffset, nyoffset, NUM_SHIP_VERTICES);
    }
    else
    {
        animate_and_draw_explosion(ship.vertices, NUM_SHIP_VERTICES,
                                   ship.position.x/SCALE,
                                   ship.position.y/SCALE);
        if(game_state != PAUSE_MODE)
        {
            ship.explode_countdown--;
            if(!ship.explode_countdown)
            {
                num_lives--;
                if(!num_lives)
                {
                    show_game_over = SHOW_GAME_OVER_TIME;
                    game_state = GAME_OVER;
                }
                else
                {
                    initialise_ship();
                    ship.waiting_for_space = true;
                }
            }
        }
    }
    
    if(game_state != PAUSE_MODE && game_state != GAME_OVER)
        move_point(&ship.position);
}

void thrust_ship(void)
{
    if(!ship.waiting_for_space)
    {
        ship.position.dx += ( ship.vertices[0].x - ship.vertices[2].x )/10;
        ship.position.dy += ( ship.vertices[0].y - ship.vertices[2].y )/10;
        /*if dx and dy are below a certain threshold, then set 'em to 0*/
    }
}

/**************************************************
** Rotate the ship using the passed sin & cos values
***************************************************/
void rotate_ship(int c, int s)
{
    struct Point* point;
    int n;
    double xtemp;
    
    if(!ship.waiting_for_space && !ship.explode_countdown)
    {
        point = ship.vertices;
        for(n=NUM_SHIP_VERTICES+1;--n;)
        {
            xtemp = point->x;
            point->x = xtemp*c/SIN_COS_SCALE - point->y*s/SIN_COS_SCALE;
            point->y = point->y*c/SIN_COS_SCALE + xtemp*s/SIN_COS_SCALE;
            point++;
        }
    }
}

void drawstars()
{
    struct Point* p;
    int n = NUM_STARS;
    
    p = stars;
    while(--n)
    {
        rb->lcd_drawpixel(p->x , p->y);
        p++;
    }
}

/*************************************************
**  Draw And Move all Asteroids
*************************************************/
void draw_and_move_asteroids(void)
{
    int n;
    struct Asteroid* asteroid;
    
    asteroid = asteroids_array;
    n  = MAX_NUM_ASTEROIDS;
    while(--n)
    {
        if(game_state != PAUSE_MODE)
        {
            if(asteroid->exists)
            {
                move_point(&asteroid->position);
                rotate_asteroid(asteroid);
                draw_polygon(asteroid->vertices, asteroid->position.x/SCALE,
                             asteroid->position.y/SCALE,
                             NUM_ASTEROID_VERTICES);
            }
            else if(asteroid->explode_countdown)
            {
                animate_and_draw_explosion(asteroid->vertices,
                                           NUM_ASTEROID_VERTICES,
                                           asteroid->position.x/SCALE,
                                           asteroid->position.y/SCALE);
                if(game_state != PAUSE_MODE)
                    asteroid->explode_countdown--;
            }
        }
        else
        {
            if(asteroid->exists)
                draw_polygon(asteroid->vertices,
                             asteroid->position.x/SCALE,
                             asteroid->position.y/SCALE,
                             NUM_ASTEROID_VERTICES);
        }
        asteroid++;
    }
} 

void create_stars(void)
{
    struct Point* p;
    int n;
    
    p = stars;
    n = NUM_STARS;
    while(--n)
    {
        p->x = (rb->rand()%LCD_WIDTH);
        p->y = (rb->rand()%LCD_HEIGHT);
        p++;
    }
}

/*************************************************
** Creates start_num number of new asteroids of 
** full size.
**************************************************/
void initialise_game(int start_num)
{
    int n;
    asteroid_count = next_missile_count = next_thrust_count = 0;
    struct Asteroid* asteroid;  
    struct Missile* missile;
    
    /*no enemy*/
    enemy_on_screen = 0;
    enemy_missile.survived = 0;
    
    /*clear asteroids*/
    asteroid = asteroids_array;  
    n = MAX_NUM_ASTEROIDS;
    while(--n)
    {
        asteroid->exists = false;
        asteroid++;
    }
    
    /*make some LARGE asteroids*/
    for(n = 0; n < start_num; n++)
        initialise_asteroid(&asteroids_array[n], LARGE);
    
    /*ensure all missiles are out of action:  */
    missile = missiles_array;
    n = MAX_NUM_MISSILES;
    while(--n)
    {
        missile->survived=0;
        missile++;
    }
}

void start_attract_mode(void)
{
    current_level = 5;
    num_lives = START_LIVES;
    current_score = 0;
    attract_flip_timeout = ATTRACT_FLIP_TIME;
    game_state = ATTRACT_MODE;
    if(asteroid_count < 3)
        initialise_game(current_level);
}

enum plugin_status start_game(void)
{
    char s[20];
    char level[10];
    int button;
    int end;
    int CYCLETIME = 30;
    
    /*create stars once, and once only:*/
    create_stars();
    
    while(true)
    {
        /*game starts with at level 1 
          with 1 asteroid.*/
        start_attract_mode();
        
        /*Main loop*/
        while(true)
        {
            end = *rb->current_tick + (CYCLETIME * HZ) / 1000;
            rb->lcd_clear_display();
            switch(game_state)
            {
            case(ATTRACT_MODE):
                if(attract_flip_timeout < ATTRACT_FLIP_TIME/2)
                {
                    rb->lcd_putsxy(CENTER_LCD_X - 39,
                                   CENTER_LCD_Y + CENTER_LCD_Y/2 - 4,
                                   "Fire to Start");
                    if(!attract_flip_timeout)
                        attract_flip_timeout = ATTRACT_FLIP_TIME;
                }
                else
                {
                    rb->snprintf(s, sizeof(s), "Hi Score %d ", high_score);
                    rb->lcd_putsxy(CENTER_LCD_X - 30,
                                   CENTER_LCD_Y + CENTER_LCD_Y/2 - 4, s);
          }
                attract_flip_timeout--;
                break;
                
            case(GAME_OVER):
                rb->lcd_putsxy(CENTER_LCD_X - 25,
                               CENTER_LCD_Y + CENTER_LCD_Y/2 - 4, "Game Over");
                rb->snprintf(s, sizeof(s), "score %d ", current_score);
                rb->lcd_putsxy(1,LCD_HEIGHT-8, s);
                show_game_over--;
                if(!show_game_over)
                    start_attract_mode();
                break;
        
            case(PAUSE_MODE):
                rb->snprintf(s, sizeof(s), "score %d ", current_score);
                rb->lcd_putsxy(1,LCD_HEIGHT-8, s);
                rb->lcd_putsxy(CENTER_LCD_X - 15,
                               CENTER_LCD_Y + CENTER_LCD_Y/2 - 4, "pause");
                draw_and_move_missiles();
                draw_lives();
                draw_and_move_ship();
                break;
        
            case(PLAY_MODE):
                rb->snprintf(s, sizeof(s), "score %d ", current_score);
                rb->lcd_putsxy(1,LCD_HEIGHT-8, s);
                draw_and_move_missiles();
                draw_lives();          
                check_collisions();
                draw_and_move_ship();
                break;
  
            case(SHOW_LEVEL):
                show_level_timeout--;
                rb->snprintf(s, sizeof(s), "score %d ", current_score);
                rb->lcd_putsxy(1,LCD_HEIGHT-8, s);
                rb->snprintf(level, sizeof(level), "stage %d ", current_level);
                rb->lcd_putsxy(CENTER_LCD_X - 20,
                               CENTER_LCD_Y + CENTER_LCD_Y/2 - 4, level);
                draw_and_move_ship();
                draw_lives();
                if(!show_level_timeout) 
                {
                    initialise_game(current_level);
                    game_state = PLAY_MODE;
                    draw_lives();
                }
                break;
            }
            drawstars();      
            draw_and_move_asteroids();
            draw_and_move_enemy();
            
            rb->lcd_update();
            button = rb->button_get(false);
            switch(button)
            {
            case(AST_PAUSE):
                if(game_state == PLAY_MODE)
                    game_state = PAUSE_MODE;
                else if(game_state == PAUSE_MODE)
                    game_state = PLAY_MODE;
                break;  
                
            case(AST_QUIT):
                if(game_state == ATTRACT_MODE)
                    return PLUGIN_OK;
                else if(game_state == GAME_OVER)
                {
                    start_attract_mode();  
                }
                else
                {
                    show_game_over = SHOW_GAME_OVER_TIME;
                    game_state = GAME_OVER;
                }
                break;
                
            case (AST_LEFT_REP):
            case (AST_LEFT):
                if(game_state == PLAY_MODE || game_state == SHOW_LEVEL)
                    rotate_ship(SHIP_ROT_ACW_COS, SHIP_ROT_ACW_SIN);
                break;
                
            case (AST_RIGHT_REP):
            case (AST_RIGHT):
                if(game_state == PLAY_MODE || game_state == SHOW_LEVEL)
                    rotate_ship(SHIP_ROT_CW_COS, SHIP_ROT_CW_SIN);
                break;        
                
            case (AST_THRUST_REP):
            case (AST_THRUST):
                if((game_state == PLAY_MODE || game_state == SHOW_LEVEL) && !next_thrust_count)
                {
                    thrust_ship();
                    next_thrust_count = 5;
                }
                break;    
                
            case (AST_HYPERSPACE):
                if(game_state == PLAY_MODE)          
                    hyperspace();
                /*maybe shield if it gets too hard  */
                break;       
                
            case (AST_FIRE_REP):
            case (AST_FIRE):
                if(game_state == ATTRACT_MODE)
                {
                    current_level = START_LEVEL;
                    initialise_ship();
                    initialise_game(current_level);
                    show_level_timeout = SHOW_LEVEL_TIME;
                    game_state = PLAY_MODE;
                }
                else if(game_state == PLAY_MODE)
                {
                    if(!next_missile_count)
                    {
                        fire_missile();
                        next_missile_count = 10;
                    }
                }
                else if(game_state == PAUSE_MODE)
                {
                    game_state = PLAY_MODE;
                }
                break;
                
            default:
                if (rb->default_event_handler(button)==SYS_USB_CONNECTED) 
                    return PLUGIN_USB_CONNECTED;
                break;
            }
            
            if(!num_lives)
            {
                if(high_score < current_score)
                    high_score = current_score;
                if(!show_game_over) 
                    break;
            }
            
            if(next_missile_count)
                next_missile_count--;
            
            if(next_thrust_count)
                next_thrust_count--;
            
            if (end > *rb->current_tick)
                rb->sleep(end-*rb->current_tick);
            else
                rb->yield();
        }
        
    }
}

enum plugin_status plugin_start(struct plugin_api* api, void* parameter)
{
    enum plugin_status retval;
    
    (void)(parameter);
    rb = api;
    
    game_state = ATTRACT_MODE;
    
    /* universal font */
    rb->lcd_setfont(FONT_SYSFIXED);
    rb->backlight_set_timeout(1);
    iohiscore();
    retval = start_game();  
    iohiscore();
    rb->lcd_setfont(FONT_UI);
    /* restore normal backlight setting*/
    rb->backlight_set_timeout(rb->global_settings->backlight_timeout);
    
    return retval;
}
