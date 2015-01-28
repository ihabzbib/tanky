//============================================================================
// Name        : game.cpp
// Author      : Ihab Zbib
// Version     :
// Copyright   : CSUSB
//============================================================================
#include <iostream>
#include <sstream>
#include <string>
#include "tank.h"
#include "playground.h"
#include "dot.h"
#include "tank.h"

using namespace std;

const int TANK_NO_REFRESH = 0;
const int TANK_REFRESH = 1;
const int TANK_FIRE = 2;

extern SDL_Event event;

extern SDL_mutex *missile_mutex;		//mutex for accessing missile
extern SDL_mutex *target_mutex;			//mutex for accessing target
extern SDL_mutex *score_mutex;			//mutex for accessing score
extern SDL_mutex *sound_mutex;			//mutex for accessing sound
extern SDL_mutex *tank_mutex;			//mutex for accessing tank
extern SDL_mutex *lives_mutex;			//mutex for accessing lives
extern SDL_mutex *key_mutex;			//mutex for accessing lives

extern bool quit;		//global variable

void movemissile(PlayGround* pg){
	if(pg->missile->show){
		if(pg->missile->x > 0){
			if(pg->missile_hit){
				SDL_mutexP(missile_mutex); //Lock mutex
				pg->missile->setImage("img/explosion.png");
				SDL_mutexV(missile_mutex); //Release mutex
				SDL_Delay(500); // Pause
				SDL_mutexP(missile_mutex); //Lock mutex
				pg->missile->show = false;
				pg->missile->x = -50;
				SDL_mutexV(missile_mutex); //Release mutex
			}
			else if(pg->collide_bounding_box(pg->missile, pg->target))
			{
				SDL_mutexP(target_mutex); //Lock target mutex
				pg->target_hit = true;
				SDL_mutexV(target_mutex); //Release target mutex

				SDL_mutexP(score_mutex); //Lock score mutex
				pg->score += 100;
				SDL_mutexV(score_mutex); //Release score mutex

				SDL_mutexP(sound_mutex); //Lock sound mutex
				pg->soundtrack.explode = true;
				SDL_mutexV(sound_mutex); //Lock sound mutex

				SDL_mutexP(missile_mutex); //Lock missile mutex
				pg->missile->show = false;
				pg->missile->x = -50;
				SDL_mutexV(missile_mutex); //Release missile mutex
			}
			else{
				SDL_mutexP(missile_mutex); //Lock missile mutex
				pg->missile->x -= 30;
				SDL_mutexV(missile_mutex); //Release missile mutex
			}

		}else{
			SDL_mutexP(missile_mutex); //Lock missile mutex
			pg->missile->show = false;
			SDL_mutexV(missile_mutex); //Release missile mutex
		}
	}
}



/**
 *
 */
void load_tank_moving_sequence(Figure *figure, int start, int end, int delay, PlayGround *pg){

	if(start > end){
		throw printf("start bigger than end");
	}

	string filename;
	for(int i=start; i< end + 1; i++){

		stringstream number;

		//Number to string
		number << i;

		figure->setImage("img/l_tank" + number.str() + ".png");

		//Pause
		SDL_Delay(delay);

	}
}



/**
 *
 */
int process_key_event(SDL_Event event, Figure *figure, PlayGround *pg){
	int result = TANK_NO_REFRESH;
	//Set the proper message surface
	switch( event.key.keysym.sym ) {
		case SDLK_UP:
			if(figure->y > figure->height + 20){
				figure->y -= 10;
				result = TANK_REFRESH;
			}
			break;
		case SDLK_DOWN:
			if(figure->y + figure->height + 90 < pg->height){
				figure->y += 15;
				result = TANK_REFRESH;
			}
			break;
		case SDLK_LEFT:
			if(figure->x > figure->width && figure->x > figure->width){
				figure->x -= 15;
				result = TANK_REFRESH;
			}
			break;
		case SDLK_RIGHT:
			if(figure->x + figure->width + 100 < pg->width){
				figure->x += 15;
				result = TANK_REFRESH;
			}
			break;
		case SDLK_SPACE:
			result = TANK_FIRE;
			break;
		default:
			;
	}
	return result;
}

void tank_init(Figure* figure, PlayGround* pg){
	figure->x = pg->width - 100;
	figure->y = pg->height / 2;
	figure->bg_x = 100;
	figure->bg_y = 0;
	figure->show = true;
	figure->setImage("img/l_tank1.png");

	SDL_mutexP(tank_mutex); //Lock tank mutex
	pg->tank_hit = false;
	SDL_mutexV(tank_mutex); //Release tank mutex

}

void missile_init(Figure* figure, PlayGround* pg){
	figure->x = pg->tank->x - pg->tank->height - 30;
	figure->y = pg->tank->y + pg->tank->height + 7;
	figure->show = false;
	figure->setImage("img/missile.gif");

	SDL_mutexP(missile_mutex); //Lock missile mutex
	pg->missile_hit = false;
	SDL_mutexV(missile_mutex); //Release missile mutex

}



int tank_thread( void *play_ground ){

	PlayGround *pg = ( PlayGround* ) play_ground;

	//Create Tank
	Figure* tank = new Figure();
	tank_init(tank, pg);
	SDL_mutexP(tank_mutex); //Lock tank mutex
	pg->tank = tank;
	SDL_mutexV(tank_mutex); //Release tank mutex

	//Create missile
	Figure* missile = new Figure();
	missile_init(missile, pg);
	SDL_mutexP(missile_mutex); //Lock missile mutex
	pg->missile = missile;
	SDL_mutexV(missile_mutex); //Release missile mutex
	//While the user hasn't quit
	bool key_pressed = false;


	while( quit == false ) {
		SDL_mutexP(tank_mutex); //Lock tank mutex
		bool hit = pg->tank_hit;
		SDL_mutexV(tank_mutex); //Release tank mutex
		if(hit){
			tank->setImage("img/explosion.png");
			SDL_Delay(500); // Pause
			tank_init(tank, pg);

			SDL_mutexP(lives_mutex); //Lock lives mutex
			pg->lives -= 1;
			SDL_mutexV(lives_mutex); //Release lives mutex
		}

		else{
			SDL_mutexP(key_mutex); //Lock mutex
			bool pressed = pg->key_pressed;
			SDL_mutexV(key_mutex); //Release mutex
			if(pressed){
				switch (process_key_event(event, tank, pg)){
					case TANK_REFRESH:

						break;
						//Load the firing sequence
					case TANK_FIRE:
						key_pressed = false;
						if(!missile->show){

							load_tank_moving_sequence(tank, 1, 18, 20, pg);

							SDL_mutexP(sound_mutex); //Lock mutex
							pg->soundtrack.shoot = true;
							SDL_mutexV(sound_mutex); //Release mutex
							SDL_mutexP(missile_mutex); //Lock missile mutex
							missile_init(missile, pg);
							missile->show = true;
							SDL_mutexV(missile_mutex); //Release missile mutex

							load_tank_moving_sequence(tank, 19, 32, 20, pg);
							load_tank_moving_sequence(tank, 1, 1, 0, pg);
						}

						break;
					default:
						//Reached the border don't refresh
						key_pressed = false;
				}

			}
			movemissile(pg);
			SDL_Delay(20);
		}

	}

	tank->clean_up();
	missile->clean_up();

	return 0;
}
