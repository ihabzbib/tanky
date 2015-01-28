//============================================================================
// Name        : target.cpp
// Author      : Ihab Zbib
// Version     :
// Copyright   : CSUSB
//============================================================================
#include <iostream>
#include <sstream>
#include <string>
#include "figure.h"
#include "playground.h"
#include "target.h"
#include "dot.h"


using namespace std;

extern SDL_Event event;

extern SDL_mutex *target_mutex;
extern SDL_mutex *tank_mutex;
extern SDL_mutex *sound_mutex;

extern bool quit;		//global variable


void target_init(Figure* figure, PlayGround* pg){
	figure->x = 5;
	figure->y = pg->height + 20;
	figure->bg_x = 0;
	figure->bg_y = 0;
	pg->target_hit = false;
	figure->image = figure->load_image("img/rocket.jpg");
}

int target_thread( void *play_ground ){

	PlayGround *pg = ( PlayGround* ) play_ground;

	//Instantiate Tank
	Figure* figure = new  Figure() ;

	target_init(figure, pg);

	figure->show = true;

	SDL_mutexP(target_mutex);
	pg->target = figure;
	SDL_mutexV(target_mutex);

	//Until we quit
	while( quit == false ) {

		SDL_mutexP(target_mutex); //lock target mutex
		SDL_mutexP(tank_mutex); //Lock tank mutex
		if(pg->collide_bounding_box(figure, pg->tank)){
			pg->target_hit = true;
			pg->tank_hit = true;

			SDL_mutexP(sound_mutex);
			pg->soundtrack.explode = true;
			SDL_mutexV(sound_mutex);
		}
		SDL_mutexV(tank_mutex); //Release tank mutex

		bool is_hit = pg->target_hit;
		if(is_hit){

			figure->setImage("img/explosion.png");

			SDL_Delay(500); // Pause

			target_init(figure, pg);

		}

		else{
			//Update coordinate
			if(figure->y > 50){
				figure->y -= 20;
			}else{
				figure->y = pg->height + figure->image->h;
			}
		}
		SDL_mutexV(target_mutex); //Release target mutex

		SDL_Delay(50); // Pause

	}


	figure->clean_up();
	return 0;
}
