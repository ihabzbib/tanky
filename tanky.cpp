//============================================================================
// Name        : tanky.cpp
// Author      : Ihab Zbib
// Version     :
// Copyright   : CSUSB
//============================================================================

//The headers
#include <iostream>
#include "SDL/SDL.h"
#include "playground.h"
#include "sound.h"
#include "tank.h"
#include "target.h"
#include "dot.h"

using namespace std;

SDL_mutex *target_mutex;	//mutex to update the target status
SDL_mutex *missile_mutex;	//mutex to update the target status
SDL_mutex *tank_mutex;		//mutex to update the tank status
SDL_mutex *sound_mutex;		//mutex to update the sound status
SDL_mutex *score_mutex;		//mutex to update the score status
SDL_mutex *lives_mutex;		//mutex to update the lives status
SDL_mutex *key_mutex;		//mutex to update the key


SDL_Event event;

bool game_over = false;		//shared with sound_thread

bool quit = false;   //Make sure the program waits for a quit


/**
 * Main function
 * @return int
 */
int main( int argc, char* args[] ) {

    //create mutex for accessing key queues and mouse queue
    target_mutex 	= SDL_CreateMutex();
    missile_mutex	= SDL_CreateMutex();
    tank_mutex		= SDL_CreateMutex();
    sound_mutex		= SDL_CreateMutex();
    score_mutex		= SDL_CreateMutex();
    lives_mutex		= SDL_CreateMutex();
    key_mutex		= SDL_CreateMutex();

    if ( target_mutex == NULL )
    	printf( "Failed to create target_mutex!\n" );
    if ( missile_mutex == NULL )
    	printf( "Failed to create missile_mutex!\n" );
    if ( tank_mutex == NULL )
    	printf( "Failed to create tank_mutex!\n" );
    if ( sound_mutex == NULL )
        	printf( "Failed to create sound_mutex!\n" );
    if ( score_mutex == NULL )
        	printf( "Failed to create score_mutex!\n" );
    if ( lives_mutex == NULL )
        	printf( "Failed to create lives_mutex!\n" );
    if ( key_mutex == NULL )
        	printf( "Failed to create key_mutex!\n" );




	#ifndef ARM			//larger screen for regular PC
		const int VWIDTH = 1240;
		const int VHEIGHT = 680;
	#else 				//smaller screen for embedded system
		const int VWIDTH = 620;
		const int VHEIGHT = 440;
	#endif

    //Playground
    PlayGround pg( VWIDTH, VHEIGHT);


    SDL_Thread *tank;
    SDL_Thread *target;
    SDL_Thread *dot;
    SDL_Thread *sound;

    //Create mutex
    tank   		= SDL_CreateThread(tank_thread, &pg);
    target 		= SDL_CreateThread(target_thread, &pg);
    dot  		= SDL_CreateThread(dot_thread, &pg);
    sound  		= SDL_CreateThread( sound_thread, &pg.soundtrack);


	while( !quit ) {		//keep updating screen until some thread wants to quit
		//If there's an event to handle
		if( SDL_PollEvent( &event ) ) {
			//If a key was pressed
			if( event.type == SDL_KEYDOWN ) {
				pg.event = event.key.keysym.sym;
				if(pg.event == SDLK_ESCAPE){
					quit = true;
				}else if(pg.event == SDLK_F2){
					pg.init();

					SDL_mutexP(tank_mutex); //Lock tank mutex
					tank_init(pg.tank, &pg);
					SDL_mutexV(tank_mutex); //Release tank mutex

					SDL_mutexP(target_mutex); //Lock target mutex
					target_init(pg.target, &pg);
					SDL_mutexV(target_mutex); //Release target mutex

					dot_init_all(&pg);
				}
				else{
					SDL_mutexP(lives_mutex); //Lock lives mutex
					SDL_mutexP(key_mutex); //Lock key mutex
					if(pg.lives > 0){
						pg.key_pressed = true;
					}
					SDL_mutexV(key_mutex); //Release key mutex
					SDL_mutexV(lives_mutex); //Release lives mutex
				}
			}
			else if(event.type == SDL_KEYUP ){
				SDL_mutexP(key_mutex); //Lock key mutex
				pg.key_pressed = false;
				SDL_mutexV(key_mutex); //Release key mutex
			}

			//If the user has Xed out the window
			else if( event.type == SDL_QUIT ) {
				quit = true;
			}
		}

		SDL_PumpEvents();
		if ( SDL_PeepEvents ( NULL, 0, SDL_PEEKEVENT, SDL_QUITMASK) ) {
			break;
		}
		pg.refresh(20);

	}

	pg.clean_up();
	Mix_CloseAudio();

	SDL_WaitThread(tank, NULL);
	SDL_WaitThread(target, NULL);
	SDL_WaitThread(dot, NULL);
	SDL_WaitThread(sound, NULL);

	Mix_CloseAudio();

    //release the resources
    SDL_DestroyMutex ( target_mutex );
    SDL_DestroyMutex ( missile_mutex );
    SDL_DestroyMutex ( tank_mutex );
    SDL_DestroyMutex ( sound_mutex );
    SDL_DestroyMutex ( score_mutex );
    SDL_DestroyMutex ( lives_mutex );
    SDL_DestroyMutex ( key_mutex );


    return 0;

}
