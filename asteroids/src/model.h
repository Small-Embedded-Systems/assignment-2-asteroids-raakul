/* Asteroids Model */
struct point {
    float x,y;
};
typedef struct point coordinate_t;
typedef struct point vector_t;

/* Some initial struct types if you want to usd them */
struct ship {
		int heading; 
		int engines;
		bool fire;
    coordinate_t position;
    vector_t     velocity;
};

/* Asteroid structure for use in the asteroid linked list */
typedef struct asteroid {
    coordinate_t position;
		vector_t 	velocity;
		int size;
		bool live;
    struct asteroid *next;
} asteroid_t;

/* Missile structure for use in the missile linked list */
typedef struct missile {
    coordinate_t position;
		vector_t     velocity;
		bool live;
    struct missile *next;
} missile_t;

/* Main physics method */
void physics(void);

/* Asteroid and missile list pointers */
asteroid_t *allocateNodeAsteroid(void);
missile_t *allocateNodeMissile(void);

/* Externalised variables */
extern struct	asteroid *asteroidCurrent;
extern const int asteroidHeapSize;
extern struct missile *missileCurrent;
extern const int missileHeapSize;
