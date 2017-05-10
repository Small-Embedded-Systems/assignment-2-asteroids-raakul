/* Asteroids
    Semester 2 -- Small Embedded Systems(CM0506)
    Luke Glen McNaughton		ID: 15011875
		10/05/2017
*/

/* C libraries */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

/* Hardware platform libraries */
#include <display.h>
#include <mbed.h>

/* Main game elements */
#include "model.h"
#include "view.h"
#include "controller.h"

/* Game state */
float elapsed_time; 
int   score = 0;
int   lives = 5;
int shield = 3;
struct ship player;
bool paused = true;

/* Game speed */
float Dt = 0.01f;

/* Ticker objects to manage physics, graphics and input */
Ticker model, view, controller;

/* Local Method Declarations*/
static void death();

/* Button used to begin play from paused state */
DigitalIn userbutton(P2_10,PullUp);

/* Main Method */
int main()
{
    init_DBuffer();
    
    view.attach( draw, 0.025);
    model.attach( physics, Dt);
    controller.attach( controls, 0.025);
    
    /* Pause to start */
    while( userbutton.read() ){
        paused=true;
    }
		paused = false;
		
		/* Main Loop */
    while(true) {
			//If the shields are broken end current life
			if(shield < 1) {
				death();
			}
			//If the player runs out of lives reset the game when play is resumed
			if(lives < 0) {
				lives = 5;
				score = 0;
				elapsed_time = 0;
			}
    }
}

/* Called when the player crashes the ship. 
	 Pauses the game, resets shields and takes away a life.  */
static void death() {
	lives --;
	while( userbutton.read() ){
        paused=true;
    }
	shield = 3;
	paused = false;
}
