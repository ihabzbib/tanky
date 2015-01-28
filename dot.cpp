//============================================================================
// Name        : dot.cpp
// Author      : Ihab Zbib
// Version     :
// Copyright   : CSUSB
//============================================================================
#include <string>
#include <iostream>
#include "figure.h"
#include "playground.h"
#include "dot.h"


using namespace std;

extern SDL_mutex *tank_mutex;
extern SDL_mutex *missile_mutex;
extern SDL_mutex *sound_mutex;
extern SDL_mutex *score_mutex;

extern bool quit;		//global variable

void dot_init(Figure* figure, PlayGround* pg){
	figure->x = 40;
	figure->y = (rand()%(pg->height -40)) + 40;
	figure->dx = (rand()%5) + 2;
	figure->dy = (rand()%5) + 2;
	if(figure->y > pg->height / 2){
		figure->dy = -figure->dy;
	}
	figure->show = true;
	figure->image = figure->load_image("img/dot.bmp");
}

void dot_init_all(PlayGround* pg){
	for(int i=0; i< pg->max_dots; i++){
		//Instantiate Tank
		Figure* fig = new Figure();
		dot_init(fig, pg);
		pg->addDot(fig);
	}
}



int dot_thread( void *play_ground ){

	PlayGround *pg = ( PlayGround* ) play_ground;

	dot_init_all(pg);

	//Until we quit
	while( quit == false ) {
		for(int i=0; i< pg->dot_idx; i++ ){

			Figure* figure = pg->dots[i];
			SDL_mutexP(tank_mutex); //Lock tank mutex
			bool collide = pg->collide_bounding_box(pg->tank, figure);
			SDL_mutexV(tank_mutex); //Release tank mutex

			if(collide){
				SDL_mutexP(tank_mutex); //Lock mutex
				pg->tank_hit = true;
				SDL_mutexV(tank_mutex); //Release mutex

				SDL_mutexP(sound_mutex); //Lock mutex
				pg->soundtrack.explode = true;
				SDL_mutexV(sound_mutex); //Release mutex

				dot_init(figure, pg);
			}
			else if(pg->collide_bounding_box(pg->missile, figure)){

				SDL_mutexP(missile_mutex); //Lock mutex
				pg->missile_hit = true;
				SDL_mutexV(missile_mutex); //Release mutex

				SDL_mutexP(score_mutex); //Lock mutex
				pg->score += 10;
				SDL_mutexV(score_mutex); //Release mutex

				SDL_mutexP(sound_mutex); //Lock mutex
				pg->soundtrack.explode = true;
				SDL_mutexV(sound_mutex); //Release mutex

				dot_init(figure, pg);
			}

			//If out of boundary - UP or DOWN
			else if(figure->y < 40 || figure->y > pg->height - 30){
				//Invert dy and increment y the speed
				figure->dy = -(figure->dy + (figure->dy / abs(figure->dy)) ) ;

				//Increment x speed
				figure->dx = figure->dx + (figure->dx / abs(figure->dx)) ;
			}

			//If out of boundary - LEFT or RIGHT
			else if(figure->x < 40 || figure->x > pg->width - 30){
				//Invert dx and increment x speed
				figure->dx = -(figure->dx + (figure->dx / abs(figure->dx)));

				//Increment x speed
				figure->dy = figure->dy + (figure->dy / abs(figure->dy)) ;
			}

			if(abs(figure->dy) > 10 || abs(figure->dx) > 10){
				dot_init(figure, pg);
			}else{
				//Update coordinate
				figure->x += figure->dx;
				figure->y += figure->dy;

			}


		}



		SDL_Delay(50); // Pause

	}
	for(int i=0; i < pg->dot_idx; i++){
		 pg->dots[i]->clean_up();
	}

	return 0;
}
