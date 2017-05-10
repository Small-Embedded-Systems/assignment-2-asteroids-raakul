/* Asteroids Model 
    Semester 2 -- Small Embedded Systems(CM0506)
    Luke Glen McNaughton		ID: 15011875
		10/05/2017
*/

/* C libraries */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

/* Main game elements */
#include "model.h"
#include "utils.h"
#include "asteroids.h"

/* Local Variables */
static const int asteroidHeapSize = 15;
static const int missileHeapSize = 6;
static float missileCoolDown = 0;
static asteroid_t asteroidHeap[asteroidHeapSize];
static asteroid_t *asteroidFreeNodes;
static missile_t missileHeap[missileHeapSize];
static missile_t *missileFreeNodes;
struct asteroid *asteroidCurrent = NULL;
struct missile *missileCurrent = NULL;

/* Asteroid List Methods */
static void asteroidFreeNode(asteroid_t *i);
static void initAsteroidHeap(void);
static void killAsteroids(struct asteroid *a);
static void asteroidPhysics();
	static void newAsteroid(void);
	static void asteroidUpdate(struct asteroid *a);
	static void asteroidScreenWrap(struct asteroid *a);
	static void asteroidPlayerCollision(struct asteroid *a);

/* Missile List Methods */
static void missileFreeNode(missile_t *i);
static void initMissileHeap(void);
static void killMissiles(struct missile *m);
static void missilePhysics();
static void newMissile(void);
static void missileUpdate(struct missile *m);
static void missileOffScreen(struct missile *m);

/* Missile Asteroid Method */
static void missileAsteroidCollision(struct missile *m, struct asteroid *a);

/* Player Movement Methods */
static void resetShip();
static void shipMovement();
	static void fullRotation();
	static float directionx(int heading);
	static float directiony(int heading);
	static void screenWrap();
	static float getTotalSpeed();
	static void drag();
	
/* Scoring Method */
static void scoring();

/* Main Physics Method */
void physics(void)
{
	//Initialise heaps on start screen
	if (paused && lives == 5) {
		initAsteroidHeap();
		initMissileHeap();
	}
	//Reset the players position and remove list objects when game is paused
	if (paused) {
		resetShip();
		killAsteroids(asteroidCurrent);
		killMissiles(missileCurrent);
	}
	//While the game is in play
	else {
		shipMovement();
		asteroidPhysics();
		missilePhysics();
		missileAsteroidCollision(missileCurrent, asteroidCurrent);
		scoring();
	}
}

/* Initiate the asteroid heap */
void initAsteroidHeap(void) {
	int i;
	for (i = 0; i < (asteroidHeapSize - 1); i++) {
		asteroidHeap[i].next = &asteroidHeap[i + 1];
	}
	asteroidHeap[i].next = NULL;
	asteroidFreeNodes = &asteroidHeap[0];
}

/* Initiate the missile heap */
void initMissileHeap(void) {
	int i;
	for (i = 0; i < (missileHeapSize - 1); i++) {
		missileHeap[i].next = &missileHeap[i + 1];
	}
	missileHeap[i].next = NULL;
	missileFreeNodes = &missileHeap[0];
}

/* Allocate a asteroid node */
asteroid_t *allocateNodeAsteroid(void) {
	asteroid_t *asteroidNode = NULL;
	if (asteroidFreeNodes) {
		asteroidNode = asteroidFreeNodes;
		asteroidFreeNodes = asteroidFreeNodes->next;
	}
	return asteroidNode;
}

/* Allocate a missile node */
missile_t *allocateNodeMissile(void) {
	missile_t *missileNode = NULL;
	if (missileFreeNodes) {
		missileNode = missileFreeNodes;
		missileFreeNodes = missileFreeNodes->next;
	}
	return missileNode;
}

/* Free a asteroid node */
void asteroidFreeNode(asteroid_t *i){
	i->next = asteroidFreeNodes;
	asteroidFreeNodes = i;
}

/* Free a missile node */
void missileFreeNode(missile_t *i){
	i->next = missileFreeNodes;
	missileFreeNodes = i;
}

/* Reset the ships values */
void resetShip() {
	player.heading = 90;
	player.fire = false;
	player.position.x = 220;
	player.position.y = 120;
	player.velocity.x = 0;
	player.velocity.y = 0;
}

