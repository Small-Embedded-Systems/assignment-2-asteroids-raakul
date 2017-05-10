/* Game state */

extern float elapsed_time; /* Time this ship has been active */
extern int   score;        /* Total score so far */
extern int   lives;        /* Lives remaining */
extern int shield;				 /* Hits that can be taken before death */
extern bool paused;				 /* Stops play when true */
extern struct ship player; /* The players position and movement */

extern const float Dt; /* Time step for physics, needed for consistent motion */