/* Calculate the ships velocity and update its position */
static void shipMovement() {
	fullRotation();
	screenWrap();
	float xperc = directionx(player.heading);
	float yperc = directiony(player.heading);
	//If the ship is below its terminal velocity
	if (getTotalSpeed() < 3)
	{
		//If the player is braking reduce velocity drastically
		if (player.engines < 0) {
			player.velocity.x -= (player.velocity.x / 10);
			player.velocity.y -= (player.velocity.y / 10);
		}
		//If the player is accelerating increase velocity in the direction of the heading
		else if (player.engines > 0) {
			player.velocity.x += player.engines * xperc * 0.0005;
			player.velocity.y -= player.engines * yperc * 0.0005;
		}
	}
	drag();
	//Update the players position using their velocity
	player.position.x += player.velocity.x;
	player.position.y += player.velocity.y;
}

/* If the player has turned 360 degrees reset the heading */
void fullRotation() {
	if (player.heading > 360) {
		player.heading = 1;
	}
	else if (player.heading < 1) {
		player.heading = 360;
	}
}

/* If the player leaves the screen place them on the other side of the display */
void screenWrap() {
	if (player.position.x > 485) {
		player.position.x = 5;
	}
	else if (player.position.x < -5) {
		player.position.x = 475;
	}
	else if (player.position.y > 270) {
		player.position.y = 5;
	}
	else if (player.position.y < -5) {
		player.position.y = 265;
	}
}

/* Calculate the players x velocity with the current heading */
float directionx(int heading) {
	float xperc;
	if (heading <= 90) {
		xperc = heading;
	}
	else if (heading <= 180) {
		xperc = 90 - (heading - 90);
	}
	else if (heading <= 270) {
		xperc = 90 - (heading - 90);
	}
	else if (heading > 270) {
		xperc = - (360 - heading);
	}
	return xperc;
}

/* Calculate the players y velocity with the current heading */
float directiony(int heading) {
	float yperc;
	if (heading <= 90) {
		yperc = 90 - heading;
	}
	else if (heading <= 180) {
		yperc = - (heading - 90);
	}
	else if (heading <= 270) {
		yperc = - (270 - heading);
	}
	else if (heading > 270) {
		yperc = heading - 270;
	}
	return yperc;
}

/* Get the players current total speed */
float getTotalSpeed() {
	float speedx;
	float speedy;
	if (player.velocity.x < 0) {
		speedx = -player.velocity.x;
	}
	else {
		speedx = player.velocity.x;
	}
	if (player.velocity.y < 0) {
		speedy = -player.velocity.y;
	}
	else {
		speedy = player.velocity.y;
	}
	return speedx + speedy;
}

/* Reduce the players velocity by 1% */
void drag() {
	player.velocity.x -= (player.velocity.x / 100);
	player.velocity.y -= (player.velocity.y / 100);
}

/* Calculate the asteroids position, velocity and find any collisions with the player */
void asteroidPhysics() {
	newAsteroid();
	asteroidUpdate(asteroidCurrent);
	asteroidScreenWrap(asteroidCurrent);
	asteroidPlayerCollision(asteroidCurrent);
}

/* Randomize a new asteroids values */
void asteroidValues(struct asteroid *a) {
	a->position.x = randrange(20,461);
	a->position.y = randrange(20,241);
	a->size = randrange(10, 31);
	a->velocity.x = randrange(-10, 10);
	a->velocity.y = randrange(-10, 10);
			//If the asteroid will spawn on the player recalculate its values
			while (player.position.x > (a->position.x - (a->size) - 15) 
				&& player.position.x < (a->position.x + (a->size) + 15) 
				&& player.position.y > (a->position.y - (a->size) - 15)
				&& player.position.y < (a->position.y + (a->size) + 15)) {
					a->position.x = randrange(20,461);
					a->position.y = randrange(20,241);
				}
	a->live = true;
}

/* Create a new asteroid in the list */
void newAsteroid() {
	struct asteroid *newAsteroid = allocateNodeAsteroid();
	if (newAsteroid) {
		newAsteroid->next = asteroidCurrent;
		asteroidCurrent = newAsteroid;
		asteroidValues(newAsteroid);
	}
}

/* Destroy all asteroids in the list */
void killAsteroids(struct asteroid *a) {
	for (; a; a = a->next) {
			a->live = false;
	}
}

/* Calculate the asteroids position and velocity */
void missilePhysics() {
	// If the player has pressed fire and half a second has passed 
	// since the last missile was fired then fire a missile
	if (player.fire && missileCoolDown < 0) {
		newMissile();
		missileCoolDown = 0.5;
	}
	player.fire = false;
	missileCoolDown -= Dt;
	
	missileUpdate(missileCurrent);
	missileOffScreen(missileCurrent);
}

/* Set a missiles velocity and position based on the ships current heading */
void missileValues(struct missile *m) {
	m->velocity.x = directionx(player.heading) * 0.01;
	m->velocity.y = -directiony(player.heading) * 0.01;
	m->position.x = player.position.x + m->velocity.x * 10;
	m->position.y = player.position.y + m->velocity.y * 10;
	m->live = true;;
}

/* Create a new missile in the list */
void newMissile() {
	struct missile *newMissile = allocateNodeMissile();
	if (newMissile) {
		newMissile->next = missileCurrent;
		missileCurrent = newMissile;
		missileValues(newMissile);
	}
}

/* Destroy all missiles in the list*/
void killMissiles(struct missile *m) {
	for (; m; m = m->next) {
			m->live = false;
	}
}

/* Update the asteroids positions using their velocity 
		and remove an asteroid if it has been destroyed */
void asteroidUpdate(struct asteroid *a) {
	for (; a; a = a->next) {
		a->position.x += a->velocity.x/25;
		a->position.y += a->velocity.y/25;
		if (a->next->live == false) {
			struct asteroid *dead = a->next;
			a->next = a->next->next;
			asteroidFreeNode(dead);
		}
	}
}

/* Update the missiles positions using their velocity 
		and remove an missile if it has been destroyed */
void missileUpdate(struct missile *m) {
	for (; m; m = m->next) {
		m->position.x += m->velocity.x;
		m->position.y += m->velocity.y;
		if (m->next->live == false) {
			struct missile *dead = m->next;
			m->next = m->next->next;
			missileFreeNode(dead);
		}
	}
}

/* If an asteroid has passed the boundaries of the 
		screen move it to the other side of the screen */
void asteroidScreenWrap(struct asteroid *a) {
	for (; a; a = a->next) {
		if (a->position.x > 480) {
			a->position.x = 0;
		}
		else if (a->position.x < 0) {
			a->position.x = 480;
		}
		else if (a->position.y > 270) {
			a->position.y = 0;
		}
		else if (a->position.y < 0) {
			a->position.y = 270;
		}
	}
}

/* If a missile has passed the boundaries of the screen destroy it */
void missileOffScreen(struct missile *m) {
	for (; m; m = m->next) {
		if (m->position.x > 480 || m->position.x < 0 ||
						m->position.y > 270 || m->position.y < 0) {
			m->live = false;
		}
	}
}

/* If the player touches an asteroid destroy the asteroid and damage the ships shield */
void asteroidPlayerCollision(struct asteroid *a) {
	for (; a; a = a->next) {
		if (player.position.x > (a->position.x - (a->size) - 8) 
				&& player.position.x < (a->position.x + (a->size) + 8) 
				&& player.position.y > (a->position.y - (a->size) - 8)
				&& player.position.y < (a->position.y + (a->size) + 8)
				&& a->live == true)
		{
			a->live = false;
			shield --;
		}
	}
}


/* If a missile touches an asteroid destroy them both*/
void missileAsteroidCollision(struct missile *m, struct asteroid *a) {
	for (; m; m = m->next) {
		for (; a; a = a->next) {
			if (m->position.x > (a->position.x - (a->size)) 
					&& m->position.x < (a->position.x + (a->size)) 
					&& m->position.y > (a->position.y - (a->size))
					&& m->position.y < (a->position.y + (a->size))
					&& a->live == true && m->live == true)
			{
				a->live = false;
				m->live = false;
			}
		}
	}
}

/* For every second the player survives increment the score */
void scoring() {
	elapsed_time += Dt;
	if (elapsed_time > score) {
		score ++;
	}
}
